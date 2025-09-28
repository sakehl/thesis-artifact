FROM ubuntu:22.04

WORKDIR /haliver

RUN apt update && apt install -y --no-install-recommends \
           ca-certificates build-essential curl git wget unzip \
           cmake clang-11 ninja-build zlib1g-dev llvm-11-dev \
           libclang-11-dev liblld-11 liblld-11-dev \
           openjdk-17-jre-headless \
           texlive-latex-recommended \
           elan

# Get VerCors
COPY vercors vercors

RUN cd vercors && bin/vct --version
ENV PATH="$PATH:/haliver/vercors/bin"
# Fix clang for VerCors
RUN ln -s /bin/clang-11 /bin/clang

# Get HaliVer/Halide
COPY Halide Halide

# Build HaliVer/Halide
RUN cd Halide && mkdir -p build && \
    cmake -G Ninja \
    -DWITH_TESTS=NO -DWITH_AUTOSCHEDULERS=NO -DWITH_PYTHON_BINDINGS=NO -DWITH_TUTORIALS=NO -DWITH_DOCS=NO -DCMAKE_BUILD_TYPE=Release \
    -DTARGET_AARCH64=NO -DTARGET_AMDGPU=NO -DTARGET_ARM=NO -DTARGET_HEXAGON=NO -DTARGET_MIPS=NO -DTARGET_NVPTX=NO -DTARGET_POWERPC=NO \
    -DTARGET_RISCV=NO -DTARGET_WEBASSEMBLY=NO \
    -DHalide_REQUIRE_LLVM_VERSION=11.1.0 -Wno-dev -DLLVM_PACKAGE_VERSION=11.1.0 -DLLVM_DIR=/usr/lib/llvm-11/lib/cmake/llvm \
    -S . -B build && cmake --build build

# Install HaliVer/Halide
RUN mkdir -p Halide/install && cmake --install Halide/build --prefix Halide/install


# Install Lean 4 via elan
RUN curl https://raw.githubusercontent.com/leanprover/elan/master/elan-init.sh -sSf | sh -s -- -y
ENV PATH="/root/.elan/bin:$PATH"

# Set up Lean toolchain
RUN /root/.elan/bin/elan default leanprover/lean4:v4.11.0-rc1

# Copy Lean project
COPY QuantifierLean QuantifierLean
WORKDIR /haliver/QuantifierLean

# Build Lean project dependencies
RUN /root/.elan/bin/lake exe cache get && /root/.elan/bin/lake build


# Copy HaliVer experiments
COPY HaliVerExperiments HaliVerExperiments
WORKDIR /haliver/HaliVerExperiments
RUN mkdir -p build

WORKDIR /haliver