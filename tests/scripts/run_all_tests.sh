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
# Execute all tests.
#
################################################################################


SRC_DIR=$(cd "$(dirname "$0")"; pwd)
TEST_BIN_DIR=${SRC_DIR}/../bin
LOGS_DIR=${SRC_DIR}/../logs

for test in ${TEST_BIN_DIR}/*
do
    TEST_NAME=$(basename ${test})
    echo -e "\n#####################################################################"
    echo "# Execute tests in file '${TEST_NAME}'"
    echo "#####################################################################"

    LOG_FNAME=${LOGS_DIR}/${TEST_NAME}.log
    CK_LOG_FILE_NAME=${LOG_FNAME} ${test}

    cat ${LOG_FNAME}
done
