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
#include "FileHandleTestSuite.h"

static FileHandleTestSuite suite_FileHandleTestSuite;

static CxxTest::List Tests_FileHandleTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_FileHandleTestSuite( "FileHandleTestSuite.h", 6, "FileHandleTestSuite", suite_FileHandleTestSuite, Tests_FileHandleTestSuite );

static class TestDescription_FileHandleTestSuite_test_EditLayerHandle : public CxxTest::RealTestDescription {
public:
 TestDescription_FileHandleTestSuite_test_EditLayerHandle() : CxxTest::RealTestDescription( Tests_FileHandleTestSuite, suiteDescription_FileHandleTestSuite, 33, "test_EditLayerHandle" ) {}
 void runTest() { suite_FileHandleTestSuite.test_EditLayerHandle(); }
} testDescription_FileHandleTestSuite_test_EditLayerHandle;

static class TestDescription_FileHandleTestSuite_test_EditLayerHandle_some_update : public CxxTest::RealTestDescription {
public:
 TestDescription_FileHandleTestSuite_test_EditLayerHandle_some_update() : CxxTest::RealTestDescription( Tests_FileHandleTestSuite, suiteDescription_FileHandleTestSuite, 165, "test_EditLayerHandle_some_update" ) {}
 void runTest() { suite_FileHandleTestSuite.test_EditLayerHandle_some_update(); }
} testDescription_FileHandleTestSuite_test_EditLayerHandle_some_update;

static class TestDescription_FileHandleTestSuite_test_MergeLayerHandle : public CxxTest::RealTestDescription {
public:
 TestDescription_FileHandleTestSuite_test_MergeLayerHandle() : CxxTest::RealTestDescription( Tests_FileHandleTestSuite, suiteDescription_FileHandleTestSuite, 256, "test_MergeLayerHandle" ) {}
 void runTest() { suite_FileHandleTestSuite.test_MergeLayerHandle(); }
} testDescription_FileHandleTestSuite_test_MergeLayerHandle;

static class TestDescription_FileHandleTestSuite_test_MoveLayerHandle : public CxxTest::RealTestDescription {
public:
 TestDescription_FileHandleTestSuite_test_MoveLayerHandle() : CxxTest::RealTestDescription( Tests_FileHandleTestSuite, suiteDescription_FileHandleTestSuite, 303, "test_MoveLayerHandle" ) {}
 void runTest() { suite_FileHandleTestSuite.test_MoveLayerHandle(); }
} testDescription_FileHandleTestSuite_test_MoveLayerHandle;

static class TestDescription_FileHandleTestSuite_test_MoveLayerHandle2 : public CxxTest::RealTestDescription {
public:
 TestDescription_FileHandleTestSuite_test_MoveLayerHandle2() : CxxTest::RealTestDescription( Tests_FileHandleTestSuite, suiteDescription_FileHandleTestSuite, 419, "test_MoveLayerHandle2" ) {}
 void runTest() { suite_FileHandleTestSuite.test_MoveLayerHandle2(); }
} testDescription_FileHandleTestSuite_test_MoveLayerHandle2;

#include "IEBrushMGTestSuite.h"

static IEBrushMGTestSuite suite_IEBrushMGTestSuite;

static CxxTest::List Tests_IEBrushMGTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_IEBrushMGTestSuite( "IEBrushMGTestSuite.h", 6, "IEBrushMGTestSuite", suite_IEBrushMGTestSuite, Tests_IEBrushMGTestSuite );

static class TestDescription_IEBrushMGTestSuite_test_LoadBrush_abr : public CxxTest::RealTestDescription {
public:
 TestDescription_IEBrushMGTestSuite_test_LoadBrush_abr() : CxxTest::RealTestDescription( Tests_IEBrushMGTestSuite, suiteDescription_IEBrushMGTestSuite, 33, "test_LoadBrush_abr" ) {}
 void runTest() { suite_IEBrushMGTestSuite.test_LoadBrush_abr(); }
} testDescription_IEBrushMGTestSuite_test_LoadBrush_abr;

static class TestDescription_IEBrushMGTestSuite_test_Load_Save_Load_BrushImg : public CxxTest::RealTestDescription {
public:
 TestDescription_IEBrushMGTestSuite_test_Load_Save_Load_BrushImg() : CxxTest::RealTestDescription( Tests_IEBrushMGTestSuite, suiteDescription_IEBrushMGTestSuite, 39, "test_Load_Save_Load_BrushImg" ) {}
 void runTest() { suite_IEBrushMGTestSuite.test_Load_Save_Load_BrushImg(); }
} testDescription_IEBrushMGTestSuite_test_Load_Save_Load_BrushImg;

#include "IEImgBrushTestSuite.h"

static IEImgBrushTestSuite suite_IEImgBrushTestSuite;

static CxxTest::List Tests_IEImgBrushTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_IEImgBrushTestSuite( "IEImgBrushTestSuite.h", 10, "IEImgBrushTestSuite", suite_IEImgBrushTestSuite, Tests_IEImgBrushTestSuite );

static class TestDescription_IEImgBrushTestSuite_test_Base64 : public CxxTest::RealTestDescription {
public:
 TestDescription_IEImgBrushTestSuite_test_Base64() : CxxTest::RealTestDescription( Tests_IEImgBrushTestSuite, suiteDescription_IEImgBrushTestSuite, 35, "test_Base64" ) {}
 void runTest() { suite_IEImgBrushTestSuite.test_Base64(); }
} testDescription_IEImgBrushTestSuite_test_Base64;

static class TestDescription_IEImgBrushTestSuite_test_LoadBrushImg : public CxxTest::RealTestDescription {
public:
 TestDescription_IEImgBrushTestSuite_test_LoadBrushImg() : CxxTest::RealTestDescription( Tests_IEImgBrushTestSuite, suiteDescription_IEImgBrushTestSuite, 64, "test_LoadBrushImg" ) {}
 void runTest() { suite_IEImgBrushTestSuite.test_LoadBrushImg(); }
} testDescription_IEImgBrushTestSuite_test_LoadBrushImg;

static class TestDescription_IEImgBrushTestSuite_test_RLE_Base64 : public CxxTest::RealTestDescription {
public:
 TestDescription_IEImgBrushTestSuite_test_RLE_Base64() : CxxTest::RealTestDescription( Tests_IEImgBrushTestSuite, suiteDescription_IEImgBrushTestSuite, 81, "test_RLE_Base64" ) {}
 void runTest() { suite_IEImgBrushTestSuite.test_RLE_Base64(); }
} testDescription_IEImgBrushTestSuite_test_RLE_Base64;

#include "IEIOfydTestSuite.h"

static IEIOfydTestSuite suite_IEIOfydTestSuite;

static CxxTest::List Tests_IEIOfydTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_IEIOfydTestSuite( "IEIOfydTestSuite.h", 6, "IEIOfydTestSuite", suite_IEIOfydTestSuite, Tests_IEIOfydTestSuite );

static class TestDescription_IEIOfydTestSuite_test_LoadImgFile_fyd : public CxxTest::RealTestDescription {
public:
 TestDescription_IEIOfydTestSuite_test_LoadImgFile_fyd() : CxxTest::RealTestDescription( Tests_IEIOfydTestSuite, suiteDescription_IEIOfydTestSuite, 31, "test_LoadImgFile_fyd" ) {}
 void runTest() { suite_IEIOfydTestSuite.test_LoadImgFile_fyd(); }
} testDescription_IEIOfydTestSuite_test_LoadImgFile_fyd;

static class TestDescription_IEIOfydTestSuite_test_RLE : public CxxTest::RealTestDescription {
public:
 TestDescription_IEIOfydTestSuite_test_RLE() : CxxTest::RealTestDescription( Tests_IEIOfydTestSuite, suiteDescription_IEIOfydTestSuite, 79, "test_RLE" ) {}
 void runTest() { suite_IEIOfydTestSuite.test_RLE(); }
} testDescription_IEIOfydTestSuite_test_RLE;

#include "ImgEditTestSuite.h"

static ImgEditTestSuite suite_ImgEditTestSuite;

static CxxTest::List Tests_ImgEditTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ImgEditTestSuite( "ImgEditTestSuite.h", 6, "ImgEditTestSuite", suite_ImgEditTestSuite, Tests_ImgEditTestSuite );

static class TestDescription_ImgEditTestSuite_test_hoge : public CxxTest::RealTestDescription {
public:
 TestDescription_ImgEditTestSuite_test_hoge() : CxxTest::RealTestDescription( Tests_ImgEditTestSuite, suiteDescription_ImgEditTestSuite, 31, "test_hoge" ) {}
 void runTest() { suite_ImgEditTestSuite.test_hoge(); }
} testDescription_ImgEditTestSuite_test_hoge;

#include "ImgFileTestSuite.h"

static ImgFileTestSuite suite_ImgFileTestSuite;

static CxxTest::List Tests_ImgFileTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ImgFileTestSuite( "ImgFileTestSuite.h", 6, "ImgFileTestSuite", suite_ImgFileTestSuite, Tests_ImgFileTestSuite );

static class TestDescription_ImgFileTestSuite_test_Update_return_val : public CxxTest::RealTestDescription {
public:
 TestDescription_ImgFileTestSuite_test_Update_return_val() : CxxTest::RealTestDescription( Tests_ImgFileTestSuite, suiteDescription_ImgFileTestSuite, 33, "test_Update_return_val" ) {}
 void runTest() { suite_ImgFileTestSuite.test_Update_return_val(); }
} testDescription_ImgFileTestSuite_test_Update_return_val;

static class TestDescription_ImgFileTestSuite_test_AddLayer : public CxxTest::RealTestDescription {
public:
 TestDescription_ImgFileTestSuite_test_AddLayer() : CxxTest::RealTestDescription( Tests_ImgFileTestSuite, suiteDescription_ImgFileTestSuite, 58, "test_AddLayer" ) {}
 void runTest() { suite_ImgFileTestSuite.test_AddLayer(); }
} testDescription_ImgFileTestSuite_test_AddLayer;

static class TestDescription_ImgFileTestSuite_test_RemoveLayer : public CxxTest::RealTestDescription {
public:
 TestDescription_ImgFileTestSuite_test_RemoveLayer() : CxxTest::RealTestDescription( Tests_ImgFileTestSuite, suiteDescription_ImgFileTestSuite, 104, "test_RemoveLayer" ) {}
 void runTest() { suite_ImgFileTestSuite.test_RemoveLayer(); }
} testDescription_ImgFileTestSuite_test_RemoveLayer;

static class TestDescription_ImgFileTestSuite_test_ChangeLayerLine : public CxxTest::RealTestDescription {
public:
 TestDescription_ImgFileTestSuite_test_ChangeLayerLine() : CxxTest::RealTestDescription( Tests_ImgFileTestSuite, suiteDescription_ImgFileTestSuite, 168, "test_ChangeLayerLine" ) {}
 void runTest() { suite_ImgFileTestSuite.test_ChangeLayerLine(); }
} testDescription_ImgFileTestSuite_test_ChangeLayerLine;

static class TestDescription_ImgFileTestSuite_test_MaskState : public CxxTest::RealTestDescription {
public:
 TestDescription_ImgFileTestSuite_test_MaskState() : CxxTest::RealTestDescription( Tests_ImgFileTestSuite, suiteDescription_ImgFileTestSuite, 230, "test_MaskState" ) {}
 void runTest() { suite_ImgFileTestSuite.test_MaskState(); }
} testDescription_ImgFileTestSuite_test_MaskState;

static class TestDescription_ImgFileTestSuite_test_Redo_on_Used_Mask : public CxxTest::RealTestDescription {
public:
 TestDescription_ImgFileTestSuite_test_Redo_on_Used_Mask() : CxxTest::RealTestDescription( Tests_ImgFileTestSuite, suiteDescription_ImgFileTestSuite, 301, "test_Redo_on_Used_Mask" ) {}
 void runTest() { suite_ImgFileTestSuite.test_Redo_on_Used_Mask(); }
} testDescription_ImgFileTestSuite_test_Redo_on_Used_Mask;

#include "ImgLayerTestSuite.h"

static ImgLayerTestSuite suite_ImgLayerTestSuite;

static CxxTest::List Tests_ImgLayerTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ImgLayerTestSuite( "ImgLayerTestSuite.h", 6, "ImgLayerTestSuite", suite_ImgLayerTestSuite, Tests_ImgLayerTestSuite );

static class TestDescription_ImgLayerTestSuite_test_AddEditNode_Use_Mask : public CxxTest::RealTestDescription {
public:
 TestDescription_ImgLayerTestSuite_test_AddEditNode_Use_Mask() : CxxTest::RealTestDescription( Tests_ImgLayerTestSuite, suiteDescription_ImgLayerTestSuite, 33, "test_AddEditNode_Use_Mask" ) {}
 void runTest() { suite_ImgLayerTestSuite.test_AddEditNode_Use_Mask(); }
} testDescription_ImgLayerTestSuite_test_AddEditNode_Use_Mask;

static class TestDescription_ImgLayerTestSuite_test_Undo_Before_LayerMove : public CxxTest::RealTestDescription {
public:
 TestDescription_ImgLayerTestSuite_test_Undo_Before_LayerMove() : CxxTest::RealTestDescription( Tests_ImgLayerTestSuite, suiteDescription_ImgLayerTestSuite, 164, "test_Undo_Before_LayerMove" ) {}
 void runTest() { suite_ImgLayerTestSuite.test_Undo_Before_LayerMove(); }
} testDescription_ImgLayerTestSuite_test_Undo_Before_LayerMove;

static class TestDescription_ImgLayerTestSuite_test_Undo_After_LayerMove : public CxxTest::RealTestDescription {
public:
 TestDescription_ImgLayerTestSuite_test_Undo_After_LayerMove() : CxxTest::RealTestDescription( Tests_ImgLayerTestSuite, suiteDescription_ImgLayerTestSuite, 246, "test_Undo_After_LayerMove" ) {}
 void runTest() { suite_ImgLayerTestSuite.test_Undo_After_LayerMove(); }
} testDescription_ImgLayerTestSuite_test_Undo_After_LayerMove;

#include "LayerSynthesizerTestSuite.h"

static LayerSynthesizerTestSuite suite_LayerSynthesizerTestSuite;

static CxxTest::List Tests_LayerSynthesizerTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_LayerSynthesizerTestSuite( "LayerSynthesizerTestSuite.h", 5, "LayerSynthesizerTestSuite", suite_LayerSynthesizerTestSuite, Tests_LayerSynthesizerTestSuite );

static class TestDescription_LayerSynthesizerTestSuite_test_BlendSynth_position : public CxxTest::RealTestDescription {
public:
 TestDescription_LayerSynthesizerTestSuite_test_BlendSynth_position() : CxxTest::RealTestDescription( Tests_LayerSynthesizerTestSuite, suiteDescription_LayerSynthesizerTestSuite, 29, "test_BlendSynth_position" ) {}
 void runTest() { suite_LayerSynthesizerTestSuite.test_BlendSynth_position(); }
} testDescription_LayerSynthesizerTestSuite_test_BlendSynth_position;

static class TestDescription_LayerSynthesizerTestSuite_test_BlendSynth_forcc : public CxxTest::RealTestDescription {
public:
 TestDescription_LayerSynthesizerTestSuite_test_BlendSynth_forcc() : CxxTest::RealTestDescription( Tests_LayerSynthesizerTestSuite, suiteDescription_LayerSynthesizerTestSuite, 258, "test_BlendSynth_forcc" ) {}
 void runTest() { suite_LayerSynthesizerTestSuite.test_BlendSynth_forcc(); }
} testDescription_LayerSynthesizerTestSuite_test_BlendSynth_forcc;

static class TestDescription_LayerSynthesizerTestSuite_test_BltSynth_forcc : public CxxTest::RealTestDescription {
public:
 TestDescription_LayerSynthesizerTestSuite_test_BltSynth_forcc() : CxxTest::RealTestDescription( Tests_LayerSynthesizerTestSuite, suiteDescription_LayerSynthesizerTestSuite, 508, "test_BltSynth_forcc" ) {}
 void runTest() { suite_LayerSynthesizerTestSuite.test_BltSynth_forcc(); }
} testDescription_LayerSynthesizerTestSuite_test_BltSynth_forcc;

static class TestDescription_LayerSynthesizerTestSuite_test_BlendSynth_alphablend : public CxxTest::RealTestDescription {
public:
 TestDescription_LayerSynthesizerTestSuite_test_BlendSynth_alphablend() : CxxTest::RealTestDescription( Tests_LayerSynthesizerTestSuite, suiteDescription_LayerSynthesizerTestSuite, 776, "test_BlendSynth_alphablend" ) {}
 void runTest() { suite_LayerSynthesizerTestSuite.test_BlendSynth_alphablend(); }
} testDescription_LayerSynthesizerTestSuite_test_BlendSynth_alphablend;

static class TestDescription_LayerSynthesizerTestSuite_test_BltSynth_alphablend : public CxxTest::RealTestDescription {
public:
 TestDescription_LayerSynthesizerTestSuite_test_BltSynth_alphablend() : CxxTest::RealTestDescription( Tests_LayerSynthesizerTestSuite, suiteDescription_LayerSynthesizerTestSuite, 866, "test_BltSynth_alphablend" ) {}
 void runTest() { suite_LayerSynthesizerTestSuite.test_BltSynth_alphablend(); }
} testDescription_LayerSynthesizerTestSuite_test_BltSynth_alphablend;

#include <cxxtest/Root.cpp>
