#pragma once
#include <ui_MainWindow.h>
#include <QDialog>
#include <QTcpSocket>
#include <client/CommonItem.h>
#include <client/ChatsWindow.h>
#include <Models.h>
#include <nlohmann/json.hpp>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QTcpSocket *socket, uint64_t user_id, std::string username, QWidget *parent = nullptr);
    ~MainWindow();

    void sendRequest(const nlohmann::json& json);
    uint64_t userId() const { return m_user_id; }

signals:
    void NewMessage(const uint64_t& chat_id, Message msg);
    void HistoryFetch(const uint64_t& chat_id, MessageHistory msg_history);

private slots:
    void onChatCreateClicked();
    void onChatDoubleClicked(QListWidgetItem *item);
    void onUserDoubleClicked(QListWidgetItem *item);
    void onSocketReadyRead();
    void onChatsWindowClosed();

private:
    void openChatInWindow(uint64_t chat_id, const QString& chat_name,
         const std::vector<UserPreview>& users);

    ChatsWindow* m_chats_window = nullptr;
    Ui::MainWindow *ui;
    QTcpSocket *m_socket;
    uint64_t m_user_id;
    std::string m_username;
};
