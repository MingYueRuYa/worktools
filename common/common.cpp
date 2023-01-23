/****************************************************************************
**
** Copyright (C) 2018 liushixiong (liushixiongcpp@163.com)
** All rights reserved.
**
****************************************************************************/

#include "common.h"

#include <windows.h>

#pragma comment(lib,"Version.lib")

QString GetProductVer(const QString pFilePath)
{
    std::wstring wstrfilepath = pFilePath.toStdWString().c_str();
    DWORD dwHanle = 0;
    DWORD dwSize = GetFileVersionInfoSize(wstrfilepath.c_str(), &dwHanle);
    if (0 == dwSize) {
        return "";
    }

    char *buf = new char[dwSize + 1];
    memset(buf, 0, dwSize + 1);

    DWORD dwRet = GetFileVersionInfo(wstrfilepath.c_str(), dwHanle,
        dwSize, buf);
    if (0 == dwRet) {
        return "";
    }

    LPVOID lpBuf = NULL;
    UINT uLen = 0;
    if (0 == VerQueryValue(buf, TEXT("\\"), &lpBuf, &uLen)) {
        return "";
    }

    VS_FIXEDFILEINFO *fileinfo = (VS_FIXEDFILEINFO *)lpBuf;
    WORD d1 = HIWORD(fileinfo->dwFileVersionMS);
    WORD d2 = LOWORD(fileinfo->dwFileVersionMS);
    WORD d3 = HIWORD(fileinfo->dwFileVersionLS);
    WORD d4 = LOWORD(fileinfo->dwFileVersionLS);

    return QString("%1.%2.%3.%4").arg(d1).arg(d2).arg(d3).arg(d4);
}

std::wstring StringtoWString(const std::string& str)
{
    int nLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    wchar_t* szRes = new wchar_t[nLen];
    memset(szRes, 0, nLen);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, szRes, nLen);
    std::wstring strRtn(szRes);
    delete[] szRes;
    return strRtn;
}

QString WrapHtmlStr(const QString pColor, const QString pMsgInfo, 
					uint pFontSize)
{
	QString str = QString("<p><font size=\"%1\" face=\"Microsoft YaHei\"" 
						"color=\"%2\">%3</font></p>").arg(pFontSize).arg(
						pColor).arg(pMsgInfo);
	return str;
}
