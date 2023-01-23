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

	// ����table row�ȿ�
	productInfoTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    setAcceptDrops(true);

	// ���win7 ��ק����
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

//���û��϶��ļ������ڲ�����ʱ�򣬾ͻᴥ��dragEnterEvent�¼�
void dolMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    //���Ϊ�ļ�����֧���Ϸ�
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

//���û���������ļ��󣬾ͻᴥ��dropEvent�¼�
void dolMainWindow::dropEvent(QDropEvent *event)
{
    //ע�⣺��������ж��ļ����ڣ���˼���û�һ�����϶��˶���ļ����������϶�һ��Ŀ¼
    //������ȡ����Ŀ¼�����ڲ�ͬ�Ĳ���ƽ̨�£��Լ���д����ʵ�ֶ�ȡ����Ŀ¼�ļ���
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
	//�����ļ��Ի�����  
	QFileDialog filedialog(this);
	//�����ļ��Ի������  
	filedialog.setWindowTitle(tr("Open Files"));
	//����Ĭ���ļ�·��  
	filedialog.setDirectory(".");
	//�����ļ�������  
	filedialog.setNameFilter(tr("Files(*.dll *.exe)"));
	//���ÿ���ѡ�����ļ�,Ĭ��Ϊֻ��ѡ��һ���ļ�QFileDialog::ExistingFiles  
	filedialog.setFileMode(QFileDialog::ExistingFiles);
	//������ͼģʽ  
	filedialog.setViewMode(QFileDialog::Detail);
	//��ӡ����ѡ����ļ���·��  
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