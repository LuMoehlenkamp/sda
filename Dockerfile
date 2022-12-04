# syntax=docker/dockerfile:1
# FROM debian:bullseye-slim
FROM zinen2/alpine-pigpiod
LABEL author=lmoehlenkamp
LABEL version=0.1
WORKDIR /src
RUN apt update && \
  apt install -y git && \
  apt install -y build-essential && \
  apt install -y cmake && \
  apt install -y libboost-all-dev && \
  apt install -y gpiod && \
  apt install -y libgpiod-dev

RUN git clone https://github.com/LuMoehlenkamp/sda.git

WORKDIR /src/sda

RUN git fetch --all --prune && git pull --rebase
RUN cp /src/sda/src/params.json /src/sda/bin/

WORKDIR /src/sda/build

RUN cmake ..
RUN cmake --build . --config Release --target sda -- -j 1


# apt install -y snapd
# snap install cmake