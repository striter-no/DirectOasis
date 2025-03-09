#include <utils/vector.hpp>
#include <utils/string.hpp>
#include <optional>
#include <sstream>
#include <iomanip>
#include <Network/raw/udp_server.hpp>
#include "prot.hpp"

using addr_t = boost::asio::ip::udp::endpoint;

class Broadcast {
    public:

        Message message;
        bool logging;
        std::string author_uid;
        std::vector<std::string> targets;
        std::vector<std::string> alr_send;

        Broadcast(
            const std::string& author_uid, 
            const Message& message, 
            const std::vector<std::string>& targets, 
            bool logging = false
        ) : logging(logging), author_uid(author_uid), targets(targets), message(message) {
            if (logging) 
                std::cout << "INITED: " << targets.size() << " targets" << std::endl;
            alr_send.clear();
        }

        std::optional<Message> send_next() {
            if (targets.empty()) return std::nullopt;
            if (logging) std::cout << "SENDING NEXT: " << targets.size() << " targets" << std::endl;
            if (!is_done()) {
                std::string target = targets.back();
                if (target != author_uid){
                    targets.pop_back();
                    alr_send.push_back(target);
                    return message;
                } else {
                    targets.pop_back();
                    alr_send.push_back(target);
                    return send_next();
                }
            }
            return std::nullopt;
        }

        bool is_done() const {
            return alr_send.size() == targets.size();
        }
};

class DirectMessage {
    public:
        std::string author;
        std::string addressee;
        Message message;
        bool alr_sent;

        DirectMessage(
            const std::string& author_uid, 
            const std::string& addressee_uid, 
            const Message& message
        ): author(author_uid), addressee(addressee_uid), message(message), alr_sent(false) {
            this->message.cli_from = author;
            this->message.to_cli_pub_uid = addressee;
        }

        std::optional<Message> send() {
            if (!alr_sent) {
                alr_sent = true;
                return message;
            }
            return std::nullopt;
        }
};

class ProtServer {
        
        UDPServer udp_server;
        std::unordered_map<std::string, std::function<Message(addr_t addr, const Message&)>> endpoint_handlers;
        bool logging;

        std::function<void(addr_t address, const std::string& message)> pre_update;

    public:
        ProtServer(
            const std::string& host, 
            int port, 
            bool logging = false
        ): logging(logging), udp_server(host, port, std::pow(2, 12)) {
            if (logging) std::cout << "[*] Game Server started on " << host << ":" << port << std::endl;
            if (logging) std::cout << "[+] Listening for incoming connections..." << std::endl;
            
            udp_server.set_on_message_callback([this](addr_t addr, const std::string& msg) {
                return update(addr, msg);
            });
        }

        void set_preupd( std::function<void(addr_t address, const std::string& message)> func ){
            pre_update = func;
        }

        void start() {
            udp_server.start();
        }

        void stop() {
            udp_server.stop();
        }

        void registrate_endpoint(const std::string& endpoint, std::function<Message(addr_t addr, const Message&)> handler) {
            endpoint_handlers[endpoint] = handler;
        }

    private:
        std::string update(addr_t address, const std::string& message) {
            if (logging) std::cout << "[*] Updating message at " << address << std::endl;

            if (pre_update) pre_update(address, message);

            Message dec = ProtMessage::decode(message);
            if (logging) std::cout << "[+] Checking for handlers for endpoint \"" << dec.endpoint << "\" ..." << std::endl;
            if (logging) std::cout << "[?] Message content: \n\tEndPoint: " << dec.endpoint << "\n\tDataType: " << dec.datatype << "\n\tMsgSpec: " << dec.msgspec << "\n\tUID: " << dec.uid << "\n\tPUB_UID: " << dec.pub_uid << "\n\tToCliPubUID: " << dec.to_cli_pub_uid << "\n\tContent: " << dec.content << std::endl;

            auto endpoint_handler = endpoint_handlers.find(dec.endpoint);
            if (endpoint_handler == endpoint_handlers.end()) {
                if (logging) std::cout << "[-][Not Found] No handler for endpoint \"" << dec.endpoint << "\"" << std::endl;
                return ProtMessage::from_server(std::to_string(static_cast<int>(StatusCode::EndpointPathIsNotFound)));
            }

            if (logging) std::cout << "[+] Calling handler for endpoint \"" << dec.endpoint << "\" ..." << std::endl;
            Message answer = endpoint_handler->second(address, dec);
            
            return ProtMessage::from_server(
                answer.statuscode, 
                answer.datatype, 
                answer.msgspec, 
                answer.cli_from, 
                answer.content
            );
        }
    
};

// int main() {
//     ProtServer server("127.0.0.1", 12345);
//     // server.registrate_endpoint("dir/path", [](const std::string& addr, const Message& msg) {
//     //     return msg; // Echo the message back
//     // });
//     server.start();
//     try {
//         while (true) {
//             std::this_thread::sleep_for(std::chrono::seconds(1)); // Keep the main thread alive
//         }
//     } catch (const std::exception& ex) {
//         std::cerr << ex.what() << std::endl;
//     }
//     server.stop();
//     return 0;
// }