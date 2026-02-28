/****************************************************************************
**
** Copyright (C) 2018 liushixiong (liushixiongcpp@163.com)
** All rights reserved.
**
****************************************************************************/

#include "dolmainwindow.h"
#include <windows.h>
#include <QtCore\QDebug>
#include <QtCore\QDir>
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
	connect(btn_selectdir, SIGNAL(clicked()), this, SLOT(DoOpenDirDialog()));
	connect(btn_ok, SIGNAL(clicked()), this, SLOT(DoOk()));

	// Set table header to stretch mode (columns auto-resize)
	productInfoTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    setAcceptDrops(true);

	// Win7+: allow drag-drop and WM_COPYDATA messages
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

// Override dragEnterEvent for drag-and-drop support
void dolMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept if drag contains text/uri-list
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

// Override dropEvent to handle dropped files
void dolMainWindow::dropEvent(QDropEvent *event)
{
    // Get dropped url list; return if empty
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
        if (fileinfo.isDir()) {
            mSearchPathList.append(CollectExeDllFromDir(filename));
        } else if (fileinfo.suffix().toLower() == suffixarray[0] ||
            fileinfo.suffix().toLower() == suffixarray[1]) {
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
	// Create file dialog
	QFileDialog filedialog(this);
	// Set window title
	filedialog.setWindowTitle(tr("Open Files"));
	// Set current directory
	filedialog.setDirectory(".");
	// Set name filter for dll/exe
	filedialog.setNameFilter(tr("Files(*.dll *.exe)"));
	// Allow multiple selection (ExistingFiles)
	filedialog.setFileMode(QFileDialog::ExistingFiles);
	// Detail view
	filedialog.setViewMode(QFileDialog::Detail);
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

void dolMainWindow::DoOpenDirDialog()
{
	QFileDialog filedialog(this);
	filedialog.setWindowTitle(tr("Select Directory"));
	filedialog.setDirectory(".");
	filedialog.setFileMode(QFileDialog::Directory);
	filedialog.setOption(QFileDialog::ShowDirsOnly, true);
	filedialog.setViewMode(QFileDialog::Detail);

	QStringList dirs;
	if (filedialog.exec()) {
		dirs = filedialog.selectedFiles();
	}

	if (dirs.isEmpty()) {
		return;
	}

	mSearchPathList.clear();
	for (const QString &dirPath : dirs) {
		mSearchPathList.append(CollectExeDllFromDir(dirPath));
	}

	if (mSearchPathList.empty()) {
		return;
	}

	QFileInfo fileinfo(mSearchPathList[0]);
	lineedit_path->setText(fileinfo.absolutePath());

	ShowInfo();
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

	int row = productInfoTable->rowCount() - 1;
	QString filePath = QString::fromStdString(pInfo->GetFilePath());
	QString fileName = QFileInfo(filePath).fileName();
	QTableWidgetItem *item = new QTableWidgetItem(fileName);
	productInfoTable->setItem(row, 0, item);

	QString convertstr = QString::fromLocal8Bit(pInfo->GetOriginName().c_str());
	item = new QTableWidgetItem(convertstr);
	productInfoTable->setItem(row, 1, item);

	convertstr = QString::fromLocal8Bit(pInfo->GetFileDescription().c_str());
	item = new QTableWidgetItem(convertstr);
	productInfoTable->setItem(row, 2, item);

	convertstr = QString::fromLocal8Bit(pInfo->GetDigSignature().c_str());
	item = new QTableWidgetItem(convertstr);
	productInfoTable->setItem(row, 3, item);

	item = new QTableWidgetItem(QString(pInfo->GetType().c_str()));
	productInfoTable->setItem(row, 4, item);

	item = new QTableWidgetItem(QString(pInfo->GetFileVersion().c_str()));
	productInfoTable->setItem(row, 5, item);

	convertstr = QString::fromLocal8Bit(pInfo->GetProductName().c_str());
	item = new QTableWidgetItem(convertstr);
	productInfoTable->setItem(row, 6, item);

	item = new QTableWidgetItem(QString(pInfo->GetProductVersion().c_str()));
	productInfoTable->setItem(row, 7, item);

	convertstr = QString::fromLocal8Bit(pInfo->GetCopyRight().c_str());
	item = new QTableWidgetItem(convertstr);
	productInfoTable->setItem(row, 8, item);

	item = new QTableWidgetItem(QString(pInfo->GetSize().c_str()));
	productInfoTable->setItem(row, 9, item);

	item = new QTableWidgetItem(QString(pInfo->GetModifiedTime().c_str()));
	productInfoTable->setItem(row, 10, item);
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

QStringList dolMainWindow::CollectExeDllFromDir(const QString &dirPath)
{
	QStringList result;
	QDir dir(dirPath);
	if (!dir.exists()) {
		return result;
	}
	QFileInfoList entries = dir.entryInfoList(QDir::Files);
	for (const QFileInfo &fi : entries) {
		QString suffix = fi.suffix().toLower();
		if (suffix == QLatin1String("exe") || suffix == QLatin1String("dll")) {
			result.append(fi.absoluteFilePath());
		}
	}
	return result;
}

#include "moc_dolmainwindow.cpp"