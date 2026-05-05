import socket
import random
import time

remote_ip = "0.0.0.0"
remote_port = 9999
remote_info = (remote_ip, remote_port)

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print('created socket...')

client_socket.connect(remote_info)
print('connected')

while True:
    number = random.randint(0, 100)

    data = number.to_bytes(4, byteorder='big')

    client_socket.send(data)
    print('sent ' + str(number))

    time.sleep(3)

