#include <QDesktopWidget>
#include <QGraphicsPixmapItem>
#include <QPainter>

#include "largevisform.h"
#include "ui_largevisform.h"

LargeVisForm::LargeVisForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LargeVisForm)
{
    ui->setupUi(this);
}

void LargeVisForm::setVisualization(QPixmap *pixmap_graph_visualization, refer current_visualization_size)
{
    QDesktopWidget desktopWidget;
    refer widget_width = current_visualization_size + 10;
    refer widget_height = current_visualization_size + 10;

    if (widget_height > desktopWidget.height() * 0.9)
    {
        widget_height = desktopWidget.height() * 0.9;
    }
    if (widget_width > desktopWidget.width() * 0.9)
    {
        widget_width = desktopWidget.width() * 0.9;
    }

    ui->graphicsView_graphdrawing->setFixedSize(widget_width, widget_height);

    QGraphicsPixmapItem *pixmapItemGV = new QGraphicsPixmapItem();
    pixmapItemGV->setPixmap(*pixmap_graph_visualization);
    QGraphicsScene *scene_graph_visualization = new QGraphicsScene(0,0,current_visualization_size,current_visualization_size);
    scene_graph_visualization->addItem(pixmapItemGV);
    ui->graphicsView_graphdrawing->setScene(scene_graph_visualization);
    ui->graphicsView_graphdrawing->repaint();

    this->adjustSize();
}

LargeVisForm::~LargeVisForm()
{
    delete ui;
}
