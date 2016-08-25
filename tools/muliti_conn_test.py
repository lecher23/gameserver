#!/bin/env/python
# -*- coding:utf-8 -*-
import unittest
import os
import sys
import httplib
import time
import json
import random
import multiprocessing

HOST = "127.0.0.1:9876"

def query(idx):
    f = open("result_%d" % idx, "w")
    begin = time.time()
    time_used = 0.0
    REQ_TIMES = 400
    for i in range (0, REQ_TIMES):
        mid = int(time.time() * 1000000) + random.randint(2, 999)

        begin_ = time.time()
        #conn.request("POST", "/slots/login?mid=%d" % mid)
        conn = httplib.HTTPConnection(HOST)
        conn.request("POST", "/slots/login?mid=123456")
        rsp = conn.getresponse()
        end_ = time.time()

        time_used += end_ - begin_
        body = rsp.read()
    end = time.time()
    ot = "%20s: %20d\n%20s: %20d\n" % ("begin", begin, "end", end)
    ot += "%20s: %20d\n" % ("Request Num", REQ_TIMES)
    ot += "%20s: %20f\n" % ("Each request", time_used / REQ_TIMES)
    f.write(ot)
    f.close()

if __name__ == "__main__":
    print "CPU count: %s" % str(multiprocessing.cpu_count())
    process_list = []
    for i in range(0, 4):
        process_list.append(multiprocessing.Process(target = query, args=(i, )))

    print time.time()
    for item in process_list:
        item.start()

