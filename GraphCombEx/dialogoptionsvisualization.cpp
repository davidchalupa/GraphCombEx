#include "dialogoptionsvisualization.h"
#include "ui_dialogoptionsvisualization.h"

DialogOptionsVisualization::DialogOptionsVisualization(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOptionsVisualization)
{
    ui->setupUi(this);

    ui->spinBox_crossing_minimization->setValue(CommonSettings::crossing_minimization_time_limit / 1000);
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(setParams()));
}

DialogOptionsVisualization::~DialogOptionsVisualization()
{
    delete ui;
}

void DialogOptionsVisualization::setParams()
{
    CommonSettings::crossing_minimization_time_limit = ui->spinBox_crossing_minimization->value() * 1000;
}
