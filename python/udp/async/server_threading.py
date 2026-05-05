from collections import deque
import socket
import threading

local_ip = "0.0.0.0"
local_port = 9999
local_info = (local_ip, local_port)

local_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

local_socket.bind(local_info)

#string representation of 2-tuple mapped to queue and mutex
connection_table = {}

def worker_thread(ip_port, entry):
    cur_sum = 0

    while True:
        with entry['cond']:
            entry['cond'].wait()
            num = int.from_bytes(entry['queue'].popleft(), byteorder='big')
            cur_sum = num + cur_sum

            print(ip_port + ' thread: recieved ' + str(num))
            print(ip_port + ' thread: sum ' + str(cur_sum))

while True:
    data, addr = local_socket.recvfrom(4)

    table_key = addr[0] + str(addr[1])

    if table_key in connection_table:
        with connection_table[table_key]['cond']:
            connection_table[table_key]['queue'].append(data)
            connection_table[table_key]['cond'].notify()
    else:
        connection_table[table_key] = {}
        connection_table[table_key]['queue'] = deque([data])
        connection_table[table_key]['cond'] = threading.Condition()

        new_thread = threading.Thread(target=worker_thread, args=(table_key, connection_table[table_key]))
        new_thread.start()

