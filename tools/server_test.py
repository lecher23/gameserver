#!/bin/env/python
# -*- coding:utf-8 -*-
import unittest
import os
import sys
import httplib
import time

CUR_DIR = os.path.split(os.path.realpath(__file__))[0]

class ServerTest(unittest.TestCase):
    def setUp(self):
        self.ip = '127.0.0.1'
        self.port = 9876
        self.path = ""

    def tearDown(self):
        #time.sleep(0.5)
        pass

    '''Return: code status body'''
    def _request(self):
        dest = "%s:%d" % (self.ip, self.port)
        conn = httplib.HTTPConnection(dest)
        conn.request("POST", self.path)
        rsp = conn.getresponse()
        return rsp.status, rsp.reason, rsp.read()

    def testRefreshConfig_WithoutQuestionMark(self):
        self.path = "/refresh"
        status, reason, body = self._request()
        self.assertEqual(200, status)
        self.assertEqual("OK", reason)
        self.assertEqual("Refresh config done.\n", body)

    def testRefreshConfig(self):
        self.path = "/refresh?user_id=1"
        status, reason, body = self._request()
        self.assertEqual(200, status)
        self.assertEqual("OK", reason)
        self.assertEqual("Refresh config done.\n", body)

    def testGetUserSetting_UserIdNotExist(self):
        self.path = "/getUserSetting?user_id=1"
        status, reason, body = self._request()
        self.assertEqual(200, status)
        self.assertEqual("OK", reason)
        self.assertEqual("NO", body)

    def testGetUserSetting_UserIdExist(self):
        self.path = "/getUserSetting?user_id=C0C1W"
        status, reason, body = self._request()
        self.assertEqual(200, status)
        self.assertEqual("OK", reason)
        self.assertEqual("YES", body)
        self.path = "/getUserSetting?user_id=C0C2W"
        status, reason, body = self._request()
        self.assertEqual(200, status)
        self.assertEqual("OK", reason)
        self.assertEqual("NO", body)
