#!/usr/bin/env bash

SCRIPTS_DIR=$(cd $(dirname $0) && pwd)
TIMING_BASE_DIR=$(dirname ${SCRIPTS_DIR})
LOGS_DIR=${TIMING_BASE_DIR}/logs

# Only plots outside the local directory will be checked in for git
PLOTS_DIR=${TIMING_BASE_DIR}/plots
if [[ -z "${PUBLISH}" ]]; then
    PLOTS_DIR=${PLOTS_DIR}/local
fi
${SCRIPTS_DIR}/gen_runtime_plots.py                         

${SCRIPTS_DIR}/gen_perf_plot.py                         \
    --plot_fname "${PLOTS_DIR}/abs_plot.png"    		\
    --title "BigInt abs Runtime Plot"                   \
    ${LOGS_DIR}/bench_big_int_abs.log
	${LOGS_DIR}/bench_big_int_neg.log
