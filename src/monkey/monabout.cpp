#include "monabout.h"
#include "..\common\common.h"

monAbout::monAbout(QWidget *pParent)
    : QDialog(pParent)
{
    setupUi(this);

    lbl_version->setText(GetProductVer(qApp->applicationFilePath()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(DoCancel()));
}

void monAbout::DoCancel()
{
    reject();
}

#include "moc_monabout.cpp"
