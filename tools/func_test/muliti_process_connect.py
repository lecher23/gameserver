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

def query():
    for i in range (1, 400):
        conn = httplib.HTTPConnection("127.0.0.1:9876")
        mid = int(time.time() * 1000000) + random.randint(2, 999)
        conn.request("POST", "/slots/login?mid=%d" % mid)
        rsp = conn.getresponse()
        body = rsp.read()
    print time.time()

if __name__ == "__main__":
    print "CPU count: %s" % str(multiprocessing.cpu_count())
    process_list = []
    for i in range(0, 4):
        process_list.append(multiprocessing.Process(target = query))

    print time.time()
    for item in process_list:
        item.start()

