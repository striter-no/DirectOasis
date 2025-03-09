from src.game.netprot.prot import *
from src.net.udp_client import *
from base64 import b64encode, b64decode
from colorama import Fore
import os
from random import *
from string import ascii_letters, digits, punctuation
alph = ascii_letters + digits + punctuation

LEN = 25
def generate_password():
    return ''.join(choice(alph) for _ in range(LEN))

class GameClient:
    def __init__(
        self,
        server_ip: str,
        server_port: int,
        nickname: str,
        password: str = "",
        logging = False
    ):
        self.logging = logging
        if logging:print(f"{Fore.BLUE}[*] Game Client started and will connect to {server_ip}:{server_port} {Fore.RESET}")
        self.uid = ""
        self.nickname = nickname

        if password == "":
            path = f"{os.getcwd()}/.cookie-{nickname}"

            if not os.path.exists(path):
                self.password = generate_password()
                if logging:print(f"{Fore.YELLOW}[!] Password generated: {self.password} {Fore.RESET}")
                with open(path, "w") as f:
                    f.write(self.password)
            else:
                with open(path, "r") as f:
                    self.password = f.read()
                    if logging:print(f"{Fore.YELLOW}[!] Password loaded: {self.password} {Fore.RESET}")

        self.password = password
        self.server_ip = server_ip
        self.server_port = server_port

        self.udp_client = UDPClient(
            server_ip=self.server_ip,
            server_port=self.server_port,
            data_size=2**12,
            logging=logging
        )
        if logging:print(f"{Fore.LIGHTWHITE_EX}[=] Client initialized:\n\tnickname={self.nickname}\n\tpassword={self.password} {Fore.RESET}\n")
    
    def connect(self):
        if self.logging:print(f"{Fore.LIGHTMAGENTA_EX}[*] Client connecting {Fore.RESET}")
        self.udp_client.connect()
        if self.logging:print(f"{Fore.LIGHTWHITE_EX}[=] Client connected {Fore.RESET}")
    
    def disconnect(self):
        if self.logging:print(f"{Fore.LIGHTMAGENTA_EX}[*] Client disconnecting {Fore.RESET}")
        self.udp_client.disconnect()
        if self.logging:print(f"{Fore.LIGHTWHITE_EX}[=] Client disconnected {Fore.RESET}")

    def registrate(self):
        if self.logging:print(f"{Fore.LIGHTMAGENTA_EX}[*] Client registrating")
        self.uid = b64encode((self.nickname + ':' + self.password).encode()).decode()
        if self.logging:print(f"{Fore.LIGHTGREEN_EX}[+] Client registered with uid=\"{self.uid}\" {Fore.RESET}")
        ans = self.udp_client.send_and_wait(
            ProtMessage.from_user(
                endpoint="registrate",
                datatype="text",
                msgspec="cli-server",
                to_cli_pub_uid="-1",
                pub_uid=self.nickname,
                uid=self.uid,
                content=""
            )
        )
        if self.logging:print(f"{Fore.LIGHTGREEN_EX}[+] Send registrate message and received answer {Fore.RESET}")

        dec = ProtMessage.decode(ans)
        if self.logging:print(f"{Fore.LIGHTGREEN_EX}[+] Decoded message {Fore.RESET}")
        
        if self.logging:print(f"{Fore.LIGHTCYAN_EX}[+] Reset my UID to server registred {Fore.RESET}")
        self.uid = dec.content.strip()

        if ProtMessage.status(dec.statuscode) != StatusCode.Success:
            if self.logging:print(f"{Fore.RED}[-] Registration failed: {ProtMessage.status(dec.statuscode)} {Fore.RESET}")
            raise Exception("Failed to registrate, server answer: \n\n" + ans)
        
        if self.logging:print(f"{Fore.LIGHTWHITE_EX}[=] Client successfully registrated (status: {ProtMessage.status(dec.statuscode)}){Fore.RESET}")
    
    def recv_message(
        self,
        msg_type: str
    ):
        if self.logging:print(f"{Fore.LIGHTMAGENTA_EX}[*] Receiving message of type {msg_type} {Fore.RESET}")
        ans = self.udp_client.send_and_wait(
            ProtMessage.from_user(
                endpoint="recv_message",
                datatype="text",
                msgspec="cli-server",
                to_cli_pub_uid="-1",
                pub_uid=self.nickname,
                uid=self.uid,
                content=msg_type
            )
        )
        if self.logging:print(f"{Fore.LIGHTGREEN_EX}[+] Send request message and received answer {Fore.RESET}")

        dec = ProtMessage.decode(ans)
        if self.logging:print(f"{Fore.LIGHTGREEN_EX}[+] Decoded message {Fore.RESET}")

        if self.logging:print(f"{Fore.LIGHTWHITE_EX}[=] Client successfully get message (status: {ProtMessage.status(dec.statuscode)}){Fore.RESET}")
        return dec

    def get_all(
        self,
        msg_type: str
    ) -> list[Message]:
        out = []
        msg = self.recv_message("broadcast")
        while ProtMessage.status(msg.statuscode) != StatusCode.InvalidValue:
            if msg.cli_from != self.uid:
                out.append(msg)
            msg = self.recv_message("broadcast")
        return out

    def direct_message(
        self,
        data: str,
        data_type: str,
        cli_uid: str,
    ) -> Message:
        if self.logging:print(f"{Fore.CYAN}[>] Called direct message function {Fore.RESET}")
        return self.__send_message(
            data, data_type, cli_uid, "cli-cli"
        )

    def server_message(
        self,
        data: str,
        data_type: str,
    ) -> Message:
        if self.logging:print(f"{Fore.CYAN}[>] Called server message function {Fore.RESET}")
        return self.__send_message(
            data, data_type, "-1", "cli-server"
        )

    def broadcast(
        self,
        data: str,
        data_type: str,
    ) -> Message:
        if self.logging:print(f"{Fore.CYAN}[>] Called broadcast function {Fore.RESET}")
        return self.__send_message(
            data, data_type, "-1", "broadcast"
        )

    def __send_message(
        self, 
        msg: str,
        data_type: str,
        to_uid: int,
        msg_type: str
    ) -> Message:
        if self.logging:print(f"{Fore.LIGHTMAGENTA_EX}[*] Sending message with type \"{msg_type}\" {self.uid} -> {to_uid} {Fore.RESET}")
        ans = self.udp_client.send_and_wait(
            ProtMessage.from_user(
                endpoint="send_message",
                datatype=data_type,
                msgspec=msg_type,
                to_cli_pub_uid=to_uid,
                uid=self.uid,
                pub_uid=self.nickname,
                content=msg
            )
        )
        if self.logging:print(f"{Fore.LIGHTWHITE_EX}[=] Message sent {Fore.RESET}")
        return ProtMessage.decode(ans)