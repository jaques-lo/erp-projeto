#include "relatoriowindow.h"
#include "ui_relatoriowindow.h"

RelatorioWindow::RelatorioWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RelatorioWindow)
{
    ui->setupUi(this);
}

RelatorioWindow::~RelatorioWindow()
{
    delete ui;
}
