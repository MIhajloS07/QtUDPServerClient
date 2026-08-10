#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QLCDNumber>
#include "Client.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_sendButton_clicked();

    void on_exitButton_clicked();

    void updateConnectionStatus();

    void on_commandLinkButton_clicked();

private:
    Ui::MainWindow *ui;
    Client *client; // UDP client
    QLabel *statusLabel;
    QTimer *connectionTimer;
    QLCDNumber *lcdNumber;
};
#endif // MAINWINDOW_H
