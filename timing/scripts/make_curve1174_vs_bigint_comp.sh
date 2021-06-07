#!/usr/bin/env bash

SCRIPTS_DIR=$(cd $(dirname $0) && pwd)
TIMING_BASE_DIR=$(dirname ${SCRIPTS_DIR})

DEFAULT_VERSION=2
DEFAULT_SETS=10
DEFAULT_REPS=10000

if [[ -z $SETS || -z $REPS ]]; then
    echo "WARNING: using default value for SETS (${DEFAULT_SETS}) and/or "\
         "REPS (${DEFAULT_REPS})"
fi
SETS="${SETS:-$DEFAULT_SETS}"
REPS="${REPS:-$DEFAULT_REPS}"

if [[ -z $VERSION ]]; then
    echo "WARNING: using default value for VERSION (V${DEFAULT_VERSION})"
fi
VERSION="${VERSION:-$DEFAULT_VERSION}"

COMP_NAME=comp_V${VERSION}_curve1174_vs_V1_bigint

SDIR=${TIMING_BASE_DIR}/src
IDIR=${TIMING_BASE_DIR}/include
LDIR=${TIMING_BASE_DIR}/logs/${COMP_NAME}
PDIR=${TIMING_BASE_DIR}/plots/local/${COMP_NAME}
GEN_SCRIPT=${TIMING_BASE_DIR}/../scripts/gen_types.py
LATEST_LOG_PATH=${TIMING_BASE_DIR}/logs/latest_log_path.txt

BENCH_TYPES=\
"BENCH_TYPE_CURVE_1174_ADD_MOD\
 BENCH_TYPE_CURVE_1174_SUB_MOD\
 BENCH_TYPE_CURVE_1174_MUL_MOD\
 BENCH_TYPE_CURVE_1174_SQUARE_MOD\
 BENCH_TYPE_CURVE_1174_MOD\
 BENCH_TYPE_CURVE_1174_MOD_512\
 BENCH_TYPE_CURVE_1174_DIV_MOD\
 BENCH_TYPE_CURVE_1174_INV_FERMAT\
 BENCH_TYPE_CURVE_1174_COMPARE\
 BENCH_TYPE_CURVE_1174_POW\
 BENCH_TYPE_CURVE_1174_POW_SMALL\
 BENCH_TYPE_CURVE_1174_POW_Q_M1_D2\
 BENCH_TYPE_CURVE_1174_POW_Q_P1_D4\
 BENCH_TYPE_CURVE_1174_CHI\
 BENCH_TYPE_ADD_MOD_CURVE\
 BENCH_TYPE_SUB_MOD_CURVE\
 BENCH_TYPE_MUL_MOD_CURVE\
 BENCH_TYPE_MUL_SQUARE_MOD_CURVE\
 BENCH_TYPE_DIV_MOD_CURVE\
 BENCH_TYPE_MOD_CURVE\
 BENCH_TYPE_MOD_512_CURVE\
 BENCH_TYPE_INV\
 BENCH_TYPE_POW_CURVE\
 BENCH_TYPE_POW_SMALL_CURVE\
 BENCH_TYPE_POW_Q_M1_D2_CURVE\
 BENCH_TYPE_POW_Q_P1_D4_CURVE\
 BENCH_TYPE_COMPARE_TO_Q\
 BENCH_TYPE_CHI"

echo "#####################################################################"
echo "#      Generate comparison plots for V${VERSION} Curve1174 vs V1 BigInt      #"
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

echo -e "\t\t- run-runtime-benchmark"
BENCHMARKS="${BENCH_TYPES_INT}" \
    VERSION=1 \
    SETS=${SETS} \
    REPS=${REPS} \
    make \
    run-runtime-benchmark >> ${COMP_LOG}

# Get log path
BIGINT_LOG_DIR=$(cat "${LATEST_LOG_PATH}")

echo -e "\t\t- run-runtime-benchmark-curve1174"
BENCHMARKS="${BENCH_TYPES_INT}" \
    VERSION=${VERSION} \
    SETS=${SETS} \
    REPS=${REPS} \
    make \
    run-runtime-benchmark-curve1174 >> ${COMP_LOG}

# Get log path
CURVE1174_LOG_DIR=$(cat "${LATEST_LOG_PATH}")

echo -e "\t- Create benchmark plots"

LOG_SUBDIR="${BIGINT_LOG_DIR};${CURVE1174_LOG_DIR}"
LOGS_NAMES="bigint;curve1174"

${SCRIPTS_DIR}/gen_runtime_plots.py                                            \
    --title "Runtime Comparison for V${VERSION} Curve1174 vs V1 BigInt \
    (${SETS} sets, ${REPS} reps)"                                              \
    --plot_fname "${PLOTS_SUBDIR}/comparison_bar_log_scale.png"                \
    --logs_dir "${LOG_SUBDIR}"                                                 \
    --logs_names "${LOGS_NAMES}"                                               \
    --bar_plot                                                                 \
    --log_yaxis

${SCRIPTS_DIR}/gen_runtime_plots.py                                            \
    --title "Speedup Comparison for V${VERSION} Curve1174 vs V1 BigInt (${SETS} sets, ${REPS} reps)"                                              \
    --plot_fname "${PLOTS_SUBDIR}/speedup_comparison_bar_log_scale.png"        \
    --logs_dir "${LOG_SUBDIR}"                                                 \
    --logs_names "${LOGS_NAMES}"                                               \
    --speedup_plot