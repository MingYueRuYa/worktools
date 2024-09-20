/****************************************************************************
**
** Copyright (C) 2018 liushixiong (liushixiongcpp@163.com)
** All rights reserved.
**
****************************************************************************/

#include "dolproductinfo.h"

#include <wincrypt.h>
#include <wintrust.h>

#include "common.h"

#pragma comment(lib, "crypt32.lib")

#define ENCODING (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)

const int LINE_BUFFER_SIZE = 2048;

dolProductInfo::dolProductInfo(const string& pFilePath, bool pIsInit)
    : mFilePath(pFilePath) {
  if (pIsInit) {
    Init();
  }
}

dolProductInfo::~dolProductInfo() {}

bool dolProductInfo::Init() {
  DWORD temp;
  UINT size;
  char tempbuf[LINE_BUFFER_SIZE] = {0};

  HANDLE hFile = CreateFileA(mFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ,
                             NULL, OPEN_EXISTING, 0, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    return false;
  }

  char szMZHader[0x40];
  char szMewHader[0x1000];
  DWORD read;
  bool result = ReadFile(hFile, szMZHader, 0x40, &read, NULL);
  if (!result || read != 0x40) {
    return false;
  }

  DWORD filesize = GetFileSize(hFile, NULL);
  FormatDWord(filesize, tempbuf);
  mSize = tempbuf;
  mSize += " Bytes";
  ZeroMemory(tempbuf, LINE_BUFFER_SIZE);

  FILETIME ftCreation, ftModified;
  GetFileTime(hFile, &ftCreation, NULL, &ftModified);
  FormatFileTime(ftCreation, tempbuf);
  mCreationTime = tempbuf;
  ZeroMemory(tempbuf, LINE_BUFFER_SIZE);
  FormatFileTime(ftModified, tempbuf);
  mModifiedTime = tempbuf;
  ZeroMemory(tempbuf, LINE_BUFFER_SIZE);

  CloseHandle(hFile);

  if ((size = GetFileVersionInfoSizeA(mFilePath.c_str(), &temp)) < 0) {
    return false;
  }

  char* buf = new char[size];
  char* str;
  char szLang1[20];

  if (!GetFileVersionInfoA(mFilePath.c_str(), 0, size, buf)) {
    delete[] buf;
    return false;
  }

  VS_FIXEDFILEINFO* info;
  if (VerQueryValueA((LPVOID)buf, ("\\"), (LPVOID*)&info, &size)) {
    string retvalue;
    mType = GetFileType(info->dwFileType, retvalue);

    sprintf_s(tempbuf, LINE_BUFFER_SIZE, "%d.%d.%d.%d",
              (info->dwFileVersionMS >> 16), (info->dwFileVersionMS & 65535),
              (info->dwFileVersionLS >> 16), info->dwFileVersionLS & 65535);
    mFileVersion = tempbuf;
    ZeroMemory(tempbuf, LINE_BUFFER_SIZE);

    sprintf_s(
        tempbuf, LINE_BUFFER_SIZE, "%d.%d.%d.%d",
        (info->dwProductVersionMS >> 16), (info->dwProductVersionMS & 65535),
        (info->dwProductVersionLS >> 16), info->dwProductVersionLS & 65535);
    mProductVersion = tempbuf;
    ZeroMemory(tempbuf, LINE_BUFFER_SIZE);
  }

  if (!VerQueryValueA((LPCVOID)buf, ("\\VarFileInfo\\Translation"),
                      (LPVOID*)&str, &size)) {
    // return false;
  }

  sprintf_s(szLang1, sizeof(szLang1) / sizeof(char), "%4.4X%4.4X",
            *(unsigned short*)str, *(unsigned short*)(str + 2));

  result = GetInfoStr(buf, szLang1, "ProductName", tempbuf);
  if (!result) {
    ZeroMemory(szLang1, 20);
    sprintf_s(szLang1, sizeof(szLang1) / sizeof(char), "%4.4X%4.4X",
              *(unsigned short*)str, 0x4E4);

    if (GetInfoStr(buf, szLang1, "ProductName", tempbuf)) {
      mProductName = tempbuf;
      ZeroMemory(tempbuf, LINE_BUFFER_SIZE);
    }
  } else {
    mProductName = tempbuf;
    ZeroMemory(tempbuf, LINE_BUFFER_SIZE);
  }

  if (GetInfoStr(buf, szLang1, "FileDescription", tempbuf)) {
    mFileDescription = tempbuf;
    ZeroMemory(tempbuf, LINE_BUFFER_SIZE);
  }

  if (GetInfoStr(buf, szLang1, "CompanyName", tempbuf)) {
    mCompanyName = tempbuf;
    ZeroMemory(tempbuf, LINE_BUFFER_SIZE);
  }

  if (GetInfoStr(buf, szLang1, "LegalCopyRight", tempbuf)) {
    mCopyRight = tempbuf;
    ZeroMemory(tempbuf, LINE_BUFFER_SIZE);
  }

  if (GetInfoStr(buf, szLang1, "OriginalFileName", tempbuf)) {
    mOriginName = tempbuf;
    ZeroMemory(tempbuf, LINE_BUFFER_SIZE);
  }

  string signstr = QueryDigSignature();

  delete[] buf;
  return true;
}

string dolProductInfo::GetFileType(DWORD pdwFileType, string& pszFileType) {
  const int count = 7;
  KEY_VALUE FilesTypes[count] = {{VFT_UNKNOWN, "Unkown"},
                                 {VFT_APP, "Application"},
                                 {VFT_DLL, "Dynamic-Link Library"},
                                 {VFT_DRV, "Device Driver"},
                                 {VFT_FONT, "Font"},
                                 {VFT_VXD, "Virtual Device"},
                                 {VFT_STATIC_LIB, "Static-Link Library"}};

  return KeyValueToStr(FilesTypes, count, pdwFileType, pszFileType);
}

string dolProductInfo::GetFilePath() const {
  return mFilePath;
}
void dolProductInfo::SetFilePath(const string& pFilePath) {
  mFilePath = pFilePath;
}

string dolProductInfo::GetFileDescription() const {
  return mFileDescription;
}

void dolProductInfo::SetFileDescription(const string& pFileDescription) {
  mFileDescription = pFileDescription;
}

string dolProductInfo::GetDigSignature() const {
  return mDigSignature;
}

void dolProductInfo::SetDigSignature(const string& pDigSignature) {
  mDigSignature = pDigSignature;
}

string dolProductInfo::GetType() const {
  return mType;
}

void dolProductInfo::SetType(const string& pType) {
  mType = pType;
}

string dolProductInfo::GetFileVersion() const {
  return mFileVersion;
}

void dolProductInfo::SetFileVersion(const string& pFileVersion) {
  mFileVersion = pFileVersion;
}

string dolProductInfo::GetProductName() const {
  return mProductName;
}

void dolProductInfo::SetProductName(const string& pProductName) {
  mProductName = pProductName;
}

string dolProductInfo::GetProductVersion() const {
  return mProductVersion;
}

void dolProductInfo::SetProductVersion(const string& pProductVersion) {
  mProductVersion = pProductVersion;
}

string dolProductInfo::GetCompanyName() const {
  return mCompanyName;
}

void dolProductInfo::SetCompanyName(const string& pCompanyName) {
  mCompanyName = pCompanyName;
}

string dolProductInfo::GetCopyRight() const {
  return mCopyRight;
}

void dolProductInfo::SetCopyRight(const string& pCopyRight) {
  mCopyRight = pCopyRight;
}

string dolProductInfo::GetSize() const {
  return mSize;
}

void dolProductInfo::SetSize(const string& pSize) {
  mSize = pSize;
}

string dolProductInfo::GetLanguage() const {
  return mLanguage;
}

void dolProductInfo::SetLanguage(const string& pLanguage) {
  mLanguage = pLanguage;
}

string dolProductInfo::GetOriginName() const {
  return mOriginName;
}

void dolProductInfo::SetOriginName(const string& pOriginName) {
  mOriginName = pOriginName;
}

string dolProductInfo::GetCreationTime() const {
  return mCreationTime;
}

void dolProductInfo::SetCreatinTime(const string& pCreationTime) {
  mCreationTime = pCreationTime;
}

string dolProductInfo::GetModifiedTime() const {
  return mModifiedTime;
}

void dolProductInfo::SetModifiedTime(const string& pModifiedTime) {
  mModifiedTime = pModifiedTime;
}

string dolProductInfo::KeyValueToStr(KEY_VALUE* pItems,
                                     DWORD pNumOfItems,
                                     DWORD pFindValue,
                                     string& pRetStr,
                                     const string& pDefaultValue) {
  for (DWORD i = 0; i < pNumOfItems; ++i) {
    if (pItems[i].mKey != pFindValue) {
      continue;
    }  // if

    pRetStr = pItems[i].mValue;
    return pRetStr;
  }  // for

  if (pDefaultValue.empty()) {
    return "";
  } else {
    return pDefaultValue;
  }
}

bool dolProductInfo::GetInfoStr(char* pBuf,
                                LPCSTR lpszLang1,
                                LPCSTR lpszName,
                                LPSTR lpszInfo) {
  UINT size;
  char* str;
  char szTemp[255];
  strcpy(szTemp, "\\StringFileInfo\\");
  strcat(szTemp, lpszLang1);
  strcat(szTemp, "\\");
  strcat(szTemp, lpszName);

  BOOL bFound = VerQueryValueA((LPVOID)pBuf, szTemp, (LPVOID*)&str, &size);
  if (bFound) {
    strcpy(lpszInfo, str);
    return true;
  } else {
    return false;
  }
}

LPSTR dolProductInfo::FormatDWord(DWORD dwValue,
                                  LPSTR lpszValue,
                                  LPSTR lpszMore) {
  char szTemp[30];
  NUMBERFMTA nf;

  char deci_sep[] = ".";
  char thoursand_sep[] = ",";
  nf.NumDigits = 0;
  nf.LeadingZero = 0;
  nf.Grouping = 3;
  nf.lpDecimalSep = deci_sep;
  nf.lpThousandSep = thoursand_sep;
  nf.NegativeOrder = 0;

  _ultoa(dwValue, szTemp, 10);
  GetNumberFormatA(LOCALE_USER_DEFAULT, 0, szTemp, &nf, lpszValue,
                   LINE_BUFFER_SIZE);
  if (lpszMore != NULL)
    strcat(lpszValue, lpszMore);

  return lpszValue;
}

LPSTR dolProductInfo::FormatFileTime(FILETIME ft, LPSTR szDateTime) {
  SYSTEMTIME st;
  char szTime[15];

  FileTimeToSystemTime(&ft, &st);
  GetDateFormatA(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, szDateTime,
                 LINE_BUFFER_SIZE);
  GetTimeFormatA(LOCALE_USER_DEFAULT, 0, &st, NULL, szTime, 15);
  strcat(szDateTime, " ");
  strcat(szDateTime, szTime);
  return szDateTime;
}

// https://support.microsoft.com/zh-cn/help/323809/how-to-get-information-from-authenticode-signed-executables
string dolProductInfo::QueryDigSignature() {
  HCERTSTORE hStore = NULL;
  HCRYPTMSG hMsg = NULL;
  DWORD dwEncoding, dwContentType, dwFormatType;
  SPROG_PUBLISHERINFO ProgPubInfo;
  ZeroMemory(&ProgPubInfo, sizeof(ProgPubInfo));
  WCHAR szFileName[MAX_PATH];
#ifdef UNICODE
  wstring filepath = StringtoWString(mFilePath);
  lstrcpynW(szFileName, filepath.c_str(), MAX_PATH);
#else
  if (mbstowcs(szFileName, mFilePath.c_str(), MAX_PATH) == -1) {
    return "";
  }
#endif

  bool fResult = CryptQueryObject(
      CERT_QUERY_OBJECT_FILE, szFileName,
      CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED, CERT_QUERY_FORMAT_FLAG_BINARY,
      0, &dwEncoding, &dwContentType, &dwFormatType, &hStore, &hMsg, NULL);

  if (!fResult) {
    return "";
  }

  DWORD dwSignerInfo;
  if (!CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, NULL, &dwSignerInfo)) {
    return "";
  }

  PCMSG_SIGNER_INFO pSignerInfo =
      (PCMSG_SIGNER_INFO)LocalAlloc(LPTR, dwSignerInfo);
  if (NULL == pSignerInfo) {
    return "";
  }

  fResult = CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0,
                             (PVOID)pSignerInfo, &dwSignerInfo);

  if (!fResult) {
    return "";
  }

  if (GetProgAndPublisherInfo(pSignerInfo, &ProgPubInfo)) {
    // TODO
  }

  CERT_INFO CertInfo;
  CertInfo.Issuer = pSignerInfo->Issuer;
  CertInfo.SerialNumber = pSignerInfo->SerialNumber;

  PCCERT_CONTEXT pCertContext = CertFindCertificateInStore(
      hStore, ENCODING, 0, CERT_FIND_SUBJECT_CERT, (PVOID)&CertInfo, NULL);
  if (NULL == pCertContext) {
    return "";
  }

  PrintCertificateInfo(pCertContext, mDigSignature);

  // Clean up.
  if (ProgPubInfo.lpszProgramName != NULL)
    LocalFree(ProgPubInfo.lpszProgramName);
  if (ProgPubInfo.lpszPublisherLink != NULL)
    LocalFree(ProgPubInfo.lpszPublisherLink);
  if (ProgPubInfo.lpszMoreInfoLink != NULL)
    LocalFree(ProgPubInfo.lpszMoreInfoLink);

  if (pSignerInfo != NULL)
    LocalFree(pSignerInfo);
  if (pCertContext != NULL)
    CertFreeCertificateContext(pCertContext);
  if (hStore != NULL)
    CertCloseStore(hStore, 0);
  if (hMsg != NULL)
    CryptMsgClose(hMsg);

  return "";
}

LPWSTR dolProductInfo::AllocateAndCopyWideString(LPCWSTR inputString) {
  LPWSTR outputString = NULL;

  outputString =
      (LPWSTR)LocalAlloc(LPTR, (wcslen(inputString) + 1) * sizeof(WCHAR));
  if (outputString != NULL) {
    lstrcpyW(outputString, inputString);
  }
  return outputString;
}

BOOL dolProductInfo::GetProgAndPublisherInfo(PCMSG_SIGNER_INFO pSignerInfo,
                                             PSPROG_PUBLISHERINFO Info) {
  BOOL fReturn = FALSE;
  PSPC_SP_OPUS_INFO OpusInfo = NULL;
  DWORD dwData;
  BOOL fResult;

  __try {
    // Loop through authenticated attributes and find
    // SPC_SP_OPUS_INFO_OBJID OID.
    for (DWORD n = 0; n < pSignerInfo->AuthAttrs.cAttr; n++) {
      if (lstrcmpA(SPC_SP_OPUS_INFO_OBJID,
                   pSignerInfo->AuthAttrs.rgAttr[n].pszObjId) == 0) {
        // Get Size of SPC_SP_OPUS_INFO structure.
        fResult = CryptDecodeObject(
            ENCODING, SPC_SP_OPUS_INFO_OBJID,
            pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].pbData,
            pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].cbData, 0, NULL,
            &dwData);
        if (!fResult) {
          _tprintf(_T("CryptDecodeObject failed with %x\n"), GetLastError());
          __leave;
        }

        // Allocate memory for SPC_SP_OPUS_INFO structure.
        OpusInfo = (PSPC_SP_OPUS_INFO)LocalAlloc(LPTR, dwData);
        if (!OpusInfo) {
          // _tprintf(_T("Unable to allocate memory for Publisher Info.\n"));
          __leave;
        }

        // Decode and get SPC_SP_OPUS_INFO structure.
        fResult = CryptDecodeObject(
            ENCODING, SPC_SP_OPUS_INFO_OBJID,
            pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].pbData,
            pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].cbData, 0, OpusInfo,
            &dwData);
        if (!fResult) {
          _tprintf(_T("CryptDecodeObject failed with %x\n"), GetLastError());
          __leave;
        }

        // Fill in Program Name if present.
        if (OpusInfo->pwszProgramName) {
          Info->lpszProgramName =
              AllocateAndCopyWideString(OpusInfo->pwszProgramName);
        } else
          Info->lpszProgramName = NULL;

        // Fill in Publisher Information if present.
        if (OpusInfo->pPublisherInfo) {
          switch (OpusInfo->pPublisherInfo->dwLinkChoice) {
            case SPC_URL_LINK_CHOICE:
              Info->lpszPublisherLink =
                  AllocateAndCopyWideString(OpusInfo->pPublisherInfo->pwszUrl);
              break;

            case SPC_FILE_LINK_CHOICE:
              Info->lpszPublisherLink =
                  AllocateAndCopyWideString(OpusInfo->pPublisherInfo->pwszFile);
              break;

            default:
              Info->lpszPublisherLink = NULL;
              break;
          }
        } else {
          Info->lpszPublisherLink = NULL;
        }

        // Fill in More Info if present.
        if (OpusInfo->pMoreInfo) {
          switch (OpusInfo->pMoreInfo->dwLinkChoice) {
            case SPC_URL_LINK_CHOICE:
              Info->lpszMoreInfoLink =
                  AllocateAndCopyWideString(OpusInfo->pMoreInfo->pwszUrl);
              break;

            case SPC_FILE_LINK_CHOICE:
              Info->lpszMoreInfoLink =
                  AllocateAndCopyWideString(OpusInfo->pMoreInfo->pwszFile);
              break;

            default:
              Info->lpszMoreInfoLink = NULL;
              break;
          }
        } else {
          Info->lpszMoreInfoLink = NULL;
        }

        fReturn = TRUE;

        break;  // Break from for loop.
      }         // lstrcmp SPC_SP_OPUS_INFO_OBJID
    }           // for
  } __finally {
    if (OpusInfo != NULL)
      LocalFree(OpusInfo);
  }

  return fReturn;
}

BOOL dolProductInfo::PrintCertificateInfo(PCCERT_CONTEXT pCertContext,
                                          string& pDigSignatureName) {
  BOOL fReturn = FALSE;
  char* szName = NULL;
  DWORD dwData;

  __try {
    // Print Serial Number.
    // _tprintf(_T("Serial Number: "));
    dwData = pCertContext->pCertInfo->SerialNumber.cbData;
    for (DWORD n = 0; n < dwData; n++) {
      //  _tprintf(_T("%02x "),
      //   pCertContext->pCertInfo->SerialNumber.pbData[dwData - (n + 1)]);
    }
    _tprintf(_T("\n"));

    // Get Issuer name size.
    if (!(dwData =
              CertGetNameStringA(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE,
                                 CERT_NAME_ISSUER_FLAG, NULL, NULL, 0))) {
      // _tprintf(_T("CertGetNameString failed.\n"));
      __leave;
    }

    // Allocate memory for Issuer name.
    szName = (char*)LocalAlloc(LPTR, dwData * sizeof(char));
    if (!szName) {
      //  _tprintf(_T("Unable to allocate memory for issuer name.\n"));
      __leave;
    }

    // Get Issuer name.
    if (!(CertGetNameStringA(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE,
                             CERT_NAME_ISSUER_FLAG, NULL, szName, dwData))) {
      // _tprintf(_T("CertGetNameString failed.\n"));
      __leave;
    }

    // print Issuer name.
    // _tprintf(_T("Issuer Name: %s\n"), szName);
    LocalFree(szName);
    szName = NULL;

    // Get Subject name size.
    if (!(dwData = CertGetNameStringA(
              pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, NULL, NULL, 0))) {
      // _tprintf(_T("CertGetNameString failed.\n"));
      __leave;
    }

    // Allocate memory for subject name.
    szName = (char*)LocalAlloc(LPTR, dwData * sizeof(char));
    if (!szName) {
      // _tprintf(_T("Unable to allocate memory for subject name.\n"));
      __leave;
    }

    // Get subject name.
    if (!(CertGetNameStringA(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                             NULL, szName, dwData))) {
      //  _tprintf(_T("CertGetNameString failed.\n"));
      __leave;
    }

    pDigSignatureName = szName;

    // Print Subject Name.
    // _tprintf(_T("Subject Name: %s\n"), szName);

    fReturn = TRUE;
  } __finally {
    if (szName != NULL)
      LocalFree(szName);
  }

  return fReturn;
}
