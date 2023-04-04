from socketserver import ThreadingMixIn, TCPServer
from http.server import SimpleHTTPRequestHandler
from threading import Thread, Lock, Event
from queue import Queue, Empty as QueueEmptyException
from uuid import uuid4
from time import sleep


class ThreadPoolMixin(ThreadingMixIn):
    def serve_forever(self):
        self.requests_queue = Queue(self.threads_n)
        self.shutdown_event = Event()

        for _ in range(self.threads_n):
            thread = Thread(target=self.process_request_thread)
            thread.daemon = True
            thread.start()

        while True:
            self.handle_request()

    def process_request_thread(self, **kwargs):
        while True:
            try:
                request, client_address = self.requests_queue.get(timeout=0.5)
            except QueueEmptyException:
                if self.shutdown_event.is_set():
                    break
                continue

            try:
                self.finish_request(request, client_address)
            except:
                self.handle_error(request, client_address)
            finally:
                self.shutdown_request(request)

            self.requests_queue.task_done()

    def process_request(self, request, client_address):
        self.requests_queue.put((request, client_address))

    def server_close(self):
        self.requests_queue.join()
        self.shutdown_event.set()


class ThreadPoolServer(ThreadPoolMixin, TCPServer):
    def __init__(self, server_address, request_class, threads_n):
        self.threads_n = threads_n
        TCPServer.__init__(self, server_address, request_class)


class ThreadPoolRequestHandler(SimpleHTTPRequestHandler):
    def do_GET(self):
        sleep(5)
        self.send_response(200)
        self.end_headers()
        self.wfile.write('Hello World!'.encode('utf-8'))


if __name__ == '__main__':
    host = 'localhost'
    port = 8004
    threads_n = 5
    server = ThreadPoolServer((host, port), ThreadPoolRequestHandler, threads_n)
    print(f'Server started: http://{host}:{port}')
    server.serve_forever()
