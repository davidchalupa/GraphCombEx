#ifndef DIALOGOPTIONSALGORITHMS_H
#define DIALOGOPTIONSALGORITHMS_H

#include <QDialog>

#include "common.h"

namespace Ui {
class DialogOptionsAlgorithms;
}

class DialogOptionsAlgorithms : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOptionsAlgorithms(QWidget *parent = 0);
    ~DialogOptionsAlgorithms();

private:
    Ui::DialogOptionsAlgorithms *ui;

public slots:
    void setParams();
};

#endif // DIALOGOPTIONSALGORITHMS_H
