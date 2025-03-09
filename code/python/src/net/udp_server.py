from colorama import Fore
import socket
import multiprocessing

class UDPServer:
    def __init__(
        self,
        host: str,
        port: int,  # Ensure port is an integer
        data_size: int = 1024
    ):
        self.host = host
        self.port = port
        self.data_size = data_size

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        self.on_cli_message = None
        self.on_cli_error   = None

    def start(self):
        self.sock.bind((self.host, self.port))
        print(f"UDP server listening on {self.host}:{self.port}")

    def send(self, address):
        pass

    def _handle_client(self, message, address):
        try:
            ans: str = "no-callback-set"
            if self.on_cli_message:
                ans = self.on_cli_message(address, message.decode())
            else:
                raise Warning("No callback set for on_cli_message")

            print(f"Sending message back to {address}: \n{'-'*20}\n{ans}\n{'-'*20}\n")
            self.sock.sendto(ans.encode(), address)  # Echo back the received message
        except Exception as e:
            print(f"[!] Error: {e}")
            if self.on_cli_error:
                self.on_cli_error(address, e)

    def _listen(self):
        while True:
            try:
                print(f"{Fore.LIGHTYELLOW_EX}[^] Waiting for incoming message...{Fore.RESET}")
                message, address = self.sock.recvfrom(self.data_size)
                if not message:
                    break
                print(f"{Fore.LIGHTYELLOW_EX}[^] Received message from {address}{Fore.RESET}")
                # Start a new process for each client message
                process = multiprocessing.Process(target=self._handle_client, args=(message, address))
                process.start()
            except Exception as e:
                if self.on_cli_error:
                    self.on_cli_error(address, e)
                break

    def run(self):
        process = multiprocessing.Process(target=self._listen)
        process.start()

    def stop(self):
        self.sock.close()

if __name__ == "__main__":
    server = UDPServer(
        host="127.0.0.1",
        port=12345,  # Ensure this is an integer
        data_size=1024,
    )

    def message(address, message) -> str:
        print(f"Received message from {address}: {message.decode()}")
        return "Message received: " + message.decode()

    server.on_cli_message = message
    server.on_cli_error   = lambda address, e:       print(f"Client ({address}) error: {e}")

    server.start()  # Call start to bind the socket
    server.run()    # Start handling clients in a non-blocking way

    try:
        while True:
            pass  # Keep the main thread alive
    except KeyboardInterrupt:
        server.stop()
