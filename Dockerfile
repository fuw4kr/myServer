FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    git cmake g++ libjsoncpp-dev uuid-dev zlib1g-dev libssl-dev libpq-dev \
    && rm -rf /var/lib/apt/lists/*

RUN git clone --recursive https://github.com/drogonframework/drogon.git /tmp/drogon && \
    cd /tmp/drogon && mkdir build && cd build && \
    cmake .. && make -j$(nproc) && make install

WORKDIR /app
COPY . .

WORKDIR /app/myServer

RUN mkdir build \
    && cd build \
    && cmake .. \
    && make -j$(nproc)

EXPOSE 8080

CMD ["./build/myServer/myServerApp"]
