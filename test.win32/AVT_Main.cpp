#include "StdAfx.h"
#include "AVT_MainDialog.h"

CAppModule _AppModule;

//WinMain Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR lpCmdLine, int nCmdShow)
{
	srand(::GetTickCount());

	// Init WTL app module
	::InitCommonControls();
	_AppModule.Init(NULL, hInstance);

	//create main dialog
	CMainDialog dlg;
	dlg.DoModal();

	// Close WTL app module
	_AppModule.Term();
	return 0;
}

