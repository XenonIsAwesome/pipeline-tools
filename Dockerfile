FROM ubuntu:latest

RUN apt-get update && apt-get install -y --no-install-recommends \
    git \
    wget \
    unzip \
    cmake \
    make \
    gcc \
    g++ \
    gdb \
    libssl-dev \
    nlohmann-json3-dev \
    libgtest-dev \
    libbenchmark-dev \
    libboost1.74-all-dev

RUN wget --no-check-certificate https://github.com/alanxz/rabbitmq-c/archive/refs/heads/master.zip && \
    unzip master.zip && rm master.zip && cd rabbitmq-c-master && \
    cmake -S . -B build && \
    cmake --build build --target install -- -j$(nproc) && \
    cd .. && rm -rf rabbitmq-c-master

RUN wget --no-check-certificate https://github.com/alanxz/SimpleAmqpClient/archive/refs/heads/master.zip && \
    unzip master.zip && rm master.zip && cd SimpleAmqpClient-master && \
    cmake -S . -B build && \
    cmake --build build --target install -- -j$(nproc) && \
    cd .. && rm -rf SimpleAmqpClient-master

RUN ldconfig