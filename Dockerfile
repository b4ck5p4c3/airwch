FROM node:22-bookworm-slim

WORKDIR /app

ARG DEBIAN_FRONTEND=noninteractive

RUN apt update -y && apt install -y build-essential cmake

RUN npm install --global xpm@latest
RUN xpm init
RUN xpm install @xpack-dev-tools/riscv-none-elf-gcc@latest

COPY . .

ARG RISCV_TOOLCHAIN_PATH=/app/xpacks/.bin
RUN cmake .
RUN cmake --build .

CMD ["/bin/sh"]