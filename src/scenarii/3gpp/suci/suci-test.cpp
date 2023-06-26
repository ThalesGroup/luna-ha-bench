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
#include <cstring>
#include <unistd.h>

#include "suci-scenario.hpp"
#include "suci-test.hpp"

SuciTest::SuciTest(const Scenario &_scenario,
                   const TEST_IDENTIFIER _identifier,
                   const unsigned long _requestsCountObjective) : Test(_scenario,
                                                                       _identifier,
                                                                       _requestsCountObjective)
{
    assert(&_scenario != nullptr);

    // Nothing else to do here.
}

CK_RV SuciTest::run()
{
    assert(state == TEST_STATE::Started);
    assert(sessionHandle != CK_INVALID_HANDLE);
    assert(pMechanism != nullptr);

    CK_RV rv = CKR_OK;

    CK_BYTE decryptedData[SUCI__DATA_LENGTH] = {0};
    CK_ULONG decryptedDataLength = GET_ARRAY_SIZE(decryptedData);

    while ((!terminationRequested) &&
           ((requestsCountObjective == 0) ||
            (requestsCount < requestsCountObjective)))
    {
        requestsCount++;

        rv = C_DecryptInit(sessionHandle,
                           pMechanism,
                           ((SuciScenario &)scenario).getPrivateKeyHandle());

        if (rv != CKR_OK)
        {
            writeError("Cannot initialize deconcealment.",
                       rv);

            errorsCount++;
        }
        else
        {
            decryptedDataLength = GET_ARRAY_SIZE(decryptedData);

            rv = C_Decrypt(sessionHandle,
                           ((SuciScenario &)scenario).encryptedData,
                           ((SuciScenario &)scenario).encryptedDataLength,
                           decryptedData,
                           &decryptedDataLength);

            if (rv != CKR_OK)
            {
                writeError("Cannot deconceal data.",
                           rv);

                errorsCount++;
            }
            else
            {
                if (decryptedDataLength != ((SuciScenario &)scenario).dataLength)
                {
                    writeError("Decrypted data length doesn't match data length.",
                               rv);

                    errorsCount++;
                }
                else
                {
                    if (memcmp(decryptedData,
                               ((SuciScenario &)scenario).data,
                               ((SuciScenario &)scenario).dataLength) != 0)
                    {
                        writeError("Decrypted data doesn't match data.",
                                   rv);

                        errorsCount++;
                    }
                }
            }
        }
    }

    return CKR_OK;
}