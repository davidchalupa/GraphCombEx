#include "dialogcompletetree.h"
#include "ui_dialogcompletetree.h"

DialogCompleteTree::DialogCompleteTree(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCompleteTree)
{
    ui->setupUi(this);
    accepted = false;

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(setParams()));
}

DialogCompleteTree::~DialogCompleteTree()
{
    delete ui;
}

void DialogCompleteTree::setParams()
{
    accepted = true;
    b = ui->spinBox_b->value();
    d = ui->spinBox_d->value();
}

void DialogCompleteTree::getParams(bool *p_accepted, refer *p_b, refer *p_d)
{
    *p_accepted = accepted;
    *p_b = b;
    *p_d = d;
}
