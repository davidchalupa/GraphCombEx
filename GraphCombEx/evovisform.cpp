#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QtConcurrent/QtConcurrentRun>

#include <math.h>
#include <time.h>

#include "evovisform.h"
#include "ui_evovisform.h"

#include "common.h"

#include "random_generator.h"

#include <QDesktopWidget>
#include <QFileDialog>
#include <QTime>

#define CROSSING_VISUALIZATION_SIZE 800

#define VISUALIZATION_POINT 6

// ToDo: try to get rid of these global variables and make it all thread-safe
WorkerThreadCrossingMinimization *workerThreadCrossingMinimization = NULL;
refer crossing_number_global;
double variance_global;

EvoVisForm::EvoVisForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EvoVisForm)
{
    ui->setupUi(this);

    ui->progressBar->hide();
    ui->label_loading->hide();

    myx = NULL;
    myy = NULL;

    // adjusting the size
    QDesktopWidget desktopWidget;
    refer widget_width = CROSSING_VISUALIZATION_SIZE + 10;
    refer widget_height = CROSSING_VISUALIZATION_SIZE + 10;
    if (widget_height > desktopWidget.height() * 0.9)
    {
        widget_height = desktopWidget.height() * 0.9;
    }
    if (widget_width > desktopWidget.width() * 0.9)
    {
        widget_width = desktopWidget.width() * 0.9;
    }
    ui->graphicsView_graphdrawing->setFixedSize(widget_width, widget_height);
    this->adjustSize();

    // pixmap and scene reset
    pixmapItemGVOptimized = NULL;
    pixmap_graph_visualization = NULL;
    scene_graph_visualization_optimized = NULL;

    srand((unsigned long long) time(NULL));

    workerThreadCrossingMinimization = new WorkerThreadCrossingMinimization();
    connect(ui->pushButtonExportGraphVisualization,SIGNAL(clicked(bool)),this,SLOT(exportGraphVisualization()));

    connect(&this->FutureWatcherVisualizationUpdate,SIGNAL(finished()),this,SLOT(finishVisualizationUpdate()));

    connect(workerThreadCrossingMinimization,SIGNAL(updateGraphVisualizationSignal(refer *,refer *)),this,SLOT(updateGraphVisualizationOptimized(refer *,refer *)));
}

void EvoVisForm::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    startCrossingMinimisation();
}

#include <QMessageBox>
void EvoVisForm::setCrossingMinimisationParams(graph G, refer max_deg, refer *myx, refer *myy, refer main_visualization_size)
{
    refer i;

    this->G = G;
    this->max_deg = max_deg;

    this->myx = new refer[G->n];
    this->myy = new refer[G->n];
    for (i=0;i<G->n;i++)
    {
        this->myx[i] = myx[i] * CROSSING_VISUALIZATION_SIZE / main_visualization_size;
        this->myy[i] = myy[i] * CROSSING_VISUALIZATION_SIZE / main_visualization_size;
    }
}

void EvoVisForm::startCrossingMinimisation()
{
    QFuture<void> future = QtConcurrent::run(this, &EvoVisForm::updateVisualizationProcessing);
    this->FutureWatcherVisualizationUpdate.setFuture(future);
}

unsigned long long compute_crossing_number(graph G, refer *myx, refer *myy, bool *crossing_vertices)
{
    // computing the crossing number
    refer v1, v2, v3, v4, i, j;
    double crossing_x, crossing_y, denominator;
    unsigned long long crossing_number = 0;
    for (v1=0;v1<G->n;v1++)
    {
        crossing_vertices[v1] = false;
    }
    // parallel lines
    for (v1=0;v1<G->n;v1++)
    {
        for (i=0;i<G->V[v1].edgecount;i++)
        {
            v2 = G->V[v1].sibl[i];
            for (v3=0;v3<G->n;v3++)
            {
                for (j=0;j<G->V[v3].edgecount;j++)
                {
                    v4 = G->V[v3].sibl[j];
                    if (v1 == v3 || v1 == v4 || v2 == v3 || v2 == v4)
                    {
                        continue;
                    }
                    // test if the points are all distinct
                    if ((myx[v1] == myx[v2] && myy[v1] == myy[v2]) ||
                        (myx[v1] == myx[v3] && myy[v1] == myy[v3]) ||
                        (myx[v1] == myx[v4] && myy[v1] == myy[v4]) ||
                        (myx[v2] == myx[v3] && myy[v2] == myy[v3]) ||
                        (myx[v2] == myx[v4] && myy[v2] == myy[v4]) ||
                        (myx[v3] == myx[v4] && myy[v3] == myy[v4]))
                    {
                        crossing_number++;
                        crossing_vertices[v1] = true;
                        crossing_vertices[v2] = true;
                        crossing_vertices[v3] = true;
                        crossing_vertices[v4] = true;
                        continue;
                    }
                    denominator = (double)((myx[v1]-myx[v2])*(myy[v3]-myy[v4]) - (myy[v1]-myy[v2])*(myx[v3]-myx[v4]));
                    // parallel lines
                    if (0 == denominator)
                    {
                        // collinearity test
                        if ((double)(myy[v2]-myy[v1])/(double)(myx[v2]-myx[v1]) == (double)(myy[v4]-myy[v3])/(double)(myx[v4]-myx[v3]))
                        {
                            // test if they overlap
                            if ((myx[v1] <= myx[v3] && myx[v3] <= myx[v2]) || (myx[v2] <= myx[v3] && myx[v3] <= myx[v1]) ||
                                (myx[v1] <= myx[v4] && myx[v4] <= myx[v2]) || (myx[v2] <= myx[v4] && myx[v4] <= myx[v1]) ||
                                (myx[v3] <= myx[v1] && myx[v1] <= myx[v4]) || (myx[v4] <= myx[v1] && myx[v1] <= myx[v3]) ||
                                (myx[v3] <= myx[v2] && myx[v2] <= myx[v4]) || (myx[v4] <= myx[v2] && myx[v2] <= myx[v3]))
                            {
                                crossing_number++;
                                crossing_vertices[v1] = true;
                                crossing_vertices[v2] = true;
                                crossing_vertices[v3] = true;
                                crossing_vertices[v4] = true;
                            }
                        }
                        continue;
                    }
                    crossing_x = (double)((myx[v1]*myy[v2]-myy[v1]*myx[v2])*(myx[v3]-myx[v4]) - (myx[v1]-myx[v2])*(myx[v3]*myy[v4] - myy[v3]*myx[v4])) / denominator;
                    crossing_y = (double)((myx[v1]*myy[v2]-myy[v1]*myx[v2])*(myy[v3]-myy[v4]) - (myy[v1]-myy[v2])*(myx[v3]*myy[v4] - myy[v3]*myx[v4])) / denominator;
                    // if the crossing is "in between", we increment the counter
                    if (((myx[v1] <= crossing_x && crossing_x <= myx[v2]) || (myx[v2] <= crossing_x && crossing_x <= myx[v1])) &&
                        ((myy[v1] <= crossing_y && crossing_y <= myy[v2]) || (myy[v2] <= crossing_y && crossing_y <= myy[v1])) &&
                        ((myx[v3] <= crossing_x && crossing_x <= myx[v4]) || (myx[v4] <= crossing_x && crossing_x <= myx[v3])) &&
                        ((myy[v3] <= crossing_y && crossing_y <= myy[v4]) || (myy[v4] <= crossing_y && crossing_y <= myy[v3]))
                       )
                    {
                        crossing_number++;
                        crossing_vertices[v1] = true;
                        crossing_vertices[v2] = true;
                        crossing_vertices[v3] = true;
                        crossing_vertices[v4] = true;
                    }
                }
            }
        }
    }
    // every crossing is counted four times
    crossing_number /= 4;

    return crossing_number;
}

void EvoVisForm::update_graph_visualization()
{
    refer v, j;
    refer current_visualization_size;
    random_generator generator;
    int shade;
    current_visualization_size = CROSSING_VISUALIZATION_SIZE;

    bool *crossing_vertices = new bool[G->n];

    QPixmap *pixmap = new QPixmap(current_visualization_size,current_visualization_size);
    QPainter *painter;

    painter = new QPainter(pixmap);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillRect(QRect(0,0,current_visualization_size,current_visualization_size),QColor(255,255,255,255));
    painter->setPen(QColor(0,0,0,255));

    crossing_number = compute_crossing_number(G, myx, myy, crossing_vertices);

    for (v=0;v<G->n;v++)
    {
        for (j=0;j<G->V[v].edgecount;j++)
        {
            shade = generator.random(50,150);
            painter->setPen(QPen(QColor(shade,shade,shade,255), 2, Qt::SolidLine));
            painter->drawLine(myx[v],myy[v],myx[G->V[v].sibl[j]],myy[G->V[v].sibl[j]]);
        }
    }

    for (v=0;v<G->n;v++)
    {
        painter->setBrush(QColor(128,0,0,255));
        painter->drawEllipse(myx[v]-VISUALIZATION_POINT,myy[v]-VISUALIZATION_POINT,VISUALIZATION_POINT*2,VISUALIZATION_POINT*2);
    }

    QFont qFont = painter->font();
    qFont.setPointSize(6);
    painter->setFont(qFont);
    if (G->n < 1000)
    {
        for (v=0;v<G->n;v++)
        {
            if (NULL != get_vertex_label(v))
            {
                painter->setPen(QColor(generator.random(64,192),0,0,255));
                painter->drawText(QPoint(myx[v],myy[v]),get_vertex_label(v));
            }
            else
            {
                painter->setPen(QColor(generator.random(32,192),0,0,255));
                painter->drawText(QPoint(myx[v],myy[v]),QString::number(v+1));
            }
        }
    }

    painter->end();

    pixmap_graph_visualization = pixmap;
    graph_visualization_visible = true;
}

void EvoVisForm::updateGraphVisualizationOptimized(refer *myx, refer *myy)
{
    refer v, j;
    refer current_visualization_size;
    random_generator generator;
    int shade;

    srand((unsigned long long) time(NULL));

    if (pixmapItemGVOptimized != NULL)
    {
        delete(pixmapItemGVOptimized);
    }
    if (scene_graph_visualization_optimized != NULL)
    {
        delete(scene_graph_visualization_optimized);
    }
    if (pixmap_graph_visualization != NULL)
    {
        delete(pixmap_graph_visualization);
    }

    current_visualization_size = CROSSING_VISUALIZATION_SIZE;
    QPixmap *pixmap = new QPixmap(current_visualization_size,current_visualization_size);
    QPainter *painter;
    painter = new QPainter(pixmap);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillRect(QRect(0,0,current_visualization_size,current_visualization_size),QColor(255,255,255,255));
    painter->setPen(QColor(0,0,0,255));

    for (v=0;v<G->n;v++)
    {
        for (j=0;j<G->V[v].edgecount;j++)
        {
            shade = generator.random(80,120);
            painter->setPen(QPen(QColor(shade,shade,shade,255), 2, Qt::SolidLine));
            painter->drawLine(myx[v],myy[v],myx[G->V[v].sibl[j]],myy[G->V[v].sibl[j]]);
        }
    }

    for (v=0;v<G->n;v++)
    {
        painter->setBrush(QColor(128,0,0,255));
        painter->drawEllipse(myx[v]-VISUALIZATION_POINT,myy[v]-VISUALIZATION_POINT,VISUALIZATION_POINT*2,VISUALIZATION_POINT*2);
    }

    QFont qFont = painter->font();
    qFont.setPointSize(8);
    painter->setFont(qFont);
    if (G->n < 1000)
    {
        for (v=0;v<G->n;v++)
        {
            if (NULL != get_vertex_label(v))
            {
                painter->setPen(QColor(generator.random(32,192),0,0,255));
                painter->drawText(QPoint(myx[v],myy[v]),get_vertex_label(v));
            }
            else
            {
                painter->setPen(QColor(generator.random(32,192),0,0,255));
                painter->drawText(QPoint(myx[v],myy[v]),QString::number(v+1));
            }
        }
    }

    painter->end();

    pixmap_graph_visualization = pixmap;
    graph_visualization_visible = true;

    pixmapItemGVOptimized = new QGraphicsPixmapItem();
    pixmapItemGVOptimized->setPixmap(*pixmap_graph_visualization);
    scene_graph_visualization_optimized = new QGraphicsScene(0,0,current_visualization_size,current_visualization_size);
    scene_graph_visualization_optimized->addItem(pixmapItemGVOptimized);
    ui->graphicsView_graphdrawing->setScene(scene_graph_visualization_optimized);
    ui->graphicsView_graphdrawing->repaint();

    delete(painter);

    ui->label_crossing->setText(QString::number(crossing_number_global));
    ui->label_crossing->repaint();
    ui->label_variance->setText(QString::number(variance_global));
    ui->label_variance->repaint();
}

void EvoVisForm::finishVisualizationUpdate()
{
    refer current_visualization_size;
    current_visualization_size = CROSSING_VISUALIZATION_SIZE;
    QGraphicsPixmapItem *pixmapItemGV = new QGraphicsPixmapItem();
    pixmapItemGV->setPixmap(*pixmap_graph_visualization);
    QGraphicsScene *scene_graph_visualization = new QGraphicsScene(0,0,current_visualization_size,current_visualization_size);
    scene_graph_visualization->addItem(pixmapItemGV);
    ui->graphicsView_graphdrawing->setScene(scene_graph_visualization);
    ui->graphicsView_graphdrawing->repaint();

    workerThreadCrossingMinimization->setParams(G, myx, myy);
    workerThreadCrossingMinimization->start();
}

void EvoVisForm::updateVisualizationProcessing()
{
    if (NULL != G && G->m <= MAX_EDGES_DRAWING)
    {
        update_graph_visualization();
    }
    else
    {
        clear_graph_visualization();
    }
}

void EvoVisForm::clear_graph_visualization()
{
    QPixmap *pixmap = new QPixmap(CROSSING_VISUALIZATION_SIZE,CROSSING_VISUALIZATION_SIZE);
    QPainter *painter;
    painter = new QPainter(pixmap);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillRect(QRect(0,0,CROSSING_VISUALIZATION_SIZE,CROSSING_VISUALIZATION_SIZE),QColor(230,230,230,255));
    painter->end();

    pixmap_graph_visualization = pixmap;
    graph_visualization_visible = false;
}


void EvoVisForm::exportGraphVisualization()
{
    graphVisualizationExportFileName = QFileDialog::getSaveFileName(this,"Choose a file to save the visualization",QString::null,QString::null);

    if (NULL == G || NULL == graphVisualizationExportFileName || "" == graphVisualizationExportFileName)
    {
        return;
    }

    if (! graphVisualizationExportFileName.toLower().endsWith(".png") &&
        ! graphVisualizationExportFileName.toLower().endsWith(".bmp") &&
        ! graphVisualizationExportFileName.toLower().endsWith(".jpg") &&
        ! graphVisualizationExportFileName.toLower().endsWith(".jpeg"))
    {
        graphVisualizationExportFileName += ".png";
    }

    QImage pixmap_image = pixmap_graph_visualization->toImage();
    pixmap_image.setDotsPerMeterX(6000);
    pixmap_image.setDotsPerMeterY(6000);
    pixmap_image.save(graphVisualizationExportFileName);
}

EvoVisForm::~EvoVisForm()
{
    if (NULL != myx)
    {
        delete[](myx);
    }
    if (NULL != myy)
    {
        delete[](myy);
    }
    if (NULL != workerThreadCrossingMinimization)
    {
        workerThreadCrossingMinimization->terminate();
    }
    delete workerThreadCrossingMinimization;
    workerThreadCrossingMinimization = NULL;
    delete ui;
}

///////////////////////////
// CROSSING MINIMIZATION //
///////////////////////////

WorkerThreadCrossingMinimization::WorkerThreadCrossingMinimization()
{
    new_myx = NULL;
    new_myy = NULL;
    acc_myx = NULL;
    acc_myy = NULL;
    draw_myx = NULL;
    draw_myy = NULL;
    crossing_vertices = NULL;
    acc_crossing_vertices = NULL;
}

WorkerThreadCrossingMinimization::~WorkerThreadCrossingMinimization()
{
    if (NULL != new_myx) delete[](new_myx);
    if (NULL != new_myy) delete[](new_myy);
    if (NULL != acc_myx) delete[](acc_myx);
    if (NULL != acc_myy) delete[](acc_myy);
    if (NULL != draw_myx) delete[](draw_myx);
    if (NULL != draw_myy) delete[](draw_myy);
    if (NULL != crossing_vertices) delete[](crossing_vertices);
    if (NULL != acc_crossing_vertices) delete[](acc_crossing_vertices);
}

// maximum independent set and clique covering
void WorkerThreadCrossingMinimization::crossingMinimization()
{
    refer v, i, j, lambda;
    random_generator generator;
    unsigned long long crossing_number = 0;
    int max_time = CommonSettings::crossing_minimization_time_limit, refresh_interval = 100;
    double mutation_x, mutation_y;
    double variance, variance_min, variance_max, c, k, k_success;
    bool changed = false;
    bool mode_full = true;
    long r;

    QTime qTime, qRefreshTime;

    lambda = 1;
    //c = 0.85;
    c = 1.0;
    variance = 100.0;
    variance_min = 1.0;
    variance_max = (double) CROSSING_VISUALIZATION_SIZE / 2.0;
    if (variance < variance_min)
    {
        variance = variance_min;
    }
    if (variance > variance_max)
    {
        variance = variance_max;
    }

    double prob_worsening_acc = 0.0;

    refer new_crossing_number = 0;
    refer acc_crossing_number = 0;

    new_myx = new refer[G->n];
    new_myy = new refer[G->n];
    acc_myx = new refer[G->n];
    acc_myy = new refer[G->n];
    draw_myx = new refer[G->n];
    draw_myy = new refer[G->n];
    crossing_vertices = new bool[G->n];
    acc_crossing_vertices = new bool[G->n];

    srand((unsigned long long) time(NULL));

    /*for (v=0;v<G->n;v++)
    {
        // random initialization
        myx[v] = generator.random(10,VISUALIZATION_SIZE-31);
        myy[v] = generator.random(10,VISUALIZATION_SIZE-31);
    }*/

    crossing_number = compute_crossing_number(G, myx, myy, crossing_vertices);

    qTime.start();
    qRefreshTime.start();

    for (v=0;v<G->n;v++)
    {
        draw_myx[v] = myx[v];
        draw_myy[v] = myy[v];
    }
    updateGraphVisualization(draw_myx, draw_myy);

    k = 0;
    k_success = 0;
    while (qTime.elapsed() < max_time)
    {
        if (0 == crossing_number)
        {
            break;
        }
        r = generator.random(0,100);
        if (r < 25)
        {
            variance = (double) CROSSING_VISUALIZATION_SIZE / (double) G->n;
            mode_full = true;
        }
        else if (r < 50)
        {
            variance = 10.0;
            mode_full = true;
        }
        else if (r < 75)
        {
            variance = 10.0;
            mode_full = false;
        }
        else
        {
            variance = 100.0;
            mode_full = false;
        }
        /*if (r < 50)
        {
            variance = 10.0;
            mode_full = false;
        }
        else
        {
            variance = 100.0;
            mode_full = false;
        }*/

        // Gaussian mutation
        acc_crossing_number = crossing_number;
        changed = false;
        for (i=0;i<lambda;i++)
        {
            if (mode_full)
            {
                for (v=0;v<G->n;v++)
                {
                    if (crossing_vertices[v])
                    {
                        do
                        {
                            mutation_x = generator.random_gauss() * variance;
                            mutation_y = generator.random_gauss() * variance;
                        }
                        while ((double) myx[v] < -mutation_x || (double) myy[v] < -mutation_y);
                        new_myx[v] = (refer)((double) myx[v] + mutation_x);
                        new_myy[v] = (refer)((double) myy[v] + mutation_y);
                    }
                    else
                    {
                        new_myx[v] = myx[v];
                        new_myy[v] = myy[v];
                    }
                    if (new_myx[v] < 10)
                    {
                        new_myx[v] = 10;
                    }
                    if (new_myy[v] < 10)
                    {
                        new_myy[v] = 10;
                    }
                    if (new_myx[v] > CROSSING_VISUALIZATION_SIZE-51)
                    {
                        new_myx[v] = CROSSING_VISUALIZATION_SIZE-51;
                    }
                    if (new_myy[v] > CROSSING_VISUALIZATION_SIZE-11)
                    {
                        new_myy[v] = CROSSING_VISUALIZATION_SIZE-11;
                    }
                }
            }
            else
            {
                j = generator.random(0,G->n-1);
                for (v=0;v<G->n;v++)
                {
                    if (crossing_vertices[v] && v == j)
                    {
                        do
                        {
                            mutation_x = generator.random_gauss() * variance;
                            mutation_y = generator.random_gauss() * variance;
                        }
                        while ((double) myx[v] < -mutation_x || (double) myy[v] < -mutation_y);
                        new_myx[v] = (refer)((double) myx[v] + mutation_x);
                        new_myy[v] = (refer)((double) myy[v] + mutation_y);
                    }
                    else
                    {
                        new_myx[v] = myx[v];
                        new_myy[v] = myy[v];
                    }
                    if (new_myx[v] < 10)
                    {
                        new_myx[v] = 10;
                    }
                    if (new_myy[v] < 10)
                    {
                        new_myy[v] = 10;
                    }
                    if (new_myx[v] > CROSSING_VISUALIZATION_SIZE-51)
                    {
                        new_myx[v] = CROSSING_VISUALIZATION_SIZE-51;
                    }
                    if (new_myy[v] > CROSSING_VISUALIZATION_SIZE-11)
                    {
                        new_myy[v] = CROSSING_VISUALIZATION_SIZE-11;
                    }
                }
            }
            new_crossing_number = compute_crossing_number(G, new_myx, new_myy, crossing_vertices);
            if (acc_crossing_number >= new_crossing_number || generator.random(0,RAND_MAX) < (long)((RAND_MAX+1)*prob_worsening_acc))
            {
                acc_crossing_number = new_crossing_number;
                for (v=0;v<G->n;v++)
                {
                    acc_myx[v] = new_myx[v];
                    acc_myy[v] = new_myy[v];
                    acc_crossing_vertices[v] = crossing_vertices[v];
                }
                changed = true;
            }
        }
        // acceptance
        if (changed)
        {
            if (qRefreshTime.elapsed() > refresh_interval)
            {
                crossing_number_global = crossing_number;
                variance_global = variance;
                for (v=0;v<G->n;v++)
                {
                    draw_myx[v] = myx[v];
                    draw_myy[v] = myy[v];
                }
                updateGraphVisualization(draw_myx, draw_myy);
                qRefreshTime.restart();
            }
            if (crossing_number > acc_crossing_number)
            {
                k_success++;
            }
            crossing_number = acc_crossing_number;
            for (v=0;v<G->n;v++)
            {
                myx[v] = acc_myx[v];
                myy[v] = acc_myy[v];
                crossing_vertices[v] = acc_crossing_vertices[v];
            }
        }
        k++;
        // adaptation
        if (k >= 10*G->n)
        {
            if ((double) k_success / (double) k < 0.2)
            {
                variance *= c;
                if (variance < variance_min)
                {
                    variance = variance_min;
                }
            }
            else if ((double) k_success / (double) k > 0.2)
            {
                variance /= c;
                if (variance > variance_max)
                {
                    variance = variance_max;
                }
            }
            k = 0;
            k_success = 0;
        }
    }

    delete[](new_myx);
    delete[](new_myy);
    delete[](acc_myx);
    delete[](acc_myy);
    delete[](draw_myx);
    delete[](draw_myy);
    delete[](crossing_vertices);
    delete[](acc_crossing_vertices);

    new_myx = NULL;
    new_myy = NULL;
    acc_myx = NULL;
    acc_myy = NULL;
    draw_myx = NULL;
    draw_myy = NULL;
    crossing_vertices = NULL;
    acc_crossing_vertices = NULL;

    crossing_number_global = crossing_number;
    updateGraphVisualization(myx, myy);
}

void WorkerThreadCrossingMinimization::setParams(graph G, refer *myx, refer *myy)
{
    this->G = G;
    this->myx = myx;
    this->myy = myy;
}

void WorkerThreadCrossingMinimization::updateGraphVisualization(refer *myx, refer *myy)
{
    emit updateGraphVisualizationSignal(myx, myy);
}

void WorkerThreadCrossingMinimization::run()
{
    crossingMinimization();
}

