/****************************************************************************
**
** Copyright (C) 2018 liushixiong (liushixiongcpp@163.com)
** All rights reserved.
**
****************************************************************************/

#include "dolmainwindow.h"
#include <windows.h>
#include <algorithm>
#include <QtCore\QDebug>
#include <QtCore\QDir>
#include <QtCore\QMap>
#include <QtCore\QDirIterator>
#include <QtCore\QFileInfo>
#include <QtWidgets\QAbstractItemView>
#include <QtWidgets\QFileDialog>
#include <QtWidgets\QHeaderView>
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

	// Allow user to resize columns by dragging header
	productInfoTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	// 单元格只读，不可编辑
	productInfoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

	// 表头点击排序（大小列按实际字节数排序）
	m_sortColumn = -1;
	m_sortOrder = Qt::AscendingOrder;
	productInfoTable->setSortingEnabled(false);
	productInfoTable->horizontalHeader()->setSectionsClickable(true);
	connect(productInfoTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(OnHeaderSectionClicked(int)));

	m_statusStatsLabel = new QLabel(this);
	m_statusStatsLabel->setTextFormat(Qt::RichText);
	dolphinClass::statusBar->addWidget(m_statusStatsLabel, 1);

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

QString dolMainWindow::formatByteSize(qint64 bytes)
{
	const qint64 K = 1024;
	const qint64 M = K * 1024;
	const qint64 G = M * 1024;
	qint64 n = (bytes < 0) ? 0 : bytes;
	if (n < K)
		return QString::number(n) + " B";
	if (n < M)
		return QString::number(n / (double)K, 'f', 2) + " K";
	if (n < G)
		return QString::number(n / (double)M, 'f', 2) + " M";
	return QString::number(n / (double)G, 'f', 2) + " G";
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

	// 选中文件后自动查询并显示信息
	ShowInfo();
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
	QTableWidgetItem *item = new QTableWidgetItem(filePath);
	productInfoTable->setItem(row, 0, item);

	QString convertstr = QString::fromLocal8Bit(pInfo->GetOriginName().c_str());
	item = new QTableWidgetItem(convertstr);
	productInfoTable->setItem(row, 1, item);

	convertstr = QString::fromLocal8Bit(pInfo->GetFileDescription().c_str());
	item = new QTableWidgetItem(convertstr);
	productInfoTable->setItem(row, 2, item);

	convertstr = QString::fromLocal8Bit(pInfo->GetDigSignature().c_str());
	bool signatureEmpty = convertstr.trimmed().isEmpty();
	item = new QTableWidgetItem(convertstr);
	productInfoTable->setItem(row, 3, item);

	item = new QTableWidgetItem(QString(pInfo->GetType().c_str()));
	productInfoTable->setItem(row, 4, item);

	convertstr = QString::fromLocal8Bit(pInfo->GetProductName().c_str());
	item = new QTableWidgetItem(convertstr);
	productInfoTable->setItem(row, 5, item);

	item = new QTableWidgetItem(QString(pInfo->GetProductVersion().c_str()));
	productInfoTable->setItem(row, 6, item);

	convertstr = QString::fromLocal8Bit(pInfo->GetCopyRight().c_str());
	item = new QTableWidgetItem(convertstr);
	productInfoTable->setItem(row, 7, item);

	item = new QTableWidgetItem(formatByteSize(QFileInfo(QString::fromLocal8Bit(pInfo->GetFilePath().c_str())).size()));
	productInfoTable->setItem(row, 8, item);

	item = new QTableWidgetItem(QString(pInfo->GetModifiedTime().c_str()));
	productInfoTable->setItem(row, 9, item);

	// 设置该行所有单元格为不可编辑
	for (int col = 0; col < productInfoTable->columnCount(); ++col) {
		QTableWidgetItem *cell = productInfoTable->item(row, col);
		if (cell) {
			cell->setFlags(cell->flags() & ~Qt::ItemIsEditable);
		}
	}

	if (signatureEmpty) {
		for (int col = 0; col < productInfoTable->columnCount(); ++col) {
			QTableWidgetItem *cell = productInfoTable->item(row, col);
			if (cell) {
				cell->setBackground(QBrush(Qt::red));
			}
		}
	}
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

	QString html;
	for (dolProductInfo *info : mProductInfoList) {
		QString sig = QString::fromLocal8Bit(info->GetDigSignature().c_str()).trimmed();
		if (!sig.isEmpty())
			continue;
		QString filePath = QString::fromStdString(info->GetFilePath());
		QString escaped = filePath.toHtmlEscaped();
		html += QStringLiteral("<span style='color:red'>%1</span>").arg(escaped);
		html += QLatin1String("<br/>");
	}
	QString body = html.isEmpty() ? QString() : QStringLiteral("<div style='font-size:20px'>%1</div>").arg(html);
	textBrowser->setHtml(body);

	UpdateStatusBarStats();
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
	ClearProductInfo();
	textBrowser->clear();
	UpdateStatusBarStats();  // 清空后显示 0 个文件
}

void dolMainWindow::OnHeaderSectionClicked(int logicalIndex)
{
	// 同一列再次点击则切换升序/降序
	if (logicalIndex == m_sortColumn) {
		m_sortOrder = (m_sortOrder == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
	} else {
		m_sortColumn = logicalIndex;
		m_sortOrder = Qt::AscendingOrder;
	}

	// 大小列（第 9 列，索引 8）按实际字节数排序
	if (logicalIndex == 8) {
		std::sort(mProductInfoList.begin(), mProductInfoList.end(),
			[this](dolProductInfo *a, dolProductInfo *b) {
				qint64 sa = QFileInfo(QString::fromStdString(a->GetFilePath())).size();
				qint64 sb = QFileInfo(QString::fromStdString(b->GetFilePath())).size();
				if (m_sortOrder == Qt::AscendingOrder)
					return sa < sb;
				return sa > sb;
			});
		ClearTable();
		for (int i = 0; i < mProductInfoList.size(); ++i) {
			InsertRow(mProductInfoList[i]);
		}
	} else {
		productInfoTable->sortItems(logicalIndex, m_sortOrder);
	}

	productInfoTable->horizontalHeader()->setSortIndicator(m_sortColumn, m_sortOrder);
}

void dolMainWindow::UpdateStatusBarStats()
{
	const int total = mProductInfoList.size();
	if (total == 0) {
		m_statusStatsLabel->setText(tr("0 files"));
		return;
	}

	int unsignedCount = 0;
	QMap<QString, int> signatureCount;
	for (dolProductInfo *info : mProductInfoList) {
		QString sig = QString::fromLocal8Bit(info->GetDigSignature().c_str()).trimmed();
		if (sig.isEmpty()) {
			++unsignedCount;
		} else {
			signatureCount[sig] += 1;
		}
	}

	QString unsignedPart = tr("%1 unsigned").arg(unsignedCount);
	if (unsignedCount > 0) {
		unsignedPart = QStringLiteral("<span style='color:red'>%1</span>").arg(unsignedPart);
	}
	QString msg = tr("%1 files, %2").arg(total).arg(unsignedPart);
	if (!signatureCount.isEmpty()) {
		msg += tr("; Signed: ");
		QStringList parts;
		for (auto it = signatureCount.constBegin(); it != signatureCount.constEnd(); ++it) {
			parts.append(QStringLiteral("%1(%2)").arg(it.key().toHtmlEscaped()).arg(it.value()));
		}
		msg += parts.join(QStringLiteral(", "));
	}
	m_statusStatsLabel->setText(msg);
}

QStringList dolMainWindow::CollectExeDllFromDir(const QString &dirPath)
{
	QStringList result;
	QDir dir(dirPath);
	if (!dir.exists()) {
		return result;
	}
	// 递归遍历目录及所有子目录
	QDirIterator it(dirPath, QDir::Files, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		QString path = it.next();
		QFileInfo fi(path);
		QString suffix = fi.suffix().toLower();
		if (suffix == QLatin1String("exe") || suffix == QLatin1String("dll")) {
			result.append(fi.absoluteFilePath());
		}
	}
	return result;
}

#include "moc_dolmainwindow.cpp"