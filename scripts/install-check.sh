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
# This script installs check 0.15.2 from source on Debian/Ubuntu
#
################################################################################


if ! pkg-config --modversion check > /dev/null 2>&1; then
    echo "Installing Check 0.15.2 from source."

    CHECK_RELEASE=https://github.com/libcheck/check/archive/0.15.2.tar.gz

    apt update

    DEBIAN_FRONTEND=noninteractive apt -y -q \
        install \
        make \
        build-essential \
        libtool \
        libsubunit-dev \
        autoconf \
        pkg-config \
        texinfo \
        wget

    mkdir -p "$1"
    cd "$1"
    wget $CHECK_RELEASE
    tar xvf *.tar.gz

    cd check-*
    autoreconf --install
    ./configure --prefix=/usr
    make
    make check
    make install
else
    echo "Check might already be installed."
fi
