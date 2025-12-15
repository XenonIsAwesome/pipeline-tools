FROM ubuntu:noble AS depends

RUN apt-get update && apt-get install --no-install-recommends -qy \
    git \
    cmake \
    make \
    gcc \
    g++ \
    libgtest-dev \
    libfmt-dev \
    libssl-dev \
    libboost1.74-all-dev \
    nlohmann-json3-dev && \
    rm -rf /var/cache/apt/*

FROM depends AS builder

#ARG DAWGLOG_TAG="v1.1"
#RUN git clone "${DAWGLOG_PATH}/dawg-logger.git" && \
#    cd dawg-logger && \
#    git checkout ${DAWGLOG_TAG} && \
#    cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && \
#    cmake --build build --target install -- -j$(nproc) && \
#    cd .. && rm -rf dawg-logger

ARG RABBITMQ_C_TAG="master"
RUN git clone https://github.com/alanxz/rabbitmq-c.git && \
    cd rabbitmq-c && \
    git checkout ${RABBITMQ_C_TAG} && \
    cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --target install -- -j$(nproc) && \
    cd .. && rm -rf rabbitmq-c

ARG SIMPLE_AMQP_CLIENT_TAG="master"
RUN git clone https://github.com/alanxz/SimpleAmqpClient.git && \
    cd SimpleAmqpClient && \
    git checkout ${SIMPLE_AMQP_CLIENT_TAG} && \
    cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --target install -- -j$(nproc) && \
    cd .. && rm -rf SimpleAmqpClient

ARG CPP_HTTPLIB_TAG="v0.26.0"
RUN git clone https://github.com/yhirose/cpp-httplib && \
    cd cpp-httplib && \
    git checkout ${CPP_HTTPLIB_TAG} && \
    cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --target install -- -j$(nproc) && \
    cd .. && rm -rf cpp-httplib

ARG NSTDLIB_TAG="v1.0"
RUN git clone https://github.com/mkmagic/nstd_any.git && \
    cd nstd-any && \
    git checkout ${NSTDLIB_TAG} && \
    cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --target install -- -j$(nproc) && \
    cd .. && rm -rf nstd-any

RUN ldconfig