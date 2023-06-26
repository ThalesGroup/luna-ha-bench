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

#include "milenage-scenario.hpp"

extern "C"
{
#include <toolkits/misc-toolkit.h>
}

const char *const MILENAGE__TITLE = "Milenage";

const CK_BYTE MilenageScenario::MILENAGE__TEST_SET_1__OP[MILENAGE__OP_LENGTH] = {0xcd, 0xc2, 0x02, 0xd5, 0x12, 0x3e, 0x20, 0xf6, 0x2b, 0x6d, 0x67, 0x6a, 0xc7, 0x2c, 0xb3, 0x18};
const CK_BYTE MilenageScenario::MILENAGE__TEST_SET_1__OPC[MILENAGE__OPC_LENGTH] = {0xcd, 0x63, 0xcb, 0x71, 0x95, 0x4a, 0x9f, 0x4e, 0x48, 0xa5, 0x99, 0x4e, 0x37, 0xa0, 0x2b, 0xaf};
const CK_BYTE MilenageScenario::MILENAGE__TEST_SET_1__RC[MILENAGE__RC_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // C1
                                                                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, // C2
                                                                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, // C3
                                                                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, // C4
                                                                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, // C5
                                                                                 0x40,                                                                                           // R1
                                                                                 0x00,                                                                                           // R2
                                                                                 0x20,                                                                                           // R3
                                                                                 0x40,                                                                                           // R4
                                                                                 0x60};                                                                                          // R5
const CK_BYTE MilenageScenario::MILENAGE__TEST_SET_1__KI[THREE_GPP__KI_LENGTH] = {0x46, 0x5b, 0x5c, 0xe8, 0xb1, 0x99, 0xb4, 0x9f, 0xaa, 0x5f, 0x0a, 0x2e, 0xe2, 0x38, 0xa6, 0xbc};
const CK_BYTE MilenageScenario::MILENAGE__TEST_SET_1__SQN[THREE_GPP__SQN_LENGTH] = {0};
const CK_BYTE MilenageScenario::MILENAGE__TEST_SET_1__AMF[THREE_GPP__AMF_LENGTH] = {0};

MilenageScenario::MilenageScenario(const ScenarioContext &_scenarioContext,
                                   const SCENARIO_FLAGS _flags,
                                   const SCENARIO_IDENTIFIER _scenarioIdentifier,
                                   const size_t testsCount,
                                   const unsigned long _requestsCountPerTest) : FivegScenario(_scenarioContext,
                                                                                              _flags,
                                                                                              _scenarioIdentifier,
                                                                                              testsCount,
                                                                                              _requestsCountPerTest,
                                                                                              MILENAGE__TITLE),
                                                                                isUsingDefaultRc(getFlagValueAsBoolean(flags,
                                                                                                                       5))
{
    assert(&_scenarioContext != nullptr);
    assert(testsCount > 0);

    skLabel = generateObjectLabel(MILENAGE__TITLE,
                                  _scenarioIdentifier,
                                  uuid,
                                  "SK");
    opLabel = generateObjectLabel(MILENAGE__TITLE,
                                  _scenarioIdentifier,
                                  uuid,
                                  "OP");
    opcLabel = generateObjectLabel(MILENAGE__TITLE,
                                   _scenarioIdentifier,
                                   uuid,
                                   "OPc");
    rcLabel = generateObjectLabel(MILENAGE__TITLE,
                                  identifier,
                                  uuid,
                                  "RC");
}

CK_RV MilenageScenario::clean()
{
    CK_RV rv = CKR_OK;

    rv = FivegScenario::clean();

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    if ((sessionHandle != CK_INVALID_HANDLE) &&
        (!isUsingDefaultRc) &&
        (rcHandle != CK_INVALID_HANDLE))
    {
        CK_RV rv2 = CKR_OK;

        rv2 = p11tk_destroyObject(sessionHandle,
                                  rcHandle);

        if (rv2 != CKR_OK)
        {
            writeError("Cannot remove the RC object.",
                       rv);
        }
    }

EXIT:
    return rv;
}

const char *MilenageScenario::getFlagDescription(const unsigned int position) const
{
    if (position == 5)
    {
        return "Use default RC";
    }

    return FivegScenario::getFlagDescription(position);
}

unsigned int MilenageScenario::getFlagsCount() const
{
    return 5;
}

CK_RV MilenageScenario::getNewMechanism(CK_MECHANISM *&pMechanism) const
{
    assert(state == SCENARIO_STATE::Initialized);
    assert(&pMechanism != nullptr);

    CK_RV rv = CKR_OK;

    auto pMechanismParameters = (CK_MILENAGE_SIGN_PARAMS *)mallocAndReset(sizeof(CK_MILENAGE_SIGN_PARAMS));

    if (pMechanismParameters == nullptr)
    {
        rv = CKR_GENERAL_ERROR;

        goto EXIT;
    }

    pMechanismParameters->pEncKi = (CK_BYTE_PTR)eki;
    pMechanismParameters->ulEncKiLen = ekiLength;

    if (isUsingOp)
    {
        if (isUsingPreStoredOpOrOpc)
        {
            pMechanismParameters->ulMilenageFlags |= LUNA_5G_OP_OBJECT;

            pMechanismParameters->hSecondaryKey = opHandle;
        }
        else
        {
            if (isUsingCipheredOpOrOpc)
            {
                pMechanismParameters->ulMilenageFlags |= LUNA_5G_ENCRYPTED_OP;

                pMechanismParameters->pEncOPc = (CK_BYTE_PTR)eop;
                pMechanismParameters->ulEncOPcLen = eopLength;
            }
            else
            {
                pMechanismParameters->pEncOPc = (CK_BYTE_PTR)op;
                pMechanismParameters->ulEncOPcLen = opLength;
            }
        }
    }
    else
    {
        pMechanismParameters->ulMilenageFlags |= LUNA_5G_OPC;

        if (isUsingPreStoredOpOrOpc)
        {
            pMechanismParameters->ulMilenageFlags |= LUNA_5G_OP_OBJECT;
            pMechanismParameters->hSecondaryKey = opcHandle;
        }
        else
        {
            if (isUsingCipheredOpOrOpc)
            {
                pMechanismParameters->ulMilenageFlags |= LUNA_5G_ENCRYPTED_OP;
                pMechanismParameters->pEncOPc = (CK_BYTE_PTR)eopc;
                pMechanismParameters->ulEncOPcLen = eopcLength;
            }
            else
            {
                pMechanismParameters->pEncOPc = (CK_BYTE_PTR)opc;
                pMechanismParameters->ulEncOPcLen = opcLength;
            }
        }
    }

    if (!isUsingDefaultRc)
    {
        pMechanismParameters->ulMilenageFlags |= LUNA_5G_USER_DEFINED_RC;
        pMechanismParameters->hRCKey = rcHandle;
    }

    COPY_ARRAY_SAFELY(sqn,
                      pMechanismParameters->sqn);
    COPY_ARRAY_SAFELY(amf,
                      pMechanismParameters->amf);

    pMechanism = (CK_MECHANISM *)mallocAndReset(sizeof(CK_MECHANISM));

    if (pMechanism == nullptr)
    {
        rv = CKR_GENERAL_ERROR;

        free(pMechanismParameters);

        goto EXIT;
    }

    pMechanism->mechanism = CKM_MILENAGE;
    pMechanism->pParameter = pMechanismParameters;
    pMechanism->ulParameterLen = sizeof(CK_MILENAGE_SIGN_PARAMS);

EXIT:
    return rv;
}

CK_RV MilenageScenario::initialize()
{
    assert(state == SCENARIO_STATE::Prepared);

    CK_RV rv = CKR_OK;

    rv = FivegScenario::initialize();

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    if (!isUsingDefaultRc)
    {
        rv = p11tk_findObjectForLabel(sessionHandle,
                                      CKO_SECRET_KEY,
                                      (CK_CHAR *)(rcLabel.c_str()),
                                      &rcHandle);

        if (rv != CKR_OK)
        {
            writeError("Cannot retrieve RC.",
                       rv);

            goto EXIT;
        }

        if (rcHandle == CK_INVALID_HANDLE)
        {
            writeError("RC is missing.",
                       rv);

            goto EXIT;
        }
    }

    goto END;

EXIT:
    // Update the scenario state.
    state = SCENARIO_STATE::Prepared;

END:
    return rv;
}

CK_RV MilenageScenario::prepareScenario()
{
    assert(state == SCENARIO_STATE::Created);

    CK_RV rv = CKR_OK;

    rv = FivegScenario::prepareScenario();

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    if (scenarioContext.isSharingObjects)
    {
        // Try to retrieve an existing SK.
        writeInformation("Look for an existing RC...\n");

        rv = p11tk_findObjectForLabel(sessionHandle,
                                      CKO_SECRET_KEY,
                                      (CK_CHAR *)(rcLabel.c_str()),
                                      &rcHandle);

        if (rv != CKR_OK)
        {
            writeError("Cannot look for an existing RC.",
                       rv);

            goto EXIT;
        }
    }

    if (!isUsingDefaultRc)
    {
        if (rcHandle == CK_INVALID_HANDLE)
        {
            auto ercLength = (CK_ULONG)GET_ARRAY_SIZE(erc);

            // Encrypt predefined RC value with SK (eRC).
            rv = p11tk_encryptWithAesKwp(sessionHandle,
                                         skHandle,
                                         rc,
                                         GET_ARRAY_SIZE(rc),
                                         erc,
                                         &ercLength);

            if (rv != CKR_OK)
            {
                writeError("Cannot encrypt the RC.", rv);

                goto EXIT;
            }

            // Unwrap eRC.
            rv = p11tk_unwrapSensitiveData(sessionHandle,
                                           skHandle,
                                           erc,
                                           ercLength,
                                           (isUsingTokenObjectsOnly ? TRUE : FALSE),
                                           MILENAGE__RC_LENGTH,
                                           (CK_CHAR *)(rcLabel.c_str()),
                                           &rcHandle);

            if (rv != CKR_OK)
            {
                writeError("Cannot unwrap the eRC.", rv);

                goto EXIT;
            }
        }
        else
        {
            writeInformation("A RC already exists.");
        }
    }

    goto END;

EXIT:
    // Update the scenario state.
    state = SCENARIO_STATE::Created;

END:
    return rv;
}

CK_RV MilenageScenario::setScenarioData()
{
    CK_RV rv = CKR_OK;

    rv = Scenario::setScenarioData();

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    if (isUsingOp)
    {
        COPY_ARRAY_SAFELY(MILENAGE__TEST_SET_1__OP,
                          op);

        opLength = MILENAGE__OP_LENGTH;

        if (isUsingCipheredOpOrOpc)
        {
            eopLength = MILENAGE__EOP_LENGTH;
        }
    }
    else
    {
        COPY_ARRAY_SAFELY(MILENAGE__TEST_SET_1__OPC,
                          opc);

        opcLength = MILENAGE__OPC_LENGTH;

        if (isUsingCipheredOpOrOpc)
        {
            eopcLength = MILENAGE__EOPC_LENGTH;
        }
    }

    if (!isUsingDefaultRc)
    {
        COPY_ARRAY_SAFELY(MILENAGE__TEST_SET_1__RC,
                          rc);
    }

    COPY_ARRAY_SAFELY(MILENAGE__TEST_SET_1__KI,
                      ki);
    COPY_ARRAY_SAFELY(MILENAGE__TEST_SET_1__SQN,
                      sqn);
    COPY_ARRAY_SAFELY(MILENAGE__TEST_SET_1__AMF,
                      amf);

EXIT:
    return rv;
}

void MilenageScenario::writeDebugInformation() const
{
    FivegScenario::writeDebugInformation();

    CK_BYTE ouid[P11TK_OUID_LENGTH] = {0};
    size_t ouidLength = GET_ARRAY_SIZE(ouid);

    if (rcHandle != CK_INVALID_HANDLE)
    {
        p11tk_getObjectUniqueIdentifier(sessionHandle,
                                        rcHandle,
                                        ouid,
                                        &ouidLength);
    }
    writeBinaryData("  RC  Value : ",
                    rc,
                    GET_ARRAY_SIZE(rc));
    writeBinaryData("  eRC Value : ",
                    erc,
                    GET_ARRAY_SIZE(erc));
    fprintf(stdout,
            "  RC  Label : %s\n",
            rcLabel.c_str());
    if (rcHandle != CK_INVALID_HANDLE)
    {
        fprintf(stdout,
                "  RC  Handle: %ld\n",
                rcHandle);
        writeBinaryData("  RC  OUID  : ",
                        ouid,
                        ouidLength);
    }
}
