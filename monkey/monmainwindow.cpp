#include "monmainwindow.h"

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include "monabout.h"

const QString RC_FILE           = "rc";
const QString FILEVERSION       = "FileVersion";
const QString PRODUCTVERSION    = "ProductVersion";
const QString FILEVERSION_UPPER = "FILEVERSION";
const QString PRODUCTVERSION_UPPER = "PRODUCTVERSION";

monMainWindow::monMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);
    monMainWindowClass::menuBar->addAction("About", this, SLOT(DoAbout()));

	connect(btn_searchpath, SIGNAL(clicked()), this, SLOT(DoOpenFileDialog()));
	connect(btn_ok, SIGNAL(clicked()), this, SLOT(DoShowVersion()));
	connect(btn_modify, SIGNAL(clicked()), this, SLOT(DoSaveRC()));
}

monMainWindow::~monMainWindow()
{
}

void monMainWindow::ShowVersion()
{
    while (tableWidget->rowCount() > 0) {
        tableWidget->removeRow(0);
    }

    mFilePathList.clear();

    ReverseFile(mSelectDir.absolutePath());    
}

void monMainWindow::DoOpenFileDialog()
{
    //�����ļ��Ի�����  
    QFileDialog filedialog(this);
    //�����ļ��Ի������  
    filedialog.setWindowTitle(tr("Open Files"));
    //����Ĭ���ļ�·��  
    filedialog.setDirectory(".");
    //�����ļ�������  
    filedialog.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    //���ÿ���ѡ�����ļ�,Ĭ��Ϊֻ��ѡ��һ���ļ�QFileDialog::ExistingFiles  
    filedialog.setFileMode(QFileDialog::DirectoryOnly);
    //������ͼģʽ  
    filedialog.setViewMode(QFileDialog::Detail);
    //��ӡ����ѡ����ļ���·��  
    QStringList filenames;
    if (filedialog.exec()) {
        filenames = filedialog.selectedFiles();
    }

    if (filenames.empty()) { return ; }

    mSelectDir = QDir(filenames[0]);
    lineedit_path->setText(filenames[0]);
}

void monMainWindow::DoShowVersion()
{
    ShowVersion();
    ReadRCFile();
}

void monMainWindow::DoSaveRC()
{
    SaveRCFile();
}

void monMainWindow::DoAbout()
{
    monAbout about(this);
    about.setModal(true);
    about.exec();
}

void monMainWindow::ReverseFile(const QString &pSearchPath)   
{
    QDir searchdir(pSearchPath);
    if (! searchdir.exists()) { return; }  

    // ��.��ͷ���ļ�������Ŀ¼��������Ŀ¼����Ҫ����
    QString dirpath = searchdir.absolutePath();
    foreach (QString name, dirpath.split("/")) {
        if (name.left(1) == ".") {
            return;
        }
    }

    searchdir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    QFileInfoList infolist = searchdir.entryInfoList();
    foreach (QFileInfo info, infolist) {
        if (info.isFile() && info.suffix() == RC_FILE) {
            mFilePathList.append(info.absoluteFilePath());
            tableWidget->insertRow(tableWidget->rowCount());
            QTableWidgetItem *item = new QTableWidgetItem(info.absoluteFilePath());
            tableWidget->setItem(tableWidget->rowCount() - 1, 0, item);
        } else if (info.isDir()) {
            ReverseFile(info.absoluteFilePath());
        }
    }
}

void monMainWindow::InsertRow()
{
    for (int i=0; i<mFilePathList.count(); ++i) {
        tableWidget->insertRow(tableWidget->rowCount());
        QTableWidgetItem *item = new QTableWidgetItem(mFilePathList[i]);
        tableWidget->setItem(tableWidget->rowCount() - 1, 0, item);
    }
}

void monMainWindow::ReadRCFile()
{
    for (int i=0; i<mFilePathList.count(); ++i) {
        QFile file(mFilePathList[i]);
        if (! file.open(QIODevice::ReadOnly)) {
            continue;
        }

        QTextStream textstream(&file);
        textstream.setCodec("UTF-8");
        while (! textstream.atEnd()) {
            QString linestr = textstream.readLine();
            QStringList linestrlist = linestr.split(",");
            if (linestrlist.count() < 2) {
                continue; 
            }
            if (linestr.contains(FILEVERSION)) {
                tableWidget->setItem(i, 1, 
                        new QTableWidgetItem(linestrlist[1].replace("\"", "")));
            } else if (linestr.contains(PRODUCTVERSION)) {
                tableWidget->setItem(i, 2, 
                        new QTableWidgetItem(linestrlist[1].replace("\"", "")));
            }
        }
        tableWidget->setItem(i, 3, 
                        new QTableWidgetItem());

        file.close();
    }
}

void monMainWindow::SaveRCFile()
{
    if (mFilePathList.empty()) {
        return;
    }

    for (int i=0; i<tableWidget->rowCount(); ++i) {
        QTableWidgetItem *item = tableWidget->item(i, 3);
        if (NULL == item) {
            continue;
        }

        // product version ��file version��һ�µ�
        item = tableWidget->item(i, 1);
        if (NULL == item) {
            continue;
        }

        QString originversion = item->text();
        if (originversion.isEmpty()) {
            continue;
        }
        originversion = originversion.trimmed();

        QString newversionstr = tableWidget->item(i, 3)->text().trimmed();
        if (newversionstr.isEmpty()) {
            continue;
        }

        QString bakfilename = mFilePathList[i]+".bak";
        QFile::remove(bakfilename);
        QFile::copy(mFilePathList[i], bakfilename);
        QFile bakfile(bakfilename);
        if (! bakfile.open(QIODevice::ReadOnly)) {
            continue;
        }

        QTextStream textstream(&bakfile);
        QString oldcontent      = textstream.readAll();
        QString replacecontent  = oldcontent.replace(originversion, 
                                                     newversionstr);
        //�޸�FILEVERSION��PRODUCTVERSION ��ʽΪ1,0,0,1
        QString semoriginversion    = originversion.replace(".", ",");
        QString semnewversion       = newversionstr.replace(".", ",");
        replacecontent              = replacecontent.replace(
                                                " "+semoriginversion, 
                                                " "+semnewversion);
        bakfile.close();

        QFile file(mFilePathList[i]);
        if (! file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
            continue;
        }
        QTextStream newstream(&file);
        // ������vs�Լ������򲻿���
//        newstream.setCodec("UTF-8");
//        newstream.setGenerateByteOrderMark(true);
        newstream << replacecontent;
        
        file.close();
    }

    QMessageBox::information(this, "title", "modified version successful.", 
                             QMessageBox::Yes);
}

#include "moc_monmainwindow.cpp"