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

#include "top.hpp"

Top::Top(const std::string _identificationString) : identificationString(_identificationString)
{
    // Nothing else to do here.
}

std::string Top::getUniqueString() const
{
    return identificationString;
}

void Top::writeDebugInformation() const
{
    Top::writeInformation("Debug information:\n");
}

void Top::writeError(const char *const message,
                     const CK_RV rv) const
{
    fprintf(stderr,
            "%s: %s ['0x%08lx']\n",
            getUniqueString().c_str(),
            message,
            rv);
}

void Top::writeInformation(const char *const message) const
{
    fprintf(stdout,
            "%s: %s",
            getUniqueString().c_str(),
            message);
}
