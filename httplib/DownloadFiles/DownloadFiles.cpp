#include "stdafx.h"
#include "DownloadFiles.h"
#include "DownloadFilesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDownloadFilesApp theApp;

BEGIN_MESSAGE_MAP(CDownloadFilesApp, CWinApp)
END_MESSAGE_MAP()

CDownloadFilesApp::CDownloadFilesApp()
{
}

BOOL CDownloadFilesApp::InitInstance()
{
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);
    CWinApp::InitInstance();

    CDownloadFilesDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    return FALSE;
}
