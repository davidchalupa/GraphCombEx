#ifndef DIALOGUDG_H
#define DIALOGUDG_H

#include <QDialog>

#include "common.h"

namespace Ui {
class DialogUDG;
}

class DialogUDG : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUDG(QWidget *parent = 0);
    ~DialogUDG();
    void getParams(bool *p_accepted, refer *p_n_max, refer *p_range, refer *p_grid);

private:
    Ui::DialogUDG *ui;

    bool accepted;
    refer n_max;
    refer range;
    refer grid;

public slots:
    void setParams();
};

#endif // DIALOGUDG_H
