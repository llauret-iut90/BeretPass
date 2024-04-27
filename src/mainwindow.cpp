#include "headers/mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPixmap pix(":/img/resources/BeretPassLogo.png");
    ui ->label_pic->setPixmap(pix.scaled(200, 200));
}

MainWindow::~MainWindow()
{
    delete ui;
}
