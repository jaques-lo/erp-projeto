#include "produtowindow.h"
#include "ui_produtowindow.h"

ProdutoWindow::ProdutoWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProdutoWindow)
{
    ui->setupUi(this);
}

ProdutoWindow::~ProdutoWindow()
{
    delete ui;
}
