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

#include "scenario-context.hpp"

ScenarioContext::ScenarioContext(const CK_SLOT_ID _slotId,
                                 const CK_CHAR *const _coPassword,
                                 const CK_ULONG _coPasswordLength,
                                 const bool _isSharingObjects,
                                 const bool _isVerbose) : slotId(_slotId),
                                                          coPassword(_coPassword),
                                                          coPasswordLength(_coPasswordLength),
                                                          isSharingObjects(_isSharingObjects),
                                                          isVerbose(_isVerbose)
{
    // Nothing else to do here.
}

CK_RV ScenarioContext::closeLoggedSession(CK_SESSION_HANDLE &sessionHandle) const
{
    assert(sessionHandle != CK_INVALID_HANDLE);

    CK_RV rv = CKR_OK;

    rv = p11tk_logout(sessionHandle);

    if ((rv != CKR_OK) &&
        (rv != CKR_USER_NOT_AUTHORIZED)) // TODO: need for clarification.
    {
        p11tk_writeError("Cannot logout user from logged session.",
                         rv);

        goto EXIT;
    }

    rv = p11tk_closeSession(sessionHandle);

    if (rv != CKR_OK)
    {
        p11tk_writeError("Cannot close the logged session.",
                         rv);
    }

    sessionHandle = CK_INVALID_HANDLE;

EXIT:
    return rv;
}

CK_RV ScenarioContext::openLoggedSession(CK_SESSION_HANDLE &sessionHandle) const
{
    CK_RV rv = CKR_OK;

    rv = p11tk_openSession(slotId,
                           &sessionHandle);

    if (rv != CKR_OK)
    {
        p11tk_writeError("Cannot open a logged session on for this slot.",
                         rv);

        goto EXIT;
    }

    rv = p11tk_login(sessionHandle,
                     (CK_CHAR *)coPassword,
                     coPasswordLength);

    if (rv != CKR_OK)
    {
        p11tk_writeError("Cannot login with this session handle.",
                         rv);

        CK_RV rv2 = CKR_OK;

        rv2 = p11tk_closeSession(sessionHandle);

        if (rv2 != CKR_OK)
        {
            p11tk_writeError("Cannot close this session after a login failure.",
                             rv);
        }

        sessionHandle = CK_INVALID_HANDLE;
    }

EXIT:
    return rv;
}

CK_RV ScenarioContext::openSession(CK_SESSION_HANDLE &sessionHandle) const
{
    CK_RV rv = CKR_OK;

    rv = p11tk_openSession(slotId,
                           &sessionHandle);

    if (rv != CKR_OK)
    {
        p11tk_writeError("Cannot open a session on for this slot.",
                         rv);

        goto EXIT;
    }

EXIT:
    return rv;
}