from scapy.all import *
def process_packet(pkt):
  # pkt.show()
  # print mac address information
  if pkt.haslayer(IP):
          ip = pkt[IP]
          print("IP:{} --> {}".format(ip.src, ip.dst))
  if pkt.haslayer(Ether):
          mac = pkt[Ether]
          print("Eth: {} --> {}".format(mac.src, mac.dst))
  elif pkt.haslayer(TCP):
          tcp = pkt[TCP]
          print ("TCP ports:{} --> {}".format(tcp. sport, tcp.dport))
  elif pkt.haslayer(UDP):
          udp = pkt[UDP]
          print ("UDP ports:{} --> {}".format(udp.sport,udp.dport))
  else:
          print("other protocol")
sniff (iface = 'eth0', filter = 'icmp', prn = process_packet, count =20)
