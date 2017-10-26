#ifndef DIALOGCOMPLETETREE_H
#define DIALOGCOMPLETETREE_H

#include <QDialog>
#include "common.h"

namespace Ui {
class DialogCompleteTree;
}

class DialogCompleteTree : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCompleteTree(QWidget *parent = 0);
    ~DialogCompleteTree();
    void getParams(bool *accepted, refer *p_b, refer *p_d);

private:
    Ui::DialogCompleteTree *ui;
    bool accepted;
    refer b;
    refer d;

public slots:
    void setParams();
};

#endif // DIALOGCOMPLETETREE_H
