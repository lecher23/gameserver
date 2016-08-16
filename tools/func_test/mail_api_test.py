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

    def testGetUserMailsWithoutSegment(self):
        params = {}
        params["uid"] = "6"
        params["type"] = "0"
        ob = self._request(params, "/slots/mail")
        self.assertEqual("OK", ob["status"])
        print ob

        
