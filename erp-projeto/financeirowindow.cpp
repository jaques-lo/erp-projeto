#include "financeirowindow.h"
#include "ui_financeirowindow.h"

FinanceiroWindow::FinanceiroWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FinanceiroWindow)
{
    ui->setupUi(this);
}

FinanceiroWindow::~FinanceiroWindow()
{
    delete ui;
}
