#include "dialogoptionsvisualization.h"
#include "ui_dialogoptionsvisualization.h"

DialogOptionsVisualization::DialogOptionsVisualization(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOptionsVisualization)
{
    ui->setupUi(this);

    ui->spinBox_crossing_minimization->setValue(CommonSettings::crossing_minimization_time_limit / 1000);
    ui->spinBox_font_size_labels->setValue(CommonSettings::label_font_size);
    ui->spinBox_vertex_radius->setValue(CommonSettings::vertex_radius);
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(setParams()));
}

DialogOptionsVisualization::~DialogOptionsVisualization()
{
    delete ui;
}

void DialogOptionsVisualization::setParams()
{
    CommonSettings::crossing_minimization_time_limit = ui->spinBox_crossing_minimization->value() * 1000;
    CommonSettings::label_font_size = ui->spinBox_font_size_labels->value();
    CommonSettings::vertex_radius = ui->spinBox_vertex_radius->value();
}
