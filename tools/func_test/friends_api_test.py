#!/bin/env/python
# -*- coding:utf-8 -*-
import unittest
import os
import sys
import httplib
import time
import json

CUR_DIR = os.path.split(os.path.realpath(__file__))[0]

class ServerTest(unittest.TestCase):
    def setUp(self):
        self.ip = '127.0.0.1'
        self.port = 9876
        self.path = "/slots/friends"

    def tearDown(self):
        #time.sleep(0.5)
        pass

    '''Return: code status body'''
    def _request(self, params):
        dest = "%s:%d" % (self.ip, self.port)
        path = self.path + "?" + "&".join(params)
        conn = httplib.HTTPConnection(dest)
        conn.request("POST", path)
        rsp = conn.getresponse()
        body = rsp.read()
        ob = None
        try:
            print body
            ob = json.loads(body)
        except Exception, e:
            ob = None
        return ob
    

    def _testGetAllInfo(self):
        params = []
        params.append("type=1")
        params.append("uid=1")
        params.append("page=0")
        params.append("pagesize=5")
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual("001", ob["UID"])
        self.assertEqual(0, len(ob["friends"]))
        self.assertEqual(0, ob["total_reward"])
        self.assertEqual(0, ob["cached_reward"])

    def testGetAllFriends(self):
        params = []
        params.append("type=0")
        params.append("uid=6")
        params.append("page=0")
        params.append("pagesize=5")
        ob = self._request(params)
        print ob
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(3, len(ob["friends"]))

    def testSearchUserByKw(self):
        params = []
        params.append("type=2")
        params.append("uid=1")
        params.append("kw=functest")
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(1, len(ob["friends"]))

    def testSearchUserById(self):
        params = []
        params.append("type=2")
        params.append("uid=1")
        params.append("tid=6")
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(1, len(ob["friends"]))

