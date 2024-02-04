---
marp: true
author: "Per-Magnus Holtmo"
title: "gRPC stuff"
header: "**header**"
footer: "_footer_"
#theme: gaia
#_class: lead
paginate: true
#backgroundColor: #fff
#backgroundImage: url('https://marp.app/assets/hero-background.svg')
---

# gRPC for "embedded" devices

---

# How to set up efficient communication

Image with micro services

<!--
try drawio app
how to setup safe, stable, efficient, easy to work with
connection handling and messaging
-->

---

# Some alternatives

- REST - http 1 and json
- MQTT - needs a broker
- RabbitMq, ZeroMq, Stream...
- Implement your own using raw sockets
- Some needs a broker process
- or...


<!--
REST is common, especially for web or cloud based systems
json is human readable but takes space, parsing not that efficient
-->

---

# We could use gRPC

- Created by Google 2015
- An RPC framework supporting most languages
- Uses Google Protocol buffers for data serialization
- Uses http 2 as transport layer

<!--
today I gonna tell you about gRPC and how it works
It is a very large library, will cover the basics
-->

---

# We all think 'g' stands for Google

---

# No, 'g' stands for...

- **1.0** 'g' stands for 'gRPC'
- **1.1** 'g' stands for 'good'
- **1.2** 'g' stands for 'green'
- **1.3** 'g' stands for 'gentle'
...
- **1.59** 'g' stands for 'generative'
- **1.60** 'g' stands for 'gjallarhorn'
- **1.61** 'g' stands for 'grand'
- **1.62** 'g' stands for 'guardian'

---

# Advantages

- an API contract, typed function calls (REST normally uses json strings)
- arguments are efficiently serialized using protobuf
- code generation for messages and server/client stubs in "any" language. (can be different)
- support implementation wither side in "any" language
- supports streaming, i.e. one end sends messages continuously
- http/2 gives more efficient and long lived connections, compared to http/1. gives TLS support if required

---

# Disadvantages

- complex and large code base
- not the best documentation
- several ways to do the same thing
- synchronous/asynchronous
here I gonna explain one way with asynchronous support
- requires large code base
- build, conan,
- requires http2 - cannot directly access from most browsers, need to run a proxy that converts http/1 <> htt/2


<!--
Depending on the application of course, the first two are the most common. 1 send or get data/settings to/from server. 2 can be used to subscribe to events on the server, which I'll show later
-->

---

# Interface definition - Protobuf messages


```proto
syntax = "proto3";

package lotr.proto;

message MordorPopulation {
    uint64 orc_count = 1;
    uint32 troll_count = 2;
    uint32 nazgul_count = 3;
}

message Weapon {
    string name = 1;
    float power = 2;
}

message AttackResult {
    uint64 orcs_killed = 1;
    uint32 trolls_killed = 2;
    uint32 nazguls_killed = 3;
}
```


<!--
gRpc uses google protobuf
package -> namespace,
message -> classed with getters and setters
add repeated field
-->

---

# protobuffers

- Can be used independently of gRPC
- Messages are defined in `.proto` files
  -  int, float, string, byte arrays, array of other messages
- Includes plugins to generate code for various languages
- Serializes/deserialize messages size and time efficiently

protoc explain

- generates c++, h, in build step, normally not checked in, could be if you want to keep track of changes, but .proto is the source

show protoc line

explain what is generated

---

# Interface definition - RPC

gRPC makes additions to the `.proto` files

```proto
service LotrService {
    rpc mordor_population(google.protobuf.Empty) returns (MordorPopulation) {}
    rpc kill_orcs(Weapon) returns (AttackResult) {}
    rpc subscribeToStatus(google.protobuf.Empty) returns (stream Status) {}
}
```

---

**Messaging options**

- Unary RPC, simple request reply (get/set)
- Server streaming, client sends one request, server responds with a stream to read several messages from, until server marks the end
- Client streaming, client writes a sequence of messages and wait for server to handle all.
- Bidirectional streaming, provides independent read/write streams.

```
show service definition
```

---

# Put in library

- proto -> cpp -> lib
- used by both server and clinet
- defines the interface
- use version


---

# Server concept

- Subclass generated service class (several options)
- Create a `grpc::Server` and add a service instance
- Start the server on the "ip:port" to listen on
- The server runs until shutdown by other thread
- Optionally enable reflection

```cpp
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
```
<!--
reflection: can make cli, query for service, methods, arguments and make calls
-->
---

# Synchronous service definition

```cpp

#include "lotr.grpc.pb.h"

class SyncService : public proto::LotrService::Service
{
public:
    SyncService() = default;

    grpc::Status mordor_population(grpc::ServerContext* context,
                                   const google::protobuf::Empty* request,
                                   proto::MordorPopulation* response) override;
};
```
<!--
include generated file, subclass a service - many options, here the most simple for sync
override methods
server context needed for async only
-->
---

# Synchronous service implementation

```cpp
grpc::Status SyncService::mordor_population(grpc::ServerContext*,
                                            const google::protobuf::Empty*,
                                            proto::MordorPopulation* response)
{
    response->set_orc_count(2'345'543);
    response->set_troll_count(7'540);
    response->set_nazgul_count(9);

    return grpc::Status::OK;
}
```

<!--
for sync - skip context
no care about the empty request
returned Status - error code and message, predefined error codes
Called from a thread pool in grpc, ensure to guard shared data
-->

---

# We need a Server to host the service


```cpp
class GrpcServer
{
public:
    GrpcServer(grpc::Service& service, std::string_view address, uint16_t port);

    void shutdown();

private:
    void run(grpc::Service& service, const std::string& listening_uri);

    std::thread m_server_thread;
    std::unique_ptr<grpc::Server> m_server;
};
```
<!--
Generic, can be reused for any service
-->

---

# gRPC Server implementation

```cpp
GrpcServer::GrpcServer(grpc::Service& service, std::string_view address, uint16_t port)
  : m_server_thread{ [this, &service, address, port]() {
      run(service, fmt::format("{}:{}", address, port));
  } } {}
```

---

# gRPC Server implementation, cont

```cpp
void GrpcServer::run(grpc::Service& service, const std::string& listening_uri)
{
    grpc::ServerBuilder builder;
    builder.AddListeningPort(listening_uri, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    m_server = builder.BuildAndStart();
    m_server->Wait();
}
```
```cpp
void GrpcServer::shutdown()
{
    if (m_server) {
        m_server->Shutdown();
        m_server_thread.join();
    }
}
```
---

# Generic client definition

```cpp
template<typename Service>
class GrpcClient
{
public:
    GrpcClient(const std::string& server_address)
      : m_channel{ grpc::CreateChannel(server_address,
                   grpc::InsecureChannelCredentials()) }
      , m_stub{ Service::NewStub(m_channel) } {}

    Service::Stub& stub() { return *m_stub.get(); }

private:
    std::shared_ptr<grpc::Channel> m_channel;
    std::unique_ptr<typename Service::Stub> m_stub;
};
```

<!--
Generic, can be reused for any service
explain channel and stub
-->

---

# Sync client definition

```cpp
class SyncClient
{
public:
    SyncClient(std::string_view address, std::uint16_t port);

    std::optional<lotr::proto::MordorPopulation> population();

private:
    com::GrpcClient<lotr::proto::LotrService> m_grpc_client;
};
```

---

# Sync client impl

```cpp
SyncClient::SyncClient(std::string_view address, std::uint16_t port)
  : m_grpc_client{ fmt::format("{}:{}", address, port) } {}
```

```cpp
std::optional<lotr::proto::MordorPopulation> SyncClient::population()
{
    google::protobuf::Empty request;
    lotr::proto::MordorPopulation response;
    grpc::ClientContext context;

    const auto status = m_grpc_client.stub().
        mordor_population(&context, request, &response);

    if (!status.ok()) {
        return std::nullopt;
    }

    return response;
}
```
<!--
can use context to set deadlines and other meta data
    context.set_deadline(std::chrono::system_clock::now() + 2s);
-->

---

# ClientContext options

Set deadline, how long to wait until call is aborted
```cpp
context.set_deadline(std::chrono::system_clock::now() + 2s);
```

Compression
```cpp
context.set_compression_algorithm(GRPC_COMPRESS_GZIP);
```

Add meta data, http header key value pairs
```cpp
context.AddMetadata("custom-header", "Custom Value");
```

And credentials, cancellation, keep alive, "wait for ready", etc...

---

# Same with callbacks, async

---

# Some streaming

---

# Same as done on neti ???

---

# Python client

---

# Typescript client

---

# Not covered

- Authentication and encryption

---

# Thank you

- link to GitHub
- email?
