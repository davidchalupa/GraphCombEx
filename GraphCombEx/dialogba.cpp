#include "dialogba.h"
#include "ui_dialogba.h"

DialogBA::DialogBA(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBA)
{
    ui->setupUi(this);
    accepted = false;

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(setParams()));
}

DialogBA::~DialogBA()
{
    delete ui;
}

void DialogBA::setParams()
{
    accepted = true;
    n_max = ui->spinBox_n_max->value();
    w = ui->spinBox_w->value();
}

void DialogBA::getParams(bool *p_accepted, refer *p_n_max, refer *p_w)
{
    *p_accepted = accepted;
    *p_n_max = n_max;
    *p_w = w;
}
