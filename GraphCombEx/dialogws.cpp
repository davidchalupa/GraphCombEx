#include "dialogws.h"
#include "ui_dialogws.h"

DialogWS::DialogWS(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWS)
{
    ui->setupUi(this);
    accepted = false;

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(setParams()));
}

DialogWS::~DialogWS()
{
    delete ui;
}

void DialogWS::setParams()
{
    accepted = true;
    n_max = ui->spinBox_n_max->value();
    beta_promile = ui->spinBox_betapromile->value();
    k = ui->spinBox_lattice->value();

    beta = (double) (beta_promile) / 1000.0;
}

void DialogWS::getParams(bool *p_accepted, refer *p_n_max, refer *p_k, double *p_beta)
{
    *p_accepted = accepted;
    *p_n_max = n_max;
    *p_k = k;
    *p_beta = beta;
}
