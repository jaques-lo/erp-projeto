#include "contaspagarwindow.h"
#include "ui_contaspagarwindow.h"

ContasPagarWindow::ContasPagarWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContasPagarWindow)
{
    ui->setupUi(this);
}

ContasPagarWindow::~ContasPagarWindow()
{
    delete ui;
}
