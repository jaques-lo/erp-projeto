#include "estoquewindow.h"
#include "ui_estoquewindow.h"

EstoqueWindow::EstoqueWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EstoqueWindow)
{
    ui->setupUi(this);
}

EstoqueWindow::~EstoqueWindow()
{
    delete ui;
}
