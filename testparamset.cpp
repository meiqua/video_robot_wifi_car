#include "testparamset.h"
#include "ui_testparamset.h"
#include "mainwindow.h"

TestParamSet::TestParamSet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestParamSet)
{
    ui->setupUi(this);

    cannyParam1 = ui->cannyP1->text().toInt();
    cannyParam2 = ui->cannyP2->text().toInt();

    isAuto = ui->autoSetThre->isChecked();
    isFindLines = ui->findLinesChecked->isChecked();
}

TestParamSet::~TestParamSet()
{
    delete ui;
}

void TestParamSet::on_pushButton_clicked()
{
   cannyParam1 = ui->cannyP1->text().toInt();
   cannyParam2 = ui->cannyP2->text().toInt();

    hide();

}

void TestParamSet::on_autoSetThre_stateChanged(int arg1)
{
    isAuto = ui->autoSetThre->isChecked();
}


void TestParamSet::on_checkBox_stateChanged(int arg1)
{

}

void TestParamSet::on_findLinesChecked_stateChanged(int arg1)
{
    isFindLines = ui->findLinesChecked->isChecked();
}
