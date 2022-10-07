# Sender
 
import socket
 
UDP_IP = "172.16.51.62"
UDP_PORT = 3300
MESSAGE = b'Hi Everett and Nick'
print("UDP target IP: %s" % UDP_IP)
print("UDP target port: %s" % UDP_PORT)
print("message: %s" % MESSAGE)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))
