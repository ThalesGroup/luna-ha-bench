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

#ifndef SUCI_TEST_HPP
#define SUCI_TEST_HPP

#include "scenarii/scenario.hpp"
#include "scenarii/test.hpp"

class SuciTest : public Test
{
public:
    SuciTest(const Scenario &scenario,
             const TEST_IDENTIFIER identifier,
             const unsigned long requestsCountObjective);
    ~SuciTest() override = default;

    SuciTest(const SuciTest &) = delete;
    SuciTest &operator=(const SuciTest &) = delete;

    CK_RV run() override;
};

#endif /* SUCI_TEST_HPP */