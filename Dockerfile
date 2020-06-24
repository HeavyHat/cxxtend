FROM ubuntu:focal AS build
WORKDIR /workspace
RUN apt-get install build-essential gcc cmake clang-format clang-tidy doxygen python3 python3-venv libbenchmark-dev libexpected-dev libgtest-dev libgmock-dev
COPY . /workspace/.
RUN pip install -r requirements.txt
