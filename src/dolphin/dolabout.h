#ifndef dolabout_h
#define dolabout_h

#include "ui_dolabout.h"
#include <QtWidgets/QDialog>

class dolAbout : public QDialog, public Ui::AboutDialog
{
    Q_OBJECT

public:
    dolAbout(QWidget *parent = Q_NULLPTR);
};



#endif // dolabout_h