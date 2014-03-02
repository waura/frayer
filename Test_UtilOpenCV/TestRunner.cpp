/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/ErrorPrinter.h>

int main() {
 return CxxTest::ErrorPrinter().run();
}
#include "UtilOpenCVTestSuite.h"

static UtilOpenCVTestSuite suite_UtilOpenCVTestSuite;

static CxxTest::List Tests_UtilOpenCVTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_UtilOpenCVTestSuite( "UtilOpenCVTestSuite.h", 5, "UtilOpenCVTestSuite", suite_UtilOpenCVTestSuite, Tests_UtilOpenCVTestSuite );

static class TestDescription_UtilOpenCVTestSuite_test_isEqualIplImage : public CxxTest::RealTestDescription {
public:
 TestDescription_UtilOpenCVTestSuite_test_isEqualIplImage() : CxxTest::RealTestDescription( Tests_UtilOpenCVTestSuite, suiteDescription_UtilOpenCVTestSuite, 16, "test_isEqualIplImage" ) {}
 void runTest() { suite_UtilOpenCVTestSuite.test_isEqualIplImage(); }
} testDescription_UtilOpenCVTestSuite_test_isEqualIplImage;

static class TestDescription_UtilOpenCVTestSuite_test_isFillZeroMask : public CxxTest::RealTestDescription {
public:
 TestDescription_UtilOpenCVTestSuite_test_isFillZeroMask() : CxxTest::RealTestDescription( Tests_UtilOpenCVTestSuite, suiteDescription_UtilOpenCVTestSuite, 49, "test_isFillZeroMask" ) {}
 void runTest() { suite_UtilOpenCVTestSuite.test_isFillZeroMask(); }
} testDescription_UtilOpenCVTestSuite_test_isFillZeroMask;

static class TestDescription_UtilOpenCVTestSuite_test_isNotFillZeroMask : public CxxTest::RealTestDescription {
public:
 TestDescription_UtilOpenCVTestSuite_test_isNotFillZeroMask() : CxxTest::RealTestDescription( Tests_UtilOpenCVTestSuite, suiteDescription_UtilOpenCVTestSuite, 62, "test_isNotFillZeroMask" ) {}
 void runTest() { suite_UtilOpenCVTestSuite.test_isNotFillZeroMask(); }
} testDescription_UtilOpenCVTestSuite_test_isNotFillZeroMask;

static class TestDescription_UtilOpenCVTestSuite_test_isFillColor : public CxxTest::RealTestDescription {
public:
 TestDescription_UtilOpenCVTestSuite_test_isFillColor() : CxxTest::RealTestDescription( Tests_UtilOpenCVTestSuite, suiteDescription_UtilOpenCVTestSuite, 75, "test_isFillColor" ) {}
 void runTest() { suite_UtilOpenCVTestSuite.test_isFillColor(); }
} testDescription_UtilOpenCVTestSuite_test_isFillColor;

static class TestDescription_UtilOpenCVTestSuite_test_isInRect : public CxxTest::RealTestDescription {
public:
 TestDescription_UtilOpenCVTestSuite_test_isInRect() : CxxTest::RealTestDescription( Tests_UtilOpenCVTestSuite, suiteDescription_UtilOpenCVTestSuite, 92, "test_isInRect" ) {}
 void runTest() { suite_UtilOpenCVTestSuite.test_isInRect(); }
} testDescription_UtilOpenCVTestSuite_test_isInRect;

#include "UtilOpenCVBltTestSuite.h"

static UtilOpenCVBltTestSuite suite_UtilOpenCVBltTestSuite;

static CxxTest::List Tests_UtilOpenCVBltTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_UtilOpenCVBltTestSuite( "UtilOpenCVBltTestSuite.h", 5, "UtilOpenCVBltTestSuite", suite_UtilOpenCVBltTestSuite, Tests_UtilOpenCVBltTestSuite );

static class TestDescription_UtilOpenCVBltTestSuite_test_ssememcpy : public CxxTest::RealTestDescription {
public:
 TestDescription_UtilOpenCVBltTestSuite_test_ssememcpy() : CxxTest::RealTestDescription( Tests_UtilOpenCVBltTestSuite, suiteDescription_UtilOpenCVBltTestSuite, 16, "test_ssememcpy" ) {}
 void runTest() { suite_UtilOpenCVBltTestSuite.test_ssememcpy(); }
} testDescription_UtilOpenCVBltTestSuite_test_ssememcpy;

#include "IplImageExtTestSuite.h"

static IplImageExtTestSuite suite_IplImageExtTestSuite;

static CxxTest::List Tests_IplImageExtTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_IplImageExtTestSuite( "IplImageExtTestSuite.h", 6, "IplImageExtTestSuite", suite_IplImageExtTestSuite, Tests_IplImageExtTestSuite );

static class TestDescription_IplImageExtTestSuite_test_Init : public CxxTest::RealTestDescription {
public:
 TestDescription_IplImageExtTestSuite_test_Init() : CxxTest::RealTestDescription( Tests_IplImageExtTestSuite, suiteDescription_IplImageExtTestSuite, 17, "test_Init" ) {}
 void runTest() { suite_IplImageExtTestSuite.test_Init(); }
} testDescription_IplImageExtTestSuite_test_Init;

static class TestDescription_IplImageExtTestSuite_test_ExtendImage : public CxxTest::RealTestDescription {
public:
 TestDescription_IplImageExtTestSuite_test_ExtendImage() : CxxTest::RealTestDescription( Tests_IplImageExtTestSuite, suiteDescription_IplImageExtTestSuite, 36, "test_ExtendImage" ) {}
 void runTest() { suite_IplImageExtTestSuite.test_ExtendImage(); }
} testDescription_IplImageExtTestSuite_test_ExtendImage;

static class TestDescription_IplImageExtTestSuite_test_IsFillZeroMask : public CxxTest::RealTestDescription {
public:
 TestDescription_IplImageExtTestSuite_test_IsFillZeroMask() : CxxTest::RealTestDescription( Tests_IplImageExtTestSuite, suiteDescription_IplImageExtTestSuite, 64, "test_IsFillZeroMask" ) {}
 void runTest() { suite_IplImageExtTestSuite.test_IsFillZeroMask(); }
} testDescription_IplImageExtTestSuite_test_IsFillZeroMask;

static class TestDescription_IplImageExtTestSuite_test_RectFillColor : public CxxTest::RealTestDescription {
public:
 TestDescription_IplImageExtTestSuite_test_RectFillColor() : CxxTest::RealTestDescription( Tests_IplImageExtTestSuite, suiteDescription_IplImageExtTestSuite, 71, "test_RectFillColor" ) {}
 void runTest() { suite_IplImageExtTestSuite.test_RectFillColor(); }
} testDescription_IplImageExtTestSuite_test_RectFillColor;

static class TestDescription_IplImageExtTestSuite_test_OperateBlt_IplImageExt_noSrc : public CxxTest::RealTestDescription {
public:
 TestDescription_IplImageExtTestSuite_test_OperateBlt_IplImageExt_noSrc() : CxxTest::RealTestDescription( Tests_IplImageExtTestSuite, suiteDescription_IplImageExtTestSuite, 113, "test_OperateBlt_IplImageExt_noSrc" ) {}
 void runTest() { suite_IplImageExtTestSuite.test_OperateBlt_IplImageExt_noSrc(); }
} testDescription_IplImageExtTestSuite_test_OperateBlt_IplImageExt_noSrc;

static class TestDescription_IplImageExtTestSuite_test_OperateBlt_IplImageExt_noSrc_After_ExtendImage : public CxxTest::RealTestDescription {
public:
 TestDescription_IplImageExtTestSuite_test_OperateBlt_IplImageExt_noSrc_After_ExtendImage() : CxxTest::RealTestDescription( Tests_IplImageExtTestSuite, suiteDescription_IplImageExtTestSuite, 206, "test_OperateBlt_IplImageExt_noSrc_After_ExtendImage" ) {}
 void runTest() { suite_IplImageExtTestSuite.test_OperateBlt_IplImageExt_noSrc_After_ExtendImage(); }
} testDescription_IplImageExtTestSuite_test_OperateBlt_IplImageExt_noSrc_After_ExtendImage;

static class TestDescription_IplImageExtTestSuite_test_OperateEdit_IplImageExt : public CxxTest::RealTestDescription {
public:
 TestDescription_IplImageExtTestSuite_test_OperateEdit_IplImageExt() : CxxTest::RealTestDescription( Tests_IplImageExtTestSuite, suiteDescription_IplImageExtTestSuite, 297, "test_OperateEdit_IplImageExt" ) {}
 void runTest() { suite_IplImageExtTestSuite.test_OperateEdit_IplImageExt(); }
} testDescription_IplImageExtTestSuite_test_OperateEdit_IplImageExt;

static class TestDescription_IplImageExtTestSuite_test_OperateEdit_IplImage : public CxxTest::RealTestDescription {
public:
 TestDescription_IplImageExtTestSuite_test_OperateEdit_IplImage() : CxxTest::RealTestDescription( Tests_IplImageExtTestSuite, suiteDescription_IplImageExtTestSuite, 381, "test_OperateEdit_IplImage" ) {}
 void runTest() { suite_IplImageExtTestSuite.test_OperateEdit_IplImage(); }
} testDescription_IplImageExtTestSuite_test_OperateEdit_IplImage;

#include <cxxtest/Root.cpp>
