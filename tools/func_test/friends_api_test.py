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
        self.path = "/slots/friends"

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

    def _testGetAllFriends(self):
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

    def _testSearchUserByKw(self):
        params = []
        params.append("type=2")
        params.append("uid=1")
        params.append("kw=functest")
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(1, len(ob["friends"]))

    def _testSearchUserById(self):
        params = []
        params.append("type=2")
        params.append("uid=1")
        params.append("tid=6")
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(1, len(ob["friends"]))

    def _testSearchUserWithPage(self):
        params = []
        params.append("type=2")
        params.append("uid=1")
        params.append("tid=6")
        params.append("page=5")
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(0, len(ob["friends"]))
        
    def _testSearchUserWithPageSize(self):
        params = []
        params.append("type=2")
        params.append("uid=1")
        params.append("tid=6")
        params.append("pagesize=0")
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(0, len(ob["friends"]))

    def _testSearchUserWithBoth(self):
        params = []
        params.append("type=2")
        params.append("uid=1")
        params.append("tid=6")
        params.append("page=0")
        params.append("pagesize=1")
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(1, len(ob["friends"]))

    def _testAddFriend(self):
        uid, m = self._addUser()
        tid, m = self._addUser()
        params = []
        params.append("type=3")
        params.append("uid=%s" % uid)
        params.append("tid=%s" % tid)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])

        params = []
        params.append("type=0")
        params.append("uid=%s" % uid)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(1, len(ob["friends"]))
        self.assertEqual(tid, ob["friends"][0]["uid"])

        '''Send gift api'''
    def testSendGold(self):
        sid, smoney = self._addUser()        
        params = []
        params.append("type=4")
        params.append("uid=1")
        params.append("tid=%s" % sid)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])

        #send again
        params = []
        params.append("type=4")
        params.append("uid=1")
        params.append("tid=%s" % sid)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("ERROR", ob["status"])

        '''Take gift api'''        
    def testTakeGold(self):
        sid, smoney = self._addUser()
        sender = int(sid)
        rid, rmoney = self._addUser()
        receiver = int(rid)
        
        # send gold
        params = []
        params.append("type=4")
        params.append("uid=%d" % sender)
        params.append("tid=%d" % receiver)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])

        # take gold
        params = []
        params.append("type=5")
        params.append("uid=%d" % receiver)
        params.append("tid=%d" % sender)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertTrue(ob["user_resource"]["fortune"] > rmoney)

        # resend
        params = []
        params.append("type=4")
        params.append("uid=%d" % sender)
        params.append("tid=%d" % receiver)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("ERROR", ob["status"])

        # retaken
        params = []
        params.append("type=5")
        params.append("uid=%d" % receiver)
        params.append("tid=%d" % sender)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("ERROR", ob["status"])

    def testGetGiftList(self):
        sid, smoney = self._addUser()
        sender = int(sid)
        rid, rmoney = self._addUser()
        receiver = int(rid)

        # send gift
        params = []
        params.append("type=4")
        params.append("uid=%d" % sender)
        params.append("tid=%d" % receiver)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])

        # get gifts
        params = []
        params.append("type=9")
        params.append("uid=%d" % receiver)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(100, ob["value"])        
        self.assertEqual(1, len(ob["gifts"]))
        self.assertEqual(sender, ob["gifts"][0]["uid"])
        self.assertFalse(ob["gifts"][0]["received"])

        '''Send gift api'''
    def testOneKeySendGift(self):
        params = []
        params.append("type=6")
        params.append("uid=1")
        params.append("tids=41,42,43")
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])

        # get gifts
        params = []
        params.append("type=9")
        params.append("uid=41")
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(100, ob["value"])        
        self.assertEqual(1, len(ob["gifts"]))

        params = []
        params.append("type=9")
        params.append("uid=42")
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(100, ob["value"])        
        self.assertEqual(1, len(ob["gifts"]))
        
        params = []
        params.append("type=9")
        params.append("uid=43")
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(100, ob["value"])        
        self.assertEqual(1, len(ob["gifts"]))

    def testGetCachedReward(self):
        u, m = self._addUser()
        params = []
        params.append("type=7")
        params.append("uid=%s" % u)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])        

    def testDeleteFriend(self):
        uid, m = self._addUser()
        tid, m = self._addUser()

        # add friend
        params = []
        params.append("type=3")
        params.append("uid=%s" % uid)
        params.append("tid=%s" % tid)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])

        params = []
        params.append("type=0")
        params.append("uid=%s" % uid)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(1, len(ob["friends"]))
        self.assertEqual(tid, ob["friends"][0]["uid"])

        # delete friend
        params = []
        params.append("type=8")
        params.append("uid=%s" % uid)
        params.append("tid=%s" % tid)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])

        params = []
        params.append("type=0")
        params.append("uid=%s" % uid)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(0, len(ob["friends"]))

    def testOneKeyTakeGold(self):
        sid, smoney = self._addUser()
        sender = int(sid)
        rid, rmoney = self._addUser()
        receiver = int(rid)

        # send gift
        params = []
        params.append("type=4")
        params.append("uid=%d" % sender)
        params.append("tid=%d" % receiver)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])

        # get gifts
        params = []
        params.append("type=9")
        params.append("uid=%d" % receiver)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(100, ob["value"])        
        self.assertEqual(1, len(ob["gifts"]))
        self.assertEqual(sender, ob["gifts"][0]["uid"])
        self.assertFalse(ob["gifts"][0]["received"])

        # take gift one key
        params = []
        params.append("type=10")
        params.append("uid=%d" % receiver)
        ob = self._request(params)        
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])

        # check gifts
        params = []
        params.append("type=9")
        params.append("uid=%d" % receiver)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(100, ob["value"])        
        self.assertEqual(1, len(ob["gifts"]))
        self.assertEqual(sender, ob["gifts"][0]["uid"])
        self.assertTrue(ob["gifts"][0]["received"])

    def testGetAllInfo(self):
        uid = 41
        params = []
        params.append("type=1")
        params.append("uid=%d" % uid)
        ob = self._request(params)
        self.assertTrue(ob is not None)
        self.assertEqual("OK", ob["status"])
        self.assertEqual(0, len(ob["friends"]))
        self.assertEqual(10, ob["invite_count"])
        self.assertEqual(300, ob["total_reward"])
        self.assertEqual(200, ob["cached_reward"])

