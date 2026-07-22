#include "permissaowindow.h"
#include "ui_permissaowindow.h"

PermissaoWindow::PermissaoWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PermissaoWindow)
{
    ui->setupUi(this);
}

PermissaoWindow::~PermissaoWindow()
{
    delete ui;
}
