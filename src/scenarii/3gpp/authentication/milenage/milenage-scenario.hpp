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

#ifndef MILENAGE_AUTHENTICATION_SCENARIO_HPP
#define MILENAGE_AUTHENTICATION_SCENARIO_HPP

#include "scenarii/3gpp/authentication/5g-scenario.hpp"

extern const char *const MILENAGE__TITLE;

#define MILENAGE__OP_LENGTH 16
#define MILENAGE__EOP_LENGTH 24

#define MILENAGE__OPC_LENGTH 16
#define MILENAGE__EOPC_LENGTH 24

#define MILENAGE__RC_LENGTH 16 + 16 + 16 + 16 + 16 + 1 + 1 + 1 + 1 + 1
#define MILENAGE__ERC_LENGTH 96

/*
 * Milenage authentication cenario flags are defined as follows:
 *   uvxyz:
 *      u=
 *          1: use default RC values.
 *          0: use specific RC values.
 *      v=
 *          1: (e)OP or (e)OPc is pre-loaded in the HSM.
 *          0: (e)OP or (e)OPc is provided in the authentication request.
 *      x=
 *          1: OP or OPc is encrypted.
 *          0: OP or OPc is in clear text.
 *      y=
 *          1: OP is used.
 *          0: OPc is used.
 *      z=
 *          1: use token objects only.
 *          0: use session objects only.
 */
class MilenageScenario : public FivegScenario
{
protected:
    std::string rcLabel = "";

    CK_OBJECT_HANDLE rcHandle = CK_INVALID_HANDLE;

    CK_BYTE rc[MILENAGE__RC_LENGTH] = {0};
    CK_BYTE erc[MILENAGE__ERC_LENGTH] = {0};

    CK_RV clean() override;

    CK_RV setScenarioData() override;

    CK_RV prepareScenario() override;

public:
    //
    // Test set 1 from https://itecspec.com/spec/3gpp-55-205-6-test-data-for-gsm-milenage-informative/.
    //
    static const CK_BYTE MILENAGE__TEST_SET_1__OP[MILENAGE__OP_LENGTH];
    static const CK_BYTE MILENAGE__TEST_SET_1__OPC[MILENAGE__OPC_LENGTH];
    static const CK_BYTE MILENAGE__TEST_SET_1__RC[MILENAGE__RC_LENGTH];
    static const CK_BYTE MILENAGE__TEST_SET_1__KI[THREE_GPP__KI_LENGTH];
    static const CK_BYTE MILENAGE__TEST_SET_1__SQN[THREE_GPP__SQN_LENGTH];
    static const CK_BYTE MILENAGE__TEST_SET_1__AMF[THREE_GPP__AMF_LENGTH];

    const bool isUsingDefaultRc;

    MilenageScenario(const ScenarioContext &scenarioContext,
                     const SCENARIO_FLAGS flags,
                     const SCENARIO_IDENTIFIER scenarioIdentifier,
                     const size_t testsCount,
                     const unsigned long _requestsCountPerTest);
    ~MilenageScenario() override = default;

    MilenageScenario(const MilenageScenario &) = delete;
    MilenageScenario &operator=(const MilenageScenario &) = delete;

    const char *getFlagDescription(const unsigned int position) const override;
    unsigned int getFlagsCount() const override;

    CK_RV getNewMechanism(CK_MECHANISM *&pMechanism) const override;

    CK_RV initialize() override;

    void writeDebugInformation() const override;
};

#endif /* MILENAGE_AUTHENTICATION_SCENARIO_HPP */