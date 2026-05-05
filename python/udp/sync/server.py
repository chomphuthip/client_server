import socket

local_ip = "0.0.0.0"
local_port = 9999
local_info = (local_ip, local_port)

local_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

local_sock.bind(local_info)

while True:
    data, addr = local_sock.recvfrom(4)

    number = int.from_bytes(data, byteorder='big')
    print('recieved ' + str(number) + ' from ' + addr[0])
