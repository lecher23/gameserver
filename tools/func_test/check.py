#!/bin/env python
import os
import re
import types
import sys
import unittest

CUR_PATH = os.path.split(os.path.realpath(__file__))[0]
#PARENT_PATH = os.path.join(CUR_PATH, "../../../")
#sys.path.append(PARENT_PATH)

def addTest(testSuite, moduleName):
    print moduleName
    testModule = __import__(moduleName)
    for name in dir(testModule):
        obj = getattr(testModule, name)
        if type(obj) == types.TypeType and issubclass(obj, unittest.TestCase):
            print 'run case: %s' % str(obj)
            testSuite.addTest(unittest.makeSuite(obj, 'test'))

def runTest(caseList):
    runner = unittest.TextTestRunner()
    allTests = unittest.TestSuite()

    curPath = os.path.split(os.path.realpath(__file__))[0]
    for case in caseList:
        if case.startswith('.'):
            continue
        m = re.match(r"(.*?)_test.py$", case)
        if m is None:
            continue
        caseName = m.group(1) + "_test"
        addTest(allTests, caseName)

    return runner.run(allTests).wasSuccessful()

if __name__ == "__main__":
    caseList = os.listdir(CUR_PATH)
    if len(sys.argv) > 1:
        caseList = []
        for i in range (1, len(sys.argv)):
            caseList.append(sys.argv[i])
    if not runTest(caseList):
        sys.exit(-1)
