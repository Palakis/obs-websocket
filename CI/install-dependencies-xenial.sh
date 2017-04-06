#!/bin/sh
set -ex

# OBS Studio deps
add-apt-repository "deb http://archive.ubuntu.com/ubuntu $(lsb_release -sc) universe" -y
add-apt-repository ppa:kirillshkrogalev/ffmpeg-next -y
apt-get -qq update
apt-get install -y \
        build-essential \
        checkinstall \
        cmake \
        libasound2-dev \
        libavcodec-ffmpeg-dev \
        libavdevice-ffmpeg-dev \
        libavfilter-ffmpeg-dev \
        libavformat-ffmpeg-dev \
        libavutil-ffmpeg-dev \
        libcurl4-openssl-dev \
        libfontconfig-dev \
        libfreetype6-dev \
        libgl1-mesa-dev \
        libjack-jackd2-dev \
        libjansson-dev \
        libpulse-dev \
        libqt5x11extras5-dev \
        libspeexdsp-dev \
        libswresample-ffmpeg-dev \
        libswscale-ffmpeg-dev \
        libudev-dev \
        libv4l-dev \
        libvlc-dev \
        libx11-dev \
        libx264-dev \
        libxcb-shm0-dev \
        libxcb-xinerama0-dev \
        libxcomposite-dev \
        libxinerama-dev \
        pkg-config \
        qtbase5-dev

# qtwebsockets deps
apt-get install -y qt5-qmake qtbase5-private-dev qtcreator

# obs-websocket deps
apt-get install libqt5websockets5-dev

# Build obs-studio
cd /root
git clone https://github.com/jp9000/obs-studio ./obs-studio
cd obs-studio
mkdir build && cd build
cmake -DUNIX_STRUCTURE=1 -DCMAKE_INSTALL_PREFIX=/usr ..
make -j4
make install

ldconfig