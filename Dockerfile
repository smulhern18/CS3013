FROM ubuntu:16.04

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && \
    apt-get -y install gcc gdb gdbserver mono-mcs man && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /cs3013
