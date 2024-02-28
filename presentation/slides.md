---
marp: true
author: "Per-Magnus Holtmo"
title: "Using gRPC to fight Mordor"
header: "_header_"
footer: "_footer_"
#theme: gaia
#_class: lead
paginate: true
#backgroundColor: #fff
#backgroundImage: url('https://marp.app/assets/hero-background.svg')
---

# Using gRPC to fight Mordor
## Hands on approach to use gRPC with `C++`

_Per-Magnus Holtmo_

<!--
question - to start interact
payoff - what to learn
story - why trust me
-->

---

# Micro service architecture

![width:1000px](services.png)

<!--
how to setup safe, stable, efficient, easy to work with
connection handling and messaging
-->

---

# Some alternatives

- REST - HTTP/1 and json
- MQTT
- RabbitMqQ, ZeroMQ, Kafka,...
- Implement your own using raw sockets
- Some needs a broker process
- or...


<!--
lots of alternatives
REST is common, especially for web or cloud based systems
json is human readable but takes space, parsing not that efficient
-->

---

# We could use gRPC

- Created by Google, open source 2015
- An RPC framework supporting most languages
- Uses Google Protocol buffers for data serialization
- HTTP/2 as transport layer

<!--
today I gonna tell you about gRPC and how it works
It is a very large library, will cover the basics

I wil explain concept using code examples - a lot of code
-->

---

# 'g' stands for Google, right?

---

# No, 'g' stands for...

- **1.0** 'g' stands for '**gRPC**'
- **1.1** 'g' stands for '**good**'
- **1.2** 'g' stands for '**green**'
- **1.3** 'g' stands for '**gentle**'
...
- **1.59** 'g' stands for '**generative**'
- **1.60** 'g' stands for '**gjallarhorn**'
- **1.61** 'g' stands for '**grand**'
- **1.62** 'g' stands for '**guardian**'


<!--
Depending on the application of course, the first two are the most common. 1 send or get data/settings to/from server. 2 can be used to subscribe to events on the server, which I'll show later
-->

---

# Lord of the rings game

<!--
Lets make a simple game engine for lord of the rings
we must stop sauron and mordor from taking over middle earth

I will skip some details and focus on the basics for better understanding
all details ara available in GitHub if anyone wants more

Will focus on code instead of bullets
Please interrupt with questions
-->

- Server exposing a gRPC service
- Service interface library
- Client applications
- Synchronous and asynchronous examples
- And some streaming

---

# Interface library

- Define interface in a protocol buffers file: `.proto`
- Generate code
- Build a library

---

# Interface messages - lotr.proto

```proto
package lotr.proto;

message MordorPopulation {
    uint64 orc_count = 1;
    uint32 troll_count = 2;
    uint32 nazgul_count = 3;
    bool sauron_alive = 4;
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

# Protocol buffers

Generate code for supported languages, e.g. as C++:
```sh
protoc -I include/path --cpp-out build/proto lotr.proto
```
Output:  `lotr.pb.h` and `lotr.pb.cc`

Serialize
```cpp
lotr::proto::Weapon weapon;
weapon.set_name("Sting");
const auto data = weapon.SerializeAsString();
```
Deserialize

```cpp
lotr::proto::Weapon weapon;
weapon.ParseFromString(data);
const auto name = weapon.name();
```

<!--
string is normally used as data buffer, can use array and streams
-->

<!--
---


# SKIP ???

- Can be used independently of gRPC
- Messages are defined in `.proto` files
  -  int, float, string, byte arrays, array of other messages
- Includes plugins to generate code for various languages
- Serializes/deserialize messages size and time efficiently
- `protoc` can generate `name.pb.h/cc` and `name.grpc.pb.h/cc` files.

```cpp
auto data = msg.SerializeAsString();

```
-->


<!--
will not go into details how to use the protocompiler, give it the protofile and out path
it will generate include and impl files (google mock code optional)
I have it all in cmake, will show you link at the end

---
-->


<!--
Depending on the use case, if the list of messages is known, one could use unary RPC with one message
containing an array of other messages
-->
---

# Interface functions - RPC

gRPC makes additions to the `.proto` files to define a Service.


```proto
service LotrService {
    rpc mordor_population(google.protobuf.Empty) returns (MordorPopulation) {}
    rpc kill_orcs(Weapon) returns (AttackResult) {}
    rpc subscribeToStatus(google.protobuf.Empty) returns (stream GameStatus) {}
}
```

When running `protoc`

```sh
protoc --grpc_out=build/proto --plugin=protoc-gen-grpc=grpc_cpp_plugin lotr.proto
```
Output: `lotr.grpc.pb.h` and `lotr.grpc.pb.cc`


<!---
- Unary RPC
- Server streaming
- Client streaming
- Bidirectional streaming

no need to think about error codes, that is implicitly handled, will show later
-->

---

# Make a library with the interface code

Contains proto files and optional C++ helper types
Exposes needed headers for both server and client

```cmake
include(../cmake/grpc.cmake)

generate_proto_cpp(lotr-proto protos/lotr.proto)
```

<!---
- good practice
- used by both server and client
- can be given a version, generated at build time, can ask server for protocol version
- provides include paths to headers
-->

---

# Server and Service concepts

A **Server** exposes one or more **services** on a port

- Inherit from a generated service class (several options)
- Create a `grpc::Server` and add a service instance
- Start the server on the "ip:port" to listen on
- The server runs until shutdown by other thread
- Optionally enable reflection

```cpp
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
```
<!--
reflection: can make cli, query for service, methods, arguments and make calls
These are the basics, will make this step by step
-->
---

# Service definition

```cpp

#include <lotr.grpc.pb.h>

class SyncService : public proto::LotrService::Service
{
public:
    grpc::Status mordor_population(grpc::ServerContext* context,
                                   const google::protobuf::Empty* request,
                                   proto::MordorPopulation* response) override;
};
```
<!--
include generated file, subclass a service - many options, here the most simple for sync
override methods
-->
---

# Service implementation

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
CALLED FROM GRPC THREAD POOL, ensure to guard shared data
returned Status - error code and message, predefined error codes
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

# Time to make a client

- Generic client, holding gRPC _channel_ and _stub_
- Specialized client

---

# Generic client helper

```cpp
template<typename Service>
class GrpcClient
{
public:
    GrpcClient(const std::string& server_url)
      : m_channel{ grpc::CreateChannel(server_url,
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
- channel provides a connection to a gRPC server on a specified host and port
- stub is create on client side form the channel - provides function api
-->

---

# Client definition

```cpp
class SyncClient
{
public:
    SyncClient(std::string_view address, std::uint16_t port);

    std::optional<proto::MordorPopulation> population();

private:
    GrpcClient<proto::LotrService> m_grpc_client;
};
```

---

# Client implementation

<!--
```cpp
SyncClient::SyncClient(std::string_view address, std::uint16_t port)
  : m_grpc_client{ fmt::format("{}:{}", address, port) } {}
```
-->
```cpp
Stub::mordor_population(grpc::ClientContext* context,
                        const google::protobuf::Empty& request,
                        proto::MordorPopulation* response);
```

```cpp
std::optional<proto::MordorPopulation> SyncClient::population()
{
    grpc::ClientContext context;
    google::protobuf::Empty request;
    proto::MordorPopulation response;

    const auto status = m_grpc_client.stub().
        mordor_population(&context, request, &response);

    if (!status.ok()) {
        return std::nullopt;
    }

    return response;
}
```

<!--
CALL WILL BLOCK - synchronous
--->
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

# Demo

---

# But there is a problem...

<!--
- we make an attack, using best wapon, will just stand still waiting, waiting for the result, cannot move
- leaving us vulnerable
- we need to be able to move right after the attack
- we need asynchronous message handling
-->

---

# Asynchronous message handling

**Do not use the suggested async api!**

_But there is a callback interface..._

<!--
Way too much boiler plate
Completion queues, polling or waiting, threading etc etc, will not mention it

callback interface makes unary async calls very simple
stream calls still requires lot of extra code, maybe it will be improved as well
-->

---

# Asynchronous service definition

```cpp
class AsyncService : public proto::LotrService::CallbackService
{
public:
    AsyncService(boost::asio::io_context& context, const ServiceCallbacks& callbacks);

    grpc::ServerUnaryReactor* mordor_population(grpc::CallbackServerContext* context,
                                                const google::protobuf::Empty* request,
                                                proto::MordorPopulation* response) override;

    grpc::ServerUnaryReactor* kill_orcs(grpc::CallbackServerContext* context,
                                        const proto::Weapon* request,
                                        proto::AttackResult* response) override;
};
```

<!--
inherit from CallbackService
provide a boost asio context for threading/handover, callbacks to get data

new return types, return a reactor instead of status directly
new ServerContext
-->

---

# Asynchronous service implementation

```cpp
grpc::ServerUnaryReactor* AsyncService::mordor_population(grpc::CallbackServerContext* context,
                                                          const google::protobuf::Empty*,
                                                          proto::MordorPopulation* response)
{
    auto reactor = context->DefaultReactor();

    boost::asio::post(m_io_context, [reactor, response, this] {
        const auto pop = m_callbacks.population();

        response->set_orc_count(pop.orc_count);
        response->set_troll_count(pop.troll_count);
        response->set_nazgul_count(pop.nazgul_count);
        response->set_sauron_alive(pop.sauron_alive);

        reactor->Finish(grpc::Status::OK);
    });

    return reactor;
}
```

<!--
called by grpc thread, must return directly
for unary calls, there is a simple reactor to use, return directly, later call Finish() on it to
return to client

using boost asio to handover to main thread, access state
access state, set response, call Finish()

common setup, we could add helper function, we could check arguments, add error handling
will show in a bit

BUT there is a problem with this simple setup
When we are shutting down the server, the main thread is about to stop grpc server
there might come a call, post to asio context, will never be run and finish will never be called
-->

---

# UnaryExecutor

```cpp
class UnaryExecutor
{
public:
    explicit UnaryExecutor(boost::asio::io_context& context);

    void shutdown();

    grpc::ServerUnaryReactor* execute(grpc::CallbackServerContext* callback_context,
                                      const std::function<grpc::Status()>& work);

private:
    boost::asio::io_context& m_io_context;
    std::atomic<bool> m_block{ false };
};
```

---

# UnaryExecutor

```cpp
void UnaryExecutor::shutdown()
{
    m_block = true;
}

grpc::ServerUnaryReactor* UnaryExecutor::execute(grpc::CallbackServerContext* callback_context,
                                                 const std::function<grpc::Status()>& work)
{
    auto reactor = callback_context->DefaultReactor();

    if (m_block) {
        reactor->Finish(grpc::Status::CANCELLED);
        return reactor;
    }

    boost::asio::post(m_io_context, [reactor, work]() {
        const auto status = work();
        reactor->Finish(std::move(status));
    });

    return reactor;
}
```
<!--
Class, has a asio context and a blocking flag
execute(), provide function that returns gprc status
if block, set cancel and return
otherwise, call func and mark the result

Let's see how it can be used
-->
---

# UnaryExecutor usage

```cpp
grpc::ServerUnaryReactor* AsyncService::kill_orcs(grpc::CallbackServerContext* context,
                                                  const proto::Weapon* request,
                                                  proto::AttackResult* response)
{
    if (request->power() < 0 || request->power() > 1) {
        auto reactor = context->DefaultReactor();
        reactor->Finish({ grpc::StatusCode::INVALID_ARGUMENT, "Power must be between 0 and 1" });
        return reactor;
    }

    return m_executor.execute(context, [this, request, response]() {
        const auto result = m_callbacks.kill_orcs(request->name(), request->power());
        if (!result) {
            return grpc::Status{ grpc::StatusCode::INTERNAL, "Too late, Sauron has taken over" };
        }

        response->set_orcs_killed(result.value());
        return grpc::Status::OK;
    });
}
```
<!--
In our overridden function,
first quick check of arguments
the call execute(), providing a simple function, only focusing on the state handling
 - called on main thread
 - check result
 - fill response
-->

---

# Async client definition

```cpp
class AsyncClient
{
public:
    AsyncClient(boost::asio::io_context& context,
                std::string_view address,
                std::uint16_t port);

    using KillHandler = std::function<void(const grpc::Status&, std::uint64_t)>;

    void kill_orcs(std::string_view weapon_name, float power, KillHandler handler);

private:
    boost::asio::io_context& m_io_context;
    GrpcClient<proto::LotrService> m_grpc_client;
};
```

<!--
similar to the sync client
here we use boost asio to do the work

here we have void functions, but provide a callback

-->

---

# gRPC client callback interface

```cpp
Stub::async::kill_orcs(grpc::ClientContext* context,
                       const proto::Weapon* request,
                       proto::AttackResult* response,
                       std::function<void(grpc::Status)> f);
```

```cpp
template<typename Request, typename Response>
struct ClientState
{
    grpc::ClientContext context;
    Request request;
    Response response;
};
```

<!--
for the sync case, we used function on the stub directly
and there is an async object where the callback functions hide

provide a context, request, response, and a callback with the status result
this func will return directly, but we need to keep track of the provided objects
they muse be kept in memory until the callback is called. And there may be many calls before
the first returns....

so that is the state we must store util done
-->
---

# Async client implementation

```cpp
void AsyncClient::kill_orcs(std::string_view weapon_name, float power, KillHandler handler)
{
    auto state =
      std::make_shared<ClientState<proto::Weapon, proto::AttackResult>>();

    state->request.set_name(std::string(weapon_name));
    state->request.set_power(power);

    m_grpc_client.stub().async()->kill_orcs(
      &state->context,
      &state->request,
      &state->response,
      [&io_context = m_io_context, state, handler = std::move(handler)](grpc::Status status) {
          boost::asio::post(
            io_context,
            [handler = std::move(handler), status, orcs_killed = state->response.orcs_killed()]() {
                handler(status, orcs_killed);
            });
      });
}
```

<!--
Create the state as a shared pointer
- set request values, optional context options

call function, provide pointers to objects
and a lambda for the status result, capture the state in a lambda to keep it alive, handler

callback called by grpc, must handover to our main thread, here using boost asio again

call handler with result on main thread
client does not have to bother about threading
as soon as post calls returns, state object is deleted, no more references left

-->

---

# Message streaming

- Unary calls basically all you need
  - Send large arrays of messages
  - Get state
- But some kind of event notification would be nice

<!--
---

# Some streaming


documented case
client make a call and expects a fixed number of messages back during a short period of time
then the stream is closed.
can be solved with a message holding repeated field for a large list of messages

Design choice - one large message or several small, not very interesting to show

more interesting could be
client subscribes to server, expecting events at an interval or at state change
connected forever, or reconnects at failure

you would also know that the server is running, otherwise the subscription fails
and you could try to resubscribe
Could have any number of clients connected, all would get the same status

maybe show simplest writer/reader
no specific error handling
and mentions the problems
READER: destructor must wait for on done, conditional_variable, mutex etc

TODO: MAKE SIMPLER WRITER

C++ how to wait until variable is set by other thread

and we have several clients connected (must copy message)

SENDER: must hold on to message, until sent, could move message, but must support many clients, keep until all done
- how to send new message when prev is not sent, skip, buffer (how large), are you sending regularly or just at change
-->

---

# Server streaming

**RPC definition**
```proto
service LotrService {
    rpc subscribeToStatus(google.protobuf.Empty) returns (stream GameStatus) {}
}
```

**Method to override**
```cpp
grpc::ServerWriteReactor<proto::GameStatus>* subscribeToStatus(
      grpc::CallbackServerContext* context,
      const google::protobuf::Empty* request) override;
```

<!--
send stream example, client requests a set of messages, server sends as fast as possible, then done
might be use cases, SEE ABOVE
more interesting to turn info subscription, kept a live forever
no in arg

we return a writerReactor - for unary calls, use the default, simple, but for streams, we must impl
our own

no response variable - use the writeReactor

-->

---

# Server streaming

* `MessageWriter` - writes one message at a time
* `StreamWriter` - handles several clients
* Update our service

---

<!--
will skip shutdown, will take too long time
but as I said, shutting down is the hard part, will have that code available on GitHub
-->

# MessageWriter

```cpp
template<typename Message>
class MessageWriter : public grpc::ServerWriteReactor<Message>
{
public:
    MessageWriter(boost::asio::io_context& io, const DoneCallback& done)

    void send_message(const Message& msg)

    void OnWriteDone(bool ok) override
    void OnDone() override
    void OnCancel() override
};
```

---

# MessageWriter sending messages

```cpp
void send_message(const Message& msg) {
    std::scoped_lock<std::mutex> lock(m_message_mutex);
    if (m_message || m_done) {
        return;
    }
    m_message = msg;
    StartWrite(&m_message.value());
}

void OnWriteDone(bool) override
{
    std::scoped_lock<std::mutex> lock(m_message_mutex);
    m_message.reset();
}
```

<!--
---

# MessageWriter shutting down


```cpp
void shutdown()
{
    if (!m_done) {
        this->Finish(grpc::Status(grpc::ABORTED, "Service shutdown"));
        m_done = true;
    }
}

void OnDone() override
{
    this->m_context.post([this]() { m_done_callback(); });
}

void OnCancel() override
{
    m_done = true;
    this->Finish(grpc::Status::CANCELLED);
}
```
-->
---

# StreamWriter to handle multiple clients

```cpp
template<typename Message>
class StreamWriter
{
public:
    grpc::ServerWriteReactor<Message>* create_writer();
    void send_message(const Message& msg)

private:
    std::vector<Writer> m_writers;
};
```
---

# StreamWriter sending a message

```cpp
grpc::ServerWriteReactor<Message>* StreamWriter::create_writer()
{
    ++m_id;
    auto& w = m_writers.emplace_back(
                std::make_unique<MessageWriter<Message>>(...), m_id);

    return w.writer.get();
}

void StreamWriter::send_message(const Message& msg)
{
    std::ranges::for_each(m_writers,
        [&msg](auto& w) { w.writer->send_message(msg); });
}
```
<!--
There is some more thread handling required,
create_writer() called on gRPC thread, protect vector and block at shutdown
-->

<!--

---

# StreamWriter shutting down


```cpp
void StreamWriter::shutdown()
{
    std::ranges::for_each(m_writers, [](auto& w) { w.writer->shutdown(); });
    while (!m_writers.empty()) {
        m_context.run_one_for(std::chrono::milliseconds{ 10 });
    }
}

void StreamWriter::remove_writer(std::uint32_t id)
{
    const auto it = std::ranges::find_if(m_writers,
        [id](const auto& w) { return w.id == id; });

    if (it != m_writers.end()) {
        m_writers.erase(it);
    }
}
```
-->

---

# StreamWriter usage

```cpp
grpc::ServerWriteReactor<proto::GameStatus>* AsyncService::subscribeToStatus(..)
{
    return m_status_writer.create_writer();
}

void AsyncService::send_status(const GameStatus& s)
{
    proto::GameStatus status;

    status.set_mordor_strenght(s.mordor_strength);
    status.set_gondor_strenght(s.gondor_strength);
    status.set_orc_count(s.orc_count);

    m_status_writer.send_message(status);
}
```
---

# How to read streams

Generated client method
```cpp
void Stub::async::subscribeToStatus(grpc::ClientContext* context,
                                    const google::protobuf::Empty* request,
                                    grpc::ClientReadReactor<proto::GameStatus>* reactor)
```

---

# StreamReader - reading one message at a time

```cpp
template<typename Message>
class StreamReader : public grpc::ClientReadReactor<Message>
{
public:
    StreamReader(MessageHandler message_handler, ...)

    void start();
    grpc::ClientContext* client_context();

    void OnReadDone(bool ok) override;
    void OnDone(const grpc::Status& status) override;

private:
    Message m_message;
};
```

---

# StreamReader message reading

```cpp
void StreamReader::start()
{
    StartCall();
    StartRead(&m_message);
}

void StreamReader::OnReadDone(bool ok) override
{
    if (ok) {
        boost::asio::post(m_io_context, [this]() {
            m_message_handler(m_message);
            StartRead(&m_message);
        });
    }
}
```

---

# StreamReader usage

```cpp
void AsyncClient::subscribeToStatus()
{
    m_status_reader = std::make_unique<StreamReader<proto::GameStatus>>(
        m_io_context,
        [](const proto::GameStatus& status) {
            // ...
        },
        [this](grpc::Status status) {
            // ...
            m_status_reader.reset();
        });

    m_grpc_client.stub().async()->subscribeToStatus(
        m_status_reader->client_context(),
        &empty_proto_msg,
        m_status_reader.get());

    m_status_reader->start();
}
```

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

---

# Not covered


- Clients in other languages, e.g. Python or Typescript
- Authentication and encryption
- there are more lifetime issues at shutdown that are hard to handle due to multithreading

---

# Are we connected?

- `gRPC` keep alive channel option
- Query channel if connected
- Use any `gRPC` message and check response status


<!--
Interesting question, how to know if you are connected with the service?

there are keep alive options (internal pings with configurable intervals and timeouts)
or you can ask channel if connected - very slow 5 sec to connect
or having an own unary call which is very fast
BUT if you know NOW we are connected, everything is fine, then when you do something,
connection might have been lost - so it does not say that much
=> ensure your services are always running (systemd or other means) and proper error handling

-->


---

# Thank you

Source code:
- link to GitHub
