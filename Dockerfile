FROM ubuntu:18.04

ARG DEFAULT_WORKDIR=/file_transformer
COPY ./dfile2sac.c $DEFAULT_WORKDIR/

RUN apt-get update && apt-get install -y --no-install-recommends gcc libc6-dev

WORKDIR $DEFAULT_WORKDIR

RUN gcc -Wall ./dfile2sac.c -o dfile2sac

