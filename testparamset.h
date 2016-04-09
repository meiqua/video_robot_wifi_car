#ifndef TESTPARAMSET_H
#define TESTPARAMSET_H

#include <QDialog>

namespace Ui {
class TestParamSet;
}

class TestParamSet : public QDialog
{
    Q_OBJECT

public:
    explicit TestParamSet(QWidget *parent = 0);
    ~TestParamSet();

     int cannyParam1;
     int cannyParam2;
     bool isAuto;
     bool isFindLines;

private slots:
    void on_pushButton_clicked();

    void on_autoSetThre_stateChanged(int arg1);

    void on_checkBox_stateChanged(int arg1);

    void on_findLinesChecked_stateChanged(int arg1);

private:
    Ui::TestParamSet *ui;


};

#endif // TESTPARAMSET_H
