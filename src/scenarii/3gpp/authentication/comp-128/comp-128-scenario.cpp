/****************************************************************************\
*
* This file is part of the "Luna HA-Bench" tool.
*
* The "Luna HA-Bench" tool is provided under the MIT license (see the
* following Web site for further details: https://mit-license.org/ ).
*
* Copyright © 2023 Thales Group
*
\****************************************************************************/

#include <cassert>
#include <cstring>
#include <string>

extern "C"
{
#include <toolkits/misc-toolkit.h>
}

#include "comp-128-scenario.hpp"
#include "comp-128-test.hpp"

const char *const COMP_128__TITLE = "COMP-128";

Comp128Scenario::Comp128Scenario(const ScenarioContext &_scenarioContext,
                                 const SCENARIO_FLAGS _flags,
                                 const SCENARIO_IDENTIFIER _scenarioIdentifier,
                                 const size_t testsCount,
                                 const unsigned long _requestsCountPerTest) : Scenario(_scenarioContext,
                                                                                       _flags,
                                                                                       _scenarioIdentifier,
                                                                                       testsCount,
                                                                                       _requestsCountPerTest,
                                                                                       COMP_128__TITLE),
                                                                              version(getFlagValue(_flags,
                                                                                                   2))
{
    assert(&_scenarioContext != nullptr);
    assert(testsCount > 0);

    skLabel = generateObjectLabel(COMP_128__TITLE,
                                  _scenarioIdentifier,
                                  uuid,
                                  "SK");

    tests.resize(testsCount);

    for (size_t testIndex = 0;
         testIndex < tests.size();
         testIndex++)
    {
        tests[testIndex] = std::make_shared<Comp128Test>(*this,
                                                         (TEST_IDENTIFIER)testIndex,
                                                         _requestsCountPerTest);
    }
}

bool Comp128Scenario::checkFlag(const unsigned int position,
                                const unsigned int value) const
{
    if (position == 2)
    {
        return ((value == 1) ||
                (value == 2) ||
                (value == 3));
    }

    return Scenario::checkFlag(position,
                               value);
}

CK_RV Comp128Scenario::clean()
{
    assert((state == SCENARIO_STATE::Created) ||
           (state == SCENARIO_STATE::Prepared) ||
           (state == SCENARIO_STATE::Stopped));

    CK_RV rv = CKR_OK;

    rv = Scenario::clean();

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    if ((sessionHandle != CK_INVALID_HANDLE) &&
        (skHandle != CK_INVALID_HANDLE))
    {
        CK_RV rv2 = CKR_OK;

        rv2 = p11tk_destroyObject(sessionHandle,

                                  skHandle);
        if (rv2 != CKR_OK)
        {
            writeError("Cannot remove the SK object.",
                       rv2);
        }
    }

EXIT:
    return rv;
}

const char *Comp128Scenario::getFlagDescription(const unsigned int position) const
{
    if (position == 2)
    {
        return "Version";
    }

    return Scenario::getFlagDescription(position);
}

unsigned int Comp128Scenario::getFlagsCount() const
{
    return 2;
}

CK_RV Comp128Scenario::getNewMechanism(CK_MECHANISM *&pMechanism) const
{
    assert(state == SCENARIO_STATE::Initialized);
    assert(&pMechanism != nullptr);

    CK_RV rv = CKR_OK;

    auto pMechanismParameters = (CK_COMP128_SIGN_PARAMS *)mallocAndReset(sizeof(CK_COMP128_SIGN_PARAMS));

    if (pMechanismParameters == nullptr)
    {
        rv = CKR_GENERAL_ERROR;

        goto EXIT;
    }

    pMechanismParameters->ulVersion = version;
    pMechanismParameters->pEncKi = (CK_BYTE_PTR)eki;
    pMechanismParameters->ulEncKiLen = ekiLength;

    pMechanism = (CK_MECHANISM *)mallocAndReset(sizeof(CK_MECHANISM));

    if (pMechanism == nullptr)
    {
        rv = CKR_GENERAL_ERROR;

        free(pMechanismParameters);

        goto EXIT;
    }

    pMechanism->mechanism = CKM_COMP128;
    pMechanism->pParameter = pMechanismParameters;
    pMechanism->ulParameterLen = sizeof(CK_COMP128_SIGN_PARAMS);

EXIT:
    return rv;
}

CK_OBJECT_HANDLE Comp128Scenario::getSkHandle() const
{
    return skHandle;
}

CK_RV Comp128Scenario::initialize()
{
    assert(state == SCENARIO_STATE::Prepared);

    CK_RV rv = CKR_OK;

    rv = Scenario::initialize();

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    // Retrieve the SK.
    rv = p11tk_findObjectForLabel(sessionHandle,
                                  CKO_SECRET_KEY,
                                  (CK_CHAR *)(skLabel.c_str()),
                                  &skHandle);

    if (rv != CKR_OK)
    {
        writeError("Cannot retrieve SK.",
                   rv);

        goto EXIT;
    }

    if (skHandle == CK_INVALID_HANDLE)
    {
        writeError("SK is missing.",
                   rv);

        goto EXIT;
    }

    // Encrypt the predefined Ki value with SK (eKi).
    rv = p11tk_encryptWithAesKwp(sessionHandle,
                                 skHandle,
                                 ki,
                                 kiLength,
                                 eki,
                                 &ekiLength);

    if (rv != CKR_OK)
    {
        writeError("Cannot encrypt the Ki.",
                   rv);

        goto EXIT;
    }

    goto END;

EXIT:
    // Update the scenario state.
    state = SCENARIO_STATE::Prepared;

END:
    return rv;
}

CK_RV Comp128Scenario::prepareScenario()
{
    assert(state == SCENARIO_STATE::Created);

    CK_RV rv = CKR_OK;

    rv = Scenario::prepareScenario();

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    if (scenarioContext.isSharingObjects)
    {
        writeInformation("Look for an existing SK...\n");

        rv = p11tk_findObjectForLabel(sessionHandle,
                                      CKO_SECRET_KEY,
                                      (CK_CHAR *)(skLabel.c_str()),
                                      &skHandle);

        if (rv != CKR_OK)
        {
            writeError("Cannot look for an existing SK.",
                       rv);

            goto EXIT;
        }
    }

    if (skHandle == CK_INVALID_HANDLE)
    {
        // Generate SK.
        rv = p11tk_generateStorageKey(sessionHandle,
                                      (isUsingTokenObjectsOnly ? TRUE : FALSE),
                                      (CK_CHAR *)(skLabel.c_str()),
                                      &skHandle);

        if (rv != CKR_OK)
        {
            writeError("Cannot create a SK.",
                       rv);

            goto EXIT;
        }
    }
    else
    {
        writeInformation("A SK already exists.");
    }

    goto END;

EXIT:
    // Update the scenario state.
    state = SCENARIO_STATE::Created;

END:
    return rv;
}

void Comp128Scenario::writeDebugInformation() const
{
    Scenario::writeDebugInformation();

    CK_BYTE ouid[P11TK_OUID_LENGTH] = {0};
    size_t ouidLength = GET_ARRAY_SIZE(ouid);

    fprintf(stdout,
            "  SK Label  : %s\n",
            skLabel.c_str());
    fprintf(stdout,
            "  SK Handle : %ld\n",
            skHandle);
    p11tk_getObjectUniqueIdentifier(sessionHandle,
                                    skHandle,
                                    ouid,
                                    &ouidLength);
    writeBinaryData("  SK OUID   : ",
                    ouid,
                    ouidLength);

    fprintf(stdout,
            "\n");

    writeBinaryData("  Ki Value : ",
                    ki,
                    kiLength);
    writeBinaryData("  eKi Value: ",
                    eki,
                    ekiLength);
}
