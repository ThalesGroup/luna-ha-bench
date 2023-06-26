/****************************************************************************\
*
* This file is provided under the MIT license (see the following Web site
* for further details: https://mit-license.org/ ).
*
* Copyright © 2023 Thales Group
*
\****************************************************************************/

#ifndef __MISC_TOOLKIT_H__
#define __MISC_TOOLKIT_H__

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define COPY_ARRAY_SAFELY(source, destination) copyArraySafely((const char *const)source, sizeof(source), (char *const)destination, sizeof(destination))

#define GET_STRUCT_FIELD_SIZE(type, member) sizeof(((type *)0)->member)

#define GET_ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define CLEAR_ARRAY(array) memset(array, 0, GET_ARRAY_SIZE(array))
#define CLEAR_BUFFER(buffer, size) memset(buffer, 0, size)

void copyArraySafely(const char *const source,
                     const size_t sourceLength,
                     char *const destination,
                     const size_t destinationLength);

const char *getBooleanString(const int value);

unsigned long getChecksum(const char *const buffer,
                          const size_t size);

time_t getCurrentTime(void);

unsigned long long getElapsedTime(const struct timeval begin,
                                  const struct timeval end);

void *mallocAndReset(const size_t size);

void printCurrentTime(const char *const title);

void writeBinaryData(const char *const title,
                     const unsigned char *const binaryData,
                     const size_t binaryDataLength);

void writeError(const char *const message);

void writeMessage(const char *const message);

void writeTitle(const char *const title);

#endif /* __MISC_TOOLKIT_H__ */