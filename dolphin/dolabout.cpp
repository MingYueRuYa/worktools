#include "dolabout.h"
#include <QtCore\QTime>

#include "..\common\common.h"

dolAbout::dolAbout(QWidget *pParent)
    : QDialog(pParent)
{
    setupUi(this);
    lbl_version->setText(GetProductVer(qApp->applicationFilePath()));

	QDate buildDate = QLocale( QLocale::English ).toDate(
						QString( __DATE__ ).replace( "  ", " " ), 
						"MMM dd yyyy");
	QTime buildTime = QTime::fromString( __TIME__, "hh:mm:ss" );

	lbl_compile_time->setText(
					buildDate.toString("yyyy/MM/dd") + " " + buildTime.toString("hh:mm:ss"));
}

#include "moc_dolabout.cpp"
