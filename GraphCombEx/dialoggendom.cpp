#include "dialoggendom.h"
#include "ui_dialoggendom.h"

DialogGenDom::DialogGenDom(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGenDom)
{
    ui->setupUi(this);
    accepted = false;

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(setParams()));
}

DialogGenDom::~DialogGenDom()
{
    delete ui;
}

void DialogGenDom::setParams()
{
    accepted = true;
    k = ui->spinBox_k->value();
}

void DialogGenDom::getParams(bool *p_accepted, refer *p_k)
{
    *p_accepted = accepted;
    *p_k = k;
}
