import socket
import sys
import string
import struct


def main():
    HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
    PORT = 8888        # Port to listen on (non-privileged ports are > 1023)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((HOST, PORT))
    s.listen(5)
    while True:
        try:
            conn, addr = s.accept()
            print('Connected by', addr)
            #Stage 0 make sure the person says hello
            data = conn.recv(1024)
            if not data:
                break
            print(data)
            conn.sendall(data)
        except Exception as err:
            print(err)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER,struct.pack('ii', 1, 0))
    s.close()

if __name__== "__main__":
    main()
