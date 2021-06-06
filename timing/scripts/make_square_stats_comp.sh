#!/usr/bin/env bash

SCRIPTS_DIR=$(cd $(dirname $0) && pwd)
TIMING_BASE_DIR=$(dirname ${SCRIPTS_DIR})

COMP_NAME=comp_square_stats

DEFAULT_VERSION=3
if [[ -z $VERSION ]]; then
    echo "WARNING: using default value for VERSION (V${VERSION})"
fi
VERSION="${VERSION:-$DEFAULT_VERSION}"

SDIR=${TIMING_BASE_DIR}/src
IDIR=${TIMING_BASE_DIR}/include
LDIR=${TIMING_BASE_DIR}/logs/${COMP_NAME}
PDIR=${TIMING_BASE_DIR}/plots/local/${COMP_NAME}
GEN_SCRIPT=${TIMING_BASE_DIR}/../scripts/gen_types.py
LATEST_LOG_PATH=${TIMING_BASE_DIR}/logs/latest_log_path.txt

if [ "${VERSION}" = "1" ]; then
    BENCHED_ROOT_DIR=../code_unoptimized
elif [ "${VERSION}" = "2" ]; then
	BENCHED_ROOT_DIR=../code_optimized/standard_optimizations
else
	BENCHED_ROOT_DIR=../code_optimized/avx_optimizations
fi
SRC_BENCHED=${BENCHED_ROOT_DIR}/src
IDIR_BENCHED=${BENCHED_ROOT_DIR}/include

BENCH_TYPES=\
"BENCH_TYPE_ELLIGATOR1_STR2PNT\
 BENCH_TYPE_ELLIGATOR1_PNT2STR"

echo "#####################################################################"
echo "#     Generate comparison plot for square with different sizes      #"
echo "#####################################################################"

echo -e "\t- Get benchmark integers"
BENCH_TYPES_INT=$(\
    ${GEN_SCRIPT} \
    --src_files \
        ${SDIR}/runtime_benchmark_curve1174.c \
        ${SDIR}/runtime_benchmark.c \
    --dest_file \
        ${IDIR}/benchmark_types.h \
    --strip_prefix \
        "BIGINT_TYPE_" \
    --lookup_names ${BENCH_TYPES})

TIMESTAMP_DIR="$(date "+%Y-%m-%d_%H-%M-%S")"
LOG_SUBDIR=${LDIR}/${TIMESTAMP_DIR}
PLOTS_SUBDIR=${PDIR}/${TIMESTAMP_DIR}

echo -e "\t- Prepare log folders"
mkdir -p ${LOG_SUBDIR}
mkdir -p ${PLOTS_SUBDIR}

echo -e "\t- Create benchmark logs"
COMP_LOG=${LOG_SUBDIR}/${COMP_NAME}.log
touch ${COMP_LOG}

echo -e "\t\t- run-runtime-benchmark for V${VERSION}"
COLLECT_STATS=1 \
BENCHMARKS="${BENCH_TYPES_INT}" \
    VERSION=${VERSION} \
    make \
    run-runtime-benchmark >> ${COMP_LOG}

# Get log path
LOG_DIR=$(cat "${LATEST_LOG_PATH}")

LOG_SUBDIR="${LOG_DIR}"
LOGS_NAMES="V${VERSION}"

echo -e "\t- Create benchmark plots"

${SCRIPTS_DIR}/gen_stats_plots.py                                              \
    --title "Stats Comparison for Square with Different Argument Sizes"        \
    --plot_dir "${PLOTS_SUBDIR}"                                               \
    --logs_dir "${LOG_SUBDIR}"                                                 \
    --logs_names "${LOGS_NAMES}"                                               \
    --bar_plot                                                                 \
    --pattern "big_int_squared_\d+"
