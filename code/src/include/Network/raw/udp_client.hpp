#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

class UDPClient {
public:
    typedef function<void(const string&)> MessageCallback;
    typedef function<void(const boost::system::error_code&)> ErrorCallback;

    UDPClient(const string& server_ip, unsigned short server_port, 
             size_t data_size = 1024, bool logging = false)
        : socket_(io_context_, udp::v4()),
          remote_endpoint_(address::from_string(server_ip), server_port),
          data_size_(data_size),
          logging_(logging),
          work_guard_(io_context_) {
        
        socket_.connect(remote_endpoint_);
        recv_buffer_.resize(data_size);
    }

    ~UDPClient() {
        disconnect();
    }

    void connect() {
        if (logging_) cout << "[*] Connecting to server..." << endl;
        io_thread_ = thread([this]() { io_context_.run(); });
        start_receive();
    }

    void disconnect() {
        io_context_.stop();
        if (io_thread_.joinable()) io_thread_.join();
        socket_.close();
    }

    void send(const string& data) {
        socket_.async_send_to(buffer(data), remote_endpoint_,
            [this](const boost::system::error_code& error, size_t /*bytes_sent*/) {
                if (error && on_error_) on_error_(error);
            });
    }

    string send_and_wait(const string& data) {
        if (logging_) cout << "[^] Sending data" << endl;
        send(data);
        
        unique_lock<mutex> lock(queue_mutex_);
        cv_.wait(lock, [this]{ return !data_queue_.empty(); });
        
        string result = data_queue_.front();
        data_queue_.pop();
        return result;
    }

    void set_on_received_message(MessageCallback callback) { on_message_ = callback; }
    void set_on_error(ErrorCallback callback) { on_error_ = callback; }

private:
    void start_receive() {
        socket_.async_receive_from(
            buffer(recv_buffer_), sender_endpoint_,
            [this](const boost::system::error_code& error, size_t bytes_received) {
                handle_receive(error, bytes_received);
            });
    }

    void handle_receive(const boost::system::error_code& error, size_t bytes_received) {
        if (!error) {
            string message(recv_buffer_.data(), bytes_received);
            
            {
                lock_guard<mutex> lock(queue_mutex_);
                data_queue_.push(message);
            }
            cv_.notify_one();

            if (on_message_) on_message_(message);
            start_receive();
        } 
        else if (on_error_) on_error_(error);
    }

    io_context io_context_;
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    udp::endpoint sender_endpoint_;
    std::vector<char> recv_buffer_;
    // std::array<char, 1024> recv_buffer_;
    
    size_t data_size_;
    bool logging_;
    
    queue<string> data_queue_;
    mutex queue_mutex_;
    condition_variable cv_;
    
    MessageCallback on_message_;
    ErrorCallback on_error_;
    
    thread io_thread_;
    io_context::work work_guard_;
};