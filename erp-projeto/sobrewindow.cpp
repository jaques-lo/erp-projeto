#include "sobrewindow.h"
#include "ui_sobrewindow.h"

SobreWindow::SobreWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SobreWindow)
{
    ui->setupUi(this);
}

SobreWindow::~SobreWindow()
{
    delete ui;
}
