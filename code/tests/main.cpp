#include <Network/game/game_server.hpp>

int main(){
    GameServer server(
        "127.0.0.1", //  192.168.31.100
        8080, 
        false
    );

    server.start();
    try {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Keep the main thread alive
        }
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
    server.stop();
}