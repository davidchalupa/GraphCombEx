#ifndef LARGEVISFORM_H
#define LARGEVISFORM_H

#include <QWidget>

#include "common.h"

namespace Ui {
class LargeVisForm;
}

class LargeVisForm : public QWidget
{
    Q_OBJECT

public:
    explicit LargeVisForm(QWidget *parent = 0);
    ~LargeVisForm();
    void setVisualization(QPixmap *pixmap_graph_visualization, refer current_visualization_size);

private:
    Ui::LargeVisForm *ui;
};

#endif // LARGEVISFORM_H
