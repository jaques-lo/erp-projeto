#include "clientewindow.h"
#include "ui_clientewindow.h"

ClienteWindow::ClienteWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClienteWindow)
{
    ui->setupUi(this);
}

ClienteWindow::~ClienteWindow()
{
    delete ui;
}
