# gRPC goes LOTR

Simple applications to learn [gRPC](https://grpc.io/).

## Folder structure

- lotr - application hosting `gRPC` services.
- lotr-proto - library with the `gRPC` interface.
- sync-client - application with the most basic synchronous client.
- utils - library with some common types.

## Prerequisites

We use [Conan package manager](https://conan.io/) to get the required third parties.

## Setup

``` sh
python3 -m venv penv
source penv/bin/activate
pip install -r requirements.txt
./install_deps.sh
```

## Build

``` sh
mkdir build-debug
cd build-debug
cmake .. -DCMAKE_TOOLCHAIN_FILE=../deps/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
make -j8
```
and run

``` sh
./bin/lotr
```

``` sh
./bin/sync-client
```
