#!/usr/bin/env bash

SRC_DIR=$(cd "$(dirname "$0")"; pwd)
TEST_BIN_DIR=${SRC_DIR}/../bin
LOGS_DIR=${SRC_DIR}/../logs

for test in ${TEST_BIN_DIR}/*
do
    TEST_NAME=$(basename ${test})
    echo -e "\n#####################################################################"
    echo "# Execute tests in file '${TEST_NAME}'"
    echo "#####################################################################"

    LOG_FNAME=${LOGS_DIR}/${TEST_NAME}.log
    CK_LOG_FILE_NAME=${LOG_FNAME} ${test}

    cat ${LOG_FNAME}
done
