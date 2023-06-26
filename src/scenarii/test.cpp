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
#include <unistd.h>

#include "test.hpp"

void *runTestInThread(void *arg)
{
    assert(arg != nullptr);

    pthread_exit((void *)(((Test *)arg)->run()));
}

Test::Test(const Scenario &_scenario,
           const TEST_IDENTIFIER _identifier,
           const unsigned long _requestsCountObjective) : Top(std::string("Scenario ") +
                                                              std::to_string(_scenario.identifier) +
                                                              std::string(": Test ") +
                                                              std::to_string(_identifier)),
                                                          scenario(_scenario),
                                                          identifier(_identifier),
                                                          requestsCountObjective(_requestsCountObjective)
{
    assert(&_scenario != nullptr);

    // Nothing else to do here
}

Test::~Test()
{
    CK_RV rv = CKR_OK;

    rv = releaseUsedResources();

    if (rv != CKR_OK)
    {
        writeError("Cannot destroy the test object properly.",
                   rv);
    }
}

unsigned long Test::getErrorsCount() const
{
    return errorsCount;
}

unsigned long Test::getRequestsCount() const
{
    return requestsCount;
}

TEST_STATE Test::getState() const
{
    return state;
}

unsigned long Test::getTransactionsPerSecond() const
{
    auto elapsedMicroSeconds = (unsigned long)(std::chrono::duration_cast<std::chrono::microseconds>(endTime - beginTime).count());

    if (elapsedMicroSeconds == 0L)
    {
        return 0L;
    }

    assert(requestsCount >= errorsCount);

    return (unsigned long)((requestsCount - errorsCount) / (elapsedMicroSeconds / 1000000L));
}

CK_RV Test::initialize()
{
    assert(state == TEST_STATE::Prepared);
    assert(sessionHandle != CK_INVALID_HANDLE);

    CK_RV rv = CKR_OK;

    // Get a new cryptographic mechanism.
    rv = scenario.getNewMechanism(pMechanism);

    if (rv != CKR_OK)
    {
        writeError("Cannot initialize the test: cannot get a new cryptographic mechanism.",
                   rv);

        goto EXIT;
    }

    // Update the test state.
    state = TEST_STATE::Initialized;

EXIT:
    return rv;
}

CK_RV Test::prepare()
{
    assert(state == TEST_STATE::Created);

    CK_RV rv = CKR_OK;

    // Get a new session (note that login already occured at the scenario
    // level).
    rv = scenario.scenarioContext.openSession(sessionHandle);

    if (rv != CKR_OK)
    {
        writeError("Cannot prepare the test: cannot get a new logged session.",
                   rv);

        goto EXIT;
    }

    // Update the test state.
    state = TEST_STATE::Prepared;

EXIT:
    return rv;
}

CK_RV Test::releaseUsedResources()
{
    CK_RV rv = CKR_OK;

    if (sessionHandle != CK_INVALID_HANDLE)
    {
        rv = scenario.scenarioContext.closeLoggedSession(sessionHandle);
    }

    if (pMechanism != nullptr)
    {
        free(pMechanism->pParameter);
        free(pMechanism);

        pMechanism = nullptr;
    }

    return rv;
}

CK_RV Test::run()
{
    assert(state == TEST_STATE::Started);

    while ((!terminationRequested) &&
           ((requestsCountObjective == 0) ||
            (requestsCount < requestsCountObjective)))
    {
        writeInformation("Run the test...\n");

        sleep(1);
    }

    return CKR_OK;
}

CK_RV Test::start()
{
    assert(state == TEST_STATE::Initialized);

    CK_RV rv = CKR_OK;

    writeInformation("Start the test...\n");

    requestsCount = 0L;
    errorsCount = 0L;

    terminationRequested = false;

    beginTime = std::chrono::high_resolution_clock::now();

    // Update the test state.
    // Note: the state must be updated before the thread starts.
    state = TEST_STATE::Started;

    int threadCreationResult = -1;

    threadCreationResult = pthread_create(&threadIdentifier,
                                          nullptr,
                                          &runTestInThread,
                                          this);

    if (threadCreationResult != 0)
    {
        rv = CKR_GENERAL_ERROR;

        writeError("Cannot run the test in a separate thread.",
                   rv);

        // Update the test state.
        state = TEST_STATE::Initialized;

        goto EXIT;
    }

    // Update the test state.
    state = TEST_STATE::Started;

EXIT:
    return rv;
}

CK_RV Test::stop()
{
    assert(state == TEST_STATE::Started);

    writeInformation("Prepare test termination...\n");

    terminationRequested = true;

    return CKR_OK;
}

CK_RV Test::terminate()
{
    assert((state == TEST_STATE::Prepared) ||
           (state == TEST_STATE::Initialized) ||
           (state == TEST_STATE::Stopped));

    CK_RV rv = CKR_OK;

    // Release used resources.
    rv = releaseUsedResources();

    if (rv != CKR_OK)
    {
        writeError("Cannot release used resources.",
                   rv);

        goto EXIT;
    }

    // Update the test state.
    state = TEST_STATE::Created;

EXIT:
    return rv;
}

CK_RV Test::waitForStop()
{
    assert(state == TEST_STATE::Started);

    CK_RV rv = CKR_OK;

    writeInformation("Wait for test termination...\n");

    // Wait for the thread to stop.
    int threadJoinResult = pthread_join(threadIdentifier,
                                        nullptr);

    if (threadJoinResult != 0)
    {
        rv = CKR_GENERAL_ERROR;

        writeError("Cannot wait for test termination.",
                   rv);

        goto EXIT;
    }

    endTime = std::chrono::high_resolution_clock::now();

    writeInformation("Test is completed.\n");

    // Update the test state.
    state = TEST_STATE::Stopped;

EXIT:
    return rv;
}

void Test::writeInformation(const char *const message) const
{
    if (scenario.scenarioContext.isVerbose)
    {
        Top::writeInformation(message);
    }
}