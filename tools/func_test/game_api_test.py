#!/bin/env/python
# -*- coding:utf-8 -*-
import unittest
import os
import sys
import httplib
import time
import json
import random

CUR_DIR = os.path.split(os.path.realpath(__file__))[0]

class ServerTest(unittest.TestCase):
    def setUp(self):
        self.ip = '127.0.0.1'
        self.port = 9876
        self.path = "/slots/game"

    def tearDown(self):
        #time.sleep(0.5)
        pass

    '''Return: code status body'''
    def _request(self, params, path = None):
        dest = "%s:%d" % (self.ip, self.port)
        if path is None:
            path = self.path
        path += "?" + "&".join(params)
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

    def _addUser(self):
        params = []
        mid = random.randint(2, 99999)
        params.append("mid=%d" % mid)
        ob = self._request(params, "/slots/login")
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        uid = ob["user_info"]["uid"]
        money = ob["user_resource"]["fortune"]
        return uid, money;

    def _getUser(self, uid):
        params = []
        params.append("mid=%s" % uid)
        ob = self._request(params, "/slots/login")
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        return ob

    def testOneKeyTakeGold(self):
        sid, smoney = self._addUser()

        # play game
        params = []
        params.append("type=0")
        params.append("bet=900")
        params.append("uid=%s" % sid)
        ob = self._request(params)        
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(10, ob["user_resource"]["fortune"])
