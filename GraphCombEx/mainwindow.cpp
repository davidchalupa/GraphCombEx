#include <QtGui>
#include <QtWidgets>
#include <QtWidgets/QApplication>

#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QtConcurrent/QtConcurrentRun>

#include <math.h>
#include <time.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "statistics.h"
#include "random_generator.h"

#include "dialogba.h"
#include "dialogudg.h"
#include "dialogcompletetree.h"
#include "dialoggendom.h"
#include "dialogws.h"
#include "dialoggrid.h"
#include "dialogoptionsalgorithms.h"
#include "dialogoptionsvisualization.h"

#include "algorithm.h"
#include "algorithm_brelaz.h"
#include "algorithm_greedyindset.h"
#include "algorithm_greedyclique.h"
#include "algorithm_iggcc.h"
#include "algorithm_igcol.h"
#include "algorithm_greedydom.h"
#include "algorithm_lscycle.h"

#include "problem_ccp.h"

#define VISUALIZATION_DOUBLE_SCALE 2
#define VISUALIZATION_MEDIUM_SCALE 5
#define VISUALIZATION_LARGE_SCALE 25

#define VISUALIZATION_SIZE 400
#define VISUALIZATION_SIZE_DOUBLE VISUALIZATION_SIZE * VISUALIZATION_DOUBLE_SCALE
#define VISUALIZATION_SIZE_MEDIUM VISUALIZATION_SIZE * VISUALIZATION_MEDIUM_SCALE
#define VISUALIZATION_SIZE_LARGE VISUALIZATION_SIZE * VISUALIZATION_LARGE_SCALE

#define VISUALIZATION_POINT 3

#define VISUALIZATION_MARGIN 70

WorkerThreadIGRLSCliqueCovering *workerThreadIGRLSCliqueCovering;
WorkerThreadIGRLSColoring *workerThreadIGRLSColoring;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->actionMesh_based->setChecked(false);
    ui->actionCentrality_based->setChecked(true);
    ui->action1x->setChecked(true);
    ui->action25x->setChecked(false);
    ui->progressBar->hide();
    ui->label_loading->hide();

    G = NULL;
    current_coloring = NULL;
    dominating_set = NULL;
    dominating_set_clustering = NULL;
    current_longest_cycle_vertices = NULL;
    degree_distrib = NULL;
    myx = NULL;
    myy = NULL;
    degree_distrib_size = 300;
    adjacency_matrix_visible = false;
    degree_distribution_visible = false;
    graph_visualization_visible = false;
    show_triangles = false;
    show_girth = false;
    show_diameter = false;
    show_longest_cycle = false;

    CommonSettings::label_font_size = VISUALIZATION_POINT * 3 / 2;
    CommonSettings::vertex_radius = VISUALIZATION_POINT;

    srand((unsigned long long) time(NULL));

    workerThreadIGRLSCliqueCovering = new WorkerThreadIGRLSCliqueCovering();
    workerThreadIGRLSColoring = new WorkerThreadIGRLSColoring();

    connect(ui->actionOpen,SIGNAL(triggered(bool)),this,SLOT(loadGraph()));
    connect(ui->actionSave,SIGNAL(triggered(bool)),this,SLOT(saveGraph()));
    connect(ui->actionUnit_disk_graph,SIGNAL(triggered(bool)),this,SLOT(generateGraphUDG()));
    connect(ui->actionScale_free_network,SIGNAL(triggered(bool)),this,SLOT(generateGraphBA()));
    connect(ui->actionComplete_n_ary_tree,SIGNAL(triggered(bool)),this,SLOT(generateGraphCompleteTree()));
    connect(ui->actionComplementary_graph,SIGNAL(triggered(bool)),this,SLOT(generateComplementaryGraph()));
    connect(ui->actionGeneralized_dominating_set_graph,SIGNAL(triggered(bool)),this,SLOT(generateGeneralizedDominatingSetGraph()));
    connect(ui->actionWatts_Strogatz_network,SIGNAL(triggered(bool)),this,SLOT(generateGraphWS()));
    connect(ui->actionRegular_grid_with_rewiring,SIGNAL(triggered(bool)),this,SLOT(generateGraphGrid()));
    connect(ui->actionGraph_with_pruned_leaves,SIGNAL(triggered(bool)),this,SLOT(generateGraphPrunedLeaves()));
    connect(ui->actionLargest_component,SIGNAL(triggered(bool)),this,SLOT(generateLargestComponent()));

    connect(ui->actionGraph_visualization,SIGNAL(triggered(bool)),this,SLOT(exportGraphVisualization()));
    connect(ui->actionAdjacency_matrix_visualization,SIGNAL(triggered(bool)),this,SLOT(exportAdjacencyMatrix()));
    connect(ui->actionDegree_distribution,SIGNAL(triggered(bool)),this,SLOT(exportDegreeDistribution()));
    connect(ui->actionExport_dominating_set,SIGNAL(triggered(bool)),this,SLOT(exportDominatingSet()));
    connect(ui->actionExport_dominating_set_LP_relaxation,SIGNAL(triggered(bool)),this,SLOT(exportDominatingSetLPRelaxation()));

    connect(ui->actionMesh_based,SIGNAL(triggered(bool)),this,SLOT(chooseGridbased()));
    connect(ui->actionTree_based,SIGNAL(triggered(bool)),this,SLOT(chooseTreebased()));
    connect(ui->actionCentrality_based,SIGNAL(triggered(bool)),this,SLOT(chooseCentralitybased()));
    connect(ui->actionCircular,SIGNAL(triggered(bool)),this,SLOT(chooseCircular()));
    connect(ui->actionCycle_based,SIGNAL(triggered(bool)),this,SLOT(chooseCyclebased()));
    connect(ui->actionCrossings_based,SIGNAL(triggered(bool)),this,SLOT(chooseCrossingsbased()));

    connect(ui->action1x,SIGNAL(triggered(bool)),this,SLOT(choose1x()));
    connect(ui->action2x,SIGNAL(triggered(bool)),this,SLOT(choose2x()));
    connect(ui->action5x,SIGNAL(triggered(bool)),this,SLOT(choose5x()));
    connect(ui->action25x,SIGNAL(triggered(bool)),this,SLOT(choose25x()));
    connect(ui->actionExit,SIGNAL(triggered(bool)),this,SLOT(exitApp()));
    connect(ui->actionTriangles,SIGNAL(triggered(bool)),this,SLOT(countTriangles()));
    connect(ui->actionGirth,SIGNAL(triggered(bool)),this,SLOT(computeGirth()));
    connect(ui->actionDiameter,SIGNAL(triggered(bool)),this,SLOT(computeDiameter()));
    connect(ui->actionBrelaz_s_heuristic,SIGNAL(triggered(bool)),this,SLOT(computeChromaticBrelaz()));
    connect(ui->actionGreedy_independent_set,SIGNAL(triggered(bool)),this,SLOT(computeGreedyIndependentSet()));
    connect(ui->actionGreedy_clique,SIGNAL(triggered(bool)),this,SLOT(computeGreedyClique()));
    connect(ui->actionRandomized_local_search_and_iterated_greedy_indset,SIGNAL(triggered(bool)),this,SLOT(computeIGRLSCliqueCovering()));
    connect(ui->actionIterated_greedy_and_randomized_local_search_ccp,SIGNAL(triggered(bool)),this,SLOT(computeIGRLSCliqueCovering()));
    connect(ui->actionRandomized_local_search_and_Iterated_Greedy,SIGNAL(triggered(bool)),this,SLOT(computeIGRLSColoring()));
    connect(ui->actionIterated_greedy_and_randomized_local_search,SIGNAL(triggered(bool)),this,SLOT(computeIGRLSColoring()));
    connect(ui->actionGreedy_dominating_set,SIGNAL(triggered(bool)),this,SLOT(computeGreedyDominatingSet()));
    connect(ui->actionDepth_first_search,SIGNAL(triggered(bool)),this,SLOT(computeLongestCycleDFS()));
    connect(ui->actionDepth_first_search_with_local_search,SIGNAL(triggered(bool)),this,SLOT(computeLongestCycleDFSLS()));
    connect(ui->actionShow_in_a_separate_window,SIGNAL(triggered(bool)),this,SLOT(showInSeparateWindow()));
    connect(ui->actionMinimize_crossings,SIGNAL(triggered(bool)),this,SLOT(minimizeCrossings()));
    connect(ui->actionAlgorithms,SIGNAL(triggered(bool)),this,SLOT(showOptionsAlgorithms()));
    connect(ui->actionVisualization,SIGNAL(triggered(bool)),this,SLOT(showOptionsVisualization()));
    connect(ui->actionAbout,SIGNAL(triggered(bool)),this,SLOT(showAboutWindow()));

    connect(&this->FutureWatcherGraphLoading,SIGNAL(finished()),this,SLOT(finishGraphLoading()));
    connect(&this->FutureWatcherGraphSaving,SIGNAL(finished()),this,SLOT(finishGraphSaving()));
    connect(&this->FutureWatcherVisualizationUpdate,SIGNAL(finished()),this,SLOT(finishVisualizationUpdate()));
    connect(&this->FutureWatcherTrianglesCounting,SIGNAL(finished()),this,SLOT(finishTriangleCounting()));
    connect(&this->FutureWatcherGirthComputing,SIGNAL(finished()),this,SLOT(finishGirthComputing()));
    connect(&this->FutureWatcherDiameterComputing,SIGNAL(finished()),this,SLOT(finishDiameterComputing()));
    connect(&this->FutureWatcherChromatic,SIGNAL(finished()),this,SLOT(finishChromatic()));
    connect(&this->FutureWatcherIndependentSet,SIGNAL(finished()),this,SLOT(finishIndependentSet()));
    connect(&this->FutureWatcherMaxClique,SIGNAL(finished()),this,SLOT(finishMaxClique()));
    connect(&this->FutureWatcherCliqueCovering,SIGNAL(finished()),this,SLOT(finishCliqueCovering()));
    connect(&this->FutureWatcherDominatingSet,SIGNAL(finished()),this,SLOT(finishDominatingSet()));
    connect(&this->FutureWatcherLongestCycle,SIGNAL(finished()),this,SLOT(finishLongestCycle()));

    connect(workerThreadIGRLSCliqueCovering,SIGNAL(logMessageSignal(QString)),this,SLOT(logMessage(QString)));
    connect(workerThreadIGRLSCliqueCovering,SIGNAL(finished()),this,SLOT(finishWorkerThreadCliqueCovering()));
    connect(workerThreadIGRLSColoring,SIGNAL(logMessageSignal(QString)),this,SLOT(logMessage(QString)));
    connect(workerThreadIGRLSColoring,SIGNAL(finished()),this,SLOT(finishWorkerThreadColoring()));

    clear_graph_visualization();
    clear_adjacency_matrix();
    clear_degree_distribution();

    refer current_visualization_size;
    current_visualization_size = VISUALIZATION_SIZE;
    QGraphicsPixmapItem *pixmapItemGV = new QGraphicsPixmapItem();
    pixmapItemGV->setPixmap(*pixmap_graph_visualization);
    QGraphicsScene *scene_graph_visualization = new QGraphicsScene(0,0,current_visualization_size,current_visualization_size);
    scene_graph_visualization->addItem(pixmapItemGV);
    ui->graphicsView_graphdrawing->setScene(scene_graph_visualization);

    // adjacency matrix
    QGraphicsPixmapItem *pixmapItemAM = new QGraphicsPixmapItem();
    pixmapItemAM->setPixmap(*pixmap_adjacency_matrix);
    QGraphicsScene *scene_adjacency_matrix;
    scene_adjacency_matrix = new QGraphicsScene(0,0,300,300);
    scene_adjacency_matrix->addItem(pixmapItemAM);
    ui->graphicsView_adjacencymatrix->setScene(scene_adjacency_matrix);
    ui->graphicsView_adjacencymatrix->fitInView(QRectF(0,0,25,25), Qt::KeepAspectRatio);

    // degree distribution
    QGraphicsPixmapItem *pixmapItemDD = new QGraphicsPixmapItem();
    pixmapItemDD->setPixmap(*pixmap_degree_distribution);
    QGraphicsScene *scene_degree_distribution = new QGraphicsScene(0,0,degree_distrib_size,degree_distrib_size);
    scene_degree_distribution->addItem(pixmapItemDD);
    ui->graphicsView_degreedistr->setScene(scene_degree_distribution);
    ui->graphicsView_degreedistr->fitInView(QRectF(0,0,25,25), Qt::KeepAspectRatio);
}

void MainWindow::exitApp()
{
    QApplication::quit();
}

void MainWindow::chooseCentralitybased()
{
    ui->actionMesh_based->setChecked(false);
    ui->actionCentrality_based->setChecked(true);
    ui->actionCircular->setChecked(false);
    ui->actionCycle_based->setChecked(false);
    ui->actionTree_based->setChecked(false);
    ui->actionCrossings_based->setChecked(false);

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::updateVisualizationProcessing);
    this->FutureWatcherVisualizationUpdate.setFuture(future);
}

void MainWindow::chooseGridbased()
{
    ui->actionCentrality_based->setChecked(false);
    ui->actionMesh_based->setChecked(true);
    ui->actionCircular->setChecked(false);
    ui->actionCycle_based->setChecked(false);
    ui->actionTree_based->setChecked(false);
    ui->actionCrossings_based->setChecked(false);

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::updateVisualizationProcessing);
    this->FutureWatcherVisualizationUpdate.setFuture(future);
}

void MainWindow::chooseTreebased()
{
    ui->actionCentrality_based->setChecked(false);
    ui->actionMesh_based->setChecked(false);
    ui->actionCircular->setChecked(false);
    ui->actionCycle_based->setChecked(false);
    ui->actionTree_based->setChecked(true);
    ui->actionCrossings_based->setChecked(false);

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::updateVisualizationProcessing);
    this->FutureWatcherVisualizationUpdate.setFuture(future);
}

void MainWindow::chooseCircular()
{
    ui->actionMesh_based->setChecked(false);
    ui->actionCentrality_based->setChecked(false);
    ui->actionCircular->setChecked(true);
    ui->actionCycle_based->setChecked(false);
    ui->actionTree_based->setChecked(false);
    ui->actionCrossings_based->setChecked(false);

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::updateVisualizationProcessing);
    this->FutureWatcherVisualizationUpdate.setFuture(future);
}

void MainWindow::chooseCyclebased()
{
    ui->actionMesh_based->setChecked(false);
    ui->actionCentrality_based->setChecked(false);
    ui->actionCircular->setChecked(false);
    ui->actionCycle_based->setChecked(true);
    ui->actionTree_based->setChecked(false);
    ui->actionCrossings_based->setChecked(false);

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::updateVisualizationProcessing);
    this->FutureWatcherVisualizationUpdate.setFuture(future);
}

void MainWindow::chooseCrossingsbased()
{
    ui->actionMesh_based->setChecked(false);
    ui->actionCentrality_based->setChecked(false);
    ui->actionCircular->setChecked(false);
    ui->actionCycle_based->setChecked(false);
    ui->actionTree_based->setChecked(false);
    ui->actionCrossings_based->setChecked(true);

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::updateVisualizationProcessing);
    this->FutureWatcherVisualizationUpdate.setFuture(future);
}

void MainWindow::choose1x()
{
    ui->action25x->setChecked(false);
    ui->action5x->setChecked(false);
    ui->action2x->setChecked(false);
    ui->action1x->setChecked(true);

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::updateVisualizationProcessing);
    this->FutureWatcherVisualizationUpdate.setFuture(future);
}

void MainWindow::choose2x()
{
    ui->action25x->setChecked(false);
    ui->action5x->setChecked(false);
    ui->action2x->setChecked(true);
    ui->action1x->setChecked(false);

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::updateVisualizationProcessing);
    this->FutureWatcherVisualizationUpdate.setFuture(future);
}

void MainWindow::choose5x()
{
    ui->action1x->setChecked(false);
    ui->action2x->setChecked(false);
    ui->action5x->setChecked(true);
    ui->action25x->setChecked(false);

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::updateVisualizationProcessing);
    this->FutureWatcherVisualizationUpdate.setFuture(future);
}

void MainWindow::choose25x()
{
    ui->action1x->setChecked(false);
    ui->action2x->setChecked(false);
    ui->action5x->setChecked(false);
    ui->action25x->setChecked(true);

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::updateVisualizationProcessing);
    this->FutureWatcherVisualizationUpdate.setFuture(future);
}

int MainWindow::compute_statistics(graph G)
{
    min_deg = statistics::min_degree(G);
    max_deg = statistics::max_degree(G);
    avg_deg = statistics::average_degree(G);
    stdev_deg = statistics::degree_stdev(G);
    components = statistics::components(G);

    // lower and upper bounds, which follow from the statistics
    max_clique_lower = 0;
    if (G->m > 0)
    {
        max_clique_lower = 2;
    }
    else if (G->n > 0)
    {
        max_clique_lower = 1;
    }
    max_clique_upper = max_deg + 1;

    chromatic_lower = max_clique_lower;
    chromatic_upper = G->n;

    max_independent_set_lower = 0;
    if (min_deg < G->n-1)
    {
        max_independent_set_lower = 2;
    }
    else if (G->n > 0)
    {
        max_independent_set_lower = 1;
    }
    max_independent_set_upper = G->n - min_deg;

    clique_covering_lower = max_independent_set_lower;
    clique_covering_upper = G->n;

    dominating_set_lower = (refer) ceil((double) G->n / (double) (max_deg + 1));
    if (dominating_set_lower < components)
    {
        dominating_set_lower = components;
    }
    dominating_set_upper = G->n;

    longest_cycle_lower = 0;
    longest_cycle_upper = G->n;

    return 0;
}

void MainWindow::update_statistics()
{
    QFileInfo fileinfo(graphFileName);
    if (fileinfo.exists())
    {
        ui->label_graphname->setText(fileinfo.fileName());
    }
    else
    {
        ui->label_graphname->setText(QString("-"));
    }

    ui->label_vertices->setText(QString::number(G->n));
    ui->label_edges->setText(QString::number(G->m));
    ui->label_density->setText(QString::number(G->density));
    ui->label_components->setText(QString::number(components));
    ui->label_maxdeg->setText(QString::number(max_deg));
    ui->label_mindeg->setText(QString::number(min_deg));
    ui->label_avgdeg->setText(QString::number(avg_deg));
    ui->label_stdevdeg->setText(QString::number(stdev_deg));

    if (show_triangles)
    {
        ui->label_triangles->setText(QString::number(triangles));
        ui->label_mean_clustering_coefficient->setText(QString::number(mean_clustering_coefficient));
    }
    else
    {
        ui->label_triangles->setText(QString("?"));
        ui->label_mean_clustering_coefficient->setText(QString("?"));
    }
    if (show_girth)
    {
        if (G->n+1 != girth)
        {
            ui->label_girth->setText(QString::number(girth));
        }
        else
        {
            ui->label_girth->setText(QString("cycle-free"));
        }
    }
    else
    {
        ui->label_girth->setText(QString("?"));
    }
    if (show_diameter)
    {
        ui->label_diameter->setText(QString::number(diameter));
    }
    else
    {
        ui->label_diameter->setText(QString("?"));
    }

    if (max_clique_lower != max_clique_upper)
    {
        ui->label_maxclique->setText(QString::number(max_clique_lower) + "-" + QString::number(max_clique_upper));
    }
    else
    {
        ui->label_maxclique->setText(QString::number(max_clique_lower));
    }
    if (chromatic_lower != chromatic_upper)
    {
        ui->label_chromatic->setText(QString::number(chromatic_lower) + "-" + QString::number(chromatic_upper));
    }
    else
    {
        ui->label_chromatic->setText(QString::number(chromatic_lower));
    }
    if (max_independent_set_lower != max_independent_set_upper)
    {
        ui->label_indset->setText(QString::number(max_independent_set_lower) + "-" + QString::number(max_independent_set_upper));
    }
    else
    {
        ui->label_indset->setText(QString::number(max_independent_set_lower));
    }
    if (clique_covering_lower != clique_covering_upper)
    {
        ui->label_cliquecover->setText(QString::number(clique_covering_lower) + "-" + QString::number(clique_covering_upper));
    }
    else
    {
        ui->label_cliquecover->setText(QString::number(clique_covering_lower));
    }
    if (dominating_set_lower != dominating_set_upper)
    {
        ui->label_domset->setText(QString::number(dominating_set_lower) + "-" + QString::number(dominating_set_upper));
    }
    else
    {
        ui->label_domset->setText(QString::number(dominating_set_lower));
    }
    if (show_longest_cycle)
    {
        if (0 != longest_cycle_lower)
        {
            if (longest_cycle_lower != longest_cycle_upper)
            {
                ui->label_longest_cycle->setText(QString::number(longest_cycle_lower) + "-" + QString::number(longest_cycle_upper));
            }
            else
            {
                ui->label_longest_cycle->setText(QString::number(longest_cycle_lower));
            }
        }
        else
        {
            ui->label_longest_cycle->setText(QString("cycle-free"));
        }
    }
    else
    {
        ui->label_longest_cycle->setText(QString("?"));
    }
}

void MainWindow::update_adjacency_matrix()
{
    QPixmap *pixmap = new QPixmap(G->n,G->n);
    QPainter *painter;
    painter = new QPainter(pixmap);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillRect(QRect(0,0,G->n,G->n),QColor(255,255,255,255));
    painter->setBrush(QColor(0,0,128));
    painter->setPen(QColor(0,0,0,128));

    refer v, i, j;
    refer *dominating_set_clustering_backward_mapping;

    // without clustering
    if (NULL == dominating_set_clustering)
    {
        for (v=0;v<G->n;v++)
        {
            for (j=0;j<G->V[v].edgecount;j++)
            {
                painter->drawPoint(v,G->V[v].sibl[j]);
                painter->drawPoint(G->V[v].sibl[j],v);
            }
        }
    }
    // with clustering
    else
    {
        dominating_set_clustering_backward_mapping = new refer[G->n+1];

        for (i=0;i<G->n;i++)
        {
            v = dominating_set_clustering[i];
            dominating_set_clustering_backward_mapping[v] = i;
        }

        for (i=0;i<G->n;i++)
        {
            v = dominating_set_clustering[i];
            for (j=0;j<G->V[v].edgecount;j++)
            {
                painter->drawPoint(i,dominating_set_clustering_backward_mapping[G->V[v].sibl[j]]);
                painter->drawPoint(dominating_set_clustering_backward_mapping[G->V[v].sibl[j]],i);
            }
        }

        delete[](dominating_set_clustering_backward_mapping);
    }

    painter->end();

    pixmap_adjacency_matrix = pixmap;
    adjacency_matrix_visible = true;
}

void MainWindow::clear_adjacency_matrix()
{
    QPixmap *pixmap = new QPixmap(300,300);
    QPainter *painter;
    painter = new QPainter(pixmap);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillRect(QRect(0,0,300,300),QColor(230,230,230,255));
    painter->end();

    pixmap_adjacency_matrix = pixmap;
    adjacency_matrix_visible = false;
}

void MainWindow::update_degree_distribution()
{
    if (NULL != degree_distrib)
    {
        delete[](degree_distrib);
    }
    degree_distrib = new refer[G->n];
    statistics::degree_distribution(G, degree_distrib);

    QPixmap *pixmap = new QPixmap(degree_distrib_size,degree_distrib_size);
    QPainter *painter;
    painter = new QPainter(pixmap);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillRect(QRect(0,0,degree_distrib_size,degree_distrib_size),QColor(255,255,255,255));
    painter->setBrush(QColor(0,0,0,64));
    painter->setPen(QColor(0,0,0,64));

    // here we will plot the degree distribution points
    refer i,x,y;
    for (i=1;i<=max_deg;i++)
    {
        if (0 < degree_distrib[i])
        {
            x = 5 + (refer) ((double) (degree_distrib_size - 10.0) * log10((double) (i)) / log10((double) max_deg));
            y = 5 + (refer) ((double) (degree_distrib_size - 10.0) - (double) (degree_distrib_size - 10.0)  * log10((double) (degree_distrib[i])) / log10((double) G->n));
            painter->drawEllipse(x-2,y-2,4,4);
        }
    }
    painter->end();

    pixmap_degree_distribution = pixmap;
    degree_distribution_visible = true;
}

void MainWindow::clear_degree_distribution()
{
    QPixmap *pixmap = new QPixmap(300,300);
    QPainter *painter;
    painter = new QPainter(pixmap);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillRect(QRect(0,0,300,300),QColor(230,230,230,255));
    painter->end();

    pixmap_degree_distribution = pixmap;
    degree_distribution_visible = false;
}

void MainWindow::update_graph_visualization()
{
    refer v, w, c;
    refer current_visualization_size;
    refer *visualization_colors_red = NULL;
    refer *visualization_colors_green = NULL;
    refer *visualization_colors_blue = NULL;
    random_generator generator;
    refer *vertex_permutation;
    refer *possibilities;
    refer possibilities_count;

    srand((unsigned long long) time(NULL));

    if (ui->action1x->isChecked())
    {
        current_visualization_size = VISUALIZATION_SIZE;
    }
    else if (ui->action2x->isChecked())
    {
        current_visualization_size = VISUALIZATION_SIZE_DOUBLE;
    }
    else if (ui->action5x->isChecked())
    {
        current_visualization_size = VISUALIZATION_SIZE_MEDIUM;
    }
    else
    {
        current_visualization_size = VISUALIZATION_SIZE_LARGE;
    }
    QPixmap *pixmap = new QPixmap(current_visualization_size,current_visualization_size);
    QPainter *painter;
    painter = new QPainter(pixmap);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillRect(QRect(0,0,current_visualization_size,current_visualization_size),QColor(255,255,255,255));
    painter->setPen(QColor(0,0,0,255));

    if (NULL != myx)
    {
        delete[](myx);
    }
    if (NULL != myy)
    {
        delete[](myy);
    }
    myx = new refer[G->n];
    myy = new refer[G->n];

    if (ui->actionCrossings_based->isChecked() && CommonSettings::show_evolved_graph_visualization)
    {
        refer j;
        int shade;
        for (v=0;v<G->n;v++)
        {
            myx[v] = CommonSettings::myx[v] * current_visualization_size / VISUALIZATION_SIZE;
            myy[v] = CommonSettings::myy[v] * current_visualization_size / VISUALIZATION_SIZE;
        }

        for (v=0;v<G->n;v++)
        {
            for (j=0;j<G->V[v].edgecount;j++)
            {
                shade = generator.random(50,150);
                painter->setPen(QPen(QColor(shade,shade,shade,255), 1, Qt::SolidLine));
                painter->drawLine(myx[v],myy[v],myx[G->V[v].sibl[j]],myy[G->V[v].sibl[j]]);
            }
        }
        if (NULL != current_longest_cycle_vertices && CommonSettings::highlight_longest_cycle)
        {
            for (j=0;j<longest_cycle_lower;j++)
            {
                v = current_longest_cycle_vertices[j];
                if (j < longest_cycle_lower-1)
                {
                    w = current_longest_cycle_vertices[j+1];
                }
                else
                {
                    w = current_longest_cycle_vertices[0];
                }
                painter->setPen(QPen(QColor(50,50,50,255), 5, Qt::SolidLine));
                painter->drawLine(myx[v],myy[v],myx[w],myy[w]);
            }
        }
    }
    else if (ui->actionMesh_based->isChecked())
    {
        // grid-based visualization
        refer j;
        int shade;
        refer row_count, column_count, drawing_step, current_row, current_column;
        row_count = (refer) sqrt((double) G->n);
        column_count = (refer) ceil((double) G->n / (double) row_count);
        drawing_step = (current_visualization_size - current_visualization_size / 10) / column_count;
        myx[0] = current_visualization_size / 20;
        myy[0] = current_visualization_size / 20;
        current_row = 0;
        current_column = 0;
        for (v=1;v<G->n;v++)
        {
            myx[v] = myx[v-1] + drawing_step;
            myy[v] = myy[v-1];
            if (current_column >= column_count-1 || myx[v] > current_visualization_size - current_visualization_size / 20)
            {
                current_column = 0;
                current_row++;
                myx[v] = myx[0];
                myy[v] = myy[v - row_count] + drawing_step;
            }
            else
            {
                current_column++;
            }
        }
        for (v=0;v<G->n;v++)
        {
            for (j=0;j<G->V[v].edgecount;j++)
            {
                shade = generator.random(50,150);
                painter->setPen(QPen(QColor(shade,shade,shade,255), 1, Qt::SolidLine));
                painter->drawLine(myx[v],myy[v],myx[G->V[v].sibl[j]],myy[G->V[v].sibl[j]]);
            }
        }
        if (NULL != current_longest_cycle_vertices && CommonSettings::highlight_longest_cycle)
        {
            for (j=0;j<longest_cycle_lower;j++)
            {
                v = current_longest_cycle_vertices[j];
                if (j < longest_cycle_lower-1)
                {
                    w = current_longest_cycle_vertices[j+1];
                }
                else
                {
                    w = current_longest_cycle_vertices[0];
                }
                painter->setPen(QPen(QColor(50,50,50,255), 5, Qt::SolidLine));
                painter->drawLine(myx[v],myy[v],myx[w],myy[w]);
            }
        }
    }
    // ToDo: this is just a workaround for crossings-based without crossing minimization result available
    else if (ui->actionCentrality_based->isChecked() || ui->actionCrossings_based->isChecked() && ! CommonSettings::show_evolved_graph_visualization)
    {
        // centrality-based visualization
        distances = new refer[G->n];
        queue = new refer[G->n];
        vertex_permutation = new refer[G->n];

        refer v, i, j, v_central = 0, queue_size, queue_current, current_distance, max_distance = 0;

        possibilities = new refer[G->n];
        for (v=0;v<G->n;v++)
        {
            possibilities[v] = v;
        }
        possibilities_count = G->n;
        for (v=0;v<G->n;v++)
        {
            j = generator.random(0,possibilities_count-1);
            vertex_permutation[v] = possibilities[j];
            possibilities[j] = possibilities[possibilities_count-1];
            possibilities_count--;
        }
        delete[](possibilities);

        // central vertex with max degree
        bool large_component = true;
        for (v=0;v<G->n;v++)
        {
            distances[v] = 0;
            if (G->V[v].edgecount == max_deg)
            {
                v_central = v;
            }
        }
        myx[v_central] = (current_visualization_size) / 2;
        myy[v_central] = (current_visualization_size) / 2;

        // now breadth-first search to find positions for other vertices
        queue_current = 0;
        queue_size = 1;
        queue[0] = v_central;
        distances[v_central] = 0;
        while (queue_size)
        {
            v = queue[queue_current];
            current_distance = distances[v];
            queue_current++;
            queue_size--;
            for (j=0;j<G->V[v].edgecount;j++)
            {
                if (0 == distances[G->V[v].sibl[j]] && G->V[v].sibl[j] != v_central)
                {
                    queue[queue_current+queue_size] = G->V[v].sibl[j];
                    distances[G->V[v].sibl[j]] = current_distance + 1;
                    if (max_distance < distances[G->V[v].sibl[j]])
                    {
                        max_distance = distances[G->V[v].sibl[j]];
                    }
                    queue_size++;
                }
            }
        }

        // vertices outside the main component
        refer current_count, remaining;
        double current_radius;
        j = 0;
        current_count = 0;
        for (v=0;v<G->n;v++)
        {
            if (distances[v] == j)
            {
                current_count++;
            }
        }
        if (current_count > 1)
        {
            large_component = false;
            current_radius = (double) (current_visualization_size- VISUALIZATION_MARGIN) / 2;
            remaining = current_count-1;
            for (v=0;v<G->n;v++)
            {
                if (distances[v] == 0 && v != v_central)
                {
                    myx[v] = current_visualization_size / 2 + (refer) (current_radius * cos((double) remaining * 2.0 * 3.141592653 / (double) current_count));
                    myy[v] = current_visualization_size / 2 + (refer) (current_radius * sin((double) remaining * 2.0 * 3.141592653 / (double) current_count));
                    remaining--;
                }
            }
        }

        // vertices inside the main component
        for (j=1;j<=max_distance;j++)
        {
            current_count = 0;
            for (v=0;v<G->n;v++)
            {
                if (distances[v] == j)
                {
                    current_count++;
                }
            }
            if (large_component)
            {
                current_radius = (double) (j * (current_visualization_size- VISUALIZATION_MARGIN) / 2) / (double) (max_distance);
            }
            else
            {
                current_radius = (double) (j * (current_visualization_size- VISUALIZATION_MARGIN) / 2) / (double) (max_distance+1);
            }
            remaining = current_count - 1;
            // iterating over the vertices in a randomized order (to randomize order of vertices in a particular layer)
            for (i=0;i<G->n;i++)
            {
                v = vertex_permutation[i];
                if (distances[v] == j)
                {
                    myx[v] = (current_visualization_size) / 2 + (refer) (current_radius * cos((double) remaining * 2.0 * 3.141592653 / (double) current_count));
                    myy[v] = (current_visualization_size) / 2 + (refer) (current_radius * sin((double) remaining * 2.0 * 3.141592653 / (double) current_count));
                    remaining--;
                }
            }
        }

        int shade;
        random_generator generator;
        for (v=0;v<G->n;v++)
        {
            for (j=0;j<G->V[v].edgecount;j++)
            {
                shade = generator.random(50,150);
                painter->setPen(QPen(QColor(shade,shade,shade,255), 1, Qt::SolidLine));
                painter->drawLine(myx[v],myy[v],myx[G->V[v].sibl[j]],myy[G->V[v].sibl[j]]);
            }
        }
        if (NULL != current_longest_cycle_vertices && CommonSettings::highlight_longest_cycle)
        {
            for (j=0;j<longest_cycle_lower;j++)
            {
                v = current_longest_cycle_vertices[j];
                if (j < longest_cycle_lower-1)
                {
                    w = current_longest_cycle_vertices[j+1];
                }
                else
                {
                    w = current_longest_cycle_vertices[0];
                }
                painter->setPen(QPen(QColor(50,50,50,255), 5, Qt::SolidLine));
                painter->drawLine(myx[v],myy[v],myx[w],myy[w]);
            }
        }
        delete[](distances);
        delete[](queue);
        delete[](vertex_permutation);
    }
    else if (ui->actionTree_based->isChecked())
    {
        // tree-based visualization
        distances = new refer[G->n];
        refer *distance_copies = new refer[G->n];
        queue = new refer[G->n];
        vertex_permutation = new refer[G->n];

        refer v, w, j, v_central = 0, queue_size, queue_current, current_distance, max_distance = 0;

        possibilities = new refer[G->n];
        for (v=0;v<G->n;v++)
        {
            possibilities[v] = v;
        }
        possibilities_count = G->n;
        for (v=0;v<G->n;v++)
        {
            j = generator.random(0,possibilities_count-1);
            vertex_permutation[v] = possibilities[j];
            possibilities[j] = possibilities[possibilities_count-1];
            possibilities_count--;
        }
        delete[](possibilities);

        // central vertex with max degree
        for (v=0;v<G->n;v++)
        {
            distances[v] = 0;
            if (G->V[v].edgecount == max_deg)
            {
                v_central = v;
            }
        }

        // now breadth-first search to compute the max distance
        queue_current = 0;
        queue_size = 1;
        queue[0] = v_central;
        distances[v_central] = 0;
        while (queue_size)
        {
            v = queue[queue_current];
            current_distance = distances[v];
            queue_current++;
            queue_size--;
            for (j=0;j<G->V[v].edgecount;j++)
            {
                if (0 == distances[G->V[v].sibl[j]] && G->V[v].sibl[j] != v_central)
                {
                    queue[queue_current+queue_size] = G->V[v].sibl[j];
                    distances[G->V[v].sibl[j]] = current_distance + 1;
                    if (max_distance < distances[G->V[v].sibl[j]])
                    {
                        max_distance = distances[G->V[v].sibl[j]];
                    }
                    queue_size++;
                }
            }
        }

        myx[v_central] = current_visualization_size / 2;
        myy[v_central] = current_visualization_size / (max_distance + 1) / 2;

        // vertices inside the main component
        queue_current = 0;
        queue_size = 1;
        queue[0] = v_central;
        distance_copies[v_central] = 0;
        refer current_count, current_vertex = 0;

        for (v=0;v<G->n;v++)
        {
            distance_copies[v] = 0;
        }
        while (queue_size)
        {
            v = queue[queue_current];
            if (current_distance != distance_copies[v])
            {
                current_vertex = 0;
            }
            current_distance = distance_copies[v];
            queue_current++;
            queue_size--;
            current_count = 0;
            for (w=0;w<G->n;w++)
            {
                if (current_distance + 1 == distances[w])
                {
                    current_count++;
                }
            }
            for (j=0;j<G->V[v].edgecount;j++)
            {
                if (0 == distance_copies[G->V[v].sibl[j]] && G->V[v].sibl[j] != v_central)
                {
                    queue[queue_current+queue_size] = G->V[v].sibl[j];
                    // putting the vertex in the right place
                    distance_copies[G->V[v].sibl[j]] = current_distance + 1;
                    myx[G->V[v].sibl[j]] = current_vertex * current_visualization_size / (current_count) + current_visualization_size / (current_count) / 2;
                    myy[G->V[v].sibl[j]] = distance_copies[G->V[v].sibl[j]] * current_visualization_size / (max_distance + 1) + current_visualization_size / (max_distance + 1) / 2;
                    queue_size++;
                    current_vertex++;
                }
            }
        }

        // vertices outside the main component
        refer remaining;
        double current_radius;
        j = 0;
        current_count = 0;
        for (v=0;v<G->n;v++)
        {
            if (distances[v] == j)
            {
                current_count++;
            }
        }
        if (current_count > 1)
        {
            current_radius = (double) (current_visualization_size- VISUALIZATION_MARGIN) / 2;
            remaining = current_count-1;
            for (v=0;v<G->n;v++)
            {
                if (distances[v] == 0 && v != v_central)
                {
                    myx[v] = current_visualization_size / 2 + (refer) (current_radius * cos((double) remaining * 2.0 * 3.141592653 / (double) current_count));
                    myy[v] = current_visualization_size / 2 + (refer) (current_radius * sin((double) remaining * 2.0 * 3.141592653 / (double) current_count));
                    remaining--;
                }
            }
        }

        int shade;
        random_generator generator;
        for (v=0;v<G->n;v++)
        {
            for (j=0;j<G->V[v].edgecount;j++)
            {
                shade = generator.random(50,150);
                painter->setPen(QPen(QColor(shade,shade,shade,255), 1, Qt::SolidLine));
                painter->drawLine(myx[v],myy[v],myx[G->V[v].sibl[j]],myy[G->V[v].sibl[j]]);
            }
        }
        if (NULL != current_longest_cycle_vertices && CommonSettings::highlight_longest_cycle)
        {
            for (j=0;j<longest_cycle_lower;j++)
            {
                v = current_longest_cycle_vertices[j];
                if (j < longest_cycle_lower-1)
                {
                    w = current_longest_cycle_vertices[j+1];
                }
                else
                {
                    w = current_longest_cycle_vertices[0];
                }
                painter->setPen(QPen(QColor(50,50,50,255), 5, Qt::SolidLine));
                painter->drawLine(myx[v],myy[v],myx[w],myy[w]);
            }
        }
        delete[](distances);
        delete[](distance_copies);
        delete[](queue);
        delete[](vertex_permutation);
    }
    else if (ui->actionCircular->isChecked())
    {
        // circular visualization
        refer v, j;

        refer current_count = G->n, remaining = G->n;
        double current_radius = (current_visualization_size - VISUALIZATION_MARGIN) / 2;
        for (v=0;v<G->n;v++)
        {
            myx[v] = current_visualization_size / 2 + (refer) (current_radius * cos((double) remaining * 2.0 * 3.141592653 / (double) current_count));
            myy[v] = current_visualization_size / 2 + (refer) (current_radius * sin((double) remaining * 2.0 * 3.141592653 / (double) current_count));
            remaining--;
        }

        int shade;
        random_generator generator;
        for (v=0;v<G->n;v++)
        {
            for (j=0;j<G->V[v].edgecount;j++)
            {
                shade = generator.random(50,150);
                painter->setPen(QPen(QColor(shade,shade,shade,255), 1, Qt::SolidLine));
                painter->drawLine(myx[v],myy[v],myx[G->V[v].sibl[j]],myy[G->V[v].sibl[j]]);
            }
        }
        if (NULL != current_longest_cycle_vertices && CommonSettings::highlight_longest_cycle)
        {
            for (j=0;j<longest_cycle_lower;j++)
            {
                v = current_longest_cycle_vertices[j];
                if (j < longest_cycle_lower-1)
                {
                    w = current_longest_cycle_vertices[j+1];
                }
                else
                {
                    w = current_longest_cycle_vertices[0];
                }
                painter->setPen(QPen(QColor(50,50,50,255), 5, Qt::SolidLine));
                painter->drawLine(myx[v],myy[v],myx[w],myy[w]);
            }
        }
    }
    else if (ui->actionCycle_based->isChecked())
    {
        // cycle-based visualization
        bool *finished_vertices = new bool[G->n];

        refer v, j;

        for (v=0;v<G->n;v++)
        {
            finished_vertices[v] = false;
        }

        // the long cycle
        refer current_count = longest_cycle_lower, remaining = longest_cycle_lower;
        double current_radius;
        for (j=0;j<longest_cycle_lower;j++)
        {
            v = current_longest_cycle_vertices[j];
            finished_vertices[v] = true;
        }

        // the other vertices in a centrality-based visualization
        distances = new refer[G->n];
        queue = new refer[G->n];
        vertex_permutation = new refer[G->n];

        refer i, v_central = 0, queue_size, queue_current, current_distance, max_distance = 0;

        possibilities = new refer[G->n];
        possibilities_count = 0;
        for (v=0;v<G->n;v++)
        {
            if (! finished_vertices[v])
            {
                possibilities[possibilities_count] = v;
                possibilities_count++;
            }
        }

        refer non_cycle_vertices = possibilities_count;
        v = 0;
        while (0 < possibilities_count)
        {
            j = generator.random(0,possibilities_count-1);
            vertex_permutation[v] = possibilities[j];
            possibilities[j] = possibilities[possibilities_count-1];
            possibilities_count--;
            v++;
        }
        delete[](possibilities);

        // central vertex with max degree
        bool large_component = true;
        bool v_central_found = false;
        refer current_max_deg = 0;
        for (v=0;v<G->n;v++)
        {
            distances[v] = 0;
            if (! finished_vertices[v] && G->V[v].edgecount >= current_max_deg)
            {
                v_central = v;
                v_central_found = true;
                current_max_deg = G->V[v].edgecount;
            }
        }

        if (v_central_found || longest_cycle_lower == G->n)
        {
            if (longest_cycle_lower != G->n)
            {
                myx[v_central] = (current_visualization_size) / 2;
                myy[v_central] = (current_visualization_size) / 2;

                // now breadth-first search to find positions for other vertices
                queue_current = 0;
                queue_size = 1;
                queue[0] = v_central;
                distances[v_central] = 0;
                while (queue_size)
                {
                    v = queue[queue_current];
                    current_distance = distances[v];
                    queue_current++;
                    queue_size--;
                    for (j=0;j<G->V[v].edgecount;j++)
                    {
                        if (0 == distances[G->V[v].sibl[j]] && G->V[v].sibl[j] != v_central)
                        {
                            queue[queue_current+queue_size] = G->V[v].sibl[j];
                            distances[G->V[v].sibl[j]] = current_distance + 1;
                            if (max_distance < distances[G->V[v].sibl[j]])
                            {
                                max_distance = distances[G->V[v].sibl[j]];
                            }
                            queue_size++;
                        }
                    }
                }

                // vertices outside of the main component
                j = 0;
                current_count = 0;
                for (v=0;v<G->n;v++)
                {
                    if (! finished_vertices[v] && distances[v] == j)
                    {
                        current_count++;
                    }
                }
                if (current_count > 1)
                {
                    large_component = false;
                    current_radius = (current_visualization_size - VISUALIZATION_MARGIN) / 2;
                    remaining = current_count-1;
                    for (v=0;v<G->n;v++)
                    {
                        if (! finished_vertices[v] && distances[v] == 0 && v != v_central)
                        {
                            myx[v] = current_visualization_size / 2 + (refer) (current_radius * cos((double) remaining * 2.0 * 3.141592653 / (double) current_count));
                            myy[v] = current_visualization_size / 2 + (refer) (current_radius * sin((double) remaining * 2.0 * 3.141592653 / (double) current_count));
                            remaining--;
                        }
                    }
                }
            }

            // the long cycle
            refer current_count = longest_cycle_lower, remaining = longest_cycle_lower;
            if (large_component)
            {
                current_radius = (current_visualization_size - VISUALIZATION_MARGIN) / 2;
            }
            else
            {
                current_radius = (double) ((max_distance+1) * (current_visualization_size- VISUALIZATION_MARGIN) / 2) / (double) (max_distance+2);
            }
            for (j=0;j<longest_cycle_lower;j++)
            {
                v = current_longest_cycle_vertices[j];
                myx[v] = current_visualization_size / 2 + (refer) (current_radius * cos((double) remaining * 2.0 * 3.141592653 / (double) current_count));
                myy[v] = current_visualization_size / 2 + (refer) (current_radius * sin((double) remaining * 2.0 * 3.141592653 / (double) current_count));
                remaining--;
            }

            if (longest_cycle_lower != G->n)
            {
                // vertices inside the main component
                for (j=1;j<=max_distance;j++)
                {
                    current_count = 0;
                    for (v=0;v<G->n;v++)
                    {
                        if (! finished_vertices[v] && distances[v] == j)
                        {
                            current_count++;
                        }
                    }
                    if (large_component)
                    {
                        current_radius = (double) (j * (current_visualization_size- VISUALIZATION_MARGIN) / 2) / (double) (max_distance+1);
                    }
                    else
                    {
                        current_radius = (double) (j * (current_visualization_size- VISUALIZATION_MARGIN) / 2) / (double) (max_distance+2);
                    }
                    remaining = current_count - 1;
                    // iterating over the vertices in a randomized order (to randomize order of vertices in a particular layer)
                    for (i=0;i<non_cycle_vertices;i++)
                    {
                        v = vertex_permutation[i];
                        if (! finished_vertices[v] && distances[v] == j)
                        {
                            myx[v] = (current_visualization_size) / 2 + (refer) (current_radius * cos((double) remaining * 2.0 * 3.141592653 / (double) current_count));
                            myy[v] = (current_visualization_size) / 2 + (refer) (current_radius * sin((double) remaining * 2.0 * 3.141592653 / (double) current_count));
                            remaining--;
                        }
                    }
                }
            }
        }
        delete[](queue);
        delete[](vertex_permutation);

        int shade = 0;
        random_generator generator;
        for (v=0;v<G->n;v++)
        {
            for (j=0;j<G->V[v].edgecount;j++)
            {
                shade = generator.random(50,150);
                painter->setPen(QPen(QColor(shade,shade,shade,255), 1, Qt::SolidLine));
                painter->drawLine(myx[v],myy[v],myx[G->V[v].sibl[j]],myy[G->V[v].sibl[j]]);
            }
        }
        if (NULL != current_longest_cycle_vertices && CommonSettings::highlight_longest_cycle)
        {
            for (j=0;j<longest_cycle_lower;j++)
            {
                v = current_longest_cycle_vertices[j];
                if (j < longest_cycle_lower-1)
                {
                    w = current_longest_cycle_vertices[j+1];
                }
                else
                {
                    w = current_longest_cycle_vertices[0];
                }
                if (are_adjacent(v,w))
                {
                    painter->setPen(QPen(QColor(50,50,50,255), 5, Qt::SolidLine));
                    painter->drawLine(myx[v],myy[v],myx[w],myy[w]);
                }
            }
        }

        delete[](finished_vertices);
    }

    if (NULL != current_coloring && CommonSettings::highlight_coloring)
    {
        visualization_colors_red = new refer[chromatic_upper+2];
        visualization_colors_green = new refer[chromatic_upper+2];
        visualization_colors_blue = new refer[chromatic_upper+2];
        for (c=1;c<=chromatic_upper;c++)
        {
            if (1 == c)
            {
                visualization_colors_red[c] = 255;
                visualization_colors_green[c] = 0;
                visualization_colors_blue[c] = 0;
                continue;
            }
            if (2 == c)
            {
                visualization_colors_red[c] = 0;
                visualization_colors_green[c] = 0;
                visualization_colors_blue[c] = 255;
                continue;
            }
            if (3 == c)
            {
                visualization_colors_red[c] = 0;
                visualization_colors_green[c] = 255;
                visualization_colors_blue[c] = 0;
                continue;
            }
            visualization_colors_red[c] = generator.random(0,128);
            visualization_colors_green[c] = generator.random(0,128);
            visualization_colors_blue[c] = generator.random(0,128);
            if (c % 6 == 1)
            {
                visualization_colors_red[c] = 255;
            }
            else if (c % 6 == 2)
            {
                visualization_colors_blue[c] = 255;
            }
            else if (c % 6 == 3)
            {
                visualization_colors_green[c] = 255;
            }
            else if (c % 6 == 4)
            {
                visualization_colors_red[c] = 255;
                visualization_colors_green[c] = 255;
            }
            else if (c % 6 == 5)
            {
                visualization_colors_green[c] = 255;
                visualization_colors_blue[c] = 255;
            }
            else
            {
                visualization_colors_red[c] = 255;
                visualization_colors_blue[c] = 255;
            }
        }
    }
    for (v=0;v<G->n;v++)
    {
        painter->setPen(QPen(QColor(50,50,50,255), 1, Qt::SolidLine));
        if (NULL == current_coloring || ! CommonSettings::highlight_coloring)
        {
            painter->setBrush(QColor(128,0,0,255));
        }
        else
        {
            painter->setBrush(QColor(visualization_colors_red[current_coloring[v]],
                                     visualization_colors_green[current_coloring[v]],
                                     visualization_colors_blue[current_coloring[v]],
                                     255));
        }
        if (ui->action1x->isChecked())
        {
            painter->drawEllipse(myx[v]-CommonSettings::vertex_radius,myy[v]-CommonSettings::vertex_radius,CommonSettings::vertex_radius*2,CommonSettings::vertex_radius*2);
        }
        else if (ui->action2x->isChecked())
        {
            painter->drawEllipse(myx[v]-CommonSettings::vertex_radius * VISUALIZATION_DOUBLE_SCALE,myy[v]-CommonSettings::vertex_radius * VISUALIZATION_DOUBLE_SCALE,CommonSettings::vertex_radius * VISUALIZATION_DOUBLE_SCALE*2,CommonSettings::vertex_radius * VISUALIZATION_DOUBLE_SCALE*2);
        }
        else if (ui->action5x->isChecked())
        {
            painter->drawEllipse(myx[v]-CommonSettings::vertex_radius * VISUALIZATION_MEDIUM_SCALE,myy[v]-CommonSettings::vertex_radius * VISUALIZATION_MEDIUM_SCALE,CommonSettings::vertex_radius * VISUALIZATION_MEDIUM_SCALE*2,CommonSettings::vertex_radius * VISUALIZATION_MEDIUM_SCALE*2);
        }
        else
        {
            painter->drawEllipse(myx[v]-CommonSettings::vertex_radius * VISUALIZATION_LARGE_SCALE,myy[v]-CommonSettings::vertex_radius * VISUALIZATION_LARGE_SCALE,CommonSettings::vertex_radius * VISUALIZATION_LARGE_SCALE*2,CommonSettings::vertex_radius * VISUALIZATION_LARGE_SCALE*2);
        }
    }

    QFont qFont = painter->font();
    if (ui->action1x->isChecked())
    {
        qFont.setPointSize(CommonSettings::label_font_size);
    }
    else if (ui->action2x->isChecked())
    {
        qFont.setPointSize(CommonSettings::label_font_size * CommonSettings::vertex_radius * VISUALIZATION_DOUBLE_SCALE / CommonSettings::vertex_radius);
    }
    else if (ui->action5x->isChecked())
    {
        qFont.setPointSize(CommonSettings::label_font_size * CommonSettings::vertex_radius * VISUALIZATION_MEDIUM_SCALE / CommonSettings::vertex_radius);
    }
    else
    {
        qFont.setPointSize(CommonSettings::label_font_size * CommonSettings::vertex_radius * VISUALIZATION_LARGE_SCALE / CommonSettings::vertex_radius);
    }
    painter->setFont(qFont);
    for (v=0;v<G->n;v++)
    {
        painter->setPen(QColor(50,50,50,255));
        if (NULL != get_vertex_label(v))
        {
            if (G->n < 1000)
            {
                painter->drawText(QPoint(myx[v] - CommonSettings::vertex_radius * 2,myy[v] - CommonSettings::vertex_radius * 2),get_vertex_label(v));
            }
        }
        else
        {
            if (G->n < 1000)
            {
                painter->drawText(QPoint(myx[v] - CommonSettings::vertex_radius * 2,myy[v] - CommonSettings::vertex_radius * 2),QString::number(v+1));
            }
        }
    }

    if (NULL != current_coloring && CommonSettings::highlight_coloring)
    {
        delete[](visualization_colors_red);
        delete[](visualization_colors_green);
        delete[](visualization_colors_blue);
    }
    painter->end();

    pixmap_graph_visualization = pixmap;
    graph_visualization_visible = true;
}

void MainWindow::clear_graph_visualization()
{
    QPixmap *pixmap = new QPixmap(VISUALIZATION_SIZE,VISUALIZATION_SIZE);
    QPainter *painter;
    painter = new QPainter(pixmap);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillRect(QRect(0,0,VISUALIZATION_SIZE,VISUALIZATION_SIZE),QColor(230,230,230,255));
    painter->end();

    pixmap_graph_visualization = pixmap;
    graph_visualization_visible = false;
}

void MainWindow::finishGraphLoading()
{
    if (ui->actionCycle_based->isChecked())
    {
        ui->actionCentrality_based->setChecked(true);
        ui->actionCycle_based->setChecked(false);
    }
    if (ui->actionCrossings_based->isChecked())
    {
        ui->actionCentrality_based->setChecked(true);
        ui->actionCrossings_based->setChecked(false);
    }
    ui->actionCycle_based->setEnabled(false);
    ui->actionCrossings_based->setEnabled(false);

    // adjacency matrix
    QGraphicsPixmapItem *pixmapItemAM = new QGraphicsPixmapItem();
    if (NULL != pixmap_adjacency_matrix)
    {
        pixmapItemAM->setPixmap(*pixmap_adjacency_matrix);
    }
    QGraphicsScene *scene_adjacency_matrix;
    if (adjacency_matrix_visible)
    {
        scene_adjacency_matrix = new QGraphicsScene(0,0,G->n,G->n);
    }
    else
    {
        scene_adjacency_matrix = new QGraphicsScene(0,0,300,300);
    }
    scene_adjacency_matrix->addItem(pixmapItemAM);
    ui->graphicsView_adjacencymatrix->setScene(scene_adjacency_matrix);
    ui->graphicsView_adjacencymatrix->fitInView(scene_adjacency_matrix->itemsBoundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView_adjacencymatrix->repaint();

    // degree distribution
    QGraphicsPixmapItem *pixmapItemDD = new QGraphicsPixmapItem();
    pixmapItemDD->setPixmap(*pixmap_degree_distribution);
    QGraphicsScene *scene_degree_distribution = new QGraphicsScene(0,0,degree_distrib_size,degree_distrib_size);
    scene_degree_distribution->addItem(pixmapItemDD);
    ui->graphicsView_degreedistr->setScene(scene_degree_distribution);
    ui->graphicsView_degreedistr->fitInView(scene_degree_distribution->itemsBoundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView_degreedistr->repaint();

    refer current_visualization_size;
    if (ui->action1x->isChecked())
    {
        current_visualization_size = VISUALIZATION_SIZE;
    }
    else if (ui->action2x->isChecked())
    {
        current_visualization_size = VISUALIZATION_SIZE_DOUBLE;
    }
    else if (ui->action5x->isChecked())
    {
        current_visualization_size = VISUALIZATION_SIZE_MEDIUM;
    }
    else
    {
        current_visualization_size = VISUALIZATION_SIZE_LARGE;
    }
    QGraphicsPixmapItem *pixmapItemGV = new QGraphicsPixmapItem();
    pixmapItemGV->setPixmap(*pixmap_graph_visualization);
    QGraphicsScene *scene_graph_visualization = new QGraphicsScene(0,0,current_visualization_size,current_visualization_size);
    scene_graph_visualization->addItem(pixmapItemGV);
    ui->graphicsView_graphdrawing->setScene(scene_graph_visualization);
    ui->graphicsView_graphdrawing->repaint();

    ui->progressBar->hide();
    ui->label_loading->hide();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    ui->actionSave->setEnabled(true);
    ui->actionComplementary_graph->setEnabled(true);
    ui->actionLargest_component->setEnabled(true);
    ui->actionGraph_with_pruned_leaves->setEnabled(true);
    ui->actionGeneralized_dominating_set_graph->setEnabled(true);

    QMessageBox::about(this,"Info","Data have been successfully loaded.");
}

void MainWindow::finishGraphSaving()
{
    ui->progressBar->hide();
    ui->label_loading->hide();
    ui->progressBar->repaint();
    ui->label_loading->repaint();
}
void MainWindow::finishVisualizationUpdate()
{
    refer current_visualization_size;
    if (ui->action1x->isChecked())
    {
        current_visualization_size = VISUALIZATION_SIZE;
    }
    else if (ui->action2x->isChecked())
    {
        current_visualization_size = VISUALIZATION_SIZE_DOUBLE;
    }
    else if (ui->action5x->isChecked())
    {
        current_visualization_size = VISUALIZATION_SIZE_MEDIUM;
    }
    else
    {
        current_visualization_size = VISUALIZATION_SIZE_LARGE;
    }
    QGraphicsPixmapItem *pixmapItemGV = new QGraphicsPixmapItem();
    pixmapItemGV->setPixmap(*pixmap_graph_visualization);
    QGraphicsScene *scene_graph_visualization = new QGraphicsScene(0,0,current_visualization_size,current_visualization_size);
    scene_graph_visualization->addItem(pixmapItemGV);
    ui->graphicsView_graphdrawing->setScene(scene_graph_visualization);
    ui->graphicsView_graphdrawing->repaint();

    ui->progressBar->hide();
    ui->label_loading->hide();
    ui->progressBar->repaint();
    ui->label_loading->repaint();
}

void MainWindow::finishTriangleCounting()
{
    ui->label_triangles->setText(QString::number(triangles));
    ui->label_triangles->repaint();

    ui->label_mean_clustering_coefficient->setText(QString::number(mean_clustering_coefficient));
    ui->label_mean_clustering_coefficient->repaint();

    if (0 < triangles && max_clique_lower < 3)
    {
        max_clique_lower = 3;
        chromatic_lower = max_clique_lower;
        update_statistics();
    }

    ui->progressBar->hide();
    ui->label_loading->hide();
    ui->progressBar->repaint();
    ui->label_loading->repaint();
}

void MainWindow::finishGirthComputing()
{
    if (G->n+1 != girth)
    {
        ui->label_girth->setText(QString::number(girth));
    }
    else
    {
        ui->label_girth->setText(QString("cycle-free"));
    }
    ui->label_girth->repaint();

    ui->progressBar->hide();
    ui->label_loading->hide();
    ui->progressBar->repaint();
    ui->label_loading->repaint();
}

void MainWindow::finishDiameterComputing()
{
    ui->label_diameter->setText(QString::number(diameter));
    ui->label_diameter->repaint();

    ui->progressBar->hide();
    ui->label_loading->hide();
    ui->progressBar->repaint();
    ui->label_loading->repaint();
}

void MainWindow::finishChromatic()
{
    update_statistics();

    // visualization update follows, progress bar will be hidden later
    QFuture<void> future = QtConcurrent::run(this, &MainWindow::updateVisualizationProcessing);
    this->FutureWatcherVisualizationUpdate.setFuture(future);
}

void MainWindow::finishIndependentSet()
{
    update_statistics();

    ui->progressBar->hide();
    ui->label_loading->hide();
    ui->progressBar->repaint();
    ui->label_loading->repaint();
}

void MainWindow::finishMaxClique()
{
    update_statistics();

    ui->progressBar->hide();
    ui->label_loading->hide();
    ui->progressBar->repaint();
    ui->label_loading->repaint();
}

void MainWindow::finishCliqueCovering()
{
    update_statistics();

    ui->progressBar->hide();
    ui->label_loading->hide();
    ui->progressBar->repaint();
    ui->label_loading->repaint();
}

void MainWindow::finishDominatingSet()
{
    update_statistics();
    if (G->n <= MAX_VERTICES_ADJACENCY && G->m <= MAX_EDGES_ADJACENCY)
    {
        update_adjacency_matrix();
    }
    else
    {
        clear_adjacency_matrix();
    }

    ui->progressBar->hide();
    ui->label_loading->hide();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    // adjacency matrix
    QGraphicsPixmapItem *pixmapItemAM = new QGraphicsPixmapItem();
    pixmapItemAM->setPixmap(*pixmap_adjacency_matrix);
    QGraphicsScene *scene_adjacency_matrix;
    if (adjacency_matrix_visible)
    {
        scene_adjacency_matrix = new QGraphicsScene(0,0,G->n,G->n);
    }
    else
    {
        scene_adjacency_matrix = new QGraphicsScene(0,0,300,300);
    }
    scene_adjacency_matrix->addItem(pixmapItemAM);
    ui->graphicsView_adjacencymatrix->setScene(scene_adjacency_matrix);
    ui->graphicsView_adjacencymatrix->fitInView(scene_adjacency_matrix->itemsBoundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView_adjacencymatrix->repaint();

}

void MainWindow::finishLongestCycle()
{
    if (0 != longest_cycle_lower)
    {
        update_statistics();
    }
    else
    {
        ui->label_longest_cycle->setText(QString("cycle-free"));
    }
    ui->label_longest_cycle->repaint();
    ui->actionCycle_based->setEnabled(true);

    // visualization update follows, progress bar will be hidden later
    QFuture<void> future = QtConcurrent::run(this, &MainWindow::updateVisualizationProcessing);
    this->FutureWatcherVisualizationUpdate.setFuture(future);
}

void MainWindow::finishWorkerThreadCliqueCovering()
{
    refer clique_covering_size, indset_size;

    workerThreadIGRLSCliqueCovering->getResults(&clique_covering_size, &indset_size);

    if (max_independent_set_lower < indset_size)
    {
        max_independent_set_lower = indset_size;
    }
    clique_covering_lower = max_independent_set_lower;

    if (clique_covering_upper > clique_covering_size)
    {
        clique_covering_upper = clique_covering_size;
    }
    max_independent_set_upper = clique_covering_upper;

    update_statistics();
}

void MainWindow::finishWorkerThreadColoring()
{
    refer coloring_size, clique_size;

    workerThreadIGRLSColoring->getResults(&coloring_size, &clique_size);

    if (max_clique_lower < clique_size)
    {
        max_clique_lower = clique_size;
    }
    chromatic_lower = max_clique_lower;

    if (chromatic_upper >= coloring_size)
    {
        chromatic_upper = coloring_size;
    }
    max_clique_upper = chromatic_upper;

    update_statistics();

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    // visualization update follows with a progress bar, it will be hidden later
    QFuture<void> future = QtConcurrent::run(this, &MainWindow::updateVisualizationProcessing);
    this->FutureWatcherVisualizationUpdate.setFuture(future);
}

void MainWindow::loadGraphProcessing()
{
    FILE *source;
    QString file_format;

    if (graphFileName.endsWith(".gml"))
    {
        file_format = "gml";
    }
    else
    {
        file_format = "col";
    }

    if ((source = fopen(graphFileName.toLatin1(),"r")) == NULL)
    {
        QMessageBox::about(this,"Error","An error occured during opening of the file.");
        return;
    }
    if (input_graph(source, file_format.toLatin1()) > 0)
    {
        QMessageBox::about(this,"Error","An error occured during opening of the file.");
        return;
    }
    fclose(source);

    this->G = get_graph();
    show_triangles = false;
    show_girth = false;
    show_diameter = false;
    show_longest_cycle = false;
    compute_statistics(this->G);
    update_statistics();
    if (G->n <= MAX_VERTICES_ADJACENCY && G->m <= MAX_EDGES_ADJACENCY)
    {
        update_adjacency_matrix();
    }
    else
    {
        clear_adjacency_matrix();
    }
    if (G->m <= MAX_EDGES_DRAWING)
    {
        update_graph_visualization();
    }
    else
    {
        clear_graph_visualization();
    }
    update_degree_distribution();
}

void MainWindow::generateGraphBAProcessing()
{
    generate_graph_BA_model(w, n_max);

    graphFileName = "-";

    this->G = get_graph();
    show_triangles = false;
    show_diameter = false;
    show_diameter = false;
    show_longest_cycle = false;
    compute_statistics(this->G);
    update_statistics();
    if (G->n <= MAX_VERTICES_ADJACENCY && G->m <= MAX_EDGES_ADJACENCY)
    {
        update_adjacency_matrix();
    }
    else
    {
        clear_adjacency_matrix();
    }
    if (G->m <= MAX_EDGES_DRAWING)
    {
        update_graph_visualization();
    }
    else
    {
        clear_graph_visualization();
    }
    update_degree_distribution();
}

void MainWindow::generateGraphUDGProcessing()
{
    generate_graph_UDG(n_max, range, grid);

    graphFileName = "-";

    this->G = get_graph();
    show_triangles = false;
    show_diameter = false;
    show_diameter = false;
    show_longest_cycle = false;
    compute_statistics(this->G);
    update_statistics();
    if (G->n <= MAX_VERTICES_ADJACENCY && G->m <= MAX_EDGES_ADJACENCY)
    {
        update_adjacency_matrix();
    }
    else
    {
        clear_adjacency_matrix();
    }
    if (G->m <= MAX_EDGES_DRAWING)
    {
        update_graph_visualization();
    }
    else
    {
        clear_graph_visualization();
    }
    update_degree_distribution();
}

void MainWindow::generateGraphCompleteTreeProcessing()
{
    generate_graph_complete_tree(branching_factor, depth);

    graphFileName = "-";

    this->G = get_graph();
    show_triangles = false;
    show_diameter = false;
    show_diameter = false;
    show_longest_cycle = false;
    compute_statistics(this->G);
    update_statistics();
    if (G->n <= MAX_VERTICES_ADJACENCY && G->m <= MAX_EDGES_ADJACENCY)
    {
        update_adjacency_matrix();
    }
    else
    {
        clear_adjacency_matrix();
    }
    if (G->m <= MAX_EDGES_DRAWING)
    {
        update_graph_visualization();
    }
    else
    {
        clear_graph_visualization();
    }
    update_degree_distribution();
}

void MainWindow::generateComplementaryGraphProcessing()
{
    generate_complementary_graph(G);

    this->G = get_graph();
    show_triangles = false;
    show_diameter = false;
    show_diameter = false;
    show_longest_cycle = false;
    compute_statistics(this->G);
    update_statistics();
    if (G->n <= MAX_VERTICES_ADJACENCY && G->m <= MAX_EDGES_ADJACENCY)
    {
        update_adjacency_matrix();
    }
    else
    {
        clear_adjacency_matrix();
    }
    if (G->m <= MAX_EDGES_DRAWING)
    {
        update_graph_visualization();
    }
    else
    {
        clear_graph_visualization();
    }
    update_degree_distribution();
}

void MainWindow::generateGraphPrunedLeavesProcessing()
{
    if (NULL == G)
    {
        return;
    }

    generate_graph_pruned_leaves(G);

    this->G = get_graph();
    show_triangles = false;
    show_diameter = false;
    show_diameter = false;
    show_longest_cycle = false;
    compute_statistics(this->G);
    update_statistics();
    if (G->n <= MAX_VERTICES_ADJACENCY && G->m <= MAX_EDGES_ADJACENCY)
    {
        update_adjacency_matrix();
    }
    else
    {
        clear_adjacency_matrix();
    }
    if (G->m <= MAX_EDGES_DRAWING)
    {
        update_graph_visualization();
    }
    else
    {
        clear_graph_visualization();
    }
    update_degree_distribution();
}

void MainWindow::generateLargestComponentProcessing()
{
    if (NULL == G)
    {
        return;
    }

    generate_largest_component(G);

    this->G = get_graph();
    show_triangles = false;
    show_diameter = false;
    show_diameter = false;
    show_longest_cycle = false;
    compute_statistics(this->G);
    update_statistics();
    if (G->n <= MAX_VERTICES_ADJACENCY && G->m <= MAX_EDGES_ADJACENCY)
    {
        update_adjacency_matrix();
    }
    else
    {
        clear_adjacency_matrix();
    }
    if (G->m <= MAX_EDGES_DRAWING)
    {
        update_graph_visualization();
    }
    else
    {
        clear_graph_visualization();
    }
    update_degree_distribution();
}

void MainWindow::generateGeneralizedDominatingSetGraphProcessing()
{
    generate_shortcut_graph(G, max_generalized_dominating_set_distance);

    this->G = get_graph();
    show_triangles = false;
    show_diameter = false;
    show_diameter = false;
    show_longest_cycle = false;
    compute_statistics(this->G);
    update_statistics();
    if (G->n <= MAX_VERTICES_ADJACENCY && G->m <= MAX_EDGES_ADJACENCY)
    {
        update_adjacency_matrix();
    }
    else
    {
        clear_adjacency_matrix();
    }
    if (G->m <= MAX_EDGES_DRAWING)
    {
        update_graph_visualization();
    }
    else
    {
        clear_graph_visualization();
    }
    update_degree_distribution();
}

void MainWindow::generateGraphWSProcessing()
{
    generate_graph_WS_model(n_max, lattice_branching_factor / 2, beta);

    graphFileName = "-";

    this->G = get_graph();
    show_triangles = false;
    show_diameter = false;
    show_diameter = false;
    show_longest_cycle = false;
    compute_statistics(this->G);
    update_statistics();
    if (G->n <= MAX_VERTICES_ADJACENCY && G->m <= MAX_EDGES_ADJACENCY)
    {
        update_adjacency_matrix();
    }
    else
    {
        clear_adjacency_matrix();
    }
    if (G->m <= MAX_EDGES_DRAWING)
    {
        update_graph_visualization();
    }
    else
    {
        clear_graph_visualization();
    }
    update_degree_distribution();
}

void MainWindow::generateGraphGridProcessing()
{
    generate_graph_grid(rows, columns, beta);

    graphFileName = "-";

    this->G = get_graph();
    show_triangles = false;
    show_diameter = false;
    show_diameter = false;
    show_longest_cycle = false;
    compute_statistics(this->G);
    update_statistics();
    if (G->n <= MAX_VERTICES_ADJACENCY && G->m <= MAX_EDGES_ADJACENCY)
    {
        update_adjacency_matrix();
    }
    else
    {
        clear_adjacency_matrix();
    }
    if (G->m <= MAX_EDGES_DRAWING)
    {
        update_graph_visualization();
    }
    else
    {
        clear_graph_visualization();
    }
    update_degree_distribution();
}


void MainWindow::saveGraphProcessing()
{
    FILE *target;

    if ((target = fopen(newGraphFileName.toLatin1(),"w")) == NULL)
    {
        QMessageBox::about(this,"Error","An error occured during opening of the file.");
        return;
    }
    if (output_graph(target) > 0)
    {
        QMessageBox::about(this,"Error","An error occured during opening of the file.");
        return;
    }
    fclose(target);

    graphFileName = newGraphFileName;
}

void MainWindow::updateVisualizationProcessing()
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

void MainWindow::countTrianglesProcessing()
{
    // ToDo: warning that it might take a long time
    // complexity: O(m*Delta*log(Delta))

    triangles = statistics::triangles(G);
    mean_clustering_coefficient = statistics::mean_clustering_coefficient(G);
    show_triangles = true;
}

void MainWindow::computeGirthProcessing()
{
    // ToDo: warning that it might take a long time
    // complexity: O(n*m)

    girth = statistics::girth(G);
    show_girth = true;
}

void MainWindow::computeDiameterProcessing()
{
    // ToDo: warning that it might take a long time
    // complexity: O(n*m)

    diameter = statistics::diameter(G);
    show_diameter = true;
}

void MainWindow::computeChromaticBrelazProcessing()
{
    refer brelaz_colors;
    refer *coloring;
    algorithm *algorithm_brelaz_instance;

    refer run, v;

    algorithm_brelaz_instance = new algorithm_brelaz();
    coloring = new refer[G->n];

    for (run=0;run<CommonSettings::runs_brelaz;run++)
    {
        brelaz_colors = algorithm_brelaz_instance->brelaz_with_heap(G, coloring);

        if (chromatic_upper >= brelaz_colors)
        {
            for (v=0;v<G->n;v++)
            {
                current_coloring[v] = coloring[v];
            }
        }
        if (chromatic_upper > brelaz_colors)
        {
            chromatic_upper = brelaz_colors;
        }
        max_clique_upper = chromatic_upper;
    }

    delete(algorithm_brelaz_instance);
    delete[](coloring);
}

void MainWindow::computeGreedyIndependentSetProcessing()
{
    refer run;
    refer greedy_independent_set_size;
    refer *independent_set;
    algorithm *algorithm_greedyindset_instance;

    algorithm_greedyindset_instance = new algorithm_greedyindset();
    independent_set = new refer[G->n];

    for (run=0;run<CommonSettings::runs_greedy_independent_set;run++)
    {
        greedy_independent_set_size = algorithm_greedyindset_instance->greedy_indset(G, independent_set);

        if (max_independent_set_lower < greedy_independent_set_size)
        {
            max_independent_set_lower = greedy_independent_set_size;
        }
        clique_covering_lower = max_independent_set_lower;
    }

    delete(algorithm_greedyindset_instance);
    delete[](independent_set);
}

void MainWindow::computeGreedyCliqueProcessing()
{
    refer run;
    refer greedy_clique_size;
    refer *clique;
    algorithm *algorithm_greedyclique_instance;

    algorithm_greedyclique_instance = new algorithm_greedyclique();
    clique = new refer[G->n];

    for (run=0;run<CommonSettings::runs_greedy_independent_set;run++)
    {
        greedy_clique_size = algorithm_greedyclique_instance->greedy_clique(G, clique);

        if (max_clique_lower < greedy_clique_size)
        {
            max_clique_lower = greedy_clique_size;
        }
        chromatic_lower = max_clique_lower;
    }

    delete(algorithm_greedyclique_instance);
    delete[](clique);
}

void MainWindow::computeGreedyDominatingSetProcessing()
{
    refer run;
    refer greedy_dominating_set_size;
    algorithm *algorithm_greedydom_instance;
    refer second_lower_bound;
    refer v, j, current_label;

    bool *greedy_dominating_set = new bool[G->n];
    refer *dominating_set_cluster_labels = new refer[G->n];
    refer *dominating_set_cluster_sizes = new refer[G->n+1];
    refer *dominating_set_cluster_beginnings = new refer[G->n+1];

    algorithm_greedydom_instance = new algorithm_greedydom();

    if (NULL == dominating_set)
    {
        dominating_set = new bool[G->n];
        dominating_set_clustering = new refer[G->n];
    }

    for (run=0;run<CommonSettings::runs_greedy_dominating_set;run++)
    {
        greedy_dominating_set_size = algorithm_greedydom_instance->greedydom(G, greedy_dominating_set);

        if (dominating_set_upper > greedy_dominating_set_size)
        {
            dominating_set_upper = greedy_dominating_set_size;
            for (v=0;v<G->n;v++)
            {
                dominating_set[v] = greedy_dominating_set[v];
            }
        }

        second_lower_bound = ceil((double) greedy_dominating_set_size / (double) (log(max_deg) + 1));
        if (dominating_set_lower < second_lower_bound)
        {
            dominating_set_lower = second_lower_bound;
        }
    }

    // computing the cluster labels
    for (v=0;v<G->n;v++)
    {
        dominating_set_cluster_labels[v] = 0;
    }

    current_label = 1;
    for (v=0;v<G->n;v++)
    {
        if (dominating_set[v])
        {
            // the dominating set vertex
            dominating_set_cluster_labels[v] = current_label;

            // the neighbors of the vertex
            for (j=0;j<G->V[v].edgecount;j++)
            {
                if (0 == dominating_set_cluster_labels[G->V[v].sibl[j]])
                {
                    dominating_set_cluster_labels[G->V[v].sibl[j]] = current_label;
                }
            }

            current_label++;
        }
    }

    // computing the cluster sizes and beginnings
    for (current_label=1;current_label<=greedy_dominating_set_size;current_label++)
    {
        dominating_set_cluster_sizes[current_label] = 0;
    }
    for (v=0;v<G->n;v++)
    {
        dominating_set_cluster_sizes[dominating_set_cluster_labels[v]]++;
    }
    dominating_set_cluster_beginnings[1] = 0;
    for (current_label=2;current_label<=greedy_dominating_set_size;current_label++)
    {
        dominating_set_cluster_beginnings[current_label] = dominating_set_cluster_beginnings[current_label-1] + dominating_set_cluster_sizes[current_label-1];
    }

    // putting the vertices in their rightful place in the permutation
    for (current_label=1;current_label<=greedy_dominating_set_size;current_label++)
    {
        dominating_set_cluster_sizes[current_label] = 0;
    }
    for (v=0;v<G->n;v++)
    {
        dominating_set_clustering[dominating_set_cluster_beginnings[dominating_set_cluster_labels[v]] + dominating_set_cluster_sizes[dominating_set_cluster_labels[v]]] = v;
        dominating_set_cluster_sizes[dominating_set_cluster_labels[v]]++;
    }

    delete[](greedy_dominating_set);
    delete[](dominating_set_cluster_labels);
    delete[](dominating_set_cluster_sizes);
    delete[](dominating_set_cluster_beginnings);

    delete(algorithm_greedydom_instance);
}

void MainWindow::computeLongestCycleDFSProcessing()
{
    refer run;
    refer v;
    refer longest_cycle_DFS;
    refer *longest_cycle_vertices = new refer[G->n+1];

    algorithm_lscycle *algorithm_lscycle_instance;

    for (run=0;run<CommonSettings::runs_dfs;run++)
    {
        algorithm_lscycle_instance = new algorithm_lscycle();
        longest_cycle_DFS = algorithm_lscycle_instance->dfs_cycle(G, longest_cycle_vertices);

        longest_cycle_upper = 0;
        for (v=0;v<G->n;v++)
        {
            if (G->V[v].edgecount >= 2)
            {
                longest_cycle_upper++;
            }
        }

        if (longest_cycle_lower <= longest_cycle_DFS)
        {
            for (v=0;v<G->n;v++)
            {
                current_longest_cycle_vertices[v] = longest_cycle_vertices[v];
            }
        }
        if (longest_cycle_lower < longest_cycle_DFS)
        {
            longest_cycle_lower = longest_cycle_DFS;
        }

        delete(algorithm_lscycle_instance);
    }

    show_longest_cycle = true;

    delete[](longest_cycle_vertices);
}

void MainWindow::computeLongestCycleDFSLSProcessing()
{
    refer run;
    refer v;
    refer longest_cycle_DFS_LS;
    refer *longest_cycle_vertices = new refer[G->n+1];

    algorithm_lscycle *algorithm_lscycle_instance;

    algorithm_lscycle_instance = new algorithm_lscycle();

    for (run=0;run<CommonSettings::runs_dfs_ls;run++)
    {
        longest_cycle_DFS_LS = algorithm_lscycle_instance->dfs_ls_cycle(G, longest_cycle_vertices);

        longest_cycle_upper = 0;
        for (v=0;v<G->n;v++)
        {
            if (G->V[v].edgecount >= 2)
            {
                longest_cycle_upper++;
            }
        }

        if (longest_cycle_lower <= longest_cycle_DFS_LS)
        {
            for (v=0;v<G->n;v++)
            {
                current_longest_cycle_vertices[v] = longest_cycle_vertices[v];
            }
        }
        if (longest_cycle_lower < longest_cycle_DFS_LS)
        {
            longest_cycle_lower = longest_cycle_DFS_LS;
        }
    }

    show_longest_cycle = true;

    delete(algorithm_lscycle_instance);
    delete[](longest_cycle_vertices);
}

void MainWindow::loadGraph()
{
    graphFileName = QFileDialog::getOpenFileName(this,"Choose a file to open",QString::null,"COL files (*.col);;GML files (*.gml)");

    if (NULL == graphFileName || "" == graphFileName)
    {
        return;
    }

    if (NULL != get_graph())
    {
        free_graph();
    }
    if (NULL != current_coloring)
    {
        delete[](current_coloring);
        current_coloring = NULL;
    }
    if (NULL != dominating_set)
    {
        delete[](dominating_set);
        dominating_set = NULL;
        delete[](dominating_set_clustering);
        dominating_set_clustering = NULL;
    }
    if (NULL != current_longest_cycle_vertices)
    {
        delete[](current_longest_cycle_vertices);
        current_longest_cycle_vertices = NULL;
    }
    clear_evolved_graph_visualization();

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::loadGraphProcessing);
    this->FutureWatcherGraphLoading.setFuture(future);
}

void MainWindow::generateGraphBA()
{
    DialogBA dialog_ba(this);
    dialog_ba.setModal(true);
    dialog_ba.exec();

    bool accepted;
    dialog_ba.getParams(&accepted, &n_max, &w);

    if (! accepted)
    {
        return;
    }

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    if (NULL != get_graph())
    {
        free_graph();
    }
    if (NULL != current_coloring)
    {
        delete[](current_coloring);
        current_coloring = NULL;
    }
    if (NULL != dominating_set)
    {
        delete[](dominating_set);
        dominating_set = NULL;
        delete[](dominating_set_clustering);
        dominating_set_clustering = NULL;
    }
    if (NULL != current_longest_cycle_vertices)
    {
        delete[](current_longest_cycle_vertices);
        current_longest_cycle_vertices = NULL;
    }
    clear_evolved_graph_visualization();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::generateGraphBAProcessing);
    this->FutureWatcherGraphLoading.setFuture(future);
}

void MainWindow::generateGraphUDG()
{
    DialogUDG dialog_udg(this);
    dialog_udg.setModal(true);
    dialog_udg.exec();

    bool accepted;
    dialog_udg.getParams(&accepted, &n_max, &range, &grid);

    if (! accepted)
    {
        return;
    }

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    if (NULL != get_graph())
    {
        free_graph();
    }
    if (NULL != current_coloring)
    {
        delete[](current_coloring);
        current_coloring = NULL;
    }
    if (NULL != dominating_set)
    {
        delete[](dominating_set);
        dominating_set = NULL;
        delete[](dominating_set_clustering);
        dominating_set_clustering = NULL;
    }
    if (NULL != current_longest_cycle_vertices)
    {
        delete[](current_longest_cycle_vertices);
        current_longest_cycle_vertices = NULL;
    }
    clear_evolved_graph_visualization();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::generateGraphUDGProcessing);
    this->FutureWatcherGraphLoading.setFuture(future);
}

void MainWindow::generateGraphCompleteTree()
{
    DialogCompleteTree dialog_complete_tree(this);
    dialog_complete_tree.setModal(true);
    dialog_complete_tree.exec();

    bool accepted;
    dialog_complete_tree.getParams(&accepted, &branching_factor, &depth);

    if (! accepted)
    {
        return;
    }

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    if (NULL != get_graph())
    {
        free_graph();
    }
    if (NULL != current_coloring)
    {
        delete[](current_coloring);
        current_coloring = NULL;
    }
    if (NULL != dominating_set)
    {
        delete[](dominating_set);
        dominating_set = NULL;
        delete[](dominating_set_clustering);
        dominating_set_clustering = NULL;
    }
    if (NULL != current_longest_cycle_vertices)
    {
        delete[](current_longest_cycle_vertices);
        current_longest_cycle_vertices = NULL;
    }
    clear_evolved_graph_visualization();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::generateGraphCompleteTreeProcessing);
    this->FutureWatcherGraphLoading.setFuture(future);
}

void MainWindow::generateComplementaryGraph()
{
    if (NULL != current_coloring)
    {
        delete[](current_coloring);
        current_coloring = NULL;
    }
    if (NULL != dominating_set)
    {
        delete[](dominating_set);
        dominating_set = NULL;
        delete[](dominating_set_clustering);
        dominating_set_clustering = NULL;
    }
    if (NULL != current_longest_cycle_vertices)
    {
        delete[](current_longest_cycle_vertices);
        current_longest_cycle_vertices = NULL;
    }
    clear_evolved_graph_visualization();

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::generateComplementaryGraphProcessing);
    this->FutureWatcherGraphLoading.setFuture(future);
}

void MainWindow::generateGraphPrunedLeaves()
{
    if (NULL != current_coloring)
    {
        delete[](current_coloring);
        current_coloring = NULL;
    }
    if (NULL != dominating_set)
    {
        delete[](dominating_set);
        dominating_set = NULL;
        delete[](dominating_set_clustering);
        dominating_set_clustering = NULL;
    }
    if (NULL != current_longest_cycle_vertices)
    {
        delete[](current_longest_cycle_vertices);
        current_longest_cycle_vertices = NULL;
    }
    clear_evolved_graph_visualization();

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::generateGraphPrunedLeavesProcessing);
    this->FutureWatcherGraphLoading.setFuture(future);
}

void MainWindow::generateLargestComponent()
{
    if (NULL != current_coloring)
    {
        delete[](current_coloring);
        current_coloring = NULL;
    }
    if (NULL != dominating_set)
    {
        delete[](dominating_set);
        dominating_set = NULL;
        delete[](dominating_set_clustering);
        dominating_set_clustering = NULL;
    }
    if (NULL != current_longest_cycle_vertices)
    {
        delete[](current_longest_cycle_vertices);
        current_longest_cycle_vertices = NULL;
    }
    clear_evolved_graph_visualization();

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::generateLargestComponentProcessing);
    this->FutureWatcherGraphLoading.setFuture(future);
}

void MainWindow::generateGeneralizedDominatingSetGraph()
{
    DialogGenDom dialog_gendom(this);
    dialog_gendom.setModal(true);
    dialog_gendom.exec();

    bool accepted;
    dialog_gendom.getParams(&accepted, &max_generalized_dominating_set_distance);

    if (! accepted)
    {
        return;
    }

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    if (NULL != current_coloring)
    {
        delete[](current_coloring);
        current_coloring = NULL;
    }
    if (NULL != current_longest_cycle_vertices)
    {
        delete[](current_longest_cycle_vertices);
        current_longest_cycle_vertices = NULL;
    }
    clear_evolved_graph_visualization();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::generateGeneralizedDominatingSetGraphProcessing);
    this->FutureWatcherGraphLoading.setFuture(future);
}

void MainWindow::generateGraphWS()
{
    DialogWS dialog_ws(this);
    dialog_ws.setModal(true);
    dialog_ws.exec();

    bool accepted;
    dialog_ws.getParams(&accepted, &n_max, &lattice_branching_factor, &beta);

    if (! accepted)
    {
        return;
    }

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    if (NULL != get_graph())
    {
        free_graph();
    }
    if (NULL != current_coloring)
    {
        delete[](current_coloring);
        current_coloring = NULL;
    }
    if (NULL != dominating_set)
    {
        delete[](dominating_set);
        dominating_set = NULL;
        delete[](dominating_set_clustering);
        dominating_set_clustering = NULL;
    }
    if (NULL != current_longest_cycle_vertices)
    {
        delete[](current_longest_cycle_vertices);
        current_longest_cycle_vertices = NULL;
    }
    clear_evolved_graph_visualization();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::generateGraphWSProcessing);
    this->FutureWatcherGraphLoading.setFuture(future);
}

void MainWindow::generateGraphGrid()
{
    DialogGrid dialog_grid(this);
    dialog_grid.setModal(true);
    dialog_grid.exec();

    bool accepted;
    dialog_grid.getParams(&accepted, &rows, &columns, &beta);

    if (! accepted)
    {
        return;
    }

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    if (NULL != get_graph())
    {
        free_graph();
    }
    if (NULL != current_coloring)
    {
        delete[](current_coloring);
        current_coloring = NULL;
    }
    if (NULL != dominating_set)
    {
        delete[](dominating_set);
        dominating_set = NULL;
        delete[](dominating_set_clustering);
        dominating_set_clustering = NULL;
    }
    if (NULL != current_longest_cycle_vertices)
    {
        delete[](current_longest_cycle_vertices);
        current_longest_cycle_vertices = NULL;
    }
    clear_evolved_graph_visualization();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::generateGraphGridProcessing);
    this->FutureWatcherGraphLoading.setFuture(future);
}

void MainWindow::saveGraph()
{
    newGraphFileName = QFileDialog::getSaveFileName(this,"Choose a file to save the graph",QString::null,"COL file (*.col);;All files (*.*)" );
//    newGraphFileName = QFileDialog::getSaveFileName(this,"Choose a file to save the graph",QString::null,QString::null);

    if (NULL == G || NULL == newGraphFileName || "" == newGraphFileName)
    {
        return;
    }

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    // loadGraphProcessing();
    QFuture<void> future = QtConcurrent::run(this, &MainWindow::saveGraphProcessing);
    this->FutureWatcherGraphSaving.setFuture(future);
}

void MainWindow::exportGraphVisualization()
{
    graphVisualizationExportFileName = QFileDialog::getSaveFileName(
        this,
        "Choose a file to save the visualization",
        QString::null,
        "PNG file (*.png);;BNP file (*.bmp);;JPG file (*.jpg)"
    );

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

void MainWindow::exportAdjacencyMatrix()
{
    adjacencyMatrixExportFileName = QFileDialog::getSaveFileName(
                this,
                "Choose a file to save the visualization",
                QString::null,
                "PNG file (*.png);;BNP file (*.bmp);;JPG file (*.jpg)"
            );

    if (NULL == G || NULL == adjacencyMatrixExportFileName || "" == adjacencyMatrixExportFileName)
    {
        return;
    }

    if (! adjacencyMatrixExportFileName.toLower().endsWith(".png") &&
        ! adjacencyMatrixExportFileName.toLower().endsWith(".bmp") &&
        ! adjacencyMatrixExportFileName.toLower().endsWith(".jpg") &&
        ! adjacencyMatrixExportFileName.toLower().endsWith(".jpeg"))
    {
        adjacencyMatrixExportFileName += ".png";
    }

    QImage pixmap_image = pixmap_adjacency_matrix->toImage();
    pixmap_image.setDotsPerMeterX(6000);
    pixmap_image.setDotsPerMeterY(6000);
    pixmap_image.save(adjacencyMatrixExportFileName);
}

void MainWindow::exportDegreeDistribution()
{
    refer i;

    degreeDistributionExportFileName = QFileDialog::getSaveFileName(
                this,
                "Choose a file to save the visualization",
                QString::null,
                "CSV file (*.csv)"
            );

    if (NULL == G || NULL == degreeDistributionExportFileName || "" == degreeDistributionExportFileName)
    {
        return;
    }

    if (! degreeDistributionExportFileName.toLower().endsWith(".csv"))
    {
        degreeDistributionExportFileName += ".csv";
    }

    FILE *target;

    if ((target = fopen(degreeDistributionExportFileName.toLatin1(),"w")) == NULL)
    {
        QMessageBox::about(this,"Error","An error occured saving of the degree distribution data.");
        return;
    }

    fprintf(target, "Absolute,");
    for (i=0;i<=max_deg;i++)
    {
        fprintf(target, "%u,", degree_distrib[i]);
    }
    fprintf(target,"\n");
    fprintf(target, "Relative,");
    for (i=0;i<=max_deg;i++)
    {
        fprintf(target, "%0.6lf,", (double) degree_distrib[i] / (double) (2 * G->m));
    }

    fclose(target);
}

void MainWindow::exportDominatingSet()
{
    refer i;

    mpsDominatingSetExportFileName = QFileDialog::getSaveFileName(this,"Choose a file to save the ILP instance exported",QString::null,"MPS file (*.mps)");

    if (NULL == G || NULL == mpsDominatingSetExportFileName || "" == mpsDominatingSetExportFileName)
    {
        return;
    }

    if (! mpsDominatingSetExportFileName.toLower().endsWith(".mps"))
    {
        mpsDominatingSetExportFileName += ".mps";
    }

    FILE *mpsfile;

    if ((mpsfile = fopen(mpsDominatingSetExportFileName.toLatin1(),"w")) == NULL)
    {
        QMessageBox::about(this,"Error","An error occured saving of the ILP instance exported.");
        return;
    }

    refer v, j;
    bool *domination_influences = new bool[G->n];
    char filename_mps[PATH_MAX];

    fprintf(mpsfile, "NAME DOMINATINGSET\n");
    fprintf(mpsfile, "ROWS\n");
    fprintf(mpsfile, " N COST\n");
    for (v=0;v<G->n;v++)
    {
        fprintf(mpsfile, "    G LIM%u\n", v+1);
    }
    fprintf(mpsfile, "COLUMNS\n");
    for (v=0;v<G->n;v++)
    {
        fprintf(mpsfile, "    x%u   COST 1\n", v+1);
        for (j=0;j<G->n;j++)
        {
            domination_influences[j] = false;
        }
        domination_influences[v] = true;
        // other constraints - neighbours of v
        for (j=0;j<G->V[v].edgecount;j++)
        {
            domination_influences[G->V[v].sibl[j]] = true;
        }
        for (j=0;j<G->n;j++)
        {
            if (domination_influences[j])
            {
                fprintf(mpsfile, "    x%u   LIM%u %u\n", v+1, j+1, domination_influences[j]);
            }
        }
    }
    fprintf(mpsfile, "RHS\n");
    for (v=0;v<G->n;v++)
    {
        fprintf(mpsfile, "    RHS1 LIM%u 1\n", v+1);
    }
    fprintf(mpsfile, "BOUNDS\n");
    for (v=0;v<G->n;v++)
    {
        fprintf(mpsfile, " BV ONE x%u 1\n", v+1, v+1);
    }
    fprintf(mpsfile, "ENDATA\n");
    fclose(mpsfile);

    delete[](domination_influences);

    fclose(mpsfile);
}

void MainWindow::exportDominatingSetLPRelaxation()
{
    refer i;

    mpsDominatingSetExportFileName = QFileDialog::getSaveFileName(this,"Choose a file to save the ILP instance exported",QString::null,"MPS file (*.mps)");

    if (NULL == G || NULL == mpsDominatingSetExportFileName || "" == mpsDominatingSetExportFileName)
    {
        return;
    }

    if (! mpsDominatingSetExportFileName.toLower().endsWith(".mps"))
    {
        mpsDominatingSetExportFileName += ".mps";
    }

    FILE *mpsfile;

    if ((mpsfile = fopen(mpsDominatingSetExportFileName.toLatin1(),"w")) == NULL)
    {
        QMessageBox::about(this,"Error","An error occured saving of the ILP instance exported.");
        return;
    }

    refer v, j;
    bool *domination_influences = new bool[G->n];
    char filename_mps[PATH_MAX];

    fprintf(mpsfile, "NAME DOMINATINGSET\n");
    fprintf(mpsfile, "ROWS\n");
    fprintf(mpsfile, " N COST\n");
    for (v=0;v<G->n;v++)
    {
        fprintf(mpsfile, "    G LIM%u\n", v+1);
    }
    fprintf(mpsfile, "COLUMNS\n");
    for (v=0;v<G->n;v++)
    {
        fprintf(mpsfile, "    x%u   COST 1\n", v+1);
        for (j=0;j<G->n;j++)
        {
            domination_influences[j] = false;
        }
        domination_influences[v] = true;
        // other constraints - neighbours of v
        for (j=0;j<G->V[v].edgecount;j++)
        {
            domination_influences[G->V[v].sibl[j]] = true;
        }
        for (j=0;j<G->n;j++)
        {
            if (domination_influences[j])
            {
                fprintf(mpsfile, "    x%u   LIM%u %u\n", v+1, j+1, domination_influences[j]);
            }
        }
    }
    fprintf(mpsfile, "RHS\n");
    for (v=0;v<G->n;v++)
    {
        fprintf(mpsfile, "    RHS1 LIM%u 1\n", v+1);
    }
    fprintf(mpsfile, "ENDATA\n");
    fclose(mpsfile);

    delete[](domination_influences);

    fclose(mpsfile);
}

void MainWindow::countTriangles()
{
    if (NULL == G)
    {
        return;
    }

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::countTrianglesProcessing);
    this->FutureWatcherTrianglesCounting.setFuture(future);
}

void MainWindow::computeGirth()
{
    if (NULL == G)
    {
        return;
    }

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::computeGirthProcessing);
    this->FutureWatcherGirthComputing.setFuture(future);
}

void MainWindow::computeDiameter()
{
    if (NULL == G)
    {
        return;
    }

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::computeDiameterProcessing);
    this->FutureWatcherDiameterComputing.setFuture(future);
}

void MainWindow::computeChromaticBrelaz()
{
    if (NULL == G)
    {
        return;
    }
    if (NULL == current_coloring)
    {
        current_coloring = new refer[G->n+1];
    }

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::computeChromaticBrelazProcessing);
    this->FutureWatcherChromatic.setFuture(future);
}

void MainWindow::computeGreedyIndependentSet()
{
    if (NULL == G)
    {
        return;
    }

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::computeGreedyIndependentSetProcessing);
    this->FutureWatcherIndependentSet.setFuture(future);
}

void MainWindow::computeGreedyClique()
{
    if (NULL == G)
    {
        return;
    }

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::computeGreedyCliqueProcessing);
    this->FutureWatcherMaxClique.setFuture(future);
}

void MainWindow::computeGreedyDominatingSet()
{
    if (NULL == G)
    {
        return;
    }

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::computeGreedyDominatingSetProcessing);
    this->FutureWatcherDominatingSet.setFuture(future);
}

void MainWindow::computeLongestCycleDFS()
{
    if (NULL == G)
    {
        return;
    }
    if (NULL == current_longest_cycle_vertices)
    {
        current_longest_cycle_vertices = new refer[G->n+1];
    }

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::computeLongestCycleDFSProcessing);
    this->FutureWatcherLongestCycle.setFuture(future);
}

void MainWindow::computeLongestCycleDFSLS()
{
    if (NULL == G)
    {
        return;
    }
    if (NULL == current_longest_cycle_vertices)
    {
        current_longest_cycle_vertices = new refer[G->n+1];
    }

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::computeLongestCycleDFSLSProcessing);
    this->FutureWatcherLongestCycle.setFuture(future);
}

void MainWindow::computeIGRLSCliqueCovering()
{
    if (NULL == G)
    {
        return;
    }

    workerThreadIGRLSCliqueCovering->setParams(G);
    workerThreadIGRLSCliqueCovering->start();
}

void MainWindow::computeIGRLSColoring()
{
    if (NULL == G)
    {
        return;
    }
    if (NULL == current_coloring)
    {
        current_coloring = new refer[G->n+1];
    }

    workerThreadIGRLSColoring->setParams(G, current_coloring);
    workerThreadIGRLSColoring->start();
}

void MainWindow::showInSeparateWindow()
{
    refer current_visualization_size;

    if (NULL == G)
    {
        return;
    }

    LargeVisForm *largeVisForm = new LargeVisForm();
    // ToDo: this should perform the delete once the thread with the crossing minimization is finished
    //largeVisForm->setAttribute(Qt::WA_DeleteOnClose);

    if (ui->action1x->isChecked())
    {
        current_visualization_size = VISUALIZATION_SIZE;
    }
    else if (ui->action2x->isChecked())
    {
        current_visualization_size = VISUALIZATION_SIZE_DOUBLE;
    }
    else if (ui->action5x->isChecked())
    {
        current_visualization_size = VISUALIZATION_SIZE_MEDIUM;
    }
    else
    {
        current_visualization_size = VISUALIZATION_SIZE_LARGE;
    }

    largeVisForm->setVisualization(pixmap_graph_visualization, current_visualization_size);
    largeVisForm->show();
}

void MainWindow::minimizeCrossings()
{
    if (NULL == G)
    {
        return;
    }

    ui->actionCrossings_based->setEnabled(true);

    // ToDo: check whether tidying up should be done here
    EvoVisForm *evoVisForm = new EvoVisForm();
    evoVisForm->setAttribute(Qt::WA_DeleteOnClose);

    refer current_visualization_size;
    if (ui->action1x->isChecked())
    {
        current_visualization_size = VISUALIZATION_SIZE;
    }
    else if (ui->action2x->isChecked())
    {
        current_visualization_size = VISUALIZATION_SIZE_DOUBLE;
    }
    else if (ui->action5x->isChecked())
    {
        current_visualization_size = VISUALIZATION_SIZE_MEDIUM;
    }
    else
    {
        current_visualization_size = VISUALIZATION_SIZE_LARGE;
    }

    evoVisForm->setCrossingMinimisationParams(G, max_deg, myx, myy, current_visualization_size);

    evoVisForm->show();
}

void MainWindow::showOptionsAlgorithms()
{
    DialogOptionsAlgorithms dialog_options_algorithms(this);
    dialog_options_algorithms.setModal(true);
    dialog_options_algorithms.exec();
}

void MainWindow::showOptionsVisualization()
{
    DialogOptionsVisualization dialog_options_visualization(this);
    dialog_options_visualization.setModal(true);
    dialog_options_visualization.exec();

    ui->progressBar->show();
    ui->label_loading->show();
    ui->progressBar->repaint();
    ui->label_loading->repaint();

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::updateVisualizationProcessing);
    this->FutureWatcherVisualizationUpdate.setFuture(future);
}

void MainWindow::showAboutWindow()
{
    QMessageBox::about(this, "About", "Graph Combinatorial Explorer, version 1.1 \n\n(c) David Chalupa, 2011-2026");
}

void MainWindow::logMessage(QString msg)
{
    ui->textBrowser->append(msg);
}

MainWindow::~MainWindow()
{
    if (NULL != current_coloring)
    {
        delete[] current_coloring;
    }
    if (NULL != dominating_set)
    {
        delete[] dominating_set;
        delete[] dominating_set_clustering;
    }
    if (NULL != current_longest_cycle_vertices)
    {
        delete[] current_longest_cycle_vertices;
    }
    if (NULL != myx)
    {
        delete[](myx);
    }
    if (NULL != myy)
    {
        delete[](myy);
    }
    if (NULL != CommonSettings::myx)
    {
        delete[](CommonSettings::myx);
    }
    if (NULL != CommonSettings::myy)
    {
        delete[](CommonSettings::myy);
    }

    delete workerThreadIGRLSCliqueCovering;
    delete ui;
}

////////////////////
// WORKER THREADS //
////////////////////

// maximum independent set and clique covering
void WorkerThreadIGRLSCliqueCovering::computeIGRLSCliqueCoveringProcessing()
{
    refer *clique_covering;
    algorithm *algorithm_IGRLS_instance;
    algorithm *algorithm_greedyindset_instance;

    refer greedy_independent_set_size;
    refer *independent_set;

    srand((unsigned long long) time(NULL));

    logMessage("Starting IG/RLS for clique covering and maximum independent set...");

    // initializing with the greedy independent set
    algorithm_greedyindset_instance = new algorithm_greedyindset();
    independent_set = new refer[G->n];

    greedy_independent_set_size = algorithm_greedyindset_instance->greedy_indset(G, independent_set);

    delete(algorithm_greedyindset_instance);

    // IG/RLS itself
    algorithm_IGRLS_instance = new algorithm_iggcc(G);
    clique_covering = new refer[G->n];

    refer run;
    for (run=0;run<CommonSettings::runs_rls_ig_independent_set;run++)
    {
        algorithm_IGRLS_instance->iggcc_ccp(G, clique_covering, &indset_size, independent_set, greedy_independent_set_size);

        clique_covering_size = problem_ccp::count_labels(G, clique_covering);

        logMessage("Finished with result " + QString::number(indset_size) + " - " + QString::number(clique_covering_size) + ".\n");
    }

    delete(algorithm_IGRLS_instance);
    delete[](clique_covering);
}

void WorkerThreadIGRLSCliqueCovering::getResults(refer *clique_covering_size, refer *indset_size)
{
    *clique_covering_size = this->clique_covering_size;
    *indset_size = this->indset_size;
}

void WorkerThreadIGRLSCliqueCovering::setParams(graph G)
{
    this->G = G;
}

void WorkerThreadIGRLSCliqueCovering::logMessage(QString msg)
{
    emit logMessageSignal(msg);
}

void WorkerThreadIGRLSCliqueCovering::run()
{
    computeIGRLSCliqueCoveringProcessing();
}

void logMessageIGRLSCliqueCovering(QString msg)
{
    workerThreadIGRLSCliqueCovering->logMessage(msg);
}

// maximum clique and graph coloring
void WorkerThreadIGRLSColoring::computeIGRLSColoringProcessing()
{
    refer *coloring;
    algorithm *algorithm_IGRLS_instance;
    algorithm *algorithm_greedyclique_instance;

    refer greedy_clique_size;
    refer *clique;

    refer v;

    srand((unsigned long long) time(NULL));

    logMessage("Starting IG/RLS for graph coloring and maximum clique...");

    clique = new refer[G->n];
    coloring = new refer[G->n];

    refer run;
    algorithm_greedyclique_instance = new algorithm_greedyclique();
    algorithm *algorithm_brelaz_instance;
    algorithm_brelaz_instance = new algorithm_brelaz();

    for (run=0;run<CommonSettings::runs_rls_ig_clique;run++)
    {

        // initializing the clique with greedy algorithm
        greedy_clique_size = algorithm_greedyclique_instance->greedy_clique(G, clique);

        // initializing the coloring with Brelaz's heuristic
        refer brelaz_colors;
        brelaz_colors = algorithm_brelaz_instance->brelaz_with_heap(G, coloring);

        // IG/RLS itself
        algorithm_IGRLS_instance = new algorithm_igcol(G, brelaz_colors);
        algorithm_IGRLS_instance->igcol(G, false, coloring, &clique_size, clique, greedy_clique_size);
        //algorithm_IGRLS_instance = new algorithm_igcol(G, G->n);
        //algorithm_IGRLS_instance->igcol(G, true, coloring, &clique_size, clique, greedy_clique_size);

        // propagating the coloring above to visualize it
        for (v=0;v<G->n;v++)
        {
            final_coloring[v] = coloring[v];
        }
        coloring_size = problem_ccp::count_labels(G, coloring);

        logMessage("Finished with result " + QString::number(clique_size) + " - " + QString::number(coloring_size) + ".\n");

        delete(algorithm_IGRLS_instance);
    }

    delete(algorithm_brelaz_instance);
    delete(algorithm_greedyclique_instance);
    delete[](coloring);
}

void WorkerThreadIGRLSColoring::getResults(refer *coloring_size, refer *clique_size)
{
    *coloring_size = this->coloring_size;
    *clique_size = this->clique_size;
}

void WorkerThreadIGRLSColoring::setParams(graph G, refer *final_coloring)
{
    this->G = G;
    this->final_coloring = final_coloring;
}

void WorkerThreadIGRLSColoring::logMessage(QString msg)
{
    emit logMessageSignal(msg);
}

void WorkerThreadIGRLSColoring::run()
{
    computeIGRLSColoringProcessing();
}

void logMessageIGRLSColoring(QString msg)
{
    workerThreadIGRLSColoring->logMessage(msg);
}
