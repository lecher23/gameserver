import socket
import time

buf_size = 2048
host = "localhost"
port = 9876

def do_test():
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect((host, port))
    time.sleep(10000)

if __name__ == "__main__":
    do_test()
