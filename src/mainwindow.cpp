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

    QPixmap pix(":/img/resources/BeretPassLogo.png");
    ui ->label_pic->setPixmap(pix.scaled(200, 200));
    ui->stackedWidget->setCurrentIndex(0);

    //sql
    QSqlDatabase beretpass = QSqlDatabase::addDatabase("QSQLITE");
    //chemin du répertoire du projet
    /*
     *❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗
     *❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗
     *❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗
     * VÉRIFIE SI ÇA MARCHE DE TON CôTÉ LE CHEMIN
     *❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗
     *❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗
     *❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗
     */
    QString projectDir = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();

    //chemin du répertoire parent du projet
    QString dbPath = QDir(projectDir).absolutePath() + "/beret_pass.db";
    beretpass.setDatabaseName(dbPath);


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
    };
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
        QMessageBox::warning(this, "signUp", "Nom d'utilisateur déjà existant !");
        return;
    }

    if (query.numRowsAffected() > 0) {
        QString message = QString("Bienvenue %1").arg(username);
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
        ui->loggedAs->setText(username);
        QMessageBox::information(this, "Login", message);
        ui->stackedWidget->setCurrentIndex(1);
        qDebug() << "JE SUIS DANS LA PAGE DE MDP";
        refreshPasswordList();
    } else {
        qDebug() << "No rows returned.";
        QMessageBox::warning(this, "Login", "Nom d'utilisateur ou mot de passe incorrect !");
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
        QMessageBox::warning(this, "Add Item", "Please fill all the fields");
    } else {
        QSqlQuery query;
        query.prepare("INSERT INTO password (title, username, password, user_id) SELECT :title, :username, :password, users.id FROM users WHERE users.username = :main_user;");
        query.bindValue(":title", title);
        query.bindValue(":username", username);
        query.bindValue(":password", password);
        query.bindValue(":main_user", currentUser);

        if (!query.exec()) {
            qDebug() << "Error executing query:" << query.lastError().text();
            QMessageBox::warning(this, "signUp", "Ne peux pas ajouter le mdp !");
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
