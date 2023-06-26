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

#ifndef AUTHENTICATION_AUTHENTICATION_SCENARIO_HPP
#define AUTHENTICATION_AUTHENTICATION_SCENARIO_HPP

#include "scenarii/scenario.hpp"

// Maximum of Milenage and TUAK sizes.
#define THREE_GPP__OP_LENGTH 32

// Maximum of Milenage and TUAK sizes.
#define THREE_GPP__EOP_LENGTH 40

// Maximum of Milenage and TUAK sizes.
#define THREE_GPP__OPC_LENGTH 32

// Maximum of Milenage and TUAK sizes.
#define THREE_GPP__EOPC_LENGTH 40

#define THREE_GPP__KI_LENGTH 16
#define THREE_GPP__EKI_LENGTH 24

#define THREE_GPP__SQN_LENGTH 6
#define THREE_GPP__AMF_LENGTH 2

/*
 * 5g authentication scenario flags are defined as follows:
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
class FivegScenario : public Scenario
{
protected:
    std::string skLabel = "";
    std::string opLabel = "";
    std::string opcLabel = "";

    CK_OBJECT_HANDLE skHandle = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE opHandle = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE opcHandle = CK_INVALID_HANDLE;

    CK_BYTE op[THREE_GPP__OP_LENGTH] = {0};
    CK_ULONG opLength = GET_ARRAY_SIZE(op);

    CK_BYTE eop[THREE_GPP__EOP_LENGTH] = {0};
    CK_ULONG eopLength = GET_ARRAY_SIZE(eop);

    CK_BYTE opc[THREE_GPP__OPC_LENGTH] = {0};
    CK_ULONG opcLength = THREE_GPP__OPC_LENGTH;

    CK_BYTE eopc[THREE_GPP__EOPC_LENGTH] = {0};
    CK_ULONG eopcLength = GET_ARRAY_SIZE(eopc);

    CK_BYTE ki[THREE_GPP__KI_LENGTH] = {0};
    CK_ULONG kiLength = GET_ARRAY_SIZE(ki);

    CK_BYTE eki[THREE_GPP__EKI_LENGTH] = {0};
    CK_ULONG ekiLength = GET_ARRAY_SIZE(eki);

    CK_BYTE sqn[THREE_GPP__SQN_LENGTH] = {0};
    CK_BYTE amf[THREE_GPP__AMF_LENGTH] = {0};

    FivegScenario(const ScenarioContext &scenarioContext,
                  const SCENARIO_FLAGS flags,
                  const SCENARIO_IDENTIFIER scenarioIdentifier,
                  const size_t testsCount,
                  const unsigned long _requestsCountPerTest,
                  const std::string title);

    CK_RV clean() override;

    CK_RV setScenarioData() override;

    CK_RV prepareScenario() override;

public:
    const bool isUsingOp;
    const bool isUsingCipheredOpOrOpc;
    const bool isUsingPreStoredOpOrOpc;

    // Note: cannot use default destructor (cannot be inlined because it is too large).
    ~FivegScenario() override;

    FivegScenario(const FivegScenario &) = delete;
    FivegScenario &operator=(const FivegScenario &) = delete;

    virtual CK_OBJECT_HANDLE getSkHandle() const;

    bool checkFlags(const SCENARIO_FLAGS flags) const override;
    const char *getFlagDescription(const unsigned int position) const override;
    unsigned int getFlagsCount() const override;

    CK_RV initialize() override;

    void writeDebugInformation() const override;
};

#endif /* AUTHENTICATION_AUTHENTICATION_SCENARIO_HPP */