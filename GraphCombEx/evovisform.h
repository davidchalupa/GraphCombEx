#ifndef EVOVISFORM_H
#define EVOVISFORM_H

#include <QWidget>
#include <QFutureWatcher>
#include <QGraphicsScene>
#include <QPixmap>
#include <QThread>

#include "graphs.h"

namespace Ui {
class EvoVisForm;
}

class EvoVisForm : public QWidget
{
    Q_OBJECT

public:
    explicit EvoVisForm(QWidget *parent = 0);
    ~EvoVisForm();
    void setCrossingMinimisationParams(graph G, refer max_deg, refer *myx, refer *myy, refer main_visualization_size);
    void startCrossingMinimisation();

private:
    Ui::EvoVisForm *ui;

    graph G;
    QString graphFileName;
    QString newGraphFileName;
    QString graphVisualizationExportFileName;

    unsigned long max_deg;

    refer *myx, *myy, *distances, *queue;

    unsigned long long crossing_number;

    bool graph_visualization_visible;
    QPixmap *pixmap_graph_visualization;

    QFutureWatcher<void> FutureWatcherVisualizationUpdate;

    void showEvent(QShowEvent *event);

    void update_graph_visualization();
    void clear_graph_visualization();

    void updateVisualizationProcessing();

    QGraphicsPixmapItem *pixmapItemGVOptimized;
    QGraphicsScene *scene_graph_visualization_optimized;

public slots:
    void exportGraphVisualization();
    void finishVisualizationUpdate();
    void updateGraphVisualizationOptimized(refer *myx, refer *myy);
};

class WorkerThreadCrossingMinimization : public QThread
{
    Q_OBJECT

private:
    graph G;
    refer *myx;
    refer *myy;
    refer *new_myx;
    refer *new_myy;
    refer *acc_myx;
    refer *acc_myy;
    refer *draw_myx;
    refer *draw_myy;
    bool *crossing_vertices;
    bool *acc_crossing_vertices;

public:
    WorkerThreadCrossingMinimization();
    ~WorkerThreadCrossingMinimization();
    void crossingMinimization();
    void setParams(graph G, refer *myx, refer *myy);
    void run() Q_DECL_OVERRIDE;
    void updateGraphVisualization(refer *myx, refer *myy);

signals:
    void updateGraphVisualizationSignal(refer *myx, refer *myy);

};

#endif // EVOVISFORM_H
