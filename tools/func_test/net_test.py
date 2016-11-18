import socket
import time
import threading

buf_size = 2048
host = "localhost"
port = 9888

def do_test():
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect((host, port))
    time.sleep(10000)

def make_packet(packet_type, packet_msg):
    msg_bytes = ""
    msg_bytes += chr(packet_type)
    msg_bytes += chr(len(packet_msg) >> 8 & 0xff)
    msg_bytes += chr(len(packet_msg) & 0xff)
    msg_bytes += packet_msg
    return msg_bytes

def do_send(client):
    out = make_packet(1, 'hello')
    client.send(out)
    time.sleep(1)
    out = make_packet(1, 'sdhello')
    client.send(out)
    out = make_packet(1, 'hedllo')
    client.send(out)
    out = make_packet(1, 'exit')
    client.send(out)
    time.sleep(1)

def do_recv(client):
    for i in range(0, 10):
        data = client.recv(1024)
        print "read_data: %s" % data
        time.sleep(0.2)

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
