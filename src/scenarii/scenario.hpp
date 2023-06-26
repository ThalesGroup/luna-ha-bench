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

#ifndef SCENARIO_HPP
#define SCENARIO_HPP

#include <chrono>
#include <cstdlib>
#include <memory>
#include <vector>

#include "top.hpp"
#include "scenario-context.hpp"

typedef unsigned long SCENARIO_IDENTIFIER;
typedef int SCENARIO_CLASS;

#define SCENARIO_CLASS__COMP_128_AUTHENTICATION 0
#define SCENARIO_CLASS__MILENAGE_AUTHENTICATION 1
#define SCENARIO_CLASS__TUAK_AUTHENTICATION 2
#define SCENARIO_CLASS__SUCI_DECONCEALMENT 3

#define SCENARIO__ERROR_CODE__NO_ERROR 0
#define SCENARIO__ERROR_CODE__UNKNOWN_SCENARIO_CLASS -1
#define SCENARIO__ERROR_CODE__INCONSISTENT_SCENARIO_FLAGS -2
#define SCENARIO__ERROR_CODE__INVALID_TESTS_COUNT -3

class Test;

/*
 * A scenario must be instanciated before to be used. A scenario can be instanciated several times.
 *
 * Once a scenario has been instanciated, it must be prepared to set a context for the tests.
 *
 * Once a scenario has been prepared, it can be started to run several tests concurrently.
 *
 * A scenario must be stopped before to terminate it (to release the used resources).
 *
 * A terminated scenario can be reused.
 *
 * Metrics can be asked while the scenario is running or stopped.
 *
 * The state machine of a scenario is as follows:
 *
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
 *
 * Notes:
 *   - A separate 'Initialized' state is required to help for the
 *     synchronization of all the tests before to run.
 */
typedef unsigned long SCENARIO_FLAGS;

enum class SCENARIO_STATE
{
    Created = 1,
    Prepared = 2,
    Initialized = 3,
    Started = 4,
    Stopped = 5
};

/*
 * Notes:
 *   - A scenario is using its own session and object handles, that are not
 *     shared with its Test instances (each test must have its own session
 *     to run operations concurrently, knowing that only one operation can
 *     run in a session).
 */
class Scenario : public Top
{
protected:
    SCENARIO_STATE state = SCENARIO_STATE::Created;

    std::vector<std::shared_ptr<Test>> tests = {};

    std::chrono::high_resolution_clock::time_point beginTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point endTime = beginTime;

    unsigned long requestsCount = 0L;
    unsigned long errorsCount = 0L;

    unsigned long minTestTps = 0L;
    unsigned long maxTestTps = 0L;
    unsigned long meanTestTps = 0L;

    CK_SESSION_HANDLE sessionHandle = CK_INVALID_HANDLE;

    Scenario(const ScenarioContext &scenarioContext,
             const SCENARIO_FLAGS flags,
             const SCENARIO_IDENTIFIER identifier,
             const size_t testsCount,
             const unsigned long _requestsCountPerTest,
             const std::string title);

    virtual std::string generateObjectLabel(const char *const ownerLabel,
                                            const unsigned long ownerIdentifier,
                                            const unsigned long long ownerUuid,
                                            const char *const objectTypeLabel) const;
    virtual unsigned long long generateUuid() const;

    // Cleaning operations, as well as resources releases can
    // occur in any state.
    virtual CK_RV clean();
    virtual CK_RV releaseUsedResources();

    virtual CK_RV setScenarioData();

    virtual CK_RV prepareScenario();
    virtual CK_RV prepareTests();

public:
    static int getInstance(const SCENARIO_CLASS scenarioClass,
                           const ScenarioContext &scenarioContext,
                           const SCENARIO_FLAGS flags,
                           const SCENARIO_IDENTIFIER identifier,
                           const size_t testsCount,
                           const unsigned long requestsCountPerTest,
                           std::shared_ptr<Scenario> &pScenario);

    const ScenarioContext &scenarioContext;
    const SCENARIO_FLAGS flags;
    const SCENARIO_IDENTIFIER identifier;
    const unsigned long requestsCountPerTest;

    const bool isUsingTokenObjectsOnly;

    // A UUID is a random long value that is used to produce pseudo-unique
    // (unicity is likely but not guaranteed) object identifiers.
    //
    // UUIDs allow several instances to run concurrently without sharing
    // any data.
    const unsigned long long uuid;

    virtual SCENARIO_STATE getState() const;

    /*
     * Note: cannot use default destructor (cannot be inlined because it
     * is too large).
     */
    ~Scenario() override;

    Scenario(const Scenario &) = delete;
    Scenario &operator=(const Scenario &) = delete;

    virtual bool checkFlag(const unsigned int position,
                           const unsigned int value) const;
    virtual bool checkFlags(const SCENARIO_FLAGS flags) const;
    virtual void displayFlags() const;
    virtual const char *getFlagDescription(const unsigned int position) const;
    virtual unsigned int getFlagsCount() const;
    virtual unsigned int getFlagValue(const SCENARIO_FLAGS flags,
                                      const unsigned int position) const;
    virtual bool getFlagValueAsBoolean(const SCENARIO_FLAGS flags,
                                       const unsigned int position) const;

    virtual CK_RV prepare();
    virtual CK_RV initialize();
    virtual CK_RV start();
    virtual CK_RV stop();
    virtual CK_RV waitForStop();
    virtual CK_RV terminate();

    virtual CK_RV getNewMechanism(CK_MECHANISM *&pMechanism) const;

    virtual unsigned long getElapsedMicroSeconds() const;

    virtual unsigned long getRequestsCount() const;
    virtual unsigned long getErrorsCount() const;

    virtual unsigned long getMinTestTps() const;
    virtual unsigned long getMaxTestTps() const;
    virtual unsigned long getMeanTestTps() const;

    virtual unsigned long getTps() const;

    void writeDebugInformation() const override;
    void writeInformation(const char *const message) const override;
};

#endif /* SCENARIO_HPP */