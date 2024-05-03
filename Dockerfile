FROM python:3.10-slim-bookworm as GEN_KEYS

WORKDIR /app

COPY utils/generate_keys.py /app

RUN pip3 install --prefer-binary cryptography

RUN python3 generate_keys.py


FROM node:22-bookworm-slim

WORKDIR /app

ARG DEBIAN_FRONTEND=noninteractive
RUN apt update -y && apt install -y build-essential cmake

RUN npm install --global xpm@latest

RUN xpm init

RUN xpm install @xpack-dev-tools/riscv-none-elf-gcc@latest

COPY libs/. /app/libs/
COPY src/. /app/src
COPY cmake/. /app/cmake
COPY CMakeLists.txt link.ld /app/

WORKDIR /app/build

COPY --from=GEN_KEYS /app/output /app/build

RUN cmake ..

RUN make
