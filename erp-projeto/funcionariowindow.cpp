#include "funcionariowindow.h"
#include "ui_funcionariowindow.h"

FuncionarioWindow::FuncionarioWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FuncionarioWindow)
{
    ui->setupUi(this);
}

FuncionarioWindow::~FuncionarioWindow()
{
    delete ui;
}
