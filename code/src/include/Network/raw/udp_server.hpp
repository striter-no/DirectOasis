// g++ -o udp_server udp_server.cpp -lboost_system -lpthread

#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <functional>
#include <string>

class UDPServer {
        bool logging;
    public:
        UDPServer(
            const std::string& host, 
            int port, 
            std::size_t data_size = 1024, 
            bool logging = false
        ): socket_(io_service_, boost::asio::ip::udp::endpoint(boost::asio::ip::make_address(host), port)),
            data_size_(data_size), 
            logging(logging) {}

        void start() {
            if (logging) std::cout << "UDP server listening on " << socket_.local_endpoint().address().to_string() << ":" << socket_.local_endpoint().port() << std::endl;
            listen();
        }

        void stop(){
            socket_.close();
        }

        void set_on_message_callback(std::function<std::string(boost::asio::ip::udp::endpoint, const std::string&)> callback) {
            on_message_ = callback;
        }

        void set_on_error_callback(std::function<void(boost::asio::ip::udp::endpoint, const std::exception&)> callback) {
            on_error_ = callback;
        }

    private:
        void listen() {
            while (true) {
                try {
                    if (logging) std::cout << "[^] Waiting for incoming message..." << std::endl;
                    boost::asio::ip::udp::endpoint sender_endpoint;
                    std::vector<char> recv_buffer(data_size_);
                    std::size_t len = socket_.receive_from(boost::asio::buffer(recv_buffer), sender_endpoint);

                    if (len > 0) {
                        std::string message(recv_buffer.data(), len);
                        if (logging) std::cout << "[^] Received message from " << sender_endpoint << std::endl;
                        handle_client(message, sender_endpoint);
                    }
                } catch (const std::exception& e) {
                    if (on_error_) {
                        on_error_(boost::asio::ip::udp::endpoint(), e);
                    }
                    break;
                }
            }
        }

        void handle_client(const std::string& message, boost::asio::ip::udp::endpoint sender_endpoint) {
            try {
                std::string response = "no-callback-set";
                if (on_message_) {
                    response = on_message_(sender_endpoint, message);
                } else {
                    throw std::runtime_error("No callback set for on_message");
                }

                if (logging) std::cout << "Sending message back to " << sender_endpoint << ": \n" << response << "\n" << std::string(20, '-') << std::endl;
                socket_.send_to(boost::asio::buffer(response), sender_endpoint);
            } catch (const std::exception& e) {
                if (logging) std::cout << "[!] Error: " << e.what() << std::endl;
                if (on_error_) {
                    on_error_(sender_endpoint, e);
                }
            }
        }

        boost::asio::io_service io_service_;
        boost::asio::ip::udp::socket socket_;
        std::size_t data_size_;
        std::function<std::string(boost::asio::ip::udp::endpoint, const std::string&)> on_message_;
        std::function<void(boost::asio::ip::udp::endpoint, const std::exception&)> on_error_;
};