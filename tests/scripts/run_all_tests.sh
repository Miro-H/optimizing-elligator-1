#!/usr/bin/env bash

SRC_DIR=$(cd "$(dirname "$0")"; pwd)
echo $SRC_DIR
TEST_BIN_DIR=${SRC_DIR}/../bin

for test in ${TEST_BIN_DIR}/*
do
    $test
done
