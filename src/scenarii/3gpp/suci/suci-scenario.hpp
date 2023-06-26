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

#ifndef SUCI_AUTHENTICATION_SCENARIO_HPP
#define SUCI_AUTHENTICATION_SCENARIO_HPP

#include "scenarii/scenario.hpp"

#define SUCI__TITLE "SUCI"

#define SUCI__SUPI_LENGTH 16

// ECIES block size.
#define SUCI__DATA_LENGTH 80

// TODO: to be confirmed (should be 81 or 113).
#define SUCI__ENCRYPTED_DATA_LENGTH 256

#define SUCI__PROFILE_A_CURVE 19
#define SUCI__PROFILE_B_CURVE 69

/*
 * SUCI scenario flags are defined as follows:
 *   xyz:
 *      x= (for profile A only)
 *          1: curve points are compressed.
 *          0: curve points are not compressed.
 *      y=
 *          1: profile B is used.
 *          0: profile A is used.
 *      z=
 *          1: use token objects only.
 *          0: use session objects only.
 */
class SuciScenario : public Scenario
{
protected:
    //
    // Test set 1.
    //
    static const CK_BYTE SUCI__TEST_SET_1__SUPI[SUCI__SUPI_LENGTH];

    std::string publicKeyLabel = "";
    std::string privateKeyLabel = "";

    CK_OBJECT_HANDLE publicKeyHandle = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE privateKeyHandle = CK_INVALID_HANDLE;

    CK_RV clean() override;

    CK_RV setScenarioData() override;

    CK_RV prepareScenario() override;

public:
    const bool isUsingProfileB;
    const bool isUsingCompressedCurvedPoints;

    unsigned int ellipticCurveIdentifier = SUCI__PROFILE_A_CURVE;

    // These value are exposed publicly for the sake of simplicity, without risk.
    CK_BYTE data[SUCI__DATA_LENGTH] = {0};
    CK_ULONG dataLength = GET_ARRAY_SIZE(data);
    CK_BYTE encryptedData[SUCI__ENCRYPTED_DATA_LENGTH] = {0};
    CK_ULONG encryptedDataLength = GET_ARRAY_SIZE(encryptedData);

    SuciScenario(const ScenarioContext &scenarioContext,
                 const SCENARIO_FLAGS flags,
                 const SCENARIO_IDENTIFIER identifier,
                 const size_t testsCount,
                 const unsigned long _requestsCountPerTest);
    ~SuciScenario() override = default;

    SuciScenario(const SuciScenario &) = delete;
    SuciScenario &operator=(const SuciScenario &) = delete;

    bool checkFlags(const SCENARIO_FLAGS flags) const override;
    const char *getFlagDescription(const unsigned int position) const override;
    unsigned int getFlagsCount() const override;

    virtual CK_OBJECT_HANDLE getPrivateKeyHandle() const;

    CK_RV initialize() override;

    CK_RV getNewMechanism(CK_MECHANISM *&pMechanism) const override;

    void writeDebugInformation() const override;
};

#endif /* SUCI_AUTHENTICATION_SCENARIO_HPP */