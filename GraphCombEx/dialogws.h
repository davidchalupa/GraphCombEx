#ifndef DIALOGWS_H
#define DIALOGWS_H

#include <QDialog>

#include "common.h"

namespace Ui {
class DialogWS;
}

class DialogWS : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWS(QWidget *parent = 0);
    ~DialogWS();
    void getParams(bool *p_accepted, refer *p_n_max, refer *p_k, double *p_beta);

private:
    Ui::DialogWS *ui;

    bool accepted;
    refer n_max;
    refer k;
    refer beta_promile;
    double beta;

public slots:
    void setParams();
};

#endif // DIALOGWS_H
