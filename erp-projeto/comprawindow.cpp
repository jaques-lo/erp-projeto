#include "comprawindow.h"
#include "ui_comprawindow.h"

CompraWindow::CompraWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CompraWindow)
{
    ui->setupUi(this);
}

CompraWindow::~CompraWindow()
{
    delete ui;
}
