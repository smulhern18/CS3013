FROM ubuntu:16.04

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && \
    apt-get -y install gcc gdb gdbserver mono-mcs && \
    rm -rf /var/lib/apt/lists/*

COPY . ./cs3013

WORKDIR /cs3013
