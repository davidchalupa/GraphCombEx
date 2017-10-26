#ifndef DIALOGGRID_H
#define DIALOGGRID_H

#include <QDialog>

#include "common.h"

namespace Ui {
class DialogGrid;
}

class DialogGrid : public QDialog
{
    Q_OBJECT

public:
    explicit DialogGrid(QWidget *parent = 0);
    ~DialogGrid();
    void getParams(bool *p_accepted, refer *p_rows, refer *p_columns, double *p_beta);

private:
    Ui::DialogGrid *ui;

    bool accepted;
    refer rows;
    refer columns;
    refer beta_promile;
    double beta;

public slots:
    void setParams();
};

#endif // DIALOGGRID_H
