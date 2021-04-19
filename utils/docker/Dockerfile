# based upon https://hub.docker.com/r/dolanor/qt-android/~/dockerfile/
FROM debian:bullseye

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -qq update && apt-get -qq dist-upgrade && apt-get install -qq -y --no-install-recommends \
    build-essential \
    ca-certificates \
    ccache \
    cmake \
    dos2unix \
    doxygen \
    g++-10 \
    gcc-10 \
    git \
    google-mock \
    googletest \
    graphviz \
    lcov \
    libboost-filesystem-dev \
    libgmock-dev \
    libgtest-dev \
    libprotobuf-dev \
    protobuf-compiler \
    python3 \
    python3-distutils \
    python3-junitparser \
    python3-lxml \
    python3-pandas \
    python3-pytest \
    qtbase5-dev \
    && apt-get -qq clean

