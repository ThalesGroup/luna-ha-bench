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

#ifndef TEST_HPP
#define TEST_HPP

#include <chrono>
#include <pthread.h>

#include "top.hpp"
#include "scenario.hpp"

typedef unsigned long TEST_IDENTIFIER;

/*
 * State machine of a test:
 *      Created <------+-------------+-------------+
 *       |             |             |             |
 *       | prepare()   | terminate() |             |
 *       |             |             |             |
 *       +-> Prepared -+             | terminate() |
 *            |                      |             |
 *            | initialize()         |             |
 *            |                      |             |
 *            +-> Initialized -------+             | terminate()
 *                 |                               |
 *                 | start()                       |
 *                 |                               |
 *                 +-> Started                     |
 *                      |                          |
 *                      | stop()/waitforStop()     |
 *                      |                          |
 *                      +-> Stopped ---------------+
 */
enum class TEST_STATE
{
    Created = 1,
    Prepared = 2,
    Initialized = 3,
    Started = 4,
    Stopped = 5

};

void *runTestInThread(void *arg);

class Test : public Top
{
protected:
    TEST_STATE state = TEST_STATE::Created;

    CK_SESSION_HANDLE sessionHandle = CK_INVALID_HANDLE;
    CK_MECHANISM *pMechanism = nullptr;

    pthread_t threadIdentifier = (pthread_t)0;

    std::chrono::high_resolution_clock::time_point beginTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point endTime = beginTime;

    unsigned long requestsCount = 0L;
    unsigned long errorsCount = 0L;

    bool terminationRequested = false;

    Test(const Scenario &scenario,
         const TEST_IDENTIFIER identifier,
         const unsigned long requestsCountObjective);

    // Resources release operations can occur in any state.
    virtual CK_RV releaseUsedResources();

public:
    const Scenario &scenario;
    const TEST_IDENTIFIER identifier;
    const unsigned long requestsCountObjective;

    ~Test() override;

    Test(const Test &) = delete;
    Test &operator=(const Test &) = delete;

    virtual CK_RV run();

    virtual TEST_STATE getState() const;

    virtual CK_RV prepare();
    virtual CK_RV start();
    virtual CK_RV initialize();
    virtual CK_RV stop();
    virtual CK_RV waitForStop();
    virtual CK_RV terminate();

    virtual unsigned long getErrorsCount() const;
    virtual unsigned long getRequestsCount() const;
    virtual unsigned long getTransactionsPerSecond() const;

    void writeInformation(const char *const message) const override;
};

#endif /* TEST_HPP */