#!/bin/bash

# #############################################################################
#
# This file is part of the "Luna HA-Bench" tool.
#
# The "Luna HA-Bench" tool is provided under the MIT license (see the
# following Web site for further details: https://mit-license.org/ ).
#
# Copyright © 2023 Thales Group
#
# #############################################################################
#
# Run a set of basic performance tests on a HA group using HA-Bench.
#
# Assumptions:
#   - See usage.
#
# Caveat:
#   - See usage.
# #############################################################################

# #############################################################################
# Execution context
# #############################################################################

# Print each command that is executed (prefixed with ++).
# set -x

# Exit as soon as any line in the bash script fails.
set -e

# #############################################################################
# Configuration
# #############################################################################
# Notes:
#  - The tests are using an existing HA group, addressed through slot 0.
#  - The partition is cleared before running the test.

TEST_DURATION=5
RESULT_FILE=result.txt

# #############################################################################
# Main
# #############################################################################

# Parse the arguments.
if [ $# -ne 1 ]; then
    echo "Run a basic and quick test of HA-Bench."
    echo ""
    echo "Usage:"
    echo "  $(basename "${0}") <co-password>"
    exit 1
fi

CO_PASSWORD=${1}

# Run the tests.
>"${RESULT_FILE}"

for SHARE_MODE in share no-share; do
    for USE_CASE in \
        "COMP-128x10x10" \
        "COMP-128x11x10" \
        "COMP-128x20x10" \
        "COMP-128x21x10" \
        "COMP-128x30x10" \
        "COMP-128x31x10" \
        "Milenagex00000x10" \
        "Milenagex00001x10" \
        "Milenagex00010x10" \
        "Milenagex00011x10" \
        "Milenagex00100x10" \
        "Milenagex00101x10" \
        "Milenagex00110x10" \
        "Milenagex00111x10" \
        "Milenagex01000x10" \
        "Milenagex01001x10" \
        "Milenagex01010x10" \
        "Milenagex01011x10" \
        "Milenagex10000x10" \
        "Milenagex10001x10" \
        "Milenagex10010x10" \
        "Milenagex10011x10" \
        "Milenagex10100x10" \
        "Milenagex10101x10" \
        "Milenagex10110x10" \
        "Milenagex10111x10" \
        "Milenagex11000x10" \
        "Milenagex11001x10" \
        "Milenagex11010x10" \
        "Milenagex11011x10" \
        "TUAKx0000x10" \
        "TUAKx0001x10" \
        "TUAKx0010x10" \
        "TUAKx0011x10" \
        "TUAKx0100x10" \
        "TUAKx0101x10" \
        "TUAKx0110x10" \
        "TUAKx0111x10" \
        "TUAKx1000x10" \
        "TUAKx1001x10" \
        "TUAKx1010x10" \
        "TUAKx1011x10" \
        "SUCIx000x10" \
        "SUCIx001x10" \
        "SUCIx010x10" \
        "SUCIx011x10" \
        "SUCIx100x10" \
        "SUCIx101x10" \
        "COMP-128x30x10 Milenagex00010x10 TUAKx0010x10 SUCIx000x10"; do
        echo "# ############################################################################" >>"${RESULT_FILE}"
        echo "# Test '${USE_CASE} (${SHARE_MODE})'..." >>"${RESULT_FILE}"
        echo "# ############################################################################" >>"${RESULT_FILE}"
        echo "" >>"${RESULT_FILE}"

        ./ha-bench 0 "${CO_PASSWORD}" time-limited "${TEST_DURATION}" "${SHARE_MODE}" "${USE_CASE}" &>>"${RESULT_FILE}"

        echo "" >>"${RESULT_FILE}"
    done
done
