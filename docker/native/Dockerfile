# Note: This file was imported from https://github.com/ugermann/marian-docker/blob/master/build-environment/Dockerfile

FROM nvidia/cuda:10.2-cudnn7-devel-ubuntu18.04
ARG GITHASH
ENV IMAGE_GITHASH=${GITHASH}
# This is a docker image containing the infrastructure required for compiling
# Marian runtimes.

# Intel MKL is so expensive to install that we install it as one of the first
# Docker layers
COPY GPG-PUB-KEY-INTEL-SW-PRODUCTS-2019.PUB .
# from: https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS-2019.PUB

RUN apt-get update \
    && apt-get install -y apt-transport-https \
    && apt-key add GPG-PUB-KEY-INTEL-SW-PRODUCTS-2019.PUB \
    && echo deb https://apt.repos.intel.com/mkl all main \
       >/etc/apt/sources.list.d/intel-mkl.list \
    && apt-get update
RUN apt-get install -y intel-mkl-64bit-2018.2-046

RUN apt-get install -y \
    autoconf \
    automake \
    binutils \
    ccache \
    g++ \
    git \
    libev++-dev \
    libssl-dev \
    libboost-all-dev \
    libtool \
    libgoogle-perftools-dev \
    libpcre3-dev \
    libprotobuf-dev \
    protobuf-compiler \
    python3-pip \
    unzip \
    wget

ADD . .
# RUN rm -rf /var/lib/apt/lists/*
LABEL GITHASH=${GITHASH}

WORKDIR /usr
RUN wget https://github.com/Kitware/CMake/releases/download/v3.17.2/cmake-3.17.2-Linux-x86_64.tar.gz -qO-\
    | tar xzf - --strip-components 1

WORKDIR /
ENV GIT_DISCOVERY_ACROSS_FILESYSTEM=1
