#include "testparamset.h"
#include "ui_testparamset.h"

testparamset::testparamset(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::testparamset)
{
    ui->setupUi(this);
    steps = 0;
}

testparamset::~testparamset()
{
    delete ui;
}

void testparamset::on_verticalSlider_sliderMoved(int position)
{
    steps = position;
    stepReverse();
    emit refresh();
}

void testparamset::stepReverse()
{
    steps = 9 - steps;
}

void testparamset::on_do1_clicked()
{
    do1 = ui->do1->isChecked();
        emit refresh();
}

void testparamset::on_do2_clicked()
{
    do2 = ui->do2->isChecked();
        emit refresh();
}
