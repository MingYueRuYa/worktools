/****************************************************************************
**
** Copyright (C) 2018 liushixiong (liushixiongcpp@163.com)
** All rights reserved.
**
****************************************************************************/

#ifndef dolproductinfo_h
#define dolproductinfo_h

#include <Windows.h>
#include <tchar.h>
#include <string>

using std::string;

/*! \brief		dol_production_info
        \author		liushixiong (liushixiongcpp@163.com)
        \version	0.01
        \date		2018-5-12 14:16:20

        about dialog
*/
class dolProductInfo {
 public:
  dolProductInfo(const string& pFilePath, bool pIsInit);
  ~dolProductInfo();

  bool Init();

  string GetFileType(DWORD pdwFileType, string& pszFileType);

  string GetFilePath() const;
  void SetFilePath(const string& pFilePath);

  string GetFileDescription() const;
  void SetFileDescription(const string& pFileDescription);

  string GetDigSignature() const;
  void SetDigSignature(const string& pDigSignature);

  string GetType() const;
  void SetType(const string& pType);

  string GetFileVersion() const;
  void SetFileVersion(const string& pFileVersion);

  string GetProductName() const;
  void SetProductName(const string& pProductName);

  string GetProductVersion() const;
  void SetProductVersion(const string& pProductVersion);

  string GetCompanyName() const;
  void SetCompanyName(const string& pCompanyName);

  string GetCopyRight() const;
  void SetCopyRight(const string& pCopyRight);

  string GetSize() const;
  void SetSize(const string& pSize);

  string GetLanguage() const;
  void SetLanguage(const string& pLanguage);

  string GetOriginName() const;
  void SetOriginName(const string& pOriginName);

  string GetCreationTime() const;
  void SetCreatinTime(const string& pCreationTime);

  string GetModifiedTime() const;
  void SetModifiedTime(const string& pModifiedTime);

 private:
  struct KEY_VALUE {
    DWORD mKey;
    const char* mValue;
  };

  typedef struct {
    LPWSTR lpszProgramName;
    LPWSTR lpszPublisherLink;
    LPWSTR lpszMoreInfoLink;
  } SPROG_PUBLISHERINFO, *PSPROG_PUBLISHERINFO;

 private:
  string KeyValueToStr(KEY_VALUE* pItems,
                       DWORD pNumOfItems,
                       DWORD pFindValue,
                       string& pRetStr,
                       const string& pDefaultValue = "");

  bool GetInfoStr(char* pBuf, LPCSTR pszLang1, LPCSTR pName, LPSTR pInfo);

  LPSTR FormatDWord(DWORD dwValue, LPSTR lpszValue, LPSTR lpszMore = NULL);

  LPSTR FormatFileTime(FILETIME ft, LPSTR szDateTime);

  string QueryDigSignature();

  BOOL GetProgAndPublisherInfo(PCMSG_SIGNER_INFO pSignerInfo,
                               PSPROG_PUBLISHERINFO Info);

  LPWSTR AllocateAndCopyWideString(LPCWSTR inputString);

  BOOL PrintCertificateInfo(PCCERT_CONTEXT pCertContext,
                            string& pDigSignatureName);

 private:
  string mFilePath;
  string mFileDescription;
  string mDigSignature;
  string mType;
  string mFileVersion;
  string mProductName;
  string mProductVersion;
  string mCompanyName;
  string mCopyRight;
  string mSize;
  string mLanguage;
  string mOriginName;
  string mCreationTime;
  string mModifiedTime;
};

#endif  // dolproductinfo_h