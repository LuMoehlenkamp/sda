# syntax=docker/dockerfile:1
FROM ubuntu:22.04
LABEL author=lmoehlenkamp
LABEL version=0.1
WORKDIR /src
RUN apt update && \
  apt install -y git && \
  apt install -y build-essential && \
  apt install -y cmake && \
  apt install -y libboost-all-dev && \
  apt install -y wiringpi && \
  apt install -y gpiod && \
  apt install -y libgpiod-dev

# RUN git clone https://github.com/LuMoehlenkamp/sda.git

# WORKDIR /src/sda

# RUN git fetch --all --prune && git pull --rebase

# WORKDIR /src/sda/build

# RUN cmake ..

# apt install -y snapd
# snap install cmake