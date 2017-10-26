#ifndef DIALOGBA_H
#define DIALOGBA_H

#include <QDialog>

#include "common.h"

namespace Ui {
class DialogBA;
}

class DialogBA : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBA(QWidget *parent = 0);
    ~DialogBA();
    void getParams(bool *p_accepted, refer *p_n_max, refer *p_w);

private:
    Ui::DialogBA *ui;

    bool accepted;
    refer n_max;
    refer w;

public slots:
    void setParams();
};

#endif // DIALOGBA_H
