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

#ifndef TUAK_AUTHENTICATION_SCENARIO_HPP
#define TUAK_AUTHENTICATION_SCENARIO_HPP

#include "scenarii/3gpp/authentication/5g-scenario.hpp"

extern const char *const TUAK__TITLE;

#define TUAK__OP_LENGTH 32
#define TUAK__EOP_LENGTH 40

#define TUAK__OPC_LENGTH 32
#define TUAK__EOPC_LENGTH 40

/*
 * TUAK authentication scenario flags are defined as follows:
 *   vxyz:
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
class TuakScenario : public FivegScenario
{
protected:
    CK_RV setScenarioData() override;

public:
    //
    // Test set 1 from https://itecspec.com/spec/3gpp-35-233-6-conformance-test-data-for-tuak/
    //
    static const CK_BYTE TUAK__TEST_SET_1__OP[TUAK__OP_LENGTH];
    static const CK_BYTE TUAK__TEST_SET_1__OPC[TUAK__OPC_LENGTH];
    static const CK_BYTE TUAK__TEST_SET_1__KI[THREE_GPP__KI_LENGTH];
    static const CK_BYTE TUAK__TEST_SET_1__SQN[THREE_GPP__SQN_LENGTH];
    static const CK_BYTE TUAK__TEST_SET_1__AMF[THREE_GPP__AMF_LENGTH];
    static const CK_LONG TUAK__TEST_SET_1__ITERATIONS;
    static const CK_LONG TUAK__TEST_SET_1__RES_LENGTH;
    static const CK_LONG TUAK__TEST_SET_1__MAC_LENGTH;
    static const CK_LONG TUAK__TEST_SET_1__CK_LENGTH;
    static const CK_LONG TUAK__TEST_SET_1__IK_LENGTH;

    CK_LONG iterations = (CK_LONG)0;
    CK_LONG resLength = (CK_LONG)0;
    CK_LONG macLength = (CK_LONG)0;
    CK_LONG ckLength = (CK_LONG)0;
    CK_LONG ikLength = (CK_LONG)0;

    TuakScenario(const ScenarioContext &scenarioContext,
                 const SCENARIO_FLAGS flags,
                 const SCENARIO_IDENTIFIER identifier,
                 const size_t testsCount,
                 const unsigned long _requestsCountPerTest);
    ~TuakScenario() override = default;

    TuakScenario(const TuakScenario &) = delete;
    TuakScenario &operator=(const TuakScenario &) = delete;

    unsigned int getFlagsCount() const override;

    CK_RV getNewMechanism(CK_MECHANISM *&pMechanism) const override;
};

#endif /* TUAK_AUTHENTICATION_SCENARIO_HPP */