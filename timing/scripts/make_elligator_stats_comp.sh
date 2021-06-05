#!/usr/bin/env bash

SCRIPTS_DIR=$(cd $(dirname $0) && pwd)
TIMING_BASE_DIR=$(dirname ${SCRIPTS_DIR})

COMP_NAME=comp_elligator_stats

SDIR=${TIMING_BASE_DIR}/src
IDIR=${TIMING_BASE_DIR}/include
LDIR=${TIMING_BASE_DIR}/logs/${COMP_NAME}
PDIR=${TIMING_BASE_DIR}/plots/local/${COMP_NAME}
GEN_SCRIPT=${TIMING_BASE_DIR}/../scripts/gen_types.py
LATEST_LOG_PATH=${TIMING_BASE_DIR}/logs/latest_log_path.txt

MIN_VERSION=1
MAX_VERSION=3

BENCH_TYPES=\
"BENCH_TYPE_ELLIGATOR1_STR2PNT\
 BENCH_TYPE_ELLIGATOR1_PNT2STR"

echo "#####################################################################"
echo "#    Generate comparison plots for Elligator stats for V${MIN_VERSION} and V${MAX_VERSION}    #"
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
    echo -e "\t\t- run-runtime-benchmark for V${VERSION}"
    COLLECT_STATS=1 \
    BENCHMARKS="${BENCH_TYPES_INT}" \
        VERSION=${VERSION} \
        make \
        run-runtime-benchmark >> ${COMP_LOG}

    # Get log path
    NEW_LOG_DIR=$(cat "${LATEST_LOG_PATH}")
    LOG_SUBDIR="${LOG_SUBDIR}${SEP}${NEW_LOG_DIR}"
    LOGS_NAMES="${LOGS_NAMES}${SEP}V${VERSION}"
    SEP=";"

    # Extract roofline values
done

echo -e "\t- Create benchmark plots"

${SCRIPTS_DIR}/gen_stats_plots.py                                              \
    --title "Stats Comparison for Elligator for V${MIN_VERSION} to V${MAX_VERSION}" \
    --plot_dir "${PLOTS_SUBDIR}"                                               \
    --logs_dir "${LOG_SUBDIR}"                                                 \
    --logs_names "${LOGS_NAMES}"                                               \
    --bar_plot                                                                 \
    --log_yaxis

echo "#####################################################################"
echo "#     Generate roofline plot for the stats in different versions    #"
echo "#####################################################################"
${SCRIPTS_DIR}/gen_roofline_plot.py                                            \
    --beta 8.9                                                                 \
    --pis "4;unvec" "16;vec"                                                   \
    --alu_ports 0 1 5 6                                                        \
    --shift_ports 0 6                                                          \
    --div_ports 0                                                              \
    --mul_ports 1                                                              \
    --max_x_val 256                                                            \
    --title "Roofline Plot for Elligator V${MIN_VERSION} to V${MAX_VERSION}"   \
    --plot_fname "${PLOTS_SUBDIR}/roofline_plot_V${MIN_VERSION}_to_V${MAX_VERSION}.png" \
    --logs_names "${LOGS_NAMES}"                                               \
    --logs_dirs "${LOG_SUBDIR}"
