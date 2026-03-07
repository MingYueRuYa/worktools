#ifndef dolmainwindow_h
#define dolmainwindow_h

/****************************************************************************
**
** Copyright (C) 2018 liushixiong (liushixiongcpp@163.com)
** All rights reserved.
**
****************************************************************************/

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtGui/QDragEnterEvent>
#include <QtCore/QMimeData>

#include "ui_dolphin.h"
#include "dolproductinfo.h"

class dolMainWindow : public QMainWindow, public Ui::dolphinClass
{
    Q_OBJECT

public:
    dolMainWindow(QWidget *parent = Q_NULLPTR);
	~dolMainWindow();

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
	virtual void contextMenuEvent(QContextMenuEvent *event);

protected slots:
    void DoAbout();
	void DoOpenFileDialog();
	void DoOpenDirDialog();
	void DoOk();
	void DoClear(bool checked);
	void OnHeaderSectionClicked(int logicalIndex);

private:
	void ClearProductInfo();
	void InsertRow(dolProductInfo *pInfo);
    void ShowInfo();
	void InitMenu();
	void ClearTable();
	QStringList CollectExeDllFromDir(const QString &dirPath);
	/** 根据当前 mProductInfoList 更新状态栏统计信息 */
	void UpdateStatusBarStats();

	/** 将字节数转换为 B / K / M / G 的可读字符串（按 1024 进位） */
	static QString formatByteSize(qint64 bytes);

private:
	QMenu *mMenu;
	QLabel *m_statusStatsLabel;
	int m_sortColumn;
	Qt::SortOrder m_sortOrder;

private:
	QStringList mSearchPathList;
	QList<dolProductInfo *> mProductInfoList;

};

#endif // dolmainwindow_h
