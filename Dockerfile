FROM ubuntu:focal AS build
WORKDIR /workspace
RUN apt-get update && DEBIAN_FRONTEND="noninteractive" apt-get install -y build-essential gcc cmake clang-format clang-tidy doxygen python3 python3-venv python3-pip libbenchmark-dev libexpected-dev libgtest-dev libgmock-dev
COPY . /workspace/.
RUN python3.8 -m pip install -r requirements.txt
