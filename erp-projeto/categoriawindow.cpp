#include "categoriawindow.h"
#include "ui_categoriawindow.h"

CategoriaWindow::CategoriaWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CategoriaWindow)
{
    ui->setupUi(this);
}

CategoriaWindow::~CategoriaWindow()
{
    delete ui;
}
