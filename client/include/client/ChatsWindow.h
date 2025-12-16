#pragma once

#include <QMainWindow>
#include <ui_ChatsWindow.h>
#include <Models.h>

class MainWindow;

class ChatsWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit ChatsWindow(MainWindow* main_window, QWidget* parent = nullptr);
    ~ChatsWindow();

    void openChat(uint64_t chat_id, const QString& chat_name,
        const std::vector<UserPreview>& users);

protected:
    void closeEvent(QCloseEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

signals:
    void closed();

private slots:
    void onMessageEntryReturnPressed(uint64_t chat_id, QString msg);
    void onNewMessage(uint64_t chat_id, Message msg);
    void onHistoryFetch(uint64_t chat_id, MessageHistory msg_history);

private:

    QWidget* createNewTabFromPrototype();
    struct ChatTabData
    {
        uint64_t id;
        QString name;
        std::vector<UserPreview> users;

        bool is_active = false;
    };

    QWidget* m_prototype = nullptr;
    Ui::ChatsWindow* ui;
    MainWindow* m_main_window;
    uint64_t m_current_chat_id = 0;
    std::map<uint, ChatTabData> m_chat_tabs;
};
