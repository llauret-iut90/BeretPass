#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QtSql>
#include <QDebug>
#include <QFileInfo>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Item {
    QString title;
    QString username;
    QString password;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Login_clicked();
    void on_Register_clicked();
    void on_AddItem_clicked();
    void refreshPasswordList();
    void on_signUp_clicked();
    void on_pushButton_clicked();
    void on_passwordGenerator_clicked();
    void on_passwordGeneratorDetail_clicked();
    void on_copyPassword_clicked();
    void on_copyPasswordDetail_clicked();
    void updateItems();
    void on_DeleteItemDetail_clicked();
    void on_SaveItemDetail_clicked();
    void on_CancelItemDetail_clicked();
private:
    Ui::MainWindow *ui;
    QList<Item> items;
};

#endif // MAINWINDOW_H
