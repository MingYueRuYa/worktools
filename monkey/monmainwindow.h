#pragma once

#include "ui_monmainwindow.h"
#include <QtWidgets/QMainWindow>
#include <QtCore/QDir>

struct RCFileStatus
{
    QString file_version;
    QString product_version;
    QString path;
};

class monMainWindow : public QMainWindow, public Ui::monMainWindowClass
{
    Q_OBJECT

public:
    monMainWindow(QWidget *parent = Q_NULLPTR);
    ~monMainWindow();
    void ShowVersion();


//signals:
//    void OnAddRow();

protected slots:
    void DoAbout();
    void DoOpenFileDialog();
    void DoShowVersion();
    // void DoAddRow();
    void DoSaveRC();

private:
    void ReverseFile(const QString &pSearchPath);    
    void InsertRow();
    void ReadRCFile();
    void SaveRCFile();

private:
    QDir mSelectDir;
    QList<QString> mFilePathList;
    QList<RCFileStatus> mVecRCStatus;
};
