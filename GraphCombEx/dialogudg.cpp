#include "dialogudg.h"
#include "ui_dialogudg.h"

DialogUDG::DialogUDG(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogUDG)
{
    ui->setupUi(this);
    accepted = false;

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(setParams()));
}

DialogUDG::~DialogUDG()
{
    delete ui;
}

void DialogUDG::setParams()
{
    accepted = true;
    n_max = ui->spinBox_n_max->value();
    range = ui->spinBox_range->value();
    grid = ui->spinBox_grid->value();
}

void DialogUDG::getParams(bool *p_accepted, refer *p_n_max, refer *p_range, refer *p_grid)
{
    *p_accepted = accepted;
    *p_n_max = n_max;
    *p_range = range;
    *p_grid = grid;
}
