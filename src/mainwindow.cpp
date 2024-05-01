#include "headers/mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QMessageBox>

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

void MainWindow::on_Login_clicked()
{
    QString username = ui->inputUsername->text();
    QString password = ui->inputPassword->text();
    QString message = QString("Salut à toi %1").arg(username);

    if(username == "admin" && password == "admin"){
        QMessageBox::information(this, "Login", message);
        ui->stackedWidget->setCurrentIndex(1);

    }else {
        QMessageBox::warning(this, "Login", "Tu n'es pas la personne que tu pretends être !");
    }
}


void MainWindow::on_Register_clicked()
{

}

