#include "dialogoptionsalgorithms.h"
#include "ui_dialogoptionsalgorithms.h"

DialogOptionsAlgorithms::DialogOptionsAlgorithms(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOptionsAlgorithms)
{
    ui->setupUi(this);

    ui->spinBox_greedy_clique->setValue(CommonSettings::runs_greedy_clique);
    ui->spinBox_brelaz->setValue(CommonSettings::runs_brelaz);
    ui->spinBox_randomized_local_search_and_iterated_greedy_clique->setValue(CommonSettings::runs_rls_ig_clique);
    ui->spinBox_greedy_independent_set->setValue(CommonSettings::runs_greedy_independent_set);
    ui->spinBox_randomized_local_search_and_iterated_greedy_independent_set->setValue(CommonSettings::runs_rls_ig_independent_set);
    ui->spinBox_greedy_dominating_set->setValue(CommonSettings::runs_greedy_dominating_set);
    ui->spinBox_depth_first_search->setValue(CommonSettings::runs_dfs);
    ui->spinBox_depth_first_search_local_search->setValue(CommonSettings::runs_dfs_ls);

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(setParams()));
}

DialogOptionsAlgorithms::~DialogOptionsAlgorithms()
{
    delete ui;
}

void DialogOptionsAlgorithms::setParams()
{
    CommonSettings::runs_greedy_clique = ui->spinBox_greedy_clique->value();
    CommonSettings::runs_brelaz = ui->spinBox_brelaz->value();
    CommonSettings::runs_rls_ig_clique = ui->spinBox_randomized_local_search_and_iterated_greedy_clique->value();
    CommonSettings::runs_greedy_independent_set = ui->spinBox_greedy_independent_set->value();
    CommonSettings::runs_rls_ig_independent_set = ui->spinBox_randomized_local_search_and_iterated_greedy_independent_set->value();
    CommonSettings::runs_greedy_dominating_set = ui->spinBox_greedy_dominating_set->value();
    CommonSettings::runs_dfs = ui->spinBox_depth_first_search->value();
    CommonSettings::runs_dfs_ls = ui->spinBox_depth_first_search_local_search->value();
}

