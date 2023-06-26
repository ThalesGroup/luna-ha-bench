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
#include <cstdlib>
#include <unistd.h>

#include "5g-scenario.hpp"
#include "5g-test.hpp"

FivegTest::FivegTest(const Scenario &_scenario,
                     const TEST_IDENTIFIER _identifier,
                     const unsigned long _requestsCountObjective) : Test(_scenario,
                                                                         _identifier,
                                                                         _requestsCountObjective)
{
    assert(&_scenario != nullptr);

    // Nothing else to do here.
}

CK_RV FivegTest::run()
{
    assert(state == TEST_STATE::Started);
    assert(sessionHandle != CK_INVALID_HANDLE);
    assert(pMechanism != nullptr);

    CK_RV rv = CKR_OK;

    CK_BYTE authenticationVector[THREE_GPP__AUTHENTICATION_VECTOR_LENGTH] = {0};
    CK_ULONG authenticationVectorLength = GET_ARRAY_SIZE(authenticationVector);

    while ((!terminationRequested) &&
           ((requestsCountObjective == 0) ||
            (requestsCount < requestsCountObjective)))
    {
        requestsCount++;

        rv = C_SignInit(sessionHandle,
                        pMechanism,
                        ((FivegScenario &)scenario).getSkHandle());

        if (scenario.scenarioContext.withDebug)
        {
            writeMessage("Info:\n");
            writeDebugInformation();
        }

        if (rv != CKR_OK)
        {
            writeError("Cannot initialize authentication.",
                       rv);

            errorsCount++;
        }
        else
        {
            rv = C_Sign(sessionHandle,
                        (CK_BYTE_PTR) nullptr,
                        (CK_ULONG)0,
                        (CK_BYTE_PTR)authenticationVector,
                        &authenticationVectorLength);

            if (rv != CKR_OK)
            {
                writeError("Cannot finalize authentication.",
                           rv);

                errorsCount++;
            }
        }
    }

    return CKR_OK;
}
