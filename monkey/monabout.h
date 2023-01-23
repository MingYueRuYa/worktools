#ifndef monabout_h
#define monabout_h

#include "ui_monabout.h"
#include <QtWidgets/QDialog>

class monAbout : public QDialog , public Ui::AboutDialog
{
    Q_OBJECT

public:
    monAbout(QWidget *parent = Q_NULLPTR);

protected slots:    
     void DoCancel();

};

#endif // monabout_h