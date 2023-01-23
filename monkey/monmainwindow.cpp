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
    //定义文件对话框类  
    QFileDialog filedialog(this);
    //定义文件对话框标题  
    filedialog.setWindowTitle(tr("Open Files"));
    //设置默认文件路径  
    filedialog.setDirectory(".");
    //设置文件过滤器  
    filedialog.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles  
    filedialog.setFileMode(QFileDialog::DirectoryOnly);
    //设置视图模式  
    filedialog.setViewMode(QFileDialog::Detail);
    //打印所有选择的文件的路径  
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

    // 以.开头的文件或者是目录都是隐藏目录不需要遍历
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

        // product version 和file version是一致的
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
        //修改FILEVERSION和PRODUCTVERSION 格式为1,0,0,1
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
        // 设置了vs自己反而打不开了
//        newstream.setCodec("UTF-8");
//        newstream.setGenerateByteOrderMark(true);
        newstream << replacecontent;
        
        file.close();
    }

    QMessageBox::information(this, "title", "modified version successful.", 
                             QMessageBox::Yes);
}

#include "moc_monmainwindow.cpp"