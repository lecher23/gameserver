#!/bin/env/python
# -*- coding:utf-8 -*-
import unittest
import os
import sys
import httplib
import time
import random
import json

CUR_DIR = os.path.split(os.path.realpath(__file__))[0]

class UserApiTest(unittest.TestCase):
    def setUp(self):
        self.ip = '127.0.0.1'
        self.port = 9876
        self.path = "/slots/login"

    def tearDown(self):
        #time.sleep(0.5)
        pass

    '''Return: code status body'''
    def _request(self, params):
        dest = "%s:%d" % (self.ip, self.port)
        path = self.path + "?" + "&".join(params)
        conn = httplib.HTTPConnection(dest)
        print path
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
    

    def testNewUser(self):
        params = []
        params.append("mid=%d%d" % (time.time(), random.randint(100, 999)))
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])

    def testUserExist(self):
        # insert user into MYSQL
        params = []
        params.append("mid=test_user_exist")
        ob = self._request(params)

        # get user
        params = []
        params.append("mid=test_user_exist")
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        
        
        
        
