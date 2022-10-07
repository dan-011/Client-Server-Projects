import socket # for socket 
import sys
import json
import hashlib
from time import sleep
from math import floor, sqrt


port = 8800
host_ip = '172.16.50.166'

def prime(num):
  for i in range(2, num):
    if num % i == 0: return False
  return True

def main():
    while True:
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        except socket.error as err:
            print ("socket creation failed with error %s" %(err))
        s.bind(('', 23232))
        s.connect((host_ip, port))
        # 1
        print("Response 1")
        s.sendall(b'Hello')
        test = s.recv(1024)
        print('\t' + "Message: Hello")
        print('\t' + test.decode("utf-8"))
        # 2
        s.sendall(test[::-1])
        message = test.decode('utf-8')[::-1]
        test = s.recv(1024)
        print("Response 2")
        print("\tMessage: " + message)
        print('\t' + test.decode('utf-8'))
        # 3
        test_num = int(test.decode('utf-8'))
        msg = floor((test_num - 10)/2)
        message = msg
        msg = str(msg).encode('utf-8')
        s.sendall(msg)
        test = s.recv(1024)
        print("Response 3")
        print("\tMessage: " + str(message))
        print('\t' + test.decode('utf-8'))
        # 4
        test_num = int(test.decode('utf-8'))
        msg = int(floor(sqrt(test_num)))
        message = msg
        msg = str(msg).encode('utf-8')
        s.sendall(msg)
        test = s.recv(1024)
        print("Response 4")
        print("\tMessage: " + str(message))
        print('\t' + test.decode('utf-8'))
        # 5
        test = json.loads(test)
        l = list(test)
        msg = "["
        for num in l:
          if prime(num): msg += "1"
          else: msg += "0"
          msg += ", "
        msg = msg[:len(msg)-2]
        msg += "]"
        s.sendall(msg.encode('utf-8'))
        test = s.recv(1024)
        print("Response 5")
        # print(test)
        print("\tMessage: " + msg)
        print('\t' + test.decode('utf-8'))
        m = hashlib.sha256()
        m.update(test)
        hash = m.hexdigest()
        s.sendall(hash.encode('utf-8'))
        test = s.recv(1024)
        print(test.decode('utf-8'))

        s.close()
        sleep(5)

if __name__== "__main__":
    main()
