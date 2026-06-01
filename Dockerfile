FROM ubuntu:24.04 AS build

ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

RUN sed -i \
        -e 's|http://archive.ubuntu.com/ubuntu/|http://mirror.yandex.ru/ubuntu/|g' \
        -e 's/ noble-backports//g' \
        /etc/apt/sources.list.d/ubuntu.sources \
    && apt-get -o Acquire::Retries=5 update \
    && apt-get -o Acquire::Retries=5 install -y --no-install-recommends \
        ca-certificates \
        build-essential \
        cmake \
        git \
        libfreetype-dev \
        libgl1-mesa-dev \
        libudev-dev \
        libxcursor-dev \
        libxi-dev \
        libxrandr-dev \
        ninja-build \
        pkg-config \
        unzip \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -S . -B build -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_TESTING=ON \
        -DBUILD_SHARED_LIBS=OFF \
    && cmake --build build --target MazeApp MazeTests ScenarioUserGeneration ScenarioGuestView ScenarioAdminModeration ScenarioAlgorithmComparison

FROM build AS test
RUN ctest --test-dir build --output-on-failure

FROM ubuntu:24.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

WORKDIR /data
COPY --from=test /src/build/bin/MazeApp /app/MazeApp
COPY --from=test /src/resources /app/resources

RUN sed -i \
        -e 's|http://archive.ubuntu.com/ubuntu/|http://mirror.yandex.ru/ubuntu/|g' \
        -e 's/ noble-backports//g' \
        /etc/apt/sources.list.d/ubuntu.sources \
    && apt-get -o Acquire::Retries=5 update \
    && apt-get -o Acquire::Retries=5 install -y --no-install-recommends \
        libfreetype6 \
        libgcc-s1 \
        libgl1 \
        libstdc++6 \
        libudev1 \
        libx11-6 \
        libxcursor1 \
        libxi6 \
        libxrandr2 \
    && rm -rf /var/lib/apt/lists/*

ENTRYPOINT ["/app/MazeApp"]
CMD ["--cli", "--width", "21", "--height", "15", "--generator", "dfs", "--pathfinder", "bfs"]
