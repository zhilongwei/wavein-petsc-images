# syntax=docker/dockerfile:1.7

ARG UBUNTU_VERSION=24.04

FROM ubuntu:${UBUNTU_VERSION} AS petsc-source

ARG DEBIAN_FRONTEND=noninteractive
ARG PETSC_VERSION
ARG PETSC_GIT_SHA

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt/lists,sharing=locked \
    apt-get update && apt-get install -y --no-install-recommends \
        build-essential ca-certificates cmake gfortran git ninja-build pkg-config \
        python3 python3-packaging

WORKDIR /src

RUN test -n "${PETSC_VERSION}" && test -n "${PETSC_GIT_SHA}" && \
    git init petsc && cd petsc && \
    git remote add origin https://gitlab.com/petsc/petsc.git && \
    git fetch --depth=1 origin "refs/tags/v${PETSC_VERSION}:refs/tags/v${PETSC_VERSION}" && \
    test "$(git rev-list -n 1 "v${PETSC_VERSION}")" = "${PETSC_GIT_SHA}" && \
    git -c advice.detachedHead=false checkout --detach "v${PETSC_VERSION}" && \
    git submodule update --init --recursive --depth=1

FROM petsc-source AS petsc-build

ARG PETSC_VARIANT
WORKDIR /src/petsc

RUN case "${PETSC_VARIANT}" in \
        debug)   debugging=1; optflags="-O0 -g3" ;; \
        release) debugging=0; optflags="-O3" ;; \
        *) echo "PETSC_VARIANT must be debug or release" >&2; exit 1 ;; \
    esac && \
    python3 ./configure \
        PETSC_ARCH="arch-linux-${PETSC_VARIANT}" \
        --prefix=/opt/petsc \
        --with-debugging="${debugging}" \
        --with-scalar-type=real \
        --with-precision=double \
        --with-shared-libraries=1 \
        --with-cxx-dialect=C++17 \
        --download-mpich \
        --download-fblaslapack \
        --download-hdf5 \
        --download-scalapack \
        --download-mumps \
        --download-superlu \
        COPTFLAGS="${optflags}" CXXOPTFLAGS="${optflags}" \
        FOPTFLAGS="${optflags}" && \
    make -j"$(nproc)" PETSC_DIR=/src/petsc PETSC_ARCH="arch-linux-${PETSC_VARIANT}" all && \
    make PETSC_DIR=/src/petsc PETSC_ARCH="arch-linux-${PETSC_VARIANT}" install

FROM ubuntu:${UBUNTU_VERSION} AS image-base

ARG DEBIAN_FRONTEND=noninteractive

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt/lists,sharing=locked \
    apt-get update && apt-get install -y --no-install-recommends \
        build-essential ca-certificates cmake git libeigen3-dev libgfortran5 \
        ninja-build pkg-config python3 valgrind

ENV PETSC_DIR=/opt/petsc \
    PATH=/opt/petsc/bin:${PATH} \
    PKG_CONFIG_PATH=/opt/petsc/lib/pkgconfig \
    CMAKE_PREFIX_PATH=/opt/petsc \
    LD_LIBRARY_PATH=/opt/petsc/lib

COPY containers/petsc/CMakeLists.txt containers/petsc/smoke.cpp /tmp/wavein-petsc-smoke/

FROM image-base AS image

ARG PETSC_VARIANT
COPY --from=petsc-build /opt/petsc /opt/petsc

RUN case "${PETSC_VARIANT}" in \
        debug) build_type=Debug ;; \
        release) build_type=Release ;; \
        *) echo "PETSC_VARIANT must be debug or release" >&2; exit 1 ;; \
    esac && \
    cmake -S /tmp/wavein-petsc-smoke -B /tmp/wavein-petsc-smoke-build \
        -G Ninja -DCMAKE_BUILD_TYPE="${build_type}" && \
    cmake --build /tmp/wavein-petsc-smoke-build --parallel && \
    cmake --install /tmp/wavein-petsc-smoke-build --prefix /usr/local && \
    mpiexec -n 1 /usr/local/bin/wavein-petsc-smoke && \
    valgrind --version && \
    rm -rf /tmp/wavein-petsc-smoke /tmp/wavein-petsc-smoke-build \
        /tmp/wavein-petsc-smoke.h5

CMD ["/bin/bash"]
