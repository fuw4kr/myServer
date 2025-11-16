FROM ubuntu:22.04 AS build

RUN apt-get update && apt-get install -y \
    git \
    cmake \
    g++ \
    make \
    libjsoncpp-dev \
    uuid-dev \
    zlib1g-dev \
    libssl-dev \
    libpq-dev \
    && rm -rf /var/lib/apt/lists/*

# Build Drogon from source
RUN git clone --recursive https://github.com/drogonframework/drogon.git /tmp/drogon && \
    cd /tmp/drogon && mkdir build && cd build && \
    cmake .. && make -j"$(nproc)" && make install

WORKDIR /src
COPY . .

WORKDIR /src/myServer
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --config Release -j"$(nproc)"

FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libjsoncpp25 \
    uuid-runtime \
    zlib1g \
    libssl3 \
    libpq5 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /usr/local /usr/local
COPY --from=build /src/myServer/build/myServerApp /app/myServerApp

WORKDIR /app
EXPOSE 8080

CMD ["./myServerApp"]
