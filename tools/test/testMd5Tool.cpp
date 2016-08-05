/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/ErrorPrinter.h>

int main( int argc, char *argv[] ) {
 int status;
    CxxTest::ErrorPrinter tmp;
    CxxTest::RealWorldDescription::_worldName = "cxxtest";
    status = CxxTest::Main< CxxTest::ErrorPrinter >( tmp, argc, argv );
    return status;
}
bool suite_TestMd5Tool_init = false;
#include "/home/licheng/workspace/CgServerPlus/tools/test/testMd5Tool.h"

static TestMd5Tool suite_TestMd5Tool;

static CxxTest::List Tests_TestMd5Tool = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestMd5Tool( "/home/licheng/workspace/CgServerPlus/tools/test/testMd5Tool.h", 10, "TestMd5Tool", suite_TestMd5Tool, Tests_TestMd5Tool );

static class TestDescription_suite_TestMd5Tool_test_update_long_str : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestMd5Tool_test_update_long_str() : CxxTest::RealTestDescription( Tests_TestMd5Tool, suiteDescription_TestMd5Tool, 13, "test_update_long_str" ) {}
 void runTest() { suite_TestMd5Tool.test_update_long_str(); }
} testDescription_suite_TestMd5Tool_test_update_long_str;

static class TestDescription_suite_TestMd5Tool_test_update_normal_str : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestMd5Tool_test_update_normal_str() : CxxTest::RealTestDescription( Tests_TestMd5Tool, suiteDescription_TestMd5Tool, 23, "test_update_normal_str" ) {}
 void runTest() { suite_TestMd5Tool.test_update_normal_str(); }
} testDescription_suite_TestMd5Tool_test_update_normal_str;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
