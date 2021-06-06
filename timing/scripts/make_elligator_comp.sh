#!/usr/bin/env bash

SCRIPTS_DIR=$(cd $(dirname $0) && pwd)
TIMING_BASE_DIR=$(dirname ${SCRIPTS_DIR})

DEFAULT_SETS=10
DEFAULT_REPS=10000

if [[ -z $SETS || -z $REPS ]]; then
    echo "WARNING: using default value for SETS (${DEFAULT_SETS}) and/or "\
         "REPS (${DEFAULT_REPS})"
fi
SETS="${SETS:-$DEFAULT_SETS}"
REPS="${REPS:-$DEFAULT_REPS}"

DEFAULT_MIN_VERSION=1
DEFAULT_MAX_VERSION=3

MIN_VERSION="${MIN_VERSION:-$DEFAULT_MIN_VERSION}"
MAX_VERSION="${MAX_VERSION:-$DEFAULT_MAX_VERSION}"
COMP_NAME=comp_elligator

SDIR=${TIMING_BASE_DIR}/src
IDIR=${TIMING_BASE_DIR}/include
LDIR=${TIMING_BASE_DIR}/logs/${COMP_NAME}
PDIR=${TIMING_BASE_DIR}/plots/local/${COMP_NAME}
GEN_SCRIPT=${TIMING_BASE_DIR}/../scripts/gen_types.py
SAGE_ELLIGATOR=${TIMING_BASE_DIR}/scripts/runtime_elligator.sage
LATEST_LOG_PATH=${TIMING_BASE_DIR}/logs/latest_log_path.txt

BENCH_TYPES=\
"BENCH_TYPE_CURVE_1174_POW_Q_M2"

# BENCH_TYPES=\
# "BENCH_TYPE_ELLIGATOR1_STR2PNT\
#  BENCH_TYPE_ELLIGATOR1_PNT2STR"

echo "#####################################################################"
echo "#       Generate comparison plots for Elligator from V${MIN_VERSION} to V${MAX_VERSION}       #"
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

LOG_SUBDIR=""
LOGS_NAMES=""
SEP=""

for VERSION in `seq ${MIN_VERSION} ${MAX_VERSION}`;
do
    if [ "${VERSION}" = "1" ]; then
        MAKEFILE_RUNTIME_TARGETS="run-runtime-benchmark"
    else
        MAKEFILE_RUNTIME_TARGETS="run-runtime-benchmark run-runtime-benchmark-curve1174"
    fi

    echo -e "\t\t- run-runtime-benchmark for V${VERSION}"
    BENCHMARKS="${BENCH_TYPES_INT}" \
        VERSION=${VERSION} \
        SETS=${SETS} \
        REPS=${REPS} \
        make \
        ${MAKEFILE_RUNTIME_TARGETS} >> ${COMP_LOG}

    # Get log path
    NEW_LOG_DIR=$(cat "${LATEST_LOG_PATH}")
    LOG_SUBDIR="${LOG_SUBDIR}${SEP}${NEW_LOG_DIR}"
    LOGS_NAMES="${LOGS_NAMES}${SEP}V${VERSION}"
    SEP=";"
done

#
# GMP benchmark
#
if [[ -z $SKIP_COMP_TO_GMP ]]; then
    echo -e "\t\t- run-gmp-benchmark"
    BENCHMARKS="${BENCH_TYPES_INT}" \
        VERSION=1 \
        SETS=${SETS} \
        REPS=${REPS} \
        make \
        run-gmp-benchmark >> ${COMP_LOG}

    # Get log path
    NEW_LOG_DIR=$(cat "${LATEST_LOG_PATH}")
    LOG_SUBDIR="${LOG_SUBDIR}${SEP}${NEW_LOG_DIR}"
    LOGS_NAMES="${LOGS_NAMES}${SEP}gmp"
    SEP=";"
fi

#
# Sage benchmark
#
if [[ ! -z $COMP_TO_SAGE ]]; then
    echo -e "\t\t- running sage benchmark (this takes a while)"
    SAGE_ELLIGATOR_LDIR=${LDIR}/sage_elligator
    mkdir -p ${SAGE_ELLIGATOR_LDIR}

    sage ${SAGE_ELLIGATOR}                                                         \
        --sets ${SETS}                                                             \
        --reps ${REPS}                                                             \
        --logs_dir ${SAGE_ELLIGATOR_LDIR} >> ${COMP_LOG}

    # Store log path
    NEW_LOG_DIR=$(cat "${LATEST_LOG_PATH}")
    LOG_SUBDIR="${LOG_SUBDIR}${SEP}${NEW_LOG_DIR}"
    LOGS_NAMES="${LOGS_NAMES}${SEP}sage"
fi

echo -e "\t- Create benchmark plots"

${SCRIPTS_DIR}/gen_runtime_plots.py                                            \
    --title "Runtime Comparison for different Elligator implementations (${SETS} sets, ${REPS} reps)" \
    --plot_fname "${PLOTS_SUBDIR}/comparison_bar_log_scale.png"                \
    --logs_dir "${LOG_SUBDIR}"                                                 \
    --logs_names "${LOGS_NAMES}"                                               \
    --bar_plot                                                                 \
    --log_yaxis

${SCRIPTS_DIR}/gen_runtime_plots.py                                            \
    --title "Speedup Comparison for different Elligator implementations (${SETS} sets, ${REPS} reps)" \
    --plot_fname "${PLOTS_SUBDIR}/speedup_comparison_bar_log_scale.png"                \
    --logs_dir "${LOG_SUBDIR}"                                                 \
    --logs_names "${LOGS_NAMES}"                                               \
    --speedup_plot
