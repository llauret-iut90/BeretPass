#include "headers/mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QMessageBox>
#include <QClipboard>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFile file(":qss/resources/styles.qss");
    if (file.open(QIODevice::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
        qDebug() << "Stylesheet loaded";
    }

    ui->or_label->setAlignment(Qt::AlignCenter);

    QPixmap pix(":/img/resources/BeretPassLogo.png");
    ui->label_pic->setPixmap(pix.scaled(200, 200));
    ui->stackedWidget->setCurrentIndex(0);

    //sql
    QSqlDatabase beretpass = QSqlDatabase::addDatabase("QSQLITE");
    //chemin du répertoire parent du projet
    QString projectDir = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();
    qDebug() << "chemin" << projectDir;
    //chemin du répertoire parent du projet
    QString dbPath = QDir(projectDir).absolutePath() + "/beret_pass.db";
    qDebug() << "chemin" << dbPath;
    beretpass.setDatabaseName(dbPath);

    // Ouvrir la base de données
    if (!beretpass.open()) {
        QPixmap pix(":img/resources/db_red.png");
        ui->info_db->setPixmap(pix.scaled(30,30));
        qDebug() << "Database file path:" << beretpass.databaseName();
        qDebug() << "Error opening database:" << beretpass.lastError().text();
    }
    else {
        QPixmap pix(":img/resources/db_green.png");
        ui->info_db->setPixmap(pix.scaled(30,30));
        qDebug() << "Database" << beretpass.isOpen();
        QSqlDatabase db = QSqlDatabase::database(); // Obtenez la base de données actuellement ouverte
        QStringList tables = db.tables(); // Récupérez la liste des tables dans la base de données

        qDebug() << "List of tables in the database:";

        // Parcourez la liste des tables et affichez les noms de table
        foreach (const QString &table, tables) {
            qDebug() << table;
        }

        // Vérifier si la base de données est vide en regardant si elle contient des tables
        // obligé de faire une par une sinon : "not an error Unable to execute multiple statements at a time"
        if (tables.isEmpty()) {
            qDebug() << "Database is empty. Creating tables...";

            // Exécuter les instructions SQL pour créer la table 'password'
            QSqlQuery query(beretpass);
            if (!query.exec("PRAGMA foreign_keys = off;")) {
                qDebug() << "Error disabling foreign keys:" << query.lastError().text();
            }
            if (!query.exec("BEGIN TRANSACTION;")) {
                qDebug() << "Error beginning transaction:" << query.lastError().text();
            }
            if (!query.exec("DROP TABLE IF EXISTS password;")) {
                qDebug() << "Error dropping table password:" << query.lastError().text();
            }
            if (!query.exec("CREATE TABLE IF NOT EXISTS password ( "
                            "    id       INTEGER PRIMARY KEY AUTOINCREMENT "
                            "                     NOT NULL "
                            "                     UNIQUE, "
                            "    title    TEXT    NOT NULL, "
                            "    username TEXT    NOT NULL, "
                            "    password TEXT    NOT NULL, "
                            "    user_id  INTEGER REFERENCES users (id)  "
                            ");")) {
                qDebug() << "Error creating table password:" << query.lastError().text();
            }

            // Exécuter les instructions SQL pour créer la table 'users'
            if (!query.exec("DROP TABLE IF EXISTS users;")) {
                qDebug() << "Error dropping table users:" << query.lastError().text();
            }
            if (!query.exec("CREATE TABLE IF NOT EXISTS users ( "
                            "    id            INTEGER PRIMARY KEY AUTOINCREMENT "
                            "                          UNIQUE "
                            "                          NOT NULL, "
                            "    username      TEXT    NOT NULL "
                            "                          UNIQUE, "
                            "    main_password TEXT    NOT NULL "
                            ");")) {
                qDebug() << "Error creating table users:" << query.lastError().text();
            }

            // Finaliser la transaction
            if (!query.exec("COMMIT TRANSACTION;")) {
                qDebug() << "Error committing transaction:" << query.lastError().text();
            }

            // Réactiver les clés étrangères
            if (!query.exec("PRAGMA foreign_keys = on;")) {
                qDebug() << "Error enabling foreign keys:" << query.lastError().text();
            }
        }
        else {
            qDebug() << "Database contains tables. No need to create schema.";
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Bouton cancel pour retourner au menu il apparît dans la page de register
void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

/*
 * Cette fonction est appelée lorsque le bouton "signUp" est cliqué dans l'interface utilisateur.
 * Elle récupère le nom d'utilisateur et le mot de passe entrés par l'utilisateur, puis les insère dans la table "users" de la base de données.
 * Si l'insertion réussit, un message de bienvenue est affiché à l'utilisateur.
 * Si le nom d'utilisateur est déjà présent dans la base de données, un avertissement est affiché.
 */
void MainWindow::on_signUp_clicked()
{
    QString username = ui->inputUsername_2->text();
    QString password = ui->inputPassword_2->text();

    QSqlQuery query;
    query.prepare("INSERT INTO users (username, main_password) VALUES (:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        QMessageBox::warning(this, "signUp", "Username already exists!");
        return;
    }

    if (query.numRowsAffected() > 0) {
        QString message = QString("Account created with username: %1").arg(username);
        QMessageBox::information(this, "signUp", message);
        ui->stackedWidget->setCurrentIndex(0);
    }
}

// Change de page pour aller sur la page de register
void MainWindow::on_Register_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


/*
 * Va vérifier si l'user existe bien dans la bdd avec un select
 */
void MainWindow::on_Login_clicked()
{
    QString username = ui->inputUsername->text();
    QString password = ui->inputPassword->text();

    // Vérification des champs vides
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login", "Please enter an username and a password.");
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
        QMessageBox::critical(this, "Error", "Error during execution of the request!");
        return;
    }

    // Vérification des résultats de la requête
    if (query.exec() && query.next()) {
        ui->loggedAs->setText(username);
        ui->stackedWidget->setCurrentIndex(1);
        qDebug() << "Login successful";
        refreshPasswordList();
    } else {
        qDebug() << "No rows returned.";
        QMessageBox::warning(this, "Login", "Incorrect username or password!");
    }
}

/*
 * Cette fonction est appelée lorsque le bouton "AddItem" est cliqué dans l'interface utilisateur.
 * Elle récupère le titre, le nom d'utilisateur et le mot de passe entrés par l'utilisateur.
 * Ensuite, elle vérifie si tous les champs sont remplis.
 * Si des champs sont vides, un avertissement est affiché à l'utilisateur.
 * Sinon, elle exécute une requête SQL pour insérer les données dans la table "password".
 * La requête utilise une jointure pour lier l'utilisateur à son identifiant dans la table "users".
 * Si l'insertion réussit, l'interface utilisateur est mise à jour pour afficher le nouvel élément ajouté.
 */
void MainWindow::on_AddItem_clicked()
{
    QString currentUser = ui->loggedAs->text();
    QString title = ui->titleAddItemInput->text();
    QString username = ui->usernameAddItemInput->text();
    QString password = ui->passwordAddItemInput->text();

    if (title.isEmpty() || username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Add Item", "Please fill all the fields.");
    } else {
        QSqlQuery query;
        query.prepare("INSERT INTO password (title, username, password, user_id) SELECT :title, :username, :password, users.id FROM users WHERE users.username = :main_user;");
        query.bindValue(":title", title);
        query.bindValue(":username", username);
        query.bindValue(":password", password);
        query.bindValue(":main_user", currentUser);

        if (!query.exec()) {
            qDebug() << "Error executing query:" << query.lastError().text();
            QMessageBox::warning(this, "signUp", "Error to insert this password!");
            return;
        }

        if (query.numRowsAffected() > 0) {
            ui->titleAddItemInput->clear();
            ui->usernameAddItemInput->clear();
            ui->passwordAddItemInput->clear();
            Item newItem = {title, username, password};
            items.append(newItem);

            refreshPasswordList();
        }
    }
}

void MainWindow::refreshPasswordList() {
    ui->passwordsList->clear();

    // Récupérer le nom d'utilisateur actuellement connecté
    QString currentUser = ui->loggedAs->text();

    // Requête SQL pour récupérer les mots de passe de l'utilisateur connecté
    QSqlQuery query;
    query.prepare("SELECT password.title FROM password JOIN users ON users.id = password.user_id WHERE users.username = :username;");
    query.bindValue(":username", currentUser);

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        return;
    }

    // Ajouter chaque titre de mot de passe dans la liste
    while (query.next()) {
        QString title = query.value(0).toString();
        ui->passwordsList->addItem(title);
    }
}



void MainWindow::on_passwordGenerator_clicked()
{
    const QString characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+";
    const int passwordLength = 12;

    QString generatedPassword;

    // Génération du mot de passe aléatoire
    for (int i = 0; i < passwordLength; ++i) {
        int index = QRandomGenerator::global()->bounded(characters.length());
        QChar character = characters.at(index);
        generatedPassword.append(character);
    }

    qDebug() << "Mot de passe généré:" << generatedPassword;
    ui->generatedPassword->setText(generatedPassword);
}

void MainWindow::on_copyPassword_clicked()
{
    QString textToCopy = ui->generatedPassword->text();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(textToCopy);
    qDebug() << "Texte copié dans le presse-papiers :" << textToCopy;
}
