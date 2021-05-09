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

# Only plots outside the local directory will be checked in for git
PLOTS_DIR=${TIMING_BASE_DIR}/plots
SUBFOLDER=""
if [[ -z "${PUBLISH}" ]]; then
    PLOTS_DIR=${PLOTS_DIR}/local
    SUBFOLDER="/$(date "+%Y-%m-%d_%H-%M-%S")"
fi

if [[ -z "${COLLECT_STATS}" ]]; then
    PLOTS_DIR="${PLOTS_DIR}/runtime${SUBFOLDER}"
    mkdir -p ${PLOTS_DIR}

    echo "#####################################################################"
    echo "#             Generate bar plots of runtime measurements            #"
    echo "#####################################################################"

    # ${SCRIPTS_DIR}/gen_runtime_plots.py                                            \
    #     --title "Overview Median Runtime (${SETS} sets, ${REPS} reps)"             \
    #     --plot_fname "${PLOTS_DIR}/overview.png"                                   \
    #     --logs_dir "${LOGS_DIR}"
    #
    # ${SCRIPTS_DIR}/gen_runtime_plots.py                                            \
    #     --title "Overview Median Runtime (log scale, ${SETS} sets, ${REPS} reps)"  \
    #     --plot_fname "${PLOTS_DIR}/overview_log_scale.png"                         \
    #     --logs_dir "${LOGS_DIR}"                                                   \
    #     --log_yaxis
    #
    # ${SCRIPTS_DIR}/gen_runtime_plots.py                                            \
    #     --title "Overview Median Runtime (${SETS} sets, ${REPS} reps)"             \
    #     --plot_fname "${PLOTS_DIR}/overview_bar.png"                               \
    #     --logs_dir "${LOGS_DIR}"                                                   \
    #     --bar_plot

    ${SCRIPTS_DIR}/gen_runtime_plots.py                                            \
        --title "Overview Median Runtime (log scale, ${SETS} sets, ${REPS} reps)"  \
        --plot_fname "${PLOTS_DIR}/overview_bar_log_scale.png"                     \
        --logs_dir "${LOGS_DIR}"                                                   \
        --bar_plot                                                                 \
        --log_yaxis

    # echo "#####################################################################"
    # echo "#             Generate line plots of runtime measurements           #"
    # echo "#####################################################################"
    #
    # ${SCRIPTS_DIR}/gen_perf_plot.py                         \
    #     --plot_fname "${PLOTS_DIR}/runtime_big_int_abs.png" \
    #     --title "BigInt abs Runtime Plot"                   \
    #     ${LOGS_DIR}/bench_big_int_abs.log

else
    PLOTS_DIR="${PLOTS_DIR}/stats${SUBFOLDER}"
    mkdir -p ${PLOTS_DIR}

    echo "#####################################################################"
    echo "#                    Generate plots for stats                       #"
    echo "#####################################################################"

    ${SCRIPTS_DIR}/gen_stats_plots.py                                          \
        --plot_dir "${PLOTS_DIR}"                                              \
        --logs_dir "${LOGS_DIR}"                                               \
        --bar_plot                                                             \
        --log_yaxis
fi
