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

#include "suci-scenario.hpp"
#include "suci-test.hpp"

const CK_BYTE SuciScenario::SUCI__TEST_SET_1__SUPI[SUCI__SUPI_LENGTH] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

SuciScenario::SuciScenario(const ScenarioContext &_scenarioContext,
                           const SCENARIO_FLAGS _flags,
                           const SCENARIO_IDENTIFIER _scenarioIdentifier,
                           const size_t testsCount,
                           const unsigned long _requestsCountPerTest) : Scenario(_scenarioContext,
                                                                                 _flags,
                                                                                 _scenarioIdentifier,
                                                                                 testsCount,
                                                                                 _requestsCountPerTest,
                                                                                 SUCI__TITLE),
                                                                        isUsingProfileB(getFlagValueAsBoolean(_flags,
                                                                                                              2)),
                                                                        isUsingCompressedCurvedPoints(getFlagValueAsBoolean(_flags,
                                                                                                                            3))
{
    assert(&_scenarioContext != nullptr);
    assert(testsCount > 0);

    publicKeyLabel = generateObjectLabel(SUCI__TITLE,
                                         _scenarioIdentifier,
                                         uuid,
                                         "Public Key");
    privateKeyLabel = generateObjectLabel(SUCI__TITLE,
                                          _scenarioIdentifier,
                                          uuid,
                                          "Private Key");

    ellipticCurveIdentifier = (isUsingProfileB ? SUCI__PROFILE_B_CURVE : SUCI__PROFILE_A_CURVE);

    tests.resize(testsCount);

    for (size_t testIndex = 0;
         testIndex < tests.size();
         testIndex++)
    {
        tests[testIndex] = std::make_shared<SuciTest>(*this,
                                                      (TEST_IDENTIFIER)testIndex,
                                                      _requestsCountPerTest);
    }
}

CK_RV SuciScenario::clean()
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

        if (publicKeyHandle != CK_INVALID_HANDLE)
        {
            rv2 = p11tk_destroyObject(sessionHandle,
                                      publicKeyHandle);

            if (rv2 != CKR_OK)
            {
                writeError("Cannot remove the public key object.",
                           rv2);
            }
        }

        if (privateKeyHandle != CK_INVALID_HANDLE)
        {
            rv2 = p11tk_destroyObject(sessionHandle,
                                      privateKeyHandle);

            if (rv2 != CKR_OK)
            {
                writeError("Cannot remove the private key object.",
                           rv2);
            }
        }
    }

EXIT:
    return rv;
}

bool SuciScenario::checkFlags(const SCENARIO_FLAGS _flags) const
{
    // Profile B does not support compressed points.
    const bool _isUsingProfileB = getFlagValueAsBoolean(_flags,
                                                        2);
    const bool _isUsingCompressedCurvedPoints = getFlagValueAsBoolean(_flags,
                                                                      3);

    if (_isUsingProfileB &&
        _isUsingCompressedCurvedPoints)
    {
        return false;
    }

    return Scenario::checkFlags(_flags);
}

const char *SuciScenario::getFlagDescription(const unsigned int position) const
{
    switch (position)
    {
    case 2:
        return "Use profile B";

    case 3:
        return "Use compressed curved points";

    default:
        return Scenario::getFlagDescription(position);
    }
}

unsigned int SuciScenario::getFlagsCount() const
{
    return 3;
}

CK_RV SuciScenario::getNewMechanism(CK_MECHANISM *&pMechanism) const
{
    assert(state == SCENARIO_STATE::Initialized);
    assert(&pMechanism != nullptr);

    CK_RV rv = CKR_OK;

    auto pMechanismParameters = (CK_ECIES_PARAMS *)mallocAndReset(sizeof(CK_ECIES_PARAMS));

    if (pMechanismParameters == nullptr)
    {
        rv = CKR_GENERAL_ERROR;

        goto EXIT;
    }

    pMechanismParameters->kdf = CKD_SHA256_KDF;
    pMechanismParameters->encScheme = CKES_AES_CTR;
    pMechanismParameters->ulEncKeyLenInBits = 128;
    pMechanismParameters->macScheme = CKMS_HMAC_SHA256;
    pMechanismParameters->ulMacKeyLenInBits = 256;
    pMechanismParameters->ulMacLenInBits = 256;
    pMechanismParameters->dhPrimitive = CKDHP_STANDARD;

    pMechanism = (CK_MECHANISM *)mallocAndReset(sizeof(CK_MECHANISM));

    if (pMechanism == nullptr)
    {
        rv = CKR_GENERAL_ERROR;

        free(pMechanismParameters);

        goto EXIT;
    }

    pMechanism->mechanism = CKM_ECIES;
    pMechanism->pParameter = pMechanismParameters;
    pMechanism->ulParameterLen = sizeof(CK_ECIES_PARAMS);

EXIT:
    return rv;
}

CK_OBJECT_HANDLE SuciScenario::getPrivateKeyHandle() const
{
    return privateKeyHandle;
}

CK_RV SuciScenario::initialize()
{
    assert(state == SCENARIO_STATE::Prepared);

    CK_RV rv = CKR_OK;
    CK_MECHANISM *pMechanism = nullptr;

    rv = Scenario::initialize();

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    // Retrieve the public key.
    rv = p11tk_findObjectForLabel(sessionHandle,
                                  CKO_PUBLIC_KEY,
                                  (CK_CHAR *)(publicKeyLabel.c_str()),
                                  &publicKeyHandle);

    if (rv != CKR_OK)
    {
        writeError("Cannot retrieve the public key.",
                   rv);

        goto EXIT;
    }

    if (publicKeyHandle == CK_INVALID_HANDLE)
    {
        writeError("The public key is missing.",
                   rv);

        goto EXIT;
    }

    // Retrieve the private key.
    rv = p11tk_findObjectForLabel(sessionHandle,
                                  CKO_PRIVATE_KEY,
                                  (CK_CHAR *)(privateKeyLabel.c_str()),
                                  &privateKeyHandle);

    if (rv != CKR_OK)
    {
        writeError("Cannot retrieve the private key.",
                   rv);

        goto EXIT;
    }

    if (privateKeyHandle == CK_INVALID_HANDLE)
    {
        writeError("The private key is missing.",
                   rv);

        goto EXIT;
    }

    // Get a new mechanism for the encryption purposes.
    rv = getNewMechanism(pMechanism);

    if (rv != CKR_OK)
    {
        writeError("Cannot prepare the scenario: cannot get a new mechanism to encrypt the test data.",
                   rv);

        goto EXIT;
    }

    // Encrypt the test data.
    rv = C_EncryptInit(sessionHandle,
                       pMechanism,
                       publicKeyHandle);

    if (rv != CKR_OK)
    {
        writeError("Cannot prepare the scenario: cannot initialize data encryption for the test data.",
                   rv);

        goto EXIT;
    }

    encryptedDataLength = GET_ARRAY_SIZE(encryptedData);

    rv = C_Encrypt(sessionHandle,
                   data,
                   dataLength,
                   encryptedData,
                   &encryptedDataLength);

    if (rv != CKR_OK)
    {
        writeError("Cannot encrypt data.",
                   rv);

        goto EXIT;
    }

    // If compressed points are requested, compress it.
    // Set the leading byte to 0x02 to indicate a compress point with an even
    // Y value or 0x03 to indicate a compressed point with an odd Y value.
    // And then move the encrypted data to the left to overwrite the Y value.
    if (isUsingCompressedCurvedPoints)
    {
        const P11_ELLIPTIC_CURVES_LIST_ELEMENT *pEllipticCurveData = nullptr;

        rv = p11tk_getEllipticCurveDataForCurveIdentifier(ellipticCurveIdentifier,
                                                          &pEllipticCurveData);

        if (rv != CKR_OK)
        {
            writeError("Cannot retrieve elliptic curve data.",
                       rv);

            goto EXIT;
        }

        unsigned int curveFieldLength = pEllipticCurveData->fieldLength;

        encryptedData[0] = (CK_BYTE)((encryptedData[curveFieldLength * 2] & 0x01) ? 0x03 : 0x02);

        memmove(&encryptedData[curveFieldLength + 1],
                &encryptedData[curveFieldLength * 2 + 1],
                encryptedDataLength - (curveFieldLength * 2 + 1));

        encryptedDataLength -= curveFieldLength;
    }

    goto END;

EXIT:
    // Update the scenario state
    state = SCENARIO_STATE::Prepared;

END:
    if (pMechanism != nullptr)
    {
        free(pMechanism->pParameter);
        free(pMechanism);
    }

    return rv;
}

CK_RV SuciScenario::prepareScenario()
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
        writeInformation("Look for an existing public key...\n");

        rv = p11tk_findObjectForLabel(sessionHandle,
                                      CKO_PUBLIC_KEY,
                                      (CK_CHAR *)(publicKeyLabel.c_str()),
                                      &publicKeyHandle);

        if (rv != CKR_OK)
        {
            writeError("Cannot look for an existing public key.",
                       rv);

            goto EXIT;
        }
    }

    if (publicKeyHandle == CK_INVALID_HANDLE)
    {
        writeInformation("Generate needed HSM objects...\n");

        // Generate key pair.
        rv = p11tk_generateEllipticKeyPair(sessionHandle,
                                           (isUsingTokenObjectsOnly ? TRUE : FALSE),
                                           ellipticCurveIdentifier,
                                           (CK_CHAR *)(publicKeyLabel.c_str()),
                                           (CK_CHAR *)(privateKeyLabel.c_str()),
                                           &publicKeyHandle,
                                           &privateKeyHandle);

        if (rv != CKR_OK)
        {
            writeError("Cannot generate an elliptic key pair.",
                       rv);

            goto EXIT;
        }
    }
    else
    {
        writeInformation("A public key already exists.");
    }

    goto END;

EXIT:
    // Update the scenario state
    state = SCENARIO_STATE::Created;

END:
    return rv;
}

CK_RV SuciScenario::setScenarioData()
{
    CK_RV rv = CKR_OK;

    rv = Scenario::setScenarioData();

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    CLEAR_ARRAY(data);
    COPY_ARRAY_SAFELY(SUCI__TEST_SET_1__SUPI,
                      data);
    dataLength = GET_ARRAY_SIZE(data);

EXIT:
    return rv;
}

void SuciScenario::writeDebugInformation() const
{
    Scenario::writeDebugInformation();

    CK_BYTE ouid[P11TK_OUID_LENGTH] = {0};
    size_t ouidLength = GET_ARRAY_SIZE(ouid);

    p11tk_getObjectUniqueIdentifier(sessionHandle,
                                    publicKeyHandle,
                                    ouid,
                                    &ouidLength);
    fprintf(stdout,
            "  Public Key Label  : %s\n",
            publicKeyLabel.c_str());
    fprintf(stdout,
            "  Public Key Handle : %ld\n",
            publicKeyHandle);
    writeBinaryData("  Public Key OUID   : ",
                    ouid,
                    ouidLength);

    p11tk_getObjectUniqueIdentifier(sessionHandle,
                                    privateKeyHandle,
                                    ouid,
                                    &ouidLength);
    fprintf(stdout,
            "  Private Key Label  : %s\n",
            privateKeyLabel.c_str());
    fprintf(stdout,
            "  Private Key Handle : %ld\n",
            privateKeyHandle);
    writeBinaryData("  Private Key OUID   : ",
                    ouid,
                    ouidLength);

    writeBinaryData("  Data: ",
                    data,
                    dataLength);
    writeBinaryData("  Encrypted Data: ",
                    encryptedData,
                    encryptedDataLength);
}