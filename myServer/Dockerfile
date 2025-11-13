FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    git cmake g++ curl unzip zip pkg-config libssl-dev libjsoncpp-dev uuid-dev zlib1g-dev libpq-dev \
    && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/microsoft/vcpkg.git /opt/vcpkg && \
    /opt/vcpkg/bootstrap-vcpkg.sh && \
    /opt/vcpkg/vcpkg install drogon[core,orm,postgres]:x64-linux

ENV VCPKG_ROOT=/opt/vcpkg
ENV CMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake

WORKDIR /app
COPY . .

RUN mkdir build && cd build && \
    cmake -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE .. && \
    make -j$(nproc) && \
    ls -la

WORKDIR /app/build
EXPOSE 8080
CMD ["./myServerApp"]
