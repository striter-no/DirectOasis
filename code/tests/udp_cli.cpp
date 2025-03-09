#include <Network/raw/udp_client.hpp>

int main() {
    try {
        UDPClient client("127.0.0.1", 12345, std::pow(2, 10), true);
        
        client.set_on_received_message([](const string& msg) {
            cout << "[Received] " << msg << endl;
        });
        
        client.set_on_error([](const boost::system::error_code& error) {
            cerr << "[Error] " << error.message() << endl;
        });

        client.connect();

        std::string payload = 
            "GET / HTTP/1.1\n"
            "Host: google.com\n"
            "User-Agent: curl/8.5.0\n"
            "Accept: */*\n"
            "\n\r"
        ;

        string response = client.send_and_wait(payload);
        cout << "Server response: " << response << endl;

        client.disconnect();
    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }
    return 0;
}