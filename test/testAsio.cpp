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
bool suite_TestAsio_init = false;
#include "/home/licheng/workspace/slot_asio/CgServerPlus/test/testAsio.h"

static TestAsio suite_TestAsio;

static CxxTest::List Tests_TestAsio = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestAsio( "testAsio.h", 15, "TestAsio", suite_TestAsio, Tests_TestAsio );

static class TestDescription_suite_TestAsio_test_simple : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestAsio_test_simple() : CxxTest::RealTestDescription( Tests_TestAsio, suiteDescription_TestAsio, 31, "test_simple" ) {}
 void runTest() { suite_TestAsio.test_simple(); }
} testDescription_suite_TestAsio_test_simple;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
