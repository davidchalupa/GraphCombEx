#ifndef DIALOGOPTIONSVISUALIZATION_H
#define DIALOGOPTIONSVISUALIZATION_H

#include <QDialog>

#include "common.h"

namespace Ui {
class DialogOptionsVisualization;
}

class DialogOptionsVisualization : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOptionsVisualization(QWidget *parent = 0);
    ~DialogOptionsVisualization();
    void getParams(bool *p_accepted, unsigned long long *p_crossing_minimization_time_limit);

private:
    Ui::DialogOptionsVisualization *ui;

public slots:
    void setParams();
};

#endif // DIALOGOPTIONSVISUALIZATION_H
