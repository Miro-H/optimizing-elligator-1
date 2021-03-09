#!/usr/bin/env bash

#
# This script installs check 0.15.2 from source on Debian/Ubuntu
#
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
