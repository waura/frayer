#ifndef _LIBIMGEDIT_H_
#define _LIBIMGEDIT_H_

#include "LibIEDllHeader.h"

#include "EditNode.h"
#include "IEffectEditDialog.h"
#include "IEffect.h"
#include "IEBrush.h"
#include "IEBrushIO.h"
#include "IEColor.h"
#include "IIETool.h"
#include "ImgEdit.h"
#include "ImgFile.h"
#include "ImgLayer.h"
#include "ImgLayerGroup.h"
#include "IEMenuItemInfo.h"
#include "IEMenuMG.h"
#include "ImgFileHandleMG.h"
#include "IEModules.h"
#include "IEColorDialog.h"
#include "IEDebugDialog.h"
#include "IEFileIOCommon.h"
#include "IETimer.h"
#include "OutputError.h"

//mg
#include "LayerSynthesizerMG.h"
#include "MaskSynthesizerMG.h"

//event listener
#include "ImgLayerEventListener.h"
#include "ImgFileEventListener.h"
#include "ImgEditEventListener.h"

//file handle
#include "NullHandle.h"
#include "StackHandle.h"
#include "AddCopyLayerHandle.h"
#include "AddNewLayerHandle.h"
#include "AddNewLayerGroupHandle.h"
#include "ChangeCanvasSizeHandle.h"
#include "ChangeLayerLineHandle.h"
#include "ChangeLayerOpacityHandle.h"
#include "ChangeLayerSynthFourCCHandle.h"
#include "ChangeResolutionHandle.h"
#include "EditLayerHandle.h"
#include "EditMaskHandle.h"
#include "MergeLayerHandle.h"
#include "MoveLayerHandle.h"
#include "RemoveLayerHandle.h"
#include "SetLayerNameHandle.h"

extern HMODULE g_hModule;

#endif //_LIBIMGEDIT_H_