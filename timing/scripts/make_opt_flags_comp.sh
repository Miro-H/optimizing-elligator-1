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
# Helper scripts that compare the performance when compiling the code with
# different compiler flags.
#
################################################################################



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
    echo "WARNING: using default value for VERSION (V${VERSION})"
fi
VERSION="${VERSION:-$DEFAULT_VERSION}"

SDIR=${TIMING_BASE_DIR}/src
IDIR=${TIMING_BASE_DIR}/include
LDIR=${TIMING_BASE_DIR}/logs/V${VERSION}/opt_flags
PSUBDIR=local/V${VERSION}/opt_flags
GEN_SCRIPT=${TIMING_BASE_DIR}/../scripts/gen_types.py
LATEST_LOG_PATH=${TIMING_BASE_DIR}/logs/latest_log_path.txt

# BENCH_TYPE_CURVE_1174_POW\
#  BENCH_TYPE_CURVE_1174_CHI\
BENCH_TYPES=\
"BENCH_TYPE_ELLIGATOR1_STR2PNT\
 BENCH_TYPE_ELLIGATOR1_PNT2STR"

declare -a OPT_FLAGS=(\
    "-O0 -fno-tree-vectorize" \
    "-O1 -fno-tree-vectorize" \
    "-O2 -fno-tree-vectorize" \
    "-O3 -fno-tree-vectorize" \
    # "-O3 -m64 -march=haswell" \
    # "-O3 -m64 -march=native" \
    "-Ofast -fno-tree-vectorize" \
    "-Ofast -m64 -march=haswell -fno-tree-vectorize" \
    "-Ofast -m64 -march=native -fno-tree-vectorize"\
    "-O0" \
    "-O1" \
    "-O2" \
    "-O3" \
    "-Ofast" \
    "-Ofast -m64 -march=haswell" \
    "-Ofast -m64 -march=native")

echo "#####################################################################"
echo "#         Generate comparison plots for differnt opt flags          #"
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

echo -e "\t- Prepare log folders"
ALL_OPT_LOG_DIRS=""
ALL_OPTS=""
SEP=""
for OPT_FLAG in "${OPT_FLAGS[@]}"
do
    OPT_LOG_DIR=${LOG_SUBDIR}/${OPT_FLAG//[- ]/_}
    mkdir -p ${OPT_LOG_DIR}

    ALL_OPT_LOG_DIRS="${ALL_OPT_LOG_DIRS}${SEP}${OPT_LOG_DIR}"
    ALL_OPTS="${ALL_OPTS}${SEP}${OPT_FLAG}"
    SEP=";"
done

echo -e "\t- Create benchmark logs"
COMP_LOG_DIR=${LOG_SUBDIR}/opt_flags_comparison.log
touch ${COMP_LOG_DIR}

for OPT_FLAG in "${OPT_FLAGS[@]}"
do
    echo -e "\t\t- For optimization flag '${OPT_FLAG}'"

    echo -e "\t\t\t- run-runtime-benchmark"
    OPT_FLAGS="${OPT_FLAG}" \
        BENCHMARKS="${BENCH_TYPES_INT}" \
        VERSION=${VERSION} \
        SETS=${SETS} \
        REPS=${REPS} \
        make \
        run-runtime-benchmark >> ${COMP_LOG_DIR}

    # Get log path
    LATEST_LOG_DIR=$(cat "${LATEST_LOG_PATH}")

    # Gather logs
    cp ${LATEST_LOG_DIR}/*.log ${LOG_SUBDIR}/${OPT_FLAG//[- ]/_}/

    echo -e "\t\t\t- run-runtime-benchmark-curve1174"
    OPT_FLAGS="${OPT_FLAG}" \
        BENCHMARKS="${BENCH_TYPES_INT}" \
        VERSION=${VERSION} \
        SETS=${SETS} \
        REPS=${REPS} \
        make \
        run-runtime-benchmark-curve1174 >> ${COMP_LOG_DIR}

    # Get log path
    LATEST_LOG_DIR=$(cat "${LATEST_LOG_PATH}")

    # Gather logs
    cp ${LATEST_LOG_DIR}/*.log ${LOG_SUBDIR}/${OPT_FLAG//[- ]/_}/
done

echo -e "\t- Create benchmark plots"
LOGS_NAMES="${ALL_OPTS}" \
    LOGS_DIRS="${ALL_OPT_LOG_DIRS}" \
    SETS=${SETS} \
    REPS=${REPS} \
    OUT_SUB_PATH="${PSUBDIR}/${TIMESTAMP_DIR}/opt_flags_comparison.png" \
    ${SCRIPTS_DIR}/make_all_plots.sh
