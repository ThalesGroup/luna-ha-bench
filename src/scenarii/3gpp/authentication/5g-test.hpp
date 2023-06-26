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

#ifndef AUTHENTICATION_TEST_HPP
#define AUTHENTICATION_TEST_HPP

#include "scenarii/scenario.hpp"
#include "scenarii/test.hpp"

#define THREE_GPP__AUTHENTICATION_VECTOR_LENGTH 72

class FivegTest : public Test
{
protected:
    CK_OBJECT_HANDLE skHandle = CK_INVALID_HANDLE;

public:
    FivegTest(const Scenario &scenario,
              const TEST_IDENTIFIER identifier,
              const unsigned long requestsCountObjective);
    ~FivegTest() override = default;

    FivegTest(const FivegTest &) = delete;
    FivegTest &operator=(const FivegTest &) = delete;

    CK_RV run() override;
};

#endif /* AUTHENTICATION_TEST_HPP */