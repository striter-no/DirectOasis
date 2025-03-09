#pragma once

#include <utils/io.hpp>
#include <sha256>
#include <mutex>

#include <Network/proto/prot_serv.hpp>

using namespace utils::vec;
using namespace utils::io;

std::string sha256(std::string str) {
    SHA256 hasher;
    hasher.update(str);
    return hasher.toString(hasher.digest());
}

std::string sint_status(StatusCode code){
    return std::to_string(static_cast<int>(code));
}

template<class K, class V>
std::optional<V> sget(std::unordered_map<K, V> &map, K key){
    if (map.find(key) != map.end()){
        return map[key];
    }
    return std::nullopt;
}

template<class K, class V>
std::vector<K> get_keys(std::unordered_map<K, V> &map){
    std::vector<K> keys;
    for (auto &it : map){
        keys.push_back(it.first);
    }
    return keys;
}

template<class K, class V>
std::vector<V> get_vals(std::unordered_map<K, V> &map){
    std::vector<V> vals;
    for (auto &it : map){
        vals.push_back(it.second);
    }
    return vals;
}

void log(std::string msg){
    std::cout << msg << std::endl;
}

class GameServer {
        ProtServer protserv;

        std::unordered_map<std::string, std::vector<Message>> server_msgs;
        std::unordered_map<std::string, std::vector<DirectMessage>> direct_msgs;
        std::unordered_map<std::string, std::string> clients;
        std::vector<Broadcast> broadcasts;

        std::mutex serv_msgs_mtx;
        std::mutex direct_msgs_mtx;
        std::mutex broadcasts_mtx;
        std::mutex clients_mtx;

        bool logging;
    public:
        GameServer(
            std::string host,
            int port,
            bool logging = false
        ): protserv(
            host, port, logging
        ), logging(logging) {
            if(logging) log("[0][*] Game Server started on " + host + ":" + std::to_string(port));
            protserv.registrate_endpoint("registrate", [this](addr_t addr, const Message& msg){
                return this->registrate_edp(addr, msg);
            });
            protserv.registrate_endpoint("send_message", [this](addr_t addr, const Message& msg){
                return this->send_edp(addr, msg);
            });
            protserv.registrate_endpoint("recv_message", [this](addr_t addr, const Message& msg){
                return this->recv_edp(addr, msg);
            });
            protserv.set_preupd([this](addr_t addr, const std::string &msg){
                this->update(addr, msg);
            });
            if(logging) log("[1][=] Endpoints registered");
        }

        void start(){ protserv.start(); }
        void stop(){ protserv.stop(); }
        void registrate_endpoint(const std::string& endpoint, std::function<Message(addr_t addr, const Message&)> handler){ 
            protserv.registrate_endpoint(endpoint, handler);
        }

        Message registrate_edp(
            addr_t address,
            const Message &msg
        ){
            if(logging) log("[2][*] Registration endpoint reached");
            if(logging) log("[3][?] Client info: \n\tUID: \"" + msg.uid + "\"\n\tPUB_UID: \"" + msg.pub_uid + "\"\n\tDataType: \"" + msg.datatype + "\"\n");
            Message out;
            out.msgspec = "server";
            out.datatype = "text";
            out.cli_from = "-1";
            
            auto pub_id = msg.pub_uid;
            if (clients.find(pub_id) == clients.end()) {
                if(logging) log("[4][+] Client is not registred");
                auto uid = sha256(
                    address.address().to_string() + ':' + std::to_string(address.port())
                );
                if(logging) log("[5][+] New UID generated: \"" + uid + '"');
                clients_mtx.lock();
                    clients[pub_id] = uid;
                clients_mtx.unlock();
                
                out.statuscode = sint_status(StatusCode::Success);
                out.content = uid;
                if(logging) log("[6][=] Registration successful");
                return out;
            } else {
                if(logging) log("[7][!] Client is already registred, aborted");
                out.statuscode = sint_status(StatusCode::ActionIsNotDoableInThisContext);
            }

            return out;
        }

        Message send_edp(
            addr_t address,
            const Message &msg
        ){
            if(logging) log("[8][*] Message send endpoint reached");
            if(logging) log("[9][?] Client info: \n\tUID: \"" + msg.uid + "\"\n\tPUB_UID: \"" + msg.pub_uid + "\"\n\tDataType: \"" + msg.datatype + "\"\n");
            Message out;
            out.msgspec = "server";
            out.datatype = "text";
            out.cli_from = "-1";

            if (msg.uid != sget(clients, msg.pub_uid)){
                if(logging) log("[10][!] Client is not authorized, aborted. Registred UID: \"" + sget(clients, msg.pub_uid).value_or("<UNKNOWN>") + '"');
                out.statuscode = sint_status(StatusCode::NotAuthorized);
                return out;
            }

            if(logging) log("[11][+] Message spec is " + msg.msgspec);
            if (msg.msgspec == "cli-cli") {
                direct_msgs_mtx.lock();
                    if (direct_msgs.find(msg.to_cli_pub_uid) == direct_msgs.end()){
                        if(logging) log("[+] New direct message channel created");
                        direct_msgs[msg.to_cli_pub_uid] = {};
                    } else {
                        if(logging) log("[+] Direct message channel already exists");
                    }
                    
                    Message tmp;
                    tmp.content = msg.content;
                    tmp.datatype = msg.datatype;

                    if(logging) log("[+] Adding message to direct channel");
                    direct_msgs[
                        msg.to_cli_pub_uid
                    ].push_back(
                        DirectMessage(
                            msg.uid, clients[msg.to_cli_pub_uid], tmp
                        )
                    );
                    if(logging) log("[=] Message added to direct channel");
                direct_msgs_mtx.unlock();
            } else if (msg.msgspec == "broadcast"){
                if(logging) log("[+] Getting targets");
                auto targets = get_vals(clients);
                if (count(targets, msg.uid) != 0){
                    if(logging) log("[+] Erasing sender from targets");
                    targets.erase(targets.begin() + index(targets, msg.uid));
                } else {
                    if(logging) log("[!] Sender is not in targets");
                }
                if(logging) log("[+] Targets count: " + std::to_string(targets.size()));

                for (auto &tar: targets){
                    if(logging) log("[+] Sending message to target: " + tar);
                }
                
                if (targets.size() == 0){
                    out.statuscode = sint_status(StatusCode::ServerIsEmpty);
                    if(logging) log("[!] Aborting broadcast due 0 targets on server");
                } else {
                    Message tmp;
                    tmp.content = msg.content;
                    tmp.datatype = msg.datatype;

                    broadcasts_mtx.lock();
                        if(logging) log("[+] Adding message to broadcast");
                        broadcasts.push_back(Broadcast(
                            msg.uid, tmp, targets
                        ));
                        if(logging) log("[=] Message added to broadcast");
                    broadcasts_mtx.unlock();
                }
            } else if (msg.msgspec == "server") {
                if(logging) log("[!] Server messages are not implemented yet");
                Message tmp;
                tmp.content = msg.content;
                tmp.datatype = msg.datatype;

                serv_msgs_mtx.lock();
                    if(logging) log("[!][+] Adding message to server messages");
                    server_msgs[
                        msg.uid
                    ].push_back(tmp);
                    if(logging) log("[?] Message content: \n--------------------" + msg.content + "\n---------------------------");
                    if(logging) log("[=] Message added to server messages");
                serv_msgs_mtx.unlock();
            } else {
                if(logging) log("[!] Unknown message type: " + msg.msgspec);
                out.statuscode = sint_status(StatusCode::InvalidValue);
            }

            out.statuscode = sint_status(StatusCode::Success);
            return out;
        }

        Message recv_edp(
            addr_t address,
            const Message &msg
        ){
            if(logging) log("[*] Recv message endpoint reached");
            if(logging) log("[?] Client info: \n\tUID: \"" + msg.uid + "\"\n\tPUB_UID: \"" + msg.pub_uid + "\"\n\tDataType: \"" + msg.datatype + "\"\n");
            Message out;
            out.msgspec = "server";
            out.datatype = "text";
            out.cli_from = "-1";
            out.statuscode = sint_status(StatusCode::InvalidValue);
            out.content = '"' + msg.content + '"';
            if (logging) log("[?] Message content: " + msg.content);
            if (msg.content == "broadcast") {
                auto copy = broadcasts;
                if (logging) log("[+] Receiveing broadcast ");
                if (logging) log("[+] Broadcasts count: " + std::to_string(copy.size()));
                bool sended = false;
                for (auto &brd: copy){
                    if(brd.author_uid == msg.uid) continue;
                    auto tmsg = brd.send_next();
                    if (tmsg){
                        if (logging) log("[+] Broadcasting message");
                        const auto &act_msg = tmsg.value();
                        if(logging) log("[?] Message content: \n--------------------\n" + act_msg.content + "\n---------------------------");
                        out.statuscode = sint_status(StatusCode::Success);
                        out.cli_from = act_msg.uid;
                        out.to_cli_pub_uid = msg.uid;
                        out.content = act_msg.content;
                        out.datatype = act_msg.datatype;
                        sended = true;
                        break;
                    }
                }
                if(!sended){
                    if (logging) log("[!] No broadcasts send");
                    out.statuscode = sint_status(StatusCode::InvalidValue);
                }
                broadcasts_mtx.lock();
                    broadcasts = copy;
                broadcasts_mtx.unlock();
            } else if (msg.content == "direct_message") {
                if (direct_msgs.find(msg.pub_uid) == direct_msgs.end())
                    return out;
                
                auto copy = direct_msgs[msg.pub_uid];
                for (auto &direct: copy){
                    auto tmsg = direct.send();
                    if (tmsg){
                        const auto &act_msg = tmsg.value();
                        out.statuscode = sint_status(StatusCode::Success);
                        out.cli_from = act_msg.cli_from;
                        out.to_cli_pub_uid = act_msg.to_cli_pub_uid;
                        out.content = act_msg.content;
                        out.datatype = act_msg.datatype;
                        break;
                    }
                }

                direct_msgs_mtx.lock();
                    direct_msgs[msg.pub_uid] = copy;
                direct_msgs_mtx.unlock();
            } else if (msg.content == "server_message") {
                ; // TODO: implement server message handling
            }

            return out;
        }

        void update(addr_t address, const std::string& message) {
            if (logging) log("[*] Pre-Update func called");
            std::unordered_map<std::string, std::vector<DirectMessage>> alive_directs;
            std::vector<Broadcast> alive_broadcasts;

            auto brd_copy = broadcasts;
            auto drct_copy = direct_msgs;

            for (auto &brd: brd_copy){
                if (!brd.is_done() && (brd.targets.size() != 0)){
                    alive_broadcasts.push_back(brd);
                }
            }
            if (logging) log("[+] Went from " + std::to_string(brd_copy.size()) + " to " + std::to_string(alive_broadcasts.size()) + " broadcasts");

            for (auto &drct: drct_copy){
                std::vector<DirectMessage> local_alive;
                for (auto &msg: drct.second){
                    if (!msg.alr_sent) local_alive.push_back(msg);
                }
                if (local_alive.size() != 0) alive_directs[drct.first] = local_alive;
            }
            if (logging) log("[+] Went from " + std::to_string(drct_copy.size()) + " to " + std::to_string(alive_directs.size()) + " direct messages");

            broadcasts_mtx.lock(); broadcasts = alive_broadcasts; broadcasts_mtx.unlock();
            direct_msgs_mtx.lock(); direct_msgs = alive_directs; direct_msgs_mtx.unlock();
            if (logging) log("[=] Pre-Update func exit");
        }
};