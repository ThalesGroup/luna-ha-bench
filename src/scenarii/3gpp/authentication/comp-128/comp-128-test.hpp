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

#ifndef COMP_128_TEST_HPP
#define COMP_128_TEST_HPP

#include "scenarii/scenario.hpp"
#include "scenarii/test.hpp"

class Comp128Test : public Test
{
public:
    Comp128Test(const Scenario &scenario,
                const TEST_IDENTIFIER identifier,
                const unsigned long requestsCountObjective);
    ~Comp128Test() override = default;

    Comp128Test(const Comp128Test &) = delete;
    Comp128Test &operator=(const Comp128Test &) = delete;

    CK_RV run() override;
};

#endif /* COMP_128_TEST_HPP */