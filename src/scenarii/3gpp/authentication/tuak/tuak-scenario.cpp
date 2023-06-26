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

#include "tuak-scenario.hpp"

const char *const TUAK__TITLE = "TUAK";

const CK_BYTE TuakScenario::TUAK__TEST_SET_1__OP[TUAK__OP_LENGTH] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
const CK_BYTE TuakScenario::TUAK__TEST_SET_1__OPC[TUAK__OPC_LENGTH] = {0xbd, 0x04, 0xd9, 0x53, 0x0e, 0x87, 0x51, 0x3c, 0x5d, 0x83, 0x7a, 0xc2, 0xad, 0x95, 0x46, 0x23, 0xa8, 0xe2, 0x33, 0x0c, 0x11, 0x53, 0x05, 0xa7, 0x3e, 0xb4, 0x5d, 0x1f, 0x40, 0xcc, 0xcb, 0xff};
const CK_BYTE TuakScenario::TUAK__TEST_SET_1__KI[THREE_GPP__KI_LENGTH] = {0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab};
const CK_BYTE TuakScenario::TUAK__TEST_SET_1__SQN[THREE_GPP__SQN_LENGTH] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
const CK_BYTE TuakScenario::TUAK__TEST_SET_1__AMF[THREE_GPP__AMF_LENGTH] = {0xff, 0xff};
const CK_LONG TuakScenario::TUAK__TEST_SET_1__ITERATIONS = (CK_LONG)1;
const CK_LONG TuakScenario::TUAK__TEST_SET_1__RES_LENGTH = (CK_LONG)4;
const CK_LONG TuakScenario::TUAK__TEST_SET_1__MAC_LENGTH = (CK_LONG)8;
const CK_LONG TuakScenario::TUAK__TEST_SET_1__CK_LENGTH = (CK_LONG)16;
const CK_LONG TuakScenario::TUAK__TEST_SET_1__IK_LENGTH = (CK_LONG)16;

TuakScenario::TuakScenario(const ScenarioContext &_scenarioContext,
                           const SCENARIO_FLAGS _flags,
                           const SCENARIO_IDENTIFIER _scenarioIdentifier,
                           const size_t testsCount,
                           const unsigned long _requestsCountPerTest) : FivegScenario(_scenarioContext,
                                                                                      _flags,
                                                                                      _scenarioIdentifier,
                                                                                      testsCount,
                                                                                      _requestsCountPerTest,
                                                                                      TUAK__TITLE)

{
    assert(&_scenarioContext != nullptr);
    assert(testsCount > 0);

    skLabel = generateObjectLabel(TUAK__TITLE,
                                  _scenarioIdentifier,
                                  uuid,
                                  "SK");
    opLabel = generateObjectLabel(TUAK__TITLE,
                                  _scenarioIdentifier,
                                  uuid,
                                  "OP");
    opcLabel = generateObjectLabel(TUAK__TITLE,
                                   _scenarioIdentifier,
                                   uuid,
                                   "OPc");
}

unsigned int TuakScenario::getFlagsCount() const
{
    return 4;
}

CK_RV TuakScenario::getNewMechanism(CK_MECHANISM *&pMechanism) const
{
    assert(state == SCENARIO_STATE::Initialized);
    assert(&pMechanism != nullptr);

    CK_RV rv = CKR_OK;

    auto pMechanismParameters = (CK_TUAK_SIGN_PARAMS *)mallocAndReset(sizeof(CK_TUAK_SIGN_PARAMS));

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
            pMechanismParameters->ulTuakFlags |= LUNA_5G_OP_OBJECT;

            pMechanismParameters->hSecondaryKey = opHandle;
        }
        else
        {
            if (isUsingCipheredOpOrOpc)
            {
                pMechanismParameters->ulTuakFlags |= LUNA_5G_ENCRYPTED_OP;

                pMechanismParameters->pEncTOPc = (CK_BYTE_PTR)eop;
                pMechanismParameters->ulEncTOPcLen = eopLength;
            }
            else
            {
                pMechanismParameters->pEncTOPc = (CK_BYTE_PTR)op;
                pMechanismParameters->ulEncTOPcLen = opLength;
            }
        }
    }
    else
    {
        pMechanismParameters->ulTuakFlags |= LUNA_5G_OPC;

        if (isUsingPreStoredOpOrOpc)
        {
            pMechanismParameters->ulTuakFlags |= LUNA_5G_OP_OBJECT;
            pMechanismParameters->hSecondaryKey = opcHandle;
        }
        else
        {
            if (isUsingCipheredOpOrOpc)
            {
                pMechanismParameters->ulTuakFlags |= LUNA_5G_ENCRYPTED_OP;
                pMechanismParameters->pEncTOPc = (CK_BYTE_PTR)eopc;
                pMechanismParameters->ulEncTOPcLen = eopcLength;
            }
            else
            {
                pMechanismParameters->pEncTOPc = (CK_BYTE_PTR)opc;
                pMechanismParameters->ulEncTOPcLen = opcLength;
            }
        }
    }

    pMechanismParameters->ulIterations = iterations;
    pMechanismParameters->ulResLen = resLength;
    pMechanismParameters->ulMacALen = macLength;
    pMechanismParameters->ulCkLen = ckLength;
    pMechanismParameters->ulIkLen = ikLength;

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

    pMechanism->mechanism = CKM_TUAK;
    pMechanism->pParameter = pMechanismParameters;
    pMechanism->ulParameterLen = sizeof(CK_TUAK_SIGN_PARAMS);

EXIT:
    return rv;
}

CK_RV TuakScenario::setScenarioData()
{
    CK_RV rv = CKR_OK;

    rv = Scenario::setScenarioData();

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    if (isUsingOp)
    {
        COPY_ARRAY_SAFELY(TUAK__TEST_SET_1__OP,
                          op);

        opLength = TUAK__OP_LENGTH;

        if (isUsingCipheredOpOrOpc)
        {
            eopLength = TUAK__EOP_LENGTH;
        }
    }
    else
    {
        COPY_ARRAY_SAFELY(TUAK__TEST_SET_1__OPC,
                          opc);

        opcLength = TUAK__OPC_LENGTH;

        if (isUsingCipheredOpOrOpc)
        {
            eopcLength = TUAK__EOPC_LENGTH;
        }
    }

    COPY_ARRAY_SAFELY(TUAK__TEST_SET_1__KI,
                      ki);
    COPY_ARRAY_SAFELY(TUAK__TEST_SET_1__SQN,
                      sqn);
    COPY_ARRAY_SAFELY(TUAK__TEST_SET_1__AMF,
                      amf);

    iterations = TUAK__TEST_SET_1__ITERATIONS;
    resLength = TUAK__TEST_SET_1__RES_LENGTH;
    macLength = TUAK__TEST_SET_1__MAC_LENGTH;
    ikLength = TUAK__TEST_SET_1__IK_LENGTH;
    ckLength = TUAK__TEST_SET_1__CK_LENGTH;

EXIT:
    return rv;
}
