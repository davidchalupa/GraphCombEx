#include "dialoggrid.h"
#include "ui_dialoggrid.h"

DialogGrid::DialogGrid(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGrid)
{
    ui->setupUi(this);
    accepted = false;

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(setParams()));
}

DialogGrid::~DialogGrid()
{
    delete ui;
}

void DialogGrid::setParams()
{
    accepted = true;
    rows = ui->spinBox_rows->value();
    columns = ui->spinBox_columns->value();
    beta_promile = ui->spinBox_betapromile->value();

    beta = (double) (beta_promile) / 1000.0;
}

void DialogGrid::getParams(bool *p_accepted, refer *p_rows, refer *p_columns, double *p_beta)
{
    *p_accepted = accepted;
    *p_rows = rows;
    *p_columns = columns;
    *p_beta = beta;
}
