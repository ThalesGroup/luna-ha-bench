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

#ifndef TOP_HPP
#define TOP_HPP

#include <string>

extern "C"
{
#include <toolkits/p11-toolkit.h>
}

#define HA_BENCH__TITLE "HA-Bench"

class Top;

class Top
{
protected:
    explicit Top(const std::string identificationString);

public:
    const std::string identificationString;

    virtual ~Top() = default;

    Top(const Top &) = default;
    Top &operator=(const Top &) = default;

    virtual std::string getUniqueString() const;

    virtual void writeDebugInformation() const;
    virtual void writeError(const char *const message,
                            const CK_RV rv) const;
    virtual void writeInformation(const char *const message) const;
};

#endif /* TOP_HPP */