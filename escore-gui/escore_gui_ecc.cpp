#include "escore_gui_pch.h"
#pragma hdrstop

USEFORM("TFmeBrowseForFolder.cpp", FmeBrowseForFolder); /* TFrame: File Type */
USEFORM("TFmeAppLog.cpp", FmeAppLog); /* TFrame: File Type */
USEFORM("TDlgUserPrompt.cpp", DlgUserPrompt);
USEFORM("TDlgBrowseForFolder.cpp", DlgBrowseForFolder);
USEFORM("TFrmReflectedClassConfig.cpp", FrmReflectedClassConfig); /* TFrame: File Type */
USEFORM("TFmeProgressMonitorView.cpp", FmeProgressMonitorView); /* TFrame: File Type */
USEFORM("TDlgBase.cpp", DlgBase);
//---------------------------------------------------------------------------
#pragma package(smart_init)
#include "autolink.cxx"

//---------------------------------------------------------------------------
//   Package source.
//---------------------------------------------------------------------------

#pragma argsused
extern "C" int _libmain(unsigned long reason)
{
	return 1;
}
//---------------------------------------------------------------------------
