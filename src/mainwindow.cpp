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
    ui->stackedWidget->setCurrentIndex(0);

    //sql
    QSqlDatabase beretpass = QSqlDatabase::addDatabase("QSQLITE");
    // Obtenez le chemin du répertoire du projet
    QString projectDir = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();

    // Obtenez le chemin du répertoire parent du projet
    QString dbPath = QDir(projectDir).absolutePath() + "/beret_pass.db";
    beretpass.setDatabaseName(dbPath);


    if (!beretpass.open()) {
        ui->info_db->setText("Erreur bdd");
        qDebug() << "Database file path:" << beretpass.databaseName();
        qDebug() << "Error opening database:" << beretpass.lastError().text();
    }
    else {
        ui->info_db->setText("C'est connecté ! :)");
        qDebug() << "Database" << beretpass.isOpen();
        QSqlDatabase db = QSqlDatabase::database(); // Obtenez la base de données actuellement ouverte
        QStringList tables = db.tables(); // Récupérez la liste des tables dans la base de données

        qDebug() << "List of tables in the database:";

        // Parcourez la liste des tables et affichez les noms de table
        foreach (const QString &table, tables) {
            qDebug() << table;
        }
    };


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

    // Vérification des champs vides
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login", "Veuillez entrer un nom d'utilisateur et un mot de passe !");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT username FROM users WHERE username = :username AND main_password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    // Affichage de la requête SQL pour le débogage
    qDebug() << "Query:" << query.lastQuery();
    query.exec();

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Erreur lors de l'exécution de la requête !");
        return;
    }

    // Vérification des résultats de la requête
    if (query.exec() && query.next()) {
        QString message = QString("Salut à toi %1").arg(username);
        QMessageBox::information(this, "Login", message);
        ui->stackedWidget->setCurrentIndex(1);
    } else {
        qDebug() << "No rows returned.";
        QMessageBox::warning(this, "Login", "Nom d'utilisateur ou mot de passe incorrect !");
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
