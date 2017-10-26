#ifndef DIALOGGENDOM_H
#define DIALOGGENDOM_H

#include <QDialog>

#include "common.h"

namespace Ui {
class DialogGenDom;
}

class DialogGenDom : public QDialog
{
    Q_OBJECT

public:
    explicit DialogGenDom(QWidget *parent = 0);
    ~DialogGenDom();
    void getParams(bool *p_accepted, refer *p_k);

private:
    Ui::DialogGenDom *ui;

    bool accepted;
    refer k;

public slots:
    void setParams();
};

#endif // DIALOGGENDOM_H
