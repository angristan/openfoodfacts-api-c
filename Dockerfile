FROM debian:stretch-slim

RUN apt-get update \
    && apt-get install -y \
    build-essential \
    libjansson-dev \
    libcurl4-gnutls-dev

ADD . /app

WORKDIR /app

RUN make

ENTRYPOINT ["./get_off_product"]
