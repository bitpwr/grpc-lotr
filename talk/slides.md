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

# Interface definition - Messages

show proto messages

<!--
package -> namespace, maybe keep app nanmespace and add one level
-->

---

# protobuffers

- serializes data, structs , int, floats, strings, byte arrays, and list of other messages

protoc explain

- plugins to generate code for various langages
- generates c++, h, in build step, normally not checked in, could be if you want to keep track of changes, but .proto is the source

show protoc line

explain what is generated

---

# Interface definition - RPC

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

- subclass generated service class (several options)
- create a `grpc::Server` and add service
- Start the server on the "ip:port" to listen on
- The server runs until shutdown by other thread - create a new thread for the server

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
---

# gRPC Server implementation

```cpp
GrpcServer::GrpcServer(grpc::Service& service, std::string_view address, uint16_t port)
  : m_server_thread{ [this, &service, address, port]() {
      run(service, fmt::format("{}:{}", address, port));
  } } {}
```

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

---

# gRPC Server implementation, cont

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

# Client


---

# impl



---

# Not covered

- Authentication and encryption

---

# Thank you

- link to GitHub
- email?
