import socket
import time
import threading

buf_size = 2048
host = "localhost"
port = 9876

def do_test():
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect((host, port))
    time.sleep(10000)

def do_send(client):
    msg = ["000312", "30004123400", "011"]
    for item in msg:
        client.send(item)
        print "send %s over." % item
        time.sleep(2)

def do_recv(client):
    for i in range(0, 10):
        data = client.recv(1024)
        print "read_data: %s" % data
        time.sleep(1)

def test_tcp():
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect((host, port))
    t1 = threading.Thread(target = do_send, args = (client, ))
    t1.setDaemon(True)
    t1.start()
    t2 = threading.Thread(target = do_recv, args = (client, ))
    t2.setDaemon(True)
    t2.start()
    t1.join()
    t2.join()

if __name__ == "__main__":
    test_tcp()
