# C++ gRPC examples inspired by Lord of the Rings

Simple applications to learn [gRPC](https://grpc.io/) with C++. The server interface is built
into a separate library, used by both the server and the clients. Contains both synchronous and
asynchronous examples as well as server streaming support.

## Folder structure

- `lotr` - application hosting gRPC services.
- `lotr-proto` - library with the gRPC interface.
- `sync-client` - application with the most basic synchronous client.
- `async-client` - asynchronous application with streaming support.
- `utils` - library with some common types.
- `cmake` - some CMake helpers.
- `presentation` - slides describing the gRPC concepts used.

## Prerequisites

We use the [Conan package manager](https://conan.io/) to get the required third parties,
such as `gRPC`, `boost` and `fmt`.

## Setup

We need python to use conan.

```sh
python3 -m venv penv
source penv/bin/activate
pip install -r requirements.txt
```

Install the external libraries.

```sh
./install_deps.sh
```

## Build

Build all applications

```sh
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../deps/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release ..
make -j8
```
and run the server

```sh
./bin/lotr
```
and the clients

```sh
./bin/sync-client
```

```sh
./bin/async-client
```

Game on!
