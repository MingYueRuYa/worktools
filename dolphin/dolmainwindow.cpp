/****************************************************************************
**
** Copyright (C) 2018 liushixiong (liushixiongcpp@163.com)
** All rights reserved.
**
****************************************************************************/

#include "dolmainwindow.h"
#include <windows.h>
#include <QtCore\QDebug>
#include <QtWidgets\QFileDialog>
#include <QtWidgets\QMessagebox>

#include "dolabout.h"

typedef BOOL (WINAPI FAR *PtrChangeWindowMessageFilterEx)(HWND hWnd, 
								UINT message,
								DWORD action,
								PCHANGEFILTERSTRUCT pChangeFilterStruct);

typedef BOOL (WINAPI FAR *PtrChangeWindowMessageFilter)(UINT,DWORD);  

static PtrChangeWindowMessageFilterEx pChangeWindowMessageFilterEx =
		(PtrChangeWindowMessageFilterEx)GetProcAddress(
		::GetModuleHandle(_T("USER32")), ("ChangeWindowMessageFilterEx"));

dolMainWindow::dolMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);
    dolphinClass::menuBar->addAction("About", this, SLOT(DoAbout()));

	InitMenu();

	connect(btn_serachpath, SIGNAL(clicked()), this, SLOT(DoOpenFileDialog()));
	connect(btn_ok, SIGNAL(clicked()), this, SLOT(DoOk()));

	// 设置table row等宽
	productInfoTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    setAcceptDrops(true);

	// 解决win7 拖拽问题
	if (pChangeWindowMessageFilterEx) {
	// Call the safer ChangeWindowMessageFilterEx API if available (Windows 7 onwards)
		pChangeWindowMessageFilterEx((HWND)winId(), 
										WM_COPYDATA, 
										MSGFLT_ALLOW, 
										0);
		pChangeWindowMessageFilterEx((HWND)winId(), 
										WM_DROPFILES, 
										MSGFLT_ALLOW, 
										0);
		pChangeWindowMessageFilterEx((HWND)winId(), 0x0049, MSGFLT_ALLOW, 0);
	}
	else {
		static PtrChangeWindowMessageFilter pChangeWindowMessageFilter =
			(PtrChangeWindowMessageFilter)GetProcAddress(
			::GetModuleHandle(_T("USER32")), "ChangeWindowMessageFilter");

		if (pChangeWindowMessageFilter) {
			// Call the deprecated ChangeWindowMessageFilter API otherwise
			pChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ALLOW);
			pChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ALLOW);
			pChangeWindowMessageFilter(0x0049, MSGFLT_ALLOW);
		}
	}
}

dolMainWindow::~dolMainWindow()
{
	ClearProductInfo();
}

//当用户拖动文件到窗口部件上时候，就会触发dragEnterEvent事件
void dolMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    //如果为文件，则支持拖放
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

//当用户放下这个文件后，就会触发dropEvent事件
void dolMainWindow::dropEvent(QDropEvent *event)
{
    //注意：这里如果有多文件存在，意思是用户一下子拖动了多个文件，而不是拖动一个目录
    //如果想读取整个目录，则在不同的操作平台下，自己编写函数实现读取整个目录文件名
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        return;
    }

    QString suffixarray [] = {
        "exe",
        "dll"
    };

    mSearchPathList.clear();
    for (int i = 0; i<urls.size(); ++i) {
        QString filename = urls[i].toLocalFile();
        QFileInfo fileinfo(filename);
        if (fileinfo.suffix() == suffixarray[0] ||
            fileinfo.suffix() == suffixarray[1] ) {
            mSearchPathList.append(filename);
        }
    }

    if (mSearchPathList.empty()) {
        return;
    }

	QFileInfo fileinfo(mSearchPathList[0]);
	lineedit_path->setText(fileinfo.absolutePath());

    ShowInfo();
}

void dolMainWindow::contextMenuEvent(QContextMenuEvent *event)
{
	mMenu->move(cursor().pos());
	mMenu->show();
}

void dolMainWindow::DoAbout()
{
    dolAbout about(this);
    about.setModal(true);
    about.exec();
}

void dolMainWindow::DoOpenFileDialog()
{
	//定义文件对话框类  
	QFileDialog filedialog(this);
	//定义文件对话框标题  
	filedialog.setWindowTitle(tr("Open Files"));
	//设置默认文件路径  
	filedialog.setDirectory(".");
	//设置文件过滤器  
	filedialog.setNameFilter(tr("Files(*.dll *.exe)"));
	//设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles  
	filedialog.setFileMode(QFileDialog::ExistingFiles);
	//设置视图模式  
	filedialog.setViewMode(QFileDialog::Detail);
	//打印所有选择的文件的路径  
	// QStringList filenames;
	if (filedialog.exec()) {
		// filenames = filedialog.selectedFiles();
		mSearchPathList = filedialog.selectedFiles();
	}

	if (mSearchPathList.empty()) {
		return;
	}

	QFileInfo fileinfo(mSearchPathList[0]);
	lineedit_path->setText(fileinfo.absolutePath());
}

void dolMainWindow::DoOk()
{
    ShowInfo();
}

void dolMainWindow::DoClear(bool checked)
{
	ClearTable();
}

void dolMainWindow::ClearProductInfo()
{
	foreach (dolProductInfo *info, mProductInfoList) {
		delete info;
		info = NULL;
	}
	mProductInfoList.clear();
}

void dolMainWindow::InsertRow(dolProductInfo *pInfo)
{
	productInfoTable->insertRow(productInfoTable->rowCount());

    QString convertstr = QString::fromLocal8Bit(pInfo->GetOriginName().c_str());
	QTableWidgetItem *item = new QTableWidgetItem(convertstr);
	item = new QTableWidgetItem(convertstr);
	productInfoTable->setItem(productInfoTable->rowCount()-1, 0, item);

	convertstr = QString::fromLocal8Bit(pInfo->GetFileDescription().c_str());
	item = new QTableWidgetItem(convertstr);
	productInfoTable->setItem(productInfoTable->rowCount()-1, 1, item);

    convertstr = QString::fromLocal8Bit(pInfo->GetDigSignature().c_str());
	item = new QTableWidgetItem(convertstr);
	productInfoTable->setItem(productInfoTable->rowCount()-1, 2, item);

	item = new QTableWidgetItem(QString(pInfo->GetType().c_str()));
	productInfoTable->setItem(productInfoTable->rowCount()-1, 3, item);

	item = new QTableWidgetItem(QString(pInfo->GetFileVersion().c_str()));
	productInfoTable->setItem(productInfoTable->rowCount()-1, 4, item);

    convertstr = QString::fromLocal8Bit(pInfo->GetProductName().c_str());
	item = new QTableWidgetItem(convertstr);
	productInfoTable->setItem(productInfoTable->rowCount()-1, 5, item);

	item = new QTableWidgetItem(QString(pInfo->GetProductVersion().c_str()));
	productInfoTable->setItem(productInfoTable->rowCount()-1, 6, item);

    convertstr = QString::fromLocal8Bit(pInfo->GetCopyRight().c_str());
	item = new QTableWidgetItem(convertstr);
	productInfoTable->setItem(productInfoTable->rowCount()-1, 7, item);

	item = new QTableWidgetItem(QString(pInfo->GetSize().c_str()));
	productInfoTable->setItem(productInfoTable->rowCount()-1, 8, item);

	item = new QTableWidgetItem(QString(pInfo->GetModifiedTime().c_str()));
	productInfoTable->setItem(productInfoTable->rowCount()-1, 9, item);
}

void dolMainWindow::ShowInfo()
{
	ClearProductInfo();

	while (productInfoTable->rowCount() > 0) {
		productInfoTable->removeRow(0);
	}

	foreach (QString path, mSearchPathList) {
		dolProductInfo *info = new dolProductInfo(path.toStdString(), true);
		mProductInfoList.append(info);

		InsertRow(info);
	}
}

void dolMainWindow::InitMenu()
{
	mMenu = new QMenu(this);
	QAction *clearAct = new QAction(tr("clear"), mMenu);
	connect(clearAct, SIGNAL(triggered(bool)), this, SLOT(DoClear(bool)));
	mMenu->addAction(clearAct);
}

void dolMainWindow::ClearTable()
{
	while (productInfoTable->rowCount() > 0) {
		productInfoTable->removeRow(0);
	}	
}

#include "moc_dolmainwindow.cpp"