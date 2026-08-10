#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QTime>
#include <thread>
#include <iostream>
#include "Server.h"

#define IP_ADDRESS "127.0.0.1"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusLabel = new QLabel("Disconnected", this);
    statusBar()->addPermanentWidget(statusLabel);

    client = new Client(IP_ADDRESS, 8888);

    std::thread serverThread([](){
        try{
            Server server(8888);
            server.run();
        } catch (const std::runtime_error& e){
            std::cerr << "Server error: " << e.what() << std::endl;
        }
    });
    serverThread.detach(); // set thread to work parallel

    connectionTimer = new QTimer(this);
    connect(connectionTimer,
            &QTimer::timeout,
            this,
            &MainWindow::updateConnectionStatus);
    connectionTimer->start(5000); // check connection every 5s
}

MainWindow::~MainWindow()
{
    delete client;
    delete ui;
}

void MainWindow::on_sendButton_clicked()
{
    QString command = ui->commandInput->text(); // input command
    try{
        client->send(command.toStdString()); // send command to server
        std::string reply = client->receiveMessage(); // reply from client command

        QTime now = QTime::currentTime(); // calculate time when server reply with message
        QString timeStr = now.toString("HH:mm:ss");
        QString logEntry = QString("[%1] %2")
                           .arg(timeStr)
                           .arg(QString::fromStdString(reply));

        // ui update
        ui->lcdNumber->display(ui->lcdNumber->intValue() + 1);
        ui->outputBox->append(logEntry);
        ui->commandInput->clear();
    } catch (const std::runtime_error& e){
        ui->outputBox->append(QString("Error: ") + e.what());
    }
}

void MainWindow::on_exitButton_clicked()
{
    auto reply = QMessageBox::question(
        this,
        "Exit confirmation",
        "Do you want to exit app?",
        QMessageBox::Yes | QMessageBox::No
    );
    if (reply == QMessageBox::Yes) this->close();
}

void MainWindow::on_commandLinkButton_clicked()
{
    QMessageBox::information(
        this,
        "Command list",
        "/ping\t\t→ pong\n"
        "/send <msg>\t→ echo message\n"
        "/time\t\t→ current time\n"
        "/date\t\t→ current date\n"
        "/datetime\t→ date + time\n"
        "/uptime\t→ server uptime\n"
        "/ip\t\t→ client IP\n"
        "/exit\t\t→ close app"
        );
}

void MainWindow::updateConnectionStatus(){
    if (client->checkConnection()){
        statusLabel->setText("Connected");
        statusLabel->setStyleSheet("color: green;");
    } else {
        statusLabel->setText("Disconnected");
        statusLabel->setStyleSheet("color: red;");
    }
}