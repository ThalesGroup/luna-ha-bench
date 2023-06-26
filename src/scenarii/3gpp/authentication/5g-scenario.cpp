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
#include <cstdio>

extern "C"
{
#include <toolkits/misc-toolkit.h>
}

#include "5g-scenario.hpp"
#include "5g-test.hpp"

FivegScenario::FivegScenario(const ScenarioContext &_scenarioContext,
                             const SCENARIO_FLAGS _flags,
                             const SCENARIO_IDENTIFIER _identifier,
                             const size_t testsCount,
                             const unsigned long _requestsCountPerTest,
                             const std::string title) : Scenario(_scenarioContext,
                                                                 _flags,
                                                                 _identifier,
                                                                 testsCount,
                                                                 _requestsCountPerTest,
                                                                 title),
                                                        isUsingOp(getFlagValueAsBoolean(_flags,
                                                                                        2)),
                                                        isUsingCipheredOpOrOpc(getFlagValueAsBoolean(_flags,
                                                                                                     3)),
                                                        isUsingPreStoredOpOrOpc(getFlagValueAsBoolean(_flags,
                                                                                                      4))
{
    assert(&_scenarioContext != nullptr);
    assert(testsCount > 0);

    tests.resize(testsCount);

    for (size_t testIndex = 0;
         testIndex < tests.size();
         testIndex++)
    {
        tests[testIndex] = std::make_shared<FivegTest>(*this,
                                                       (TEST_IDENTIFIER)testIndex,
                                                       _requestsCountPerTest);
    }
}

// Note: cannot use default destructor (cannot be inlined because it is too large).
FivegScenario::~FivegScenario()
{
    // Do nothing;
}

CK_RV FivegScenario::clean()
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

    if (sessionHandle != CK_INVALID_HANDLE)
    {
        CK_RV rv2 = CKR_OK;

        if (skHandle != CK_INVALID_HANDLE)
        {
            rv2 = p11tk_destroyObject(sessionHandle,
                                      skHandle);

            if (rv2 != CKR_OK)
            {
                writeError("Cannot remove the SK object.",
                           rv2);
            }
        }

        if (isUsingOp)
        {
            if (opHandle != CK_INVALID_HANDLE)
            {
                rv2 = p11tk_destroyObject(sessionHandle,
                                          opHandle);

                if (rv2 != CKR_OK)
                {
                    writeError("Cannot remove the OP object.",
                               rv2);
                }
            }
        }
        else
        {
            if (opcHandle != CK_INVALID_HANDLE)
            {
                rv2 = p11tk_destroyObject(sessionHandle,
                                          opcHandle);

                if (rv2 != CKR_OK)
                {
                    writeError("Cannot remove the OPc object.",
                               rv2);
                }
            }
        }
    }

EXIT:
    return rv;
}

bool FivegScenario::checkFlags(const SCENARIO_FLAGS _flags) const
{
    // If OP or OPc is pre-loaded then the "encryption of OP or OPc" flag is
    // not meaningful as it applies to the transfer of OP or OPc when these
    // values are provided to the sign command.
    const bool _isUsingCipheredOpOrOpc = getFlagValueAsBoolean(_flags,
                                                               3);
    const bool _isUsingPreStoredOpOrOpc = getFlagValueAsBoolean(_flags,
                                                                4);

    if (_isUsingPreStoredOpOrOpc &&
        _isUsingCipheredOpOrOpc)
    {
        return false;
    }

    return Scenario::checkFlags(_flags);
}

const char *FivegScenario::getFlagDescription(const unsigned int position) const
{
    switch (position)
    {
    case 2:
        return "Use OP rather than OPc";

    case 3:
        return "Use ciphered OP or OPc";

    case 4:
        return "Use pre-stored OP or OPc";

    default:
        return Scenario::getFlagDescription(position);
    }
}

unsigned int FivegScenario::getFlagsCount() const
{
    return 4;
}

CK_OBJECT_HANDLE FivegScenario::getSkHandle() const
{
    return skHandle;
}

CK_RV FivegScenario::initialize()
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

    // Retrieve the OP or OPc if pre-stored, and produce the eOP or eOPc if needed.
    if (isUsingOp)
    {
        if (isUsingPreStoredOpOrOpc)
        {
            rv = p11tk_findObjectForLabel(sessionHandle,
                                          CKO_SECRET_KEY,
                                          (CK_CHAR *)(opLabel.c_str()),
                                          &opHandle);

            if (rv != CKR_OK)
            {
                writeError("Cannot retrieve OP.",
                           rv);

                goto EXIT;
            }

            if (opHandle == CK_INVALID_HANDLE)
            {
                writeError("OP is missing.",
                           rv);

                goto EXIT;
            }
        }
        else
        {
            if (isUsingCipheredOpOrOpc)
            {
                // Encrypt predefined OP value with SK (eOP).
                rv = p11tk_encryptWithAesKwp(sessionHandle,
                                             skHandle,
                                             op,
                                             opLength,
                                             eop,
                                             &eopLength);

                if (rv != CKR_OK)
                {
                    writeError("Cannot encrypt the OP.",
                               rv);

                    goto EXIT;
                }
            }
        }
    }
    else
    {
        if (isUsingPreStoredOpOrOpc)
        {
            rv = p11tk_findObjectForLabel(sessionHandle,
                                          CKO_SECRET_KEY,
                                          (CK_CHAR *)(opcLabel.c_str()),
                                          &opcHandle);

            if (rv != CKR_OK)
            {
                writeError("Cannot retrieve OPc.",
                           rv);

                goto EXIT;
            }

            if (skHandle == CK_INVALID_HANDLE)
            {
                writeError("SK is missing.",
                           rv);

                goto EXIT;
            }
        }
        else
        {
            if (isUsingCipheredOpOrOpc)
            {
                // Encrypt predefined eOPc value with SK (eOPc).
                rv = p11tk_encryptWithAesKwp(sessionHandle,
                                             skHandle,
                                             opc,
                                             opcLength,
                                             eopc,
                                             &eopcLength);

                if (rv != CKR_OK)
                {
                    writeError("Cannot encrypt the OPc.",
                               rv);

                    goto EXIT;
                }
            }
        }
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

CK_RV FivegScenario::prepareScenario()
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
        // Try to retrieve an existing SK.
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

        // Try to retrieve an existing OP.
        writeInformation("Look for an existing OP...\n");

        rv = p11tk_findObjectForLabel(sessionHandle,
                                      CKO_SECRET_KEY,
                                      (CK_CHAR *)(opLabel.c_str()),
                                      &opHandle);

        if (rv != CKR_OK)
        {
            writeError("Cannot look for an existing OP.",
                       rv);

            goto EXIT;
        }

        // Try to retrieve an existing OPc.
        writeInformation("Look for an existing OPc...\n");

        rv = p11tk_findObjectForLabel(sessionHandle,
                                      CKO_SECRET_KEY,
                                      (CK_CHAR *)(opcLabel.c_str()),
                                      &opcHandle);

        if (rv != CKR_OK)
        {
            writeError("Cannot look for an existing OPc.",
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
        writeInformation("An SK already exists.");
    }

    if (isUsingOp)
    {
        if (isUsingCipheredOpOrOpc ||
            isUsingPreStoredOpOrOpc)
        {
            // Encrypt predefined OP value with SK (eOP).
            rv = p11tk_encryptWithAesKwp(sessionHandle,
                                         skHandle,
                                         op,
                                         opLength,
                                         eop,
                                         &eopLength);

            if (rv != CKR_OK)
            {
                writeError("Cannot encrypt the OP.",
                           rv);

                goto EXIT;
            }
        }

        if (isUsingPreStoredOpOrOpc)
        {
            if (opHandle == CK_INVALID_HANDLE)
            {
                // Unwrap eOP.
                rv = p11tk_unwrapSensitiveData(sessionHandle,
                                               skHandle,
                                               eop,
                                               eopLength,
                                               (isUsingTokenObjectsOnly ? TRUE : FALSE),
                                               opLength,
                                               (CK_CHAR *)(opLabel.c_str()),
                                               &opHandle);

                if (rv != CKR_OK)
                {
                    writeError("Cannot unwrap the eOP.",
                               rv);

                    goto EXIT;
                }
            }
            else
            {
                writeInformation("An OP already exists.");
            }
        }
    }
    else
    {
        if (isUsingCipheredOpOrOpc ||
            isUsingPreStoredOpOrOpc)
        {
            // Encrypt predefined eOPc value with SK (eOPc).
            rv = p11tk_encryptWithAesKwp(sessionHandle,
                                         skHandle,
                                         opc,
                                         opcLength,
                                         eopc,
                                         &eopcLength);

            if (rv != CKR_OK)
            {
                writeError("Cannot encrypt the OPc.",
                           rv);

                goto EXIT;
            }
        }

        if (isUsingPreStoredOpOrOpc)
        {
            if (opcHandle == CK_INVALID_HANDLE)
            { // Unwrap eOPc
                rv = p11tk_unwrapSensitiveData(sessionHandle,
                                               skHandle,
                                               eopc,
                                               eopcLength,
                                               (isUsingTokenObjectsOnly ? TRUE : FALSE),
                                               opcLength,
                                               (CK_CHAR *)(opcLabel.c_str()),
                                               &opcHandle);

                if (rv != CKR_OK)
                {
                    writeError("Cannot unwrap the eOPc.",
                               rv);

                    goto EXIT;
                }
            }
            else
            {
                writeInformation("An OPc already exists.");
            }
        }
    }

    goto END;

EXIT:
    // Update the scenario state.
    state = SCENARIO_STATE::Created;

END:
    return rv;
}

CK_RV FivegScenario::setScenarioData()
{
    CK_RV rv = CKR_OK;

    rv = Scenario::setScenarioData();

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    // Nothing else to do here.

EXIT:
    return rv;
}

void FivegScenario::writeDebugInformation() const
{
    Scenario::writeDebugInformation();

    CK_BYTE ouid[P11TK_OUID_LENGTH] = {0};
    size_t ouidLength = GET_ARRAY_SIZE(ouid);

    fprintf(stdout,
            "\n");

    p11tk_getObjectUniqueIdentifier(sessionHandle,
                                    skHandle,
                                    ouid,
                                    &ouidLength);
    fprintf(stdout,
            "  SK Label  : %s\n",
            skLabel.c_str());
    fprintf(stdout,
            "  SK Handle : %ld\n",
            skHandle);
    writeBinaryData("  SK OUID   : ",
                    ouid,
                    ouidLength);

    fprintf(stdout,
            "\n");

    if (opHandle != CK_INVALID_HANDLE)
    {
        p11tk_getObjectUniqueIdentifier(sessionHandle,
                                        opHandle,
                                        ouid,
                                        &ouidLength);
    }
    writeBinaryData("  OP Value  : ",
                    op,
                    opLength);
    writeBinaryData("  eOP Value : ",
                    eop,
                    eopLength);
    fprintf(stdout,
            "  OP Label  : %s\n",
            opLabel.c_str());
    if (opHandle != CK_INVALID_HANDLE)
    {
        fprintf(stdout,
                "  OP Handle : %ld\n",
                opHandle);
        writeBinaryData("  OP OUID   : ",
                        ouid,
                        ouidLength);
    }

    fprintf(stdout,
            "\n");

    if (opcHandle != CK_INVALID_HANDLE)
    {
        p11tk_getObjectUniqueIdentifier(sessionHandle,
                                        opcHandle,
                                        ouid,
                                        &ouidLength);
    }
    writeBinaryData("  OPc  Value : ",
                    opc,
                    opcLength);
    writeBinaryData("  eOPc Value : ",
                    eopc,
                    eopcLength);
    fprintf(stdout,
            "  OPc  Label : %s\n",
            opcLabel.c_str());
    if (opcHandle != CK_INVALID_HANDLE)
    {
        fprintf(stdout,
                "  OPc  Handle: %ld\n",
                opcHandle);
        writeBinaryData("  OPc  OUID  : ",
                        ouid,
                        ouidLength);
    }

    fprintf(stdout,
            "\n");

    writeBinaryData("  Ki  Value: ",
                    ki,
                    kiLength);
    writeBinaryData("  eKi Value: ",
                    eki,
                    ekiLength);
}
