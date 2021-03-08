#!/usr/bin/env bash

SRC_DIR=$(cd "$(dirname "$0")"; pwd)
TEST_BIN_DIR=${SRC_DIR}/../bin
LOGS_DIR=${SRC_DIR}/../logs

for test in ${TEST_BIN_DIR}/*
do
    CK_LOG_FILE_NAME=${LOGS_DIR}/$(basename ${test}).log $test
done
