#include "fornecedorwindow.h"
#include "ui_fornecedorwindow.h"

FornecedorWindow::FornecedorWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FornecedorWindow)
{
    ui->setupUi(this);
}

FornecedorWindow::~FornecedorWindow()
{
    delete ui;
}
