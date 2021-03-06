#include "login.h"
#include "ui_login.h"

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    this->setWindowTitle("Login");
    admins = getUsers();
    if(admins.size() == 0)
    {
        admins.push_back(new User("admin", "admin", true));
    }
}

login::~login()
{
    saveUsersToFile();
    for(int i = 0; i < admins.size(); ++i)
        delete admins[i];

    delete ui;
}

void login::on_pushButton_Login_clicked()
{
    QString username = ui->lineEdit_Username->text();
    QString password = ui->lineEdit_Password->text();
    if (checkCorrectLogin(username, password))
    {
        emit userIsAdmin();// Right now all users are admin
        this->close();
    }
    else
    {
        QMessageBox::information(this, "Error", "Username and Password is not Correct");
    }
}

bool login::checkCorrectLogin(QString username, QString password)
{
    for (int i = 0; i < admins.size(); i++)
    {
        if (admins[i]->correctLogin(username, password))
        {
            return true;
        }
    }
    return false;
}

void login::saveUsersToFile()
{
    QFile file("Users.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    for (int i = 0; i < admins.size(); i++)
    {
        out << admins[i]->getName()<< "\n";
        out << admins[i]->getPassword()<< "\n"; //
        if (admins[i]->adminStatus())
        {
            out << "true";
        }
        else
        {
            out << "false";
        }

        if(i + 1 != admins.size())
        {
            out<<"\n";
        }
    }
    file.close();
}

QVector<User*> login::getUsers()
{
    QVector<User*>fromFile;

    QFile file("Users.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!file.exists())
    {
        return fromFile;
    }
    QTextStream in(&file);
    QString username;
    QString password;
    QString adminText;
    bool isAdmin;
    while (!file.atEnd())
    {
        in >> username;
        in >> password;
        in >> adminText;
        if ( adminText == "true")
        {
            isAdmin = true;
        }
        else
        {
            isAdmin = false;
        }
        fromFile.push_back(new User(username, password, isAdmin));
    }
    file.close();

    return fromFile;
}

void login::on_pushButton_NewAccount_clicked()
{
    QString username = ui->lineEdit_Username->text();
    QString password = ui->lineEdit_Password->text();

    if(checkCorrectLogin(username, password))
    {
        QMessageBox::information(this, "Error", "User Already Exists!!");
    }
    else
    {
        admins.push_back(new User(username, password, true));
    }

    ui->lineEdit_Username->clear();
    ui->lineEdit_Password->clear();
    saveUsersToFile();
}
