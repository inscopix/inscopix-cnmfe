FROM ubuntu:18.04

ARG BUILD_DIR="build"

COPY . /isx-cnmfe

WORKDIR /isx-cnmfe

RUN apt update \
    && apt install -y build-essential libhdf5-dev libglib2.0-0 \
    && apt install -y cmake \
    && ln -s /lib/x86_64-linux-gnu/libz.so.1 /lib64/libz.so \
    && cmake -H. -B$BUILD_DIR && make -C $BUILD_DIR

VOLUME ["/input", "/output"]

ENTRYPOINT ["./build/runCnmfe"]
