FROM ubuntu:18.04

ARG BUILD_DIR="build"

WORKDIR /isx-cnmfe

COPY include /isx-cnmfe/include
COPY src /isx-cnmfe/src
COPY test /isx-cnmfe/test
COPY lib /isx-cnmfe/lib
COPY CMakeLists.txt example.cpp /isx-cnmfe/

RUN apt update \
    && apt install -y build-essential libglib2.0-0 \
    && apt install -y cmake \
    && apt install -y g++-4.8 gcc-4.8 \
    && ln -s /lib/x86_64-linux-gnu/libz.so.1 /lib64/libz.so \
    && cmake -DCMAKE_C_COMPILER=gcc-4.8 -DCMAKE_CXX_COMPILER=g++-4.8 -H. -B$BUILD_DIR && make -C $BUILD_DIR

VOLUME ["/input", "/output"]

ENTRYPOINT ["./build/runCnmfe"]
