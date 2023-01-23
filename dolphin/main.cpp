/****************************************************************************
**
** Copyright (C) 2018 liushixiong (liushixiongcpp@163.com)
** All rights reserved.
**
****************************************************************************/

#include "dolmainwindow.h"

#include <QtWidgets/QApplication>
#include <QtCore/QTextCodec>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	// QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));

    dolMainWindow mainwindow;
    mainwindow.show();

    return app.exec();
}
