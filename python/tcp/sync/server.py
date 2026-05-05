import socket

local_ip = "0.0.0.0"
local_port = 9999
local_info = (local_ip, local_port) #bind() takes a tuple of both

#socket.AF_INET = ipv4
#socket.SOCK_STREAM = TCP

#create socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print('created socket...')

#bind socket to local_ip and local_port
server_socket.bind(local_info)
print('bound socket...')

#now tell the socket to listen
#listening starts a queue of incoming connections
#when you call accept()
server_socket.listen()
print('listening on socket...')

#calling accept() pops the connection from the queue
new_connection, _ = server_socket.accept()
print('accepted connection...')


while True:
    raw_data = new_connection.recv(4)
    print('recieved data...')

    #convert from network order (big endian) to host order (little endian)
    number = int.from_bytes(raw_data, byteorder='big')
    print('converted data...')

    #print to screen
    print("From client: " + str(number))
