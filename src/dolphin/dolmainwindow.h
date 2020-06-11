#ifndef dolmainwindow_h
#define dolmainwindow_h

/****************************************************************************
**
** Copyright (C) 2018 liushixiong (liushixiongcpp@163.com)
** All rights reserved.
**
****************************************************************************/

#include <QtWidgets/QMainWindow>
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
	void DoOk();
	void DoClear(bool checked);

private:
	void ClearProductInfo();
	void InsertRow(dolProductInfo *pInfo);
    void ShowInfo();
	void InitMenu();
	void ClearTable();

private:
	QMenu *mMenu;

private:
	QStringList mSearchPathList;
	QList<dolProductInfo *> mProductInfoList;

};

#endif // dolmainwindow_h
