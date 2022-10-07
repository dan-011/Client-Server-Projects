import subprocess

import telnetlib
import socket

telnetlib.Telnet
ConnectionRefusedError
socket.timeout

counter = 0
ips_and_ports = {}
_ports = set()
exists = False
with open("active_ranges", "r") as ar:
  for ip in ar:
    ips_and_ports[ip.strip()] = []
    for port in range(1, 1024):
    #for port in range(1, 65536):
      try:
          t = telnetlib.Telnet(ip.strip(), str(port), 0.01)
          #print(0)
          ips_and_ports[ip.strip()].append(str(port))
          t.close()
          _ports.add(port)
      except ConnectionRefusedError:
          #print(1)
          continue
      except socket.timeout:
          #print(2)
          continue
#for ips in ips_and_ports.keys():
#  if len(ips_and_ports[ips]) != 0:
#    print(ips + ":")
#    for port in ips_and_ports[ips]:
#      print("\t" + port)
#      
for port in _ports:
  print(port)