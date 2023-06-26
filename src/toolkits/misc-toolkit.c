/****************************************************************************\
*
* This file is provided under the MIT license (see the following Web site
* for further details: https://mit-license.org/ ).
*
* Copyright © 2023 Thales Group
*
\****************************************************************************/

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "misc-toolkit.h"

void copyArraySafely(const char *const source,
                     const size_t sourceLength,
                     char *const destination,
                     const size_t destinationLength)
{
    assert(source != NULL);
    assert(sourceLength > 0);
    assert(destination != NULL);
    assert(destinationLength > 0);
    assert(destinationLength >= sourceLength);
    assert((destination > source + sourceLength) ||
           (source > destination + destinationLength));

    memcpy(destination,
           source,
           sourceLength);
}

const char *getBooleanString(const int value)
{
    if (value)
    {
        return "True";
    }

    return "False";
}

unsigned long getChecksum(const char *const buffer,
                          const size_t size)
{
    assert(buffer != NULL);

    unsigned long long checksum = 0LL;
    size_t index = size;

    while (index > 0L)
    {
        assert(checksum < 0x10000000000);

        checksum += (unsigned long)buffer[index] & 0x0FF;
        index--;
    }

    return (unsigned long)(checksum & 0x0FFFFFFFF);
}

time_t getCurrentTime()
{
    return time(NULL);
}

// In micro-seconds.
unsigned long long getElapsedTime(const struct timeval begin,
                                  const struct timeval end)
{
    assert((end.tv_sec >= begin.tv_sec) &&
           (end.tv_usec >= begin.tv_usec));

    const unsigned long elapsedMicroSeconds = (unsigned long)(end.tv_usec - begin.tv_usec);

    if (elapsedMicroSeconds > 0)
    {
        const unsigned long elapsedSeconds = (unsigned long)(end.tv_sec - begin.tv_sec);

        return elapsedSeconds * 1000000L + elapsedMicroSeconds;
    }
    else
    {
        return elapsedMicroSeconds;
    }
}

void *mallocAndReset(const size_t size)
{
    void *result = malloc(size);

    if (result == NULL)
    {
        fprintf(stderr,
                "Cannot allocate memory.");
    }
    else
    {
        CLEAR_BUFFER(result,
                     size);
    }

    return result;
}

void printCurrentTime(const char *const title)
{
    assert(title != NULL);

    const time_t currentTime = getCurrentTime();

    char const currentTimeBuffer[26];

    ctime_r(&currentTime,
            (char *)&currentTimeBuffer);

    fprintf(stdout,
            "%s(%ld) %s",
            title,
            currentTime,
            currentTimeBuffer);
}

void writeBinaryData(const char *const title,
                     const unsigned char *const binaryData,
                     const size_t binaryDataLength)
{
    assert(title != NULL);

    fprintf(stdout,
            "%s",
            title);

    if (binaryData == NULL)
    {
        fprintf(stdout,
                "NULL\n");

        return;
    }

    if (binaryDataLength == 0)
    {
        fprintf(stdout,
                "[]\n");

        return;
    }

    for (size_t i = 0;
         i < binaryDataLength;
         ++i)
    {
        fprintf(stdout,
                "%02X ",
                binaryData[i]);
    }

    fprintf(stdout,
            "\n");
}

void writeError(const char *const message)
{
    fprintf(stderr,
            "%s",
            message);
}

void writeMessage(const char *const message)
{
    fprintf(stdout,
            "%s",
            message);
}

void writeTitle(const char *const title)
{
    fprintf(stdout,
            "\n");
    fprintf(stdout,
            "*******************************************************\n");
    fprintf(stdout,
            "* %s\n",
            title);
    fprintf(stdout,
            "*******************************************************\n");
}