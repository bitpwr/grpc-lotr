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

---

# Messaging options

- Unary RPC, simple request reply (get/set)
- Server streaming, client sends one request, server responds with a stream to read several messages from, until server marks the end
- Client streaming, client writes a sequence of messages and wait for server to handle all.
- Bidirectional streaming, provides independent read/write streams.

<!--
Depending on the application of course, the first two are the most common. 1 send or get data/settings to/from server. 2 can be used to subscribe to events on the server, which I'll show later
-->

---

# Thank you

- link to GitHub
- email?
