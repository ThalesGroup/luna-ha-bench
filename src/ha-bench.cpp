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
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <unistd.h>
#include <vector>

#include "scenarii/scenario.hpp"

extern "C"
{
#include <toolkits/p11-toolkit.h>
}

//
// Toolkit.
//
bool isNumber(const std::string &value);

bool isNumber(const std::string &value)
{
    try
    {
        stoi(value);
    }
    catch (const std::invalid_argument &e)
    {
        return false;
    }
    catch (const std::out_of_range &e)
    {
        return false;
    }

    return true;
}

//
// Main.
//
int main(const int argc,
         const char *const *const argv)
{
    CK_RV rv = CKR_OK;

    try
    {
        //
        // Check command line arguments.
        //
        bool isVerbose = false;
        CK_SLOT_ID slotId = -1;
        CK_CHAR *coPassword = nullptr;
        CK_ULONG coPasswordLength = 0;
        bool isTimeLimited = true;
        unsigned int testsDuration = 0;
        unsigned long requestsCountPerTest = 0L;
        bool isSharingObjects = true;

        int argi = 1;

        if (argc < 7)
        {
            fprintf(stdout,
                    "%s <slot-id>\n\
           <co-password>\n\
           <measure-type>\n\
           <measure-objective>\n\
           <share>\n\
           {<scenario>x<flags>x<tests-count>}+\n\
\n\
Arguments:\n\
  slot-id          : slot identifier to use.\n\
  co-password      : password of the Crypto Officer.\n\
  measure-type     : 'time-limited' or 'request-limited'.\n\
  measure-objective: if 'time-limited':\n\
                       <tests-duration> (seconds; 0<.<=3600)\n\
                     If 'request-limited':\n\
                       <requests-count-per-test>\n\
  share            : allow each scenario class running in this process to\n\
                     share its objects with other instances of the same\n\
                     scenario class running on the same host or elsewhere.\n\
                     Can be:\n\
                       'share'   : all the instances of a scenario class are\n\
                                   sharing the objects handled by the scenario\n\
                                   class.\n\
                       'no-share': specific objects are created for each\n\
                                   instance of scenario class (within the\n\
                                   process, on the same host or elsewhere).\n\
                     Note:\n\
                       -  Objects are created during the preparation phase if\n\
                          needed; they are retrieved just before to run the\n\
                          test.\n\
  scenario         : scenario (epic).\n\
                     Can be:\n\
                       'comp-128'\n\
                       'milenage'\n\
                       'TUAK'\n\
                       'SUCI'\n\
  flags            : scenario flags/parameters (story).\n\
                     For COMP-128 authentication\n\
                       yz:\n\
                         y=\n\
                           1: version 1.\n\
                           2: version 2.\n\
                           3: version 3.\n\
                         z=\n\
                           1: use token objects only.\n\
                           0: use session objects only.\n\
                     For Milenage authentication\n\
                       vwxyz:\n\
                         v=\n\
                           1: use default RC values.\n\
                           0: use specific RC values.\n\
                         w=\n\
                           1: (e)OP or (e)OPc is pre-loaded in the HSM.\n\
                           0: (e)OP or (e)OPc is provided with the request.\n\
                         x=\n\
                           1: OP or OPc is encrypted.\n\
                           0: OP or OPc is in clear text.\n\
                         y=\n\
                           1: OP is used.\n\
                           0: OPc is used.\n\
                         z=\n\
                           1: use token objects only.\n\
                           0: use session objects only.\n\
                     For TUAK authentication\n\
                       wxyz:\n\
                         w=\n\
                           1: (e)OP or (e)OPc is pre-loaded in the HSM.\n\
                           0: (e)OP or (e)OPc is provided with the request.\n\
                         x=\n\
                           1: OP or OPc is encrypted.\n\
                           0: OP or OPc is in clear text.\n\
                         y=\n\
                           1: OP is used.\n\
                           0: OPc is used.\n\
                         z=\n\
                           1: use token objects only.\n\
                           0: use session objects only.\n\
                     For SUPI deconcealment\n\
                       xyz:\n\
                         x= (for profile A only)\n\
                           1: curve points are compressed.\n\
                           0: curve points are not compressed.\n\
                         y=\n\
                           1: profile B is used.\n\
                           0: profile A is used.\n\
                         z=\n\
                           1: use token objects only.\n\
                           0: use session objects only.\n\
  tests-count       : number of tests/threads to run in parallel.\n",
                    argv[0]);

            rv = CKR_GENERAL_ERROR;

            goto EXIT;
        }

        slotId = (CK_SLOT_ID)atoi(argv[argi++]);
        coPassword = (CK_CHAR *)argv[argi++];
        coPasswordLength = (CK_ULONG)strlen((char *)coPassword);

        if (coPasswordLength == 0)
        {
            fprintf(stderr,
                    "Invalid password length: '%ld'.\n",
                    coPasswordLength);

            rv = CKR_GENERAL_ERROR;

            goto EXIT;
        }

        if (strcasecmp(argv[argi],
                       "time-limited") == 0)
        {
            isTimeLimited = true;
        }
        else if (strcasecmp(argv[argi],
                            "request-limited") == 0)
        {
            isTimeLimited = false;
        }
        else
        {
            fprintf(stderr,
                    "Invalid mesure type: '%s'.\n",
                    argv[argi]);

            rv = CKR_GENERAL_ERROR;

            goto EXIT;
        }

        argi++;

        if (isTimeLimited)
        {
            testsDuration = atoi(argv[argi]);

            if ((testsDuration <= 0) ||
                (testsDuration > 3600))
            {
                fprintf(stderr,
                        "Invalid tests duration: '%d'.\n",
                        testsDuration);

                rv = CKR_GENERAL_ERROR;

                goto EXIT;
            }
        }
        else
        {
            requestsCountPerTest = atol(argv[argi]);

            if (requestsCountPerTest <= 0)
            {
                fprintf(stderr,
                        "Invalid requests count per test: '%ld'.\n",
                        requestsCountPerTest);

                rv = CKR_GENERAL_ERROR;

                goto EXIT;
            }
        }

        argi++;

        if (strcasecmp(argv[argi],
                       "share") == 0)
        {
            isSharingObjects = true;
        }
        else if (strcasecmp(argv[argi],
                            "no-share") == 0)
        {
            isSharingObjects = false;
        }
        else
        {
            fprintf(stderr,
                    "Invalid argument: '%s'.\n",
                    argv[argi]);

            rv = CKR_GENERAL_ERROR;

            goto EXIT;
        }

        argi++;

        ScenarioContext scenarioContext = ScenarioContext(slotId,
                                                          coPassword,
                                                          coPasswordLength,
                                                          isSharingObjects,
                                                          isVerbose);
        std::vector<std::shared_ptr<Scenario>> scenarii = {};
        SCENARIO_IDENTIFIER scenarioIdentifier = 0;

        while (argi < argc)
        {
            const char *const scenarioDefinitionItemSeparator = "x";
            const char *scenarioDefinitionFirstItem = nullptr;
            const char *scenarioDefinitionSecondItem = nullptr;
            const char *scenarioDefinitionThirdItem = nullptr;
            SCENARIO_CLASS scenarioClass = -1;
            SCENARIO_FLAGS scenarioFlags = 0;
            size_t scenarioTestsCount = -1;
            char *tokenizerContext = nullptr;

            scenarioDefinitionFirstItem = strtok_r((char *)argv[argi],
                                                   scenarioDefinitionItemSeparator,
                                                   &tokenizerContext);

            if (scenarioDefinitionFirstItem == nullptr)
            {
                goto INVALID_SCENARIO_DESCRIPTION;
            }

            scenarioDefinitionSecondItem = strtok_r(nullptr,
                                                    scenarioDefinitionItemSeparator,
                                                    &tokenizerContext);

            if ((scenarioDefinitionSecondItem == nullptr) ||
                !isNumber(scenarioDefinitionSecondItem))
            {
                goto INVALID_SCENARIO_DESCRIPTION;
            }

            scenarioDefinitionThirdItem = strtok_r(nullptr,
                                                   scenarioDefinitionItemSeparator,
                                                   &tokenizerContext);

            if ((scenarioDefinitionThirdItem == nullptr) ||
                !isNumber(scenarioDefinitionThirdItem) ||
                (strtok_r(nullptr,
                          scenarioDefinitionItemSeparator,
                          &tokenizerContext) != nullptr))
            {
                goto INVALID_SCENARIO_DESCRIPTION;
            }

            if (strcasecmp(scenarioDefinitionFirstItem,
                           "comp-128") == 0)
            {
                scenarioClass = SCENARIO_CLASS__COMP_128_AUTHENTICATION;
            }
            else if (strcasecmp(scenarioDefinitionFirstItem,
                                "milenage") == 0)
            {
                scenarioClass = SCENARIO_CLASS__MILENAGE_AUTHENTICATION;
            }
            else if (strcasecmp(scenarioDefinitionFirstItem,
                                "TUAK") == 0)
            {
                scenarioClass = SCENARIO_CLASS__TUAK_AUTHENTICATION;
            }
            else if (strcasecmp(scenarioDefinitionFirstItem,
                                "SUCI") == 0)
            {
                scenarioClass = SCENARIO_CLASS__SUCI_DECONCEALMENT;
            }
            else
            {
                fprintf(stderr,
                        "Invalid scenario: '%s'.\n",
                        scenarioDefinitionFirstItem);

                rv = CKR_GENERAL_ERROR;

                goto EXIT;
            }

            scenarioFlags = (SCENARIO_FLAGS)atoi(scenarioDefinitionSecondItem);
            scenarioTestsCount = (size_t)atol(scenarioDefinitionThirdItem);

            std::shared_ptr<Scenario> pScenario = nullptr;
            const int errorCode = Scenario::getInstance(scenarioClass,
                                                        scenarioContext,
                                                        scenarioFlags,
                                                        scenarioIdentifier,
                                                        scenarioTestsCount,
                                                        requestsCountPerTest,
                                                        pScenario);

            switch (errorCode)
            {
            case SCENARIO__ERROR_CODE__NO_ERROR:
                break;

            case SCENARIO__ERROR_CODE__UNKNOWN_SCENARIO_CLASS:
                fprintf(stderr,
                        "Argument %d: unknown scenario identifier.\n",
                        argi);

                break;

            case SCENARIO__ERROR_CODE__INCONSISTENT_SCENARIO_FLAGS:
                fprintf(stderr,
                        "Argument %d: invalid scenario flags.\n",
                        argi);

                break;

            case SCENARIO__ERROR_CODE__INVALID_TESTS_COUNT:
                fprintf(stderr,
                        "Argument %d: invalid tests count.\n",
                        argi);

                break;

            default:
                fprintf(stderr,
                        "Argument %d: unknown error ('%d').\n",
                        argi,
                        errorCode);

                break;
            }

            if (errorCode != SCENARIO__ERROR_CODE__NO_ERROR)
            {
                rv = CKR_GENERAL_ERROR;

                goto EXIT;
            }

            scenarii.push_back(pScenario);
            scenarioIdentifier++;

            argi++;
        }

        goto RUN_TESTS;

    INVALID_SCENARIO_DESCRIPTION:
        writeError("Invalid scenario description.\n");

        rv = CKR_GENERAL_ERROR;

        goto EXIT;

    RUN_TESTS:
        //
        // Prepare for execution.
        //
        writeMessage("Initialize the PKCS#11 client library and check the CO password...\n");

        CK_SESSION_HANDLE sessionHandle = CK_INVALID_HANDLE;

        rv = p11tk_prepare(nullptr,
                           slotId,
                           coPassword,
                           coPasswordLength,
                           &sessionHandle);

        if (rv != CKR_OK)
        {
            fprintf(stderr,
                    "Cannot open a PKCS#11 connexion. ['0x%08lx']\n",
                    rv);

            goto EXIT;
        }

        //
        // Run the scenarii.
        //
        writeTitle("Prepare the scenarii");

        for (const std::shared_ptr<Scenario> &pScenario : scenarii)
        {
            fprintf(stdout,
                    "%s:\n",
                    pScenario->getUniqueString().c_str());

            pScenario->displayFlags();

            rv = pScenario->prepare();

            if (rv != CKR_OK)
            {
                goto TERMINATE;
            }

            scenarioIdentifier++;
        }

        writeTitle("Initialize the scenarii");

        for (const std::shared_ptr<Scenario> &pScenario : scenarii)
        {
            rv = pScenario->initialize();

            if (rv != CKR_OK)
            {
                goto TERMINATE;
            }
        }

        writeTitle("Start the scenarii");

        printCurrentTime("Begin time: ");

        for (const std::shared_ptr<Scenario> &pScenario : scenarii)
        {
            rv = pScenario->start();

            if (rv != CKR_OK)
            {
                goto TERMINATE;
            }
        }

        if (isTimeLimited)
        {
            writeMessage("Wait for the end of the test period...\n");

            sleep(testsDuration);

            writeMessage("");
            writeMessage("End of test the period is reached.\n");
        }
        else
        {
            for (const std::shared_ptr<Scenario> &pScenario : scenarii)
            {
                rv = pScenario->waitForStop();

                if (rv != CKR_OK)
                {
                    goto TERMINATE;
                }
            }
        }

        printCurrentTime("End time: ");

        if (isTimeLimited)
        {
            writeTitle("Stop the scenarii");

            for (const std::shared_ptr<Scenario> &pScenario : scenarii)
            {
                rv = pScenario->stop();

                if (rv != CKR_OK)
                {
                    goto TERMINATE;
                }
            }
        }

        {
            writeTitle("Report");

            double totalDuration = 0.0;
            unsigned long totalRequestsCount = 0L;
            unsigned long totalErrorsCount = 0L;

            writeMessage("Per scenario:\n");

            for (const std::shared_ptr<Scenario> &pScenario : scenarii)
            {
                const double duration = (((double)pScenario->getElapsedMicroSeconds()) / 1000000.);
                const unsigned long requestsCount = pScenario->getRequestsCount();
                const unsigned long errorsCount = pScenario->getErrorsCount();

                fprintf(stdout,
                        "  %s:\n",
                        pScenario->getUniqueString().c_str());

                fprintf(stdout,
                        "    Duration              = %.3f seconds\n",
                        duration);
                fprintf(stdout,
                        "    Requests Count        = %ld\n",
                        requestsCount);
                fprintf(stdout,
                        "    Errors   Count        = %ld\n",
                        errorsCount);
                fprintf(stdout,
                        "    TpS                   = %ld\n",
                        pScenario->getTps());
                fprintf(stdout,
                        "    Mininum  TpS per Test = %ld\n",
                        pScenario->getMinTestTps());
                fprintf(stdout,
                        "    Maxinum  Tps per Test = %ld\n",
                        pScenario->getMaxTestTps());
                fprintf(stdout,
                        "    Mean     TpS per Test = %ld\n",
                        pScenario->getMeanTestTps());

                if (totalDuration < duration)
                {
                    totalDuration = duration;
                }

                totalRequestsCount += requestsCount;
                totalErrorsCount += errorsCount;
            }

            writeMessage("Globally:\n");

            fprintf(stdout,
                    "  Total   Duration       = %.2f seconds\n",
                    totalDuration);
            fprintf(stdout,
                    "  Total   Requests Count = %ld\n",
                    totalRequestsCount);
            fprintf(stdout,
                    "  Total   Errors   Count = %ld\n",
                    totalErrorsCount);
            fprintf(stdout,
                    "  Overall TpS            = %ld\n",
                    (unsigned long)((double)(totalRequestsCount - totalErrorsCount) / totalDuration));
        }

    TERMINATE:
        writeTitle("Terminate");

        for (const std::shared_ptr<Scenario> &pScenario : scenarii)
        {
            if (pScenario->getState() != SCENARIO_STATE::Created)
            {
                CK_RV rv2 = CKR_OK;

                rv2 = pScenario->terminate();

                if (rv2 != CKR_OK)
                {
                    fprintf(stderr,
                            "Cannot termine a scenario properly. ['0x%08lx']\n",
                            rv2);

                    rv = CKR_GENERAL_ERROR;
                }
            }
        }

        writeMessage("Close the client library...\n");

        rv = p11tk_terminate(sessionHandle);

        if (rv != CKR_OK)
        {
            fprintf(stderr,
                    "Cannot close the client library properly. ['0x%08lx']\n",
                    rv);
        }
    }
    catch (const std::exception &e)
    {
        rv = CKR_GENERAL_ERROR;

        fprintf(stderr,
                "An exception occured: '%s'.\n",
                e.what());
    }

EXIT:
    return (int)rv;
}
