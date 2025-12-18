#pragma once

#include <QDialog>
#include <QTcpSocket>
#include <nlohmann/json.hpp>
#include <ui_AuthDialog.h>

class AuthDialog : public QDialog {
    Q_OBJECT

public:
    explicit AuthDialog(QWidget *parent = nullptr);
    ~AuthDialog();

    uint64_t userId() const { return m_userId; }
    QTcpSocket* getSocket() {return m_socket;}
    std::string getUsername() {return m_username;}
private slots:
    void onLToggleClicked();
    void onRToggleClicked();
    void onLoginButtonClicked();  
    void onSocketReadyRead();     

    void onRegisterButtonClicked();

private:
    void connectToServer();

    Ui::Dialog *ui;
    QTcpSocket *m_socket;
    uint64_t m_userId;
    std::string m_username;
};
