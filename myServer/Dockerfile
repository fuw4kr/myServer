FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    g++ cmake git libjsoncpp-dev libpq-dev uuid-dev openssl libssl-dev

RUN git clone https://github.com/drogonframework/drogon.git /tmp/drogon && \
    cd /tmp/drogon && mkdir build && cd build && \
    cmake .. && make -j$(nproc) && make install

WORKDIR /app
COPY . /app

RUN mkdir build && cd build && cmake .. && make -j$(nproc)

EXPOSE 8080

CMD ["./build/myServer"]
