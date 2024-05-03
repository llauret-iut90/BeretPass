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

    // Populate passwordsList with items from the items list
    for (const Item& item : items) {
        QString itemText = QString("%1 - %2").arg(item.title).arg(item.username);
        ui->passwordsList->addItem(itemText);
    }
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

    if (username == "admin" && password == "admin") {
        QMessageBox::information(this, "Login", message);
        ui->stackedWidget->setCurrentIndex(1);
    } else {
        QMessageBox::warning(this, "Login", "Tu n'es pas la personne que tu pretends être !");
    }
}


void MainWindow::on_Register_clicked()
{

}

void MainWindow::on_AddItem_clicked()
{
    QString title = ui->titleAddItemInput->text();
    QString username = ui->usernameAddItemInput->text();
    QString password = ui->passwordAddItemInput->text();

    if (title.isEmpty() || username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Add Item", "Please fill all the fields");
    } else {
        ui->titleAddItemInput->clear();
        ui->usernameAddItemInput->clear();
        ui->passwordAddItemInput->clear();

        Item newItem = {title, username, password};
        items.append(newItem);

        refreshPasswordList();
    }
}

void MainWindow::refreshPasswordList() {
    ui->passwordsList->clear();
    for (const Item& item : items) {
        QString itemText = QString("%1").arg(item.title);
        ui->passwordsList->addItem(itemText);
    }
}
