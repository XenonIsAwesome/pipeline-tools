FROM ubuntu:noble AS builder

RUN apt update && apt install -qy --no-install-recommends \
    git \
    doxygen \
    graphviz && \
    rm -rf /var/cache/apt/*

WORKDIR /source

COPY core core
COPY docs docs
COPY CMakeLists.txt CMakeLists.txt
COPY main.cpp main.cpp
COPY README.md README.md

RUN cd docs/ && wget https://raw.githubusercontent.com/jothepro/doxygen-awesome-css/refs/heads/main/doxygen-awesome.css
RUN doxygen docs/Doxyfile

FROM nginx:latest

WORKDIR /app
COPY --from=builder /source/docs/html /usr/share/nginx/html
