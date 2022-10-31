FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-ubuntu-22.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y software-properties-common && \
    apt-add-repository ppa:lttng/stable-2.13 && \
    apt-get update

RUN apt-get install -y wget \
    curl \
    zip \
    unzip \
    tar \
    curl \
	time \
    libcurl4-openssl-dev \
    build-essential \
    libczmq-dev \
    linux-headers-$(uname -r) \
    pkg-config \
    kmod \
    libnuma-dev \
    uuid-dev \
    libpopt-dev \
    liburcu-dev \
    libxml2-dev \
    babeltrace2 \
    numactl \
    binutils \
    libc-dev \
    libstdc++-10-dev \
    gcc-10 \
    g++-10 \
    clangd-12 \
    clang-tidy \
    clang-format \
    cmake \
    git \
    make \
    ninja-build

ENV CC=gcc-10 CXX=g++-10

WORKDIR /code
COPY . .

CMD time ./run.sh 6 3
