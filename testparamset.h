#ifndef TESTPARAMSET_H
#define TESTPARAMSET_H

#include <QDialog>

namespace Ui {
class testparamset;
}

class testparamset : public QDialog
{
    Q_OBJECT

public:
    explicit testparamset(QWidget *parent = 0);
    ~testparamset();
    int steps;
    bool do1;
    bool do2;

private slots:
    void on_verticalSlider_sliderMoved(int position);

    void on_do1_clicked();

    void on_do2_clicked();

    void on_verticalSlider_actionTriggered(int action);

    void valueChanged(int);

signals:
    void refresh();

private:
    Ui::testparamset *ui;
    void stepReverse();

};

#endif // TESTPARAMSET_H
