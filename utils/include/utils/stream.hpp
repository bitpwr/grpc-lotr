#pragma once

#include <algorithm>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <fmt/format.h>
#include <grpcpp/support/server_callback.h>
#include <mutex>
#include <optional>
#include <vector>

namespace utils {

using DoneCallback = std::function<void()>;

/// Writes messages over a stream from server to one client
/// grpc::ServerWriteReactor<Message> should be internally hidden but
/// kept as a public inheritance for simplicity
template<typename Message>
class MessageWriter : public grpc::ServerWriteReactor<Message>
{
public:
    MessageWriter(boost::asio::io_context& context, const DoneCallback& done)
      : m_context{ context }
      , m_done_callback{ done }
    {
    }
    MessageWriter(const MessageWriter&) = delete;
    MessageWriter& operator=(const MessageWriter&) = delete;

    void shutdown()
    {
        if (!m_done) {
            this->Finish(grpc::Status(grpc::ABORTED, "Service shutdown"));
            m_done = true;
        }
    }

    /// Sends a message to a client.
    void send_message(const Message& msg)
    {
        std::scoped_lock<std::mutex> lock(m_message_mutex);
        if (m_message || m_done) {
            fmt::print("Failed to add message '{}' to write queue\n",
                       Message::descriptor()->full_name());
            return;
        }
        m_message = msg;
        this->StartWrite(&m_message.value());
    }

    /// called when a single message has been sent to the client
    void OnWriteDone(bool ok) override
    {
        std::scoped_lock<std::mutex> lock(m_message_mutex);
        if (!ok) {
            fmt::print("Failed to write message '{}'\n", Message::descriptor()->full_name());
        }
        m_message.reset();
    }

    /// called when writer is done with all messages after beeing aborted for some reason
    void OnDone() override
    {
        this->m_context.post([this]() { m_done_callback(); });
    }

    /// called when client side has aborted for some reason
    void OnCancel() override
    {
        m_done = true;
        this->Finish(grpc::Status::CANCELLED);
    }

private:
    boost::asio::io_context& m_context;
    DoneCallback m_done_callback;
    std::atomic<bool> m_done{ false };
    std::optional<Message> m_message;
    std::mutex m_message_mutex;
};

/// Handles gRPC calls returning streams of a given type Message
template<typename Message>
class StreamWriter
{
public:
    explicit StreamWriter(boost::asio::io_context& context)
      : m_context{ context }
    {
    }
    StreamWriter(const StreamWriter&) = delete;
    StreamWriter& operator=(const StreamWriter&) = delete;

    /// Stops all writers and waits until all are removed
    void shutdown()
    {
        m_done = true;

        {
            std::scoped_lock<std::mutex> lock(m_writers_mutex);

            // must wait for all writer to be "done" and removed
            // better with conditional variable but keep it simple
            std::ranges::for_each(m_writers, [](auto& w) { w.writer->shutdown(); });
        }

        while (!m_writers.empty()) {
            m_context.run_one_for(std::chrono::milliseconds{ 10 });
        }
    }

    /// @brief Returns the number of connected clients
    /// @return Number of connected clients
    size_t size() const { return m_writers.size(); }

    /// @brief Creates a stream writer
    /// @return  A writer reactor te be returned in the gRPC call
    grpc::ServerWriteReactor<Message>* create_writer()
    {
        if (m_done) {
            return nullptr;
        }

        std::scoped_lock<std::mutex> lock(m_writers_mutex);
        ++m_id;
        auto& w = m_writers.emplace_back(std::make_unique<MessageWriter<Message>>(
                                           m_context, [this, id = m_id]() { remove_writer(id); }),
                                         m_id);

        return w.writer.get();
    }

    /// @brief Sends a message to all connected clients
    /// @param msg The message to send
    void send_message(const Message& msg)
    {
        std::ranges::for_each(m_writers, [&msg](auto& w) { w.writer->send_message(msg); });
    }

private:
    void remove_writer(std::uint32_t id)
    {
        std::scoped_lock<std::mutex> lock(m_writers_mutex);
        const auto it = std::ranges::find_if(m_writers, [id](const auto& w) { return w.id == id; });

        if (it == m_writers.end()) {
            fmt::print("Failed to find MessageWriter<{}> ({}) to remove\n",
                       Message::descriptor()->full_name(),
                       id);
        }

        m_writers.erase(it);
    }

    struct Writer
    {
        std::unique_ptr<MessageWriter<Message>> writer;
        std::uint32_t id;
    };

    boost::asio::io_context& m_context;
    std::vector<Writer> m_writers;
    std::uint32_t m_id{};
    std::atomic<bool> m_done{ false };
    std::mutex m_writers_mutex;
};

/// Used with gRPC calls returning streams of a given type Message
/// The grpc::ClientReadReactor should be internally hidden
/// but using public inheritance for simplicity
template<typename Message>
class StreamReader : public grpc::ClientReadReactor<Message>
{
public:
    using MessageHandler = std::function<void(const Message&)>;
    using DoneHandler = std::function<void(grpc::Status)>;

    /// @param message_handler  function called for every message
    /// @param done_handler     function called when server is done or at errors
    StreamReader(boost::asio::io_context& io_context,
                 MessageHandler message_handler,
                 DoneHandler done_handler)
      : m_io_context{ io_context }
      , m_message_handler{ std::move(message_handler) }
      , m_done_handler{ std::move(done_handler) }
    {
    }

    // d'tor should wait for OnDone if running
    ~StreamReader() = default;

    void start()
    {
        this->StartCall();
        this->StartRead(&m_message);
    }
    void shutdown() { m_client_context.TryCancel(); }

    grpc::ClientContext* client_context() { return &m_client_context; }

    void OnReadDone(bool ok) override
    {
        if (!ok) {
            return;
        }

        boost::asio::post(m_io_context,
                          [this, msg = std::move(m_message)]() { m_message_handler(msg); });
        this->StartRead(&m_message);
    }
    void OnDone(const grpc::Status& status) override
    {
        boost::asio::post(m_io_context, [this, status]() { m_done_handler(std::move(status)); });
    }

private:
    boost::asio::io_context& m_io_context;
    MessageHandler m_message_handler;
    DoneHandler m_done_handler;
    grpc::ClientContext m_client_context;
    Message m_message;
};

} // namespace utils
