ARG IMAGE=zeek/zeek:5.0.7
FROM $IMAGE

RUN apt-get update && apt-get -y --no-install-recommends install \
    cmake \
    libpcap-dev \
    build-essential \
  && apt autoclean \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .
RUN rm -rf ./build && \
  ./configure && \
  make && \
  make install

# Smoke check the plugin loads...
RUN zeek -N Zeek::AF_Packet
