#!/usr/bin/env bash

################################################################################
#
# This file is part of the ASL project "Censorship-avoiding high-speed EC
# (Elligator with Curve1174)"
# (https://github.com/Miro-H/optimizing-elligator-1).
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# Contact: miro.haller@alumni.ethz.ch
#
# Short description of this file:
# Helper script to create multiple (relevant) plots
#
################################################################################


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

DEFAULT_PATTERN=".*"
PATTERN="${PATTERN:-$DEFAULT_PATTERN}"

# Only plots outside the local directory will be checked in for git
PLOTS_DIR=${TIMING_BASE_DIR}/plots
if [[ -z "${PUBLISH}" ]]; then
    PLOTS_DIR=${PLOTS_DIR}/local
fi

PLOTS_DIR="${PLOTS_DIR}/${OUT_SUB_PATH}"
mkdir -p ${PLOTS_DIR}

if [[ ! -z "${LOGS_DIRS}" ]]; then

    echo "#####################################################################"
    echo "#        Generate comparison plots for runtime measurements         #"
    echo "#####################################################################"

        # --title "Median Runtime Comparison (${SETS} sets, ${REPS} reps)"        \
    ${SCRIPTS_DIR}/gen_runtime_plots.py                                         \
        --title "Median Runtime Comparison"                                     \
        --plot_fname "${PLOTS_DIR}/comparison_bar_log_scale.eps"                \
        --logs_dir "${LOGS_DIRS}"                                               \
        --logs_names "${LOGS_NAMES}"                                            \
        --pattern "${PATTERN}"                                                  \
        --bar_plot                                                              \
        --log_xaxis

elif [[ -z "${COLLECT_STATS}" ]]; then
    echo "#####################################################################"
    echo "#             Generate bar plots of runtime measurements            #"
    echo "#####################################################################"

    # ${SCRIPTS_DIR}/gen_runtime_plots.py                                            \
    #     --title "Overview Median Runtime (${SETS} sets, ${REPS} reps)"             \
    #     --plot_fname "${PLOTS_DIR}/overview.eps"                                   \
    #     --logs_dir "${LOGS_DIR}"
    #
    # ${SCRIPTS_DIR}/gen_runtime_plots.py                                            \
    #     --title "Overview Median Runtime (log scale, ${SETS} sets, ${REPS} reps)"  \
    #     --plot_fname "${PLOTS_DIR}/overview_log_scale.eps"                         \
    #     --logs_dir "${LOGS_DIR}"                                                   \
    #     --log_xaxis

    ${SCRIPTS_DIR}/gen_runtime_plots.py                                            \
        --title "Overview Median Runtime (${SETS} sets, ${REPS} reps)"             \
        --plot_fname "${PLOTS_DIR}/overview_bar.eps"                               \
        --logs_dir "${LOGS_DIR}"                                                   \
        --bar_plot

    ${SCRIPTS_DIR}/gen_runtime_plots.py                                            \
        --title "Overview Median Runtime (${SETS} sets, ${REPS} reps)"  \
        --plot_fname "${PLOTS_DIR}/overview_bar_log_scale.eps"                     \
        --logs_dir "${LOGS_DIR}"                                                   \
        --bar_plot                                                                 \
        --log_xaxis

    # echo "#####################################################################"
    # echo "#             Generate line plots of runtime measurements           #"
    # echo "#####################################################################"
    #
    # ${SCRIPTS_DIR}/gen_perf_plot.py                         \
    #     --plot_fname "${PLOTS_DIR}/runtime_big_int_abs.eps" \
    #     --title "BigInt abs Runtime Plot"                   \
    #     ${LOGS_DIR}/bench_big_int_abs.log

else

    echo "#####################################################################"
    echo "#                    Generate plots for stats                       #"
    echo "#####################################################################"

    ${SCRIPTS_DIR}/gen_stats_plots.py                                          \
        --plot_dir "${PLOTS_DIR}"                                              \
        --logs_dir "${LOGS_DIR}"                                               \
        --bar_plot                                                             \
        --log_yaxis

fi
