#pragma once
#include "resource.h"

class CDownloadFilesApp : public CWinApp
{
public:
    CDownloadFilesApp();
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CDownloadFilesApp theApp;
