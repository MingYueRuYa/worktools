#pragma once
#include "resource.h"

class CDownloadFilesDlg : public CDialogEx
{
public:
    CDownloadFilesDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DOWNLOADFILES_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

public:
    CString m_strUrl;
    CString m_strSavePath;
    int    m_nMethod;       // 0=GET, 1=POST
    CString m_strHttpParams;
    CString m_strStatus;

    afx_msg void OnBnClickedBrowse();
    afx_msg void OnBnClickedDownload();
};
