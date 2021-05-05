#!/usr/bin/env bash

SCRIPTS_DIR=$(cd $(dirname $0) && pwd)
TIMING_BASE_DIR=$(dirname ${SCRIPTS_DIR})
LOGS_DIR=${TIMING_BASE_DIR}/logs

DEFAULT_SETS=10
DEFAULT_REPS=1000

if [[ -z $SETS || -z $REPS ]]; then
    echo "WARNING: using default value for SETS (${DEFAULT_SETS}) and/or "\
         "REPS (${DEFAULT_REPS})"
fi
SETS="${SETS:-$DEFAULT_SETS}"
REPS="${REPS:-$DEFAULT_REPS}"

# Only plots outside the local directory will be checked in for git
PLOTS_DIR=${TIMING_BASE_DIR}/plots
if [[ -z "${PUBLISH}" ]]; then
    PLOTS_DIR=${PLOTS_DIR}/local
fi

################################################################################
#                  Generate bar plots of runtime measurements                  #
################################################################################
${SCRIPTS_DIR}/gen_runtime_plots.py                                            \
    --title "Overview Median Runtime (${SETS} sets, ${REPS} reps)"             \
    --plot_fname "${PLOTS_DIR}/overview.png"                                   \
    --logs_dir "${LOGS_DIR}"

${SCRIPTS_DIR}/gen_runtime_plots.py                                            \
    --title "Overview Median Runtime (log scale, ${SETS} sets, ${REPS} reps)"  \
    --plot_fname "${PLOTS_DIR}/overview_log_scale.png"                         \
    --logs_dir "${LOGS_DIR}"                                                   \
    --log_yaxis

${SCRIPTS_DIR}/gen_runtime_plots.py                                            \
    --title "Overview Median Runtime (${SETS} sets, ${REPS} reps)"             \
    --plot_fname "${PLOTS_DIR}/overview_bar.png"                               \
    --logs_dir "${LOGS_DIR}"                                                   \
    --bar_plot

${SCRIPTS_DIR}/gen_runtime_plots.py                                            \
    --title "Overview Median Runtime (log scale, ${SETS} sets, ${REPS} reps)"  \
    --plot_fname "${PLOTS_DIR}/overview_bar_log_scale.png"                     \
    --logs_dir "${LOGS_DIR}"                                                   \
    --bar_plot                                                                 \
    --log_yaxis

################################################################################
#                 Generate line plots of runtime measurements                  #
################################################################################
${SCRIPTS_DIR}/gen_perf_plot.py                         \
    --plot_fname "${PLOTS_DIR}/abs_plot.png"    		\
    --title "BigInt abs Runtime Plot"                   \
    ${LOGS_DIR}/bench_big_int_abs.log
