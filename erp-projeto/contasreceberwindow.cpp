#include "contasreceberwindow.h"
#include "ui_contasreceberwindow.h"

ContasReceberWindow::ContasReceberWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContasReceberWindow)
{
    ui->setupUi(this);
}

ContasReceberWindow::~ContasReceberWindow()
{
    delete ui;
}
