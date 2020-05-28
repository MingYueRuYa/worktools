/****************************************************************************
**
** Copyright (C) 2018 liushixiong (liushixiongcpp@163.com)
** All rights reserved.
**
****************************************************************************/

#ifndef common_h
#define common_h

#include <QString>
#include <string>

using std::wstring;
using std::string;

QString GetProductVer(const QString pFilePath);

std::wstring StringtoWString(const std::string& str);

#endif // common_h