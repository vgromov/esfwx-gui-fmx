//---------------------------------------------------------------------------
#include "tests_pch.h"
#pragma hdrstop

//---------------------------------------------------------------------------
USEFORM("TFrmMain.cpp", FrmMain);
//---------------------------------------------------------------------------

#pragma package(smart_init)
#include "autolink.cxx"

extern "C" int FMXmain()
{
  escore::reflectionInit();
  escomm::reflectionInit();
  esmath::reflectionInit();
  esscript::reflectionInit();

	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TFrmMain), &FrmMain);
    Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
