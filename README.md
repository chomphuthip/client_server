
```
├── c
│   ├── tcp
│   │   ├── async
│   │   │   ├── server
│   │   │   └── server.c
│   │   └── sync
│   │       ├── client
│   │       ├── client.c
│   │       ├── less
│   │       ├── server
│   │       └── server.c
│   └── udp
│       ├── async
│       │   ├── server
│       │   └── server.c
│       └── sync
│           ├── client
│           ├── client.c
│           ├── server
│           └── server.c
├── python
│   ├── tcp
│   │   └── sync
│   │       ├── client.py
│   │       └── server.py
│   └── udp
│       ├── async
│       │   ├── server_socketserver.py
│       │   └── server_threading.py
│       └── sync
│           ├── client.py
│           └── server.py
└── README.md
```

basic implementations of TCP/UDP servers

**Asynchronous UDP server design**
* Main Thread
     1. socket thread calls recvfrom()
     2. compares address to `entry` in `pool`
         * if an entry exists, return pointer
         * if entry doesn't exist, create entry, create thread, and then return pointer
     3. push to queue in entry, then `pthread_cond_signal`
* Worker Thread
     1. `pthread_cond_wait()` on `entry->queue->cond`
     2. pop from queue and convert to host byte order (little endian)
     3. add to sum and print to screen
     4. restart loop and wait on signal. thread should never be awake when main thread is pushing
