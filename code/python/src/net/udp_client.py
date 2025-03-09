from colorama import *

import socket
import threading
import queue

class UDPClient:
    def __init__(self, server_ip, server_port, data_size=1024, logging = False):
        self.logging = logging
        self.server_ip = server_ip
        self.server_port = server_port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.data_size = data_size

        self.on_received_message = None
        self.on_error = None
        
        self.data_lock = threading.Lock()
        self.data_queue = []

        self.data_queue = queue.Queue()

    def _recv_cycle(self):
        while True:
            try:
                # print(f"{Fore.LIGHTCYAN_EX}[^^] Receiving data from server...{Fore.RESET}")
                response, _ = self.sock.recvfrom(self.data_size)
                if not response:
                    pass

                # print(f"{Fore.LIGHTCYAN_EX}[^^] Received data from server {Fore.RESET}")
                
                # print(f"{Fore.MAGENTA}[^^] Adding data to queue...{Fore.RESET}")
                self.data_queue.put_nowait(response.decode())
                
                # print(f"{Fore.GREEN} Data added to queue{Fore.RESET}")

                if self.on_received_message:
                    self.on_received_message(response.decode())
            
            except Exception as e:
                print(f"{Fore.RED}[!] Error: {e}{Fore.RESET}")
                if self.on_error:
                    self.on_error(e)
                pass
        # print(f"{Fore.LIGHTBLACK_EX}[!!] UDP Client stopped{Fore.RESET}")

    def getLastReply(self) -> str | None:
        try:
            return self.data_queue.get_nowait()  # Неблокирующий доступ к очереди
        except queue.Empty:
            return None

    def send(self, data: str):
        self.sock.sendto(data.encode('utf-8'), (self.server_ip, self.server_port))
    
    def send_and_wait(self, data: str) -> str:
        if self.logging:print(f"{Fore.LIGHTYELLOW_EX}[^] Sending data")
        self.send(data)
        
        if self.logging:print(f"\n{'-'*20}\n{data}\n{'-'*20}\n")

        if self.logging:print(f"{Fore.LIGHTYELLOW_EX}[^] Waiting for response")
        ans = None
        while ans is None:
            ans = self.getLastReply()
        if self.logging:print(f"{Fore.LIGHTCYAN_EX}[=] Got response")
        if self.logging:print(f"\n{'-'*20}\n{ans}\n{'-'*20}\n")
        return ans

    def connect(self):
        if self.logging:print(f"{Fore.LIGHTCYAN_EX}[*] Connecting to server {self.server_ip}:{self.server_port}...")
        threading.Thread(target=self._recv_cycle, args=(), daemon=True).start()

    def disconnect(self):
        self.sock.close()

if __name__ == "__main__":
    client = UDPClient("127.0.0.1", 12345)

    client.connect()

    try:
        while True:
            message = input("Enter message to send (or 'exit' to quit): ")
            if message.lower() == 'exit':
                break
            client.send(message)
            print(client.getLastReply())
    finally:
        client.disconnect()
