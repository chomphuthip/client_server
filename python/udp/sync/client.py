import socket
import random
import time

remote_ip = "127.0.0.1"
remote_port = 9999
remote_info = (remote_ip, remote_port)

client_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
    number = random.randint(1, 100)

    data = number.to_bytes(4, byteorder='big')

    client_sock.sendto(data, remote_info)
    
    print('sent ' + str(number))
    time.sleep(3)
