import socketserver

class SumHandler(socketserver.DatagramRequestHandler):
    def init(self):
        self.sum = 0

    def handle(self):
        data = self.rfile.read()
        num = int.from_bytes(data, byte_order='big')

        print

