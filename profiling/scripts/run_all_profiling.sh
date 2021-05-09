#!/usr/bin/env bash

ROOT_DIR=$(cd $(dirname $0)/../.. && pwd)
PROFILING_DIR="${ROOT_DIR}/profiling"
TRACES_DIR="${PROFILING_DIR}/traces"
TIMING_DIR="${ROOT_DIR}/timing"
TIMING_BIN_DIR="${TIMING_DIR}/bin"

DEFAULT_SETS=10
DEFAULT_REPS=1000

SETS="${SETS:-$DEFAULT_SETS}"
REPS="${REPS:-$DEFAULT_REPS}"

cd ${TIMING_DIR}

make clean && SETS=${SETS} REPS=${REPS} make all

declare -A traces=(
    ["add"]=12
    ["sub"]=17
    ["divrem"]=23
    ["chi"]=38
)

for trace_name in "${!traces[@]}"; do
    echo -e "\n\n###################################################################";
    echo "Start tracing ${trace_name} (number: ${traces[$trace_name]})";
    echo "###################################################################";

    trace_out_file="${TRACES_DIR}/${trace_name}.trace"
    rm -rf ${trace_out_file}

    xcrun xctrace record \
        --template "Time Profiler" \
        --output ${trace_out_file} \
        --launch -- ${TIMING_BIN_DIR}/runtime-benchmark ${traces[$trace_name]};
done
