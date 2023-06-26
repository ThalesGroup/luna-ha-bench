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
#include <memory>
#include <random>

#include "3gpp/authentication/comp-128/comp-128-scenario.hpp"
#include "3gpp/authentication/milenage/milenage-scenario.hpp"
#include "3gpp/authentication/tuak/tuak-scenario.hpp"
#include "3gpp/suci/suci-scenario.hpp"
#include "test.hpp"

Scenario::Scenario(const ScenarioContext &_scenarioContext,
                   const SCENARIO_FLAGS _flags,
                   const SCENARIO_IDENTIFIER _identifier,
                   const size_t testsCount,
                   const unsigned long _requestsCountPerTest,
                   const std::string title) : Top(std::string("Scenario ") +
                                                  std::to_string(_identifier) +
                                                  std::string(" (") +
                                                  title +
                                                  std::string("; flags='") +
                                                  std::to_string(_flags) +
                                                  std::string("'; tests=") +
                                                  std::to_string(testsCount) +
                                                  std::string(")")),
                                              scenarioContext(_scenarioContext),
                                              flags(_flags),
                                              identifier(_identifier),
                                              requestsCountPerTest(_requestsCountPerTest),
                                              isUsingTokenObjectsOnly(getFlagValueAsBoolean(_flags,
                                                                                            1)),
                                              uuid(generateUuid())
{
    assert(&_scenarioContext != nullptr);
    assert(testsCount > 0);

    // Nothing else to do here.
}

Scenario::~Scenario()
{
    CK_RV rv = CKR_OK;

    rv = releaseUsedResources();

    if (rv != CKR_OK)
    {
        writeError("Cannot destroy the scenario object properly.",
                   rv);
    }
}

bool Scenario::checkFlag(const unsigned int position,
                         const unsigned int value) const
{
    return (position > 0) &&
           ((value == 0) ||
            (value == 1));
}

bool Scenario::checkFlags(const SCENARIO_FLAGS _flags) const
{
    const unsigned int flagsCount = getFlagsCount();
    SCENARIO_FLAGS remainingFlags = _flags;
    unsigned int position = 0;

    do
    {
        position++;

        if (!checkFlag(position,
                       (unsigned int)(remainingFlags % 10)))
        {
            return false;
        }

        remainingFlags = remainingFlags / 10;
    } while (position < flagsCount);

    if (remainingFlags > 0)
    {
        return false;
    }

    return true;
}

CK_RV Scenario::clean()
{
    writeInformation("Clean the scenario...\n");

    // Nothing else to do here.

    return CKR_OK;
}

void Scenario::displayFlags() const
{
    const unsigned int flagsCount = getFlagsCount();

    if (flagsCount == 0)
    {
        fprintf(stdout,
                "  - No flags.\n");

        return;
    }

    for (unsigned int position = flagsCount;
         position > 0;
         position--)
    {
        std::string informationOnFlagValue = "no information";
        unsigned int flagValue = getFlagValue(flags,
                                              position);

        if ((flagValue == 0) ||
            (flagValue == 1))
        {
            informationOnFlagValue = getBooleanString(flagValue);
        }

        fprintf(stdout,
                "  - Flag %u (%32s) = %u (%s)\n",
                position,
                getFlagDescription(position),
                flagValue,
                informationOnFlagValue.c_str());
    }
}

std::string Scenario::generateObjectLabel(const char *const ownerLabel,
                                          const unsigned long ownerIdentifier,
                                          const unsigned long long ownerUuid,
                                          const char *const objectTypeLabel) const
{
    std::string label = HA_BENCH__TITLE;

    label.append(" - ");
    label.append(ownerLabel);
    label.append(" - ");
    label.append(objectTypeLabel);

    if (!scenarioContext.isSharingObjects)
    {
        label.append(" - ");
        label.append(std::to_string(ownerUuid));
        label.append(" - ");
        label.append(std::to_string(ownerIdentifier));
    }

    label.append(" - ");

    if (isUsingTokenObjectsOnly)
    {
        label.append("Token");
    }
    else
    {
        label.append("Session");
    }

    return label;
}

unsigned long long Scenario::generateUuid() const
{
    std::minstd_rand randomGenerator;

    randomGenerator.seed((unsigned int)(time(nullptr) & 0x0FFFF));

    return (((unsigned long long)randomGenerator() & 0x0FFFF) << 48) +
           (((unsigned long long)randomGenerator() & 0x0FFFF) << 32) +
           (((unsigned long long)randomGenerator() & 0x0FFFF) << 16) +
           ((unsigned long long)randomGenerator() & 0x0FFFF);
}

unsigned long Scenario::getElapsedMicroSeconds() const
{
    return (std::chrono::duration_cast<std::chrono::microseconds>(endTime - beginTime).count());
}

unsigned long Scenario::getErrorsCount() const
{
    return errorsCount;
}

const char *Scenario::getFlagDescription(const unsigned int position) const
{
    if (position == 1)
    {
        return "Use token objects";
    }

    return "Unknown flag";
}

unsigned int Scenario::getFlagValue(const SCENARIO_FLAGS _flags,
                                    const unsigned int position) const
{
    assert(position > 0);

    SCENARIO_FLAGS remainingFlags = _flags;
    unsigned int currentPosition = position;

    while (currentPosition > 1)
    {
        remainingFlags /= 10;
        currentPosition--;
    }

    return (unsigned int)(remainingFlags % 10);
}

bool Scenario::getFlagValueAsBoolean(const SCENARIO_FLAGS _flags,
                                     const unsigned int position) const
{
    assert(position > 0);

    return (getFlagValue(_flags,
                         position) == 1);
}

unsigned int Scenario::getFlagsCount() const
{
    return 1;
}

int Scenario::getInstance(const SCENARIO_CLASS scenarioClass,
                          const ScenarioContext &_scenarioContext,
                          const SCENARIO_FLAGS _flags,
                          const SCENARIO_IDENTIFIER _identifier,
                          const size_t testsCount,
                          const unsigned long _requestsCountPerTest,
                          std::shared_ptr<Scenario> &pScenario)
{
    assert(&_scenarioContext != nullptr);

    if (testsCount == 0)
    {
        return SCENARIO__ERROR_CODE__INVALID_TESTS_COUNT;
    }

    switch (scenarioClass)
    {
    case SCENARIO_CLASS__COMP_128_AUTHENTICATION:
        pScenario = std::make_shared<Comp128Scenario>(_scenarioContext,
                                                      _flags,
                                                      _identifier,
                                                      testsCount,
                                                      _requestsCountPerTest);

        break;

    case SCENARIO_CLASS__MILENAGE_AUTHENTICATION:
        pScenario = std::make_shared<MilenageScenario>(_scenarioContext,
                                                       _flags,
                                                       _identifier,
                                                       testsCount,
                                                       _requestsCountPerTest);

        break;

    case SCENARIO_CLASS__TUAK_AUTHENTICATION:
        pScenario = std::make_shared<TuakScenario>(_scenarioContext,
                                                   _flags,
                                                   _identifier,
                                                   testsCount,
                                                   _requestsCountPerTest);
        break;

    case SCENARIO_CLASS__SUCI_DECONCEALMENT:
        pScenario = std::make_shared<SuciScenario>(_scenarioContext,
                                                   _flags,
                                                   _identifier,
                                                   testsCount,
                                                   _requestsCountPerTest);

        break;

    default:
        return SCENARIO__ERROR_CODE__UNKNOWN_SCENARIO_CLASS;
    }

    if (!pScenario->checkFlags(_flags))
    {
        pScenario.reset();

        return SCENARIO__ERROR_CODE__INCONSISTENT_SCENARIO_FLAGS;
    }

    return SCENARIO__ERROR_CODE__NO_ERROR;
}

unsigned long Scenario::getMaxTestTps() const
{
    return maxTestTps;
}

unsigned long Scenario::getMeanTestTps() const
{
    return meanTestTps;
}

unsigned long Scenario::getMinTestTps() const
{
    return minTestTps;
}

CK_RV Scenario::getNewMechanism(CK_MECHANISM *&pMechanism) const
{
    assert(state == SCENARIO_STATE::Initialized);
    assert(&pMechanism != nullptr);

    return CKR_GENERAL_ERROR;
}

unsigned long Scenario::getRequestsCount() const
{
    return requestsCount;
}

SCENARIO_STATE Scenario::getState() const
{
    return state;
}

unsigned long Scenario::getTps() const
{
    auto elapsedMicroSeconds = getElapsedMicroSeconds();

    if (elapsedMicroSeconds == 0L)
    {
        return 0L;
    }

    assert(requestsCount >= errorsCount);

    return (unsigned long)((double)(requestsCount - errorsCount) / ((double)elapsedMicroSeconds / 1000000.));
}

CK_RV Scenario::initialize()
{
    assert(state == SCENARIO_STATE::Prepared);

    writeInformation("Initialize the scenario before to run tests...\n");

    // Update the scenario state.
    state = SCENARIO_STATE::Initialized;

    return CKR_OK;
}

CK_RV Scenario::prepare()
{
    assert(state == SCENARIO_STATE::Created);

    CK_RV rv = CKR_OK;

    if (!checkFlags(flags))
    {
        rv = CKR_GENERAL_ERROR;

        writeError("Invalid scenario flags.",
                   rv);

        goto EXIT;
    }

    rv = prepareScenario();

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    rv = prepareTests();

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    // Update the scenario state.
    state = SCENARIO_STATE::Prepared;

EXIT:
    return rv;
}

CK_RV Scenario::prepareScenario()
{
    assert(state == SCENARIO_STATE::Created);

    writeInformation("Prepare the scenario...\n");

    CK_RV rv = CKR_OK;

    // Get a session handle.
    rv = scenarioContext.openLoggedSession(sessionHandle);

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    // Reset the statistics.
    errorsCount = 0L;

    minTestTps = 0L;
    maxTestTps = 0L;
    meanTestTps = 0L;

    // Set the scenario data.
    rv = setScenarioData();

EXIT:
    return rv;
}

CK_RV Scenario::prepareTests()
{
    assert(state == SCENARIO_STATE::Created);

    CK_RV rv = CKR_OK;

    writeInformation("Prepare the tests...\n");

    for (auto &pTest : tests)
    {
        rv = pTest->prepare();

        if (rv != CKR_OK)
        {
            rv = CKR_GENERAL_ERROR;

            writeError("Cannot prepare a test in a separate thread.",
                       rv);

            goto EXIT;
        }
    }

    // Update the scenario state.
    state = SCENARIO_STATE::Prepared;

EXIT:
    return rv;
}

CK_RV Scenario::releaseUsedResources()
{
    CK_RV rv = CKR_OK;

    if (sessionHandle != CK_INVALID_HANDLE)
    {
        rv = scenarioContext.closeLoggedSession(sessionHandle);
    }

    return rv;
}

CK_RV Scenario::setScenarioData()
{
    return CKR_OK;
}

CK_RV Scenario::start()
{
    assert(state == SCENARIO_STATE::Initialized);

    writeInformation("Start the scenario...\n");

    if (scenarioContext.withDebug)
    {
        writeDebugInformation();
    }

    CK_RV rv = CKR_OK;

    // Initialize the tests.
    for (auto &pTest : tests)
    {
        rv = pTest->initialize();

        if (rv != CKR_OK)
        {
            goto EXIT;
        }
    }

    // Start the tests.
    beginTime = std::chrono::high_resolution_clock::now();

    for (auto &pTest : tests)
    {
        rv = pTest->start();

        if (rv != CKR_OK)
        {
            goto EXIT;
        }
    }

    // Update the scenario state.
    state = SCENARIO_STATE::Started;

EXIT:
    return rv;
}

CK_RV Scenario::stop()
{
    assert(state == SCENARIO_STATE::Started);

    writeInformation("Stop the scenario...\n");

    // Ask for the termination of the tests.
    for (auto &pTest : tests)
    {
        pTest->stop(); // Ignore the result code.
    }

    // Wait for the the tests to stop.
    return waitForStop();
}

CK_RV Scenario::terminate()
{
    assert((state == SCENARIO_STATE::Prepared) ||
           (state == SCENARIO_STATE::Initialized) ||
           (state == SCENARIO_STATE::Stopped));

    writeInformation("Terminate the scenario...\n");

    CK_RV rv = CKR_OK;

    // Clean the scenario.
    if (!scenarioContext.isSharingObjects)
    {
        rv = clean();

        if (rv != CKR_OK)
        {
            goto EXIT;
        }
    }

    // Terminate the tests.
    for (auto &pTest : tests)
    {
        if (pTest->getState() != TEST_STATE::Created)
        {
            rv = pTest->terminate();

            if (rv != CKR_OK)
            {
                writeError("Cannot terminate the tests.",
                           rv);

                goto EXIT;
            }
        }
    }

    // Release used resources.
    rv = releaseUsedResources();

    if (rv != CKR_OK)
    {
        writeError("Cannot release used resources.",
                   rv);

        goto EXIT;
    }

    // Update the scenario state.
    state = SCENARIO_STATE::Created;

EXIT:
    return rv;
}

CK_RV Scenario::waitForStop()
{
    assert(state == SCENARIO_STATE::Started);

    CK_RV rv = CKR_OK;

    for (auto &pTest : tests)
    {
        rv = pTest->waitForStop();

        if (rv != CKR_OK)
        {
            goto EXIT;
        }
    }

    endTime = std::chrono::high_resolution_clock::now();

    // Update the statistics.
    {
        for (auto &pTest : tests)
        {
            unsigned long tps = 0L;

            tps = pTest->getTransactionsPerSecond();

            if ((minTestTps == 0) ||
                (tps < minTestTps))
            {
                minTestTps = tps;
            }

            if (tps > maxTestTps)
            {
                maxTestTps = tps;
            }

            requestsCount += pTest->getRequestsCount();
            errorsCount += pTest->getErrorsCount();
            meanTestTps += tps;
        }

        meanTestTps /= tests.size();
    }

    // Update the scenario state.
    state = SCENARIO_STATE::Stopped;

EXIT:
    return rv;
}

void Scenario::writeDebugInformation() const
{
    Top::writeDebugInformation();

    fprintf(stdout,
            "  Session Handle: %ld\n",
            sessionHandle);

    fprintf(stdout,
            "\n");
}

void Scenario::writeInformation(const char *const message) const
{
    if (scenarioContext.isVerbose)
    {
        Top::writeInformation(message);
    }
}
