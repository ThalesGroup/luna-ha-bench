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

#ifndef SCENARIO_CONTEXT_HPP
#define SCENARIO_CONTEXT_HPP

#include "top.hpp"

extern "C"
{
#include <toolkits/p11-toolkit.h>
}

class ScenarioContext
{
protected:
    const CK_SLOT_ID slotId;
    const CK_CHAR *const coPassword;
    const CK_ULONG coPasswordLength;

public:
    const bool withDebug = false;

    const bool isSharingObjects;
    const bool isVerbose;

    ScenarioContext(const CK_SLOT_ID slotId,
                    const CK_CHAR *const coPassword,
                    const CK_ULONG coPasswordLength,
                    const bool isSharingObjects,
                    const bool isVerbose);
    virtual ~ScenarioContext() = default;

    ScenarioContext(const ScenarioContext &) = default;
    ScenarioContext &operator=(const ScenarioContext &) = delete;

    virtual CK_RV openLoggedSession(CK_SESSION_HANDLE &sessionHandle) const;
    virtual CK_RV openSession(CK_SESSION_HANDLE &sessionHandle) const;

    virtual CK_RV closeLoggedSession(CK_SESSION_HANDLE &sessionHandle) const;
};

#endif /* SCENARIO_CONTEXT_HPP */