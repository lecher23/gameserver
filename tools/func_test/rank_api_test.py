#!/bin/env/python
# -*- coding:utf-8 -*-
import unittest
import os
import sys
import httplib
import time
import random
import json
from api_test_base import ApiTestBase

CUR_DIR = os.path.split(os.path.realpath(__file__))[0]

class RankApiTest(ApiTestBase):

    def testGetCurrentEarnedRank(self):
        params = {}
        params["uid"] = "11498"
        params["type"] = "1"
        ob = self._request(params, "/slots/rank")
        self.assertEqual("OK", ob["status"])
        self.assertEqual(1, ob["myrank"])
        self.assertTrue(len(ob["data"]))

    def testGetCurrentFortuneRank(self):
        params = {}
        params["uid"] = "11498"
        params["type"] = "0"
        ob = self._request(params, "/slots/rank")
        self.assertEqual("OK", ob["status"])
        self.assertEqual(7, ob["myrank"])
        self.assertTrue(len(ob["data"]))
        
