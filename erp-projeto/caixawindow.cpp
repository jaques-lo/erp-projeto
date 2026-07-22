#include "caixawindow.h"
#include "ui_caixawindow.h"

CaixaWindow::CaixaWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CaixaWindow)
{
    ui->setupUi(this);
}

CaixaWindow::~CaixaWindow()
{
    delete ui;
}
