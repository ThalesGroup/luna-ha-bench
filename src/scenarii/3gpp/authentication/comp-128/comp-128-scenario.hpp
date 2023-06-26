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

#ifndef COMP_128_AUTHENTICATION_SCENARIO_HPP
#define COMP_128_AUTHENTICATION_SCENARIO_HPP

#include "scenarii/scenario.hpp"

extern const char *const COMP_128__TITLE;

#define COMP_128__KI_LENGTH 16
#define COMP_128__EKI_LENGTH 24

// TODO: adjust.
#define COMP_128__AUTHENTICATION_VECTOR_LENGTH 128

/*
 * COMP_128 scenario flags are defined as follows:
 *   x:
 *      x=
 *          1: version 1.
 *          2: version 2.
 *          3: version 3.
 */
class Comp128Scenario : public Scenario
{
protected:
    std::string skLabel = "";

    CK_OBJECT_HANDLE skHandle = CK_INVALID_HANDLE;

    CK_BYTE ki[COMP_128__KI_LENGTH] = {0};
    CK_ULONG kiLength = GET_ARRAY_SIZE(ki);

    CK_BYTE eki[COMP_128__EKI_LENGTH] = {0};
    CK_ULONG ekiLength = GET_ARRAY_SIZE(eki);

    CK_RV clean() override;

    CK_RV prepareScenario() override;

public:
    const unsigned int version;

    Comp128Scenario(const ScenarioContext &scenarioContext,
                    const SCENARIO_FLAGS flags,
                    const SCENARIO_IDENTIFIER identifier,
                    const size_t testsCount,
                    const unsigned long _requestsCountPerTest);
    ~Comp128Scenario() override = default;

    Comp128Scenario(const Comp128Scenario &) = delete;
    Comp128Scenario &operator=(const Comp128Scenario &) = delete;

    bool checkFlag(const unsigned int position,
                   const unsigned int value) const override;
    const char *getFlagDescription(const unsigned int position) const override;
    unsigned int getFlagsCount() const override;

    virtual CK_OBJECT_HANDLE getSkHandle() const;

    CK_RV initialize() override;

    CK_RV getNewMechanism(CK_MECHANISM *&pMechanism) const override;

    void writeDebugInformation() const override;
};

#endif /* COMP_128_AUTHENTICATION_SCENARIO_HPP */