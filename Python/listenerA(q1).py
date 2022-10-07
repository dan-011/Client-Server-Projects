# Listener

import socket

if __name__ == "__main__":
  IP= "172.16.51.61" 
  Port = 3300
  sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  sock.bind((IP,Port))
  while True:
      data, (ip,port) = sock.recvfrom(1024)
      print("Sender:{} and port:{}".format(ip,port))
      print("received message:{}".format(data))
      
