# based upon https://hub.docker.com/r/dolanor/qt-android/~/dockerfile/
FROM debian:bullseye

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -qq update && apt-get -qq dist-upgrade && apt-get install -qq -y --no-install-recommends \
    build-essential \
    ccache \
    cmake \
    doxygen \
    gcc-9 \
    g++-9 \
    graphviz \
    lcov \
    python3 \
    python3-distutils \
    python3-pip \
    qtbase5-dev \
    && apt-get -qq clean

RUN pip install \
    approvaltests \
    junitparser \
    lxml \
    pandas \
    pytest \
    watchdog

