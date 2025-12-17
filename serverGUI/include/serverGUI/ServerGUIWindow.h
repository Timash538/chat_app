#pragma once
#include <ui_ServerGUIWindow.h>
#include <QDialog>
#include <QTcpSocket>
#include <Models.h>
#include <nlohmann/json.hpp>

class ServerGUIWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ServerGUIWindow(QWidget *parent = nullptr);
    ~ServerGUIWindow();

    void sendRequest(const nlohmann::json& json);

private slots:
    void onBanPushed();
    void onKickPushed();
    void onUserClicked(QListWidgetItem* item);
    void onChatClicked(QListWidgetItem* item);
    void onSocketReadyRead();

private:
    void connectToServer();
    Ui::ServerGUIWindow *ui;
    QTcpSocket *m_socket;
    uint64_t user_id_selected;
};
