#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFutureWatcher>
#include <QGraphicsScene>
#include <QPixmap>
#include <QThread>

#include "graphs.h"

#include "largevisform.h"
#include "evovisform.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    graph G;
    QString graphFileName;
    QString newGraphFileName;
    QString graphVisualizationExportFileName;
    QString adjacencyMatrixExportFileName;
    QString degreeDistributionExportFileName;
    QString mpsDominatingSetExportFileName;

    refer n_max, w;
    refer range, grid;
    refer branching_factor, depth;
    refer rows, columns;
    refer lattice_branching_factor;
    double beta;
    refer max_generalized_dominating_set_distance;

    unsigned long max_deg,min_deg;
    double avg_deg,stdev_deg;
    refer components;

    bool show_triangles;
    unsigned long long triangles;
    double mean_clustering_coefficient;
    bool show_diameter;
    refer diameter;
    bool show_girth;
    refer girth;
    bool show_longest_cycle;

    refer max_clique_lower, max_clique_upper;
    refer chromatic_lower, chromatic_upper;
    refer max_independent_set_lower, max_independent_set_upper;
    refer clique_covering_lower, clique_covering_upper;
    refer dominating_set_lower, dominating_set_upper;
    refer longest_cycle_lower, longest_cycle_upper;

    refer runs_greedy_clique;
    refer runs_brelaz;
    refer runs_rls_ig_clique;
    refer runs_greedy_independent_set;
    refer runs_rls_ig_independent_set;
    refer runs_greedy_dominating_set;
    refer runs_dfs;
    refer runs_dfs_ls;

    refer *current_coloring;
    refer *current_longest_cycle_vertices;

    bool *dominating_set;
    refer *dominating_set_clustering;

    refer *myx, *myy, *distances, *queue;
    refer *degree_distrib;
    refer degree_distrib_size;

    bool graph_visualization_visible;
    bool adjacency_matrix_visible;
    bool degree_distribution_visible;
    QPixmap *pixmap_graph_visualization;
    QPixmap *pixmap_adjacency_matrix;
    QPixmap *pixmap_degree_distribution;

    QFutureWatcher<void> FutureWatcherGraphLoading;
    QFutureWatcher<void> FutureWatcherGraphSaving;
    QFutureWatcher<void> FutureWatcherVisualizationUpdate;
    QFutureWatcher<void> FutureWatcherTrianglesCounting;
    QFutureWatcher<void> FutureWatcherGirthComputing;
    QFutureWatcher<void> FutureWatcherDiameterComputing;
    QFutureWatcher<void> FutureWatcherChromatic;
    QFutureWatcher<void> FutureWatcherIndependentSet;
    QFutureWatcher<void> FutureWatcherMaxClique;
    QFutureWatcher<void> FutureWatcherCliqueCovering;
    QFutureWatcher<void> FutureWatcherDominatingSet;
    QFutureWatcher<void> FutureWatcherLongestCycle;

    int compute_statistics(graph G);
    void update_statistics();
    void update_adjacency_matrix();
    void update_graph_visualization();
    void clear_adjacency_matrix();
    void clear_graph_visualization();
    void update_degree_distribution();
    void clear_degree_distribution();

    void loadGraphProcessing();
    void saveGraphProcessing();
    void generateGraphBAProcessing();
    void generateGraphUDGProcessing();
    void generateGraphCompleteTreeProcessing();
    void generateComplementaryGraphProcessing();
    void generateGeneralizedDominatingSetGraphProcessing();
    void generateGraphWSProcessing();
    void generateGraphGridProcessing();
    void generateGraphPrunedLeavesProcessing();
    void generateLargestComponentProcessing();

    void updateVisualizationProcessing();
    void countTrianglesProcessing();
    void computeGirthProcessing();
    void computeDiameterProcessing();
    void computeChromaticBrelazProcessing();
    void computeGreedyIndependentSetProcessing();
    void computeGreedyCliqueProcessing();
    void computeGreedyDominatingSetProcessing();
    void computeLongestCycleDFSProcessing();
    void computeLongestCycleDFSLSProcessing();

public slots:
    void loadGraph();
    void saveGraph();
    void generateGraphBA();
    void generateGraphUDG();
    void generateGraphCompleteTree();
    void generateComplementaryGraph();
    void generateGeneralizedDominatingSetGraph();
    void generateGraphWS();
    void generateGraphGrid();
    void generateGraphPrunedLeaves();
    void generateLargestComponent();

    void exitApp();

    void chooseGridbased();
    void chooseTreebased();
    void chooseCentralitybased();
    void chooseCircular();
    void chooseCyclebased();

    void exportGraphVisualization();
    void exportAdjacencyMatrix();
    void exportDegreeDistribution();
    void exportDominatingSet();
    void exportDominatingSetLPRelaxation();

    void choose1x();
    void choose2x();
    void choose5x();
    void choose25x();

    void countTriangles();
    void computeGirth();
    void computeDiameter();

    void finishGraphLoading();
    void finishGraphSaving();
    void finishVisualizationUpdate();
    void finishTriangleCounting();
    void finishGirthComputing();
    void finishDiameterComputing();
    void finishChromatic();
    void finishIndependentSet();
    void finishMaxClique();
    void finishCliqueCovering();
    void finishDominatingSet();
    void finishLongestCycle();

    void finishWorkerThreadCliqueCovering();
    void finishWorkerThreadColoring();

    void computeChromaticBrelaz();
    void computeGreedyIndependentSet();
    void computeGreedyClique();
    void computeIGRLSCliqueCovering();
    void computeGreedyDominatingSet();
    void computeIGRLSColoring();
    void computeLongestCycleDFS();
    void computeLongestCycleDFSLS();

    void showInSeparateWindow();
    void minimizeCrossings();

    void showOptionsAlgorithms();
    void showOptionsVisualization();

    void showAboutWindow();

    void logMessage(QString msg);
};


class WorkerThreadIGRLSCliqueCovering : public QThread
{
    Q_OBJECT

private:
    graph G;
    refer clique_covering_size;
    refer indset_size;

public:
    void computeIGRLSCliqueCoveringProcessing();
    void getResults(refer *clique_covering_size, refer *indset_size);
    void setParams(graph G);
    void logMessage(QString msg);
    void run() Q_DECL_OVERRIDE;
signals:
    void logMessageSignal(QString msg);
};

void logMessageIGRLSCliqueCovering(QString msg);

class WorkerThreadIGRLSColoring : public QThread
{
    Q_OBJECT

private:
    graph G;
    refer coloring_size;
    refer clique_size;
    refer *final_coloring;

public:
    void computeIGRLSColoringProcessing();
    void getResults(refer *coloring_size, refer *clique_size);
    void setParams(graph G, refer *final_coloring);
    void logMessage(QString msg);
    void run() Q_DECL_OVERRIDE;
signals:
    void logMessageSignal(QString msg);
};

void logMessageIGRLSColoring(QString msg);

#endif // MAINWINDOW_H
