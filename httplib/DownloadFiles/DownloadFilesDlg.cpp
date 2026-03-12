#include "stdafx.h"
#include "DownloadFiles.h"
#include "DownloadFilesDlg.h"
#include "httplib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CString (Unicode) 转 std::string (UTF-8)
static std::string CStringToUtf8(const CString& str)
{
    if (str.IsEmpty()) return std::string();
    int len = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
    if (len <= 0) return std::string();
    std::string result(len - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, str, -1, &result[0], len, NULL, NULL);
    return result;
}

// 将完整 URL 解析为 (scheme_host_port, path)，供 httplib::Client 使用
static bool ParseUrl(const std::string& url, std::string& schemeHostPort, std::string& path)
{
    size_t pos = url.find("://");
    if (pos == std::string::npos) return false;
    pos += 3;
    size_t slash = url.find('/', pos);
    if (slash == std::string::npos) {
        schemeHostPort = url;
        path = "/";
    } else {
        schemeHostPort = url.substr(0, slash);
        path = url.substr(slash);
        if (path.empty()) path = "/";
    }
    return true;
}

CDownloadFilesDlg::CDownloadFilesDlg(CWnd* pParent)
    : CDialogEx(IDD_DOWNLOADFILES_DIALOG, pParent)
    , m_strUrl(_T(""))
    , m_strSavePath(_T(""))
    , m_nMethod(0)
    , m_strHttpParams(_T(""))
    , m_strStatus(_T(""))
{
}

void CDownloadFilesDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_URL, m_strUrl);
    DDX_Text(pDX, IDC_EDIT_SAVE_PATH, m_strSavePath);
    DDX_Radio(pDX, IDC_RADIO_GET, m_nMethod);
    DDX_Text(pDX, IDC_EDIT_HTTP_PARAMS, m_strHttpParams);
    DDX_Text(pDX, IDC_STATIC_STATUS, m_strStatus);
}

BEGIN_MESSAGE_MAP(CDownloadFilesDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_BROWSE, &CDownloadFilesDlg::OnBnClickedBrowse)
    ON_BN_CLICKED(IDC_BTN_DOWNLOAD, &CDownloadFilesDlg::OnBnClickedDownload)
END_MESSAGE_MAP()

BOOL CDownloadFilesDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_strStatus = _T("Ready");
    UpdateData(FALSE);
    return TRUE;
}

void CDownloadFilesDlg::OnBnClickedBrowse()
{
    CFileDialog dlg(FALSE, NULL, NULL,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        _T("All Files (*.*)|*.*||"), this);
    if (dlg.DoModal() == IDOK)
    {
        m_strSavePath = dlg.GetPathName();
        UpdateData(FALSE);
    }
}

void CDownloadFilesDlg::OnBnClickedDownload()
{
    UpdateData(TRUE);
    if (m_strUrl.IsEmpty())
    {
        AfxMessageBox(_T("Please enter the download URL."));
        return;
    }
    if (m_strSavePath.IsEmpty())
    {
        AfxMessageBox(_T("Please enter or select the save path."));
        return;
    }

    m_strStatus = _T("Downloading...");
    UpdateData(FALSE);
    GetDlgItem(IDC_BTN_DOWNLOAD)->EnableWindow(FALSE);

    std::string urlUtf8 = CStringToUtf8(m_strUrl);
    std::string schemeHostPort, path;
    if (!ParseUrl(urlUtf8, schemeHostPort, path))
    {
        m_strStatus = _T("Invalid URL format.");
        GetDlgItem(IDC_BTN_DOWNLOAD)->EnableWindow(TRUE);
        UpdateData(FALSE);
        return;
    }

    httplib::Headers headers;
    if (!m_strHttpParams.Trim().IsEmpty())
    {
        CString strParams = m_strHttpParams;
        strParams.Replace(_T("\r\n"), _T("\n"));
        strParams.Replace(_T('\r'), _T('\n'));
        int nStart = 0;
        for (;;)
        {
            int nEnd = strParams.Find(_T('\n'), nStart);
            CString strLine = (nEnd >= 0) ? strParams.Mid(nStart, nEnd - nStart) : strParams.Mid(nStart);
            strLine.Trim();
            if (!strLine.IsEmpty())
            {
                int colon = strLine.Find(_T(':'));
                if (colon > 0)
                {
                    CString key = strLine.Left(colon); key.Trim();
                    CString val = strLine.Mid(colon + 1); val.Trim();
                    headers.insert({ CStringToUtf8(key), CStringToUtf8(val) });
                }
            }
            if (nEnd < 0) break;
            nStart = nEnd + 1;
        }
    }

    httplib::Client cli(schemeHostPort);
    cli.set_connection_timeout(30, 0);
    cli.set_read_timeout(300, 0);

    httplib::Result res;
    if (m_nMethod == 0)  // GET
        res = headers.empty() ? cli.Get(path) : cli.Get(path, headers);
    else                 // POST
        res = headers.empty() ? cli.Post(path) : cli.Post(path, headers);

    if (!res)
    {
        m_strStatus.Format(_T("Request failed: %S"), httplib::to_string(res.error()).c_str());
        GetDlgItem(IDC_BTN_DOWNLOAD)->EnableWindow(TRUE);
        UpdateData(FALSE);
        return;
    }

    if (res->status != 200)
    {
        m_strStatus.Format(_T("HTTP error: %d"), res->status);
        GetDlgItem(IDC_BTN_DOWNLOAD)->EnableWindow(TRUE);
        UpdateData(FALSE);
        return;
    }

    CFile destFile;
    if (!destFile.Open(m_strSavePath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
    {
        m_strStatus = _T("Cannot create local file.");
        GetDlgItem(IDC_BTN_DOWNLOAD)->EnableWindow(TRUE);
        UpdateData(FALSE);
        return;
    }

    const std::string& body = res->body;
    if (!body.empty())
        destFile.Write(body.data(), (UINT)body.size());
    destFile.Close();

    m_strStatus.Format(_T("Download complete. %u bytes saved."), (unsigned)body.size());
    GetDlgItem(IDC_BTN_DOWNLOAD)->EnableWindow(TRUE);
    UpdateData(FALSE);
}
