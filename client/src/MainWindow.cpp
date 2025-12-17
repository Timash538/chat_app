#include <client/MainWindow.h>
#include <QMessageBox>
#include <QDebug>
#include <Models.h>

MainWindow::MainWindow(QTcpSocket *socket, uint64_t user_id, std::string username, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_socket(std::move(socket)),
    m_user_id(user_id),
    m_username(username)
{
    
    disconnect(m_socket, nullptr, nullptr, nullptr);
    ui->setupUi(this);
    setWindowTitle(QString::fromStdString(username));
    // Подключаем кнопки
    connect(ui->createChatPushButton, &QPushButton::clicked, this, &MainWindow::onChatCreateClicked);
    connect(ui->usersList, &QListWidget::itemDoubleClicked, this, &MainWindow::onUserDoubleClicked);
    connect(ui->chatsList, &QListWidget::itemDoubleClicked, this, &MainWindow::onChatDoubleClicked);

    connect(m_socket, &QTcpSocket::disconnected, this, [this]() {
        qDebug() << "Disconnected from server";
    });

    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::onSocketReadyRead);

    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, [this](QAbstractSocket::SocketError error) {
                QMessageBox::critical(this, "Connection Error",
                                      QString("Connection failed: %1").arg(m_socket->errorString()));
            });

    try {
        // Формируем JSON для логина
        sendRequest({{ "cmd","fetch_users" }});
        sendRequest({{ "cmd","fetch_chats" }});
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
                              QString("Failed to create request: %1").arg(e.what()));
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onChatCreateClicked() {
 // открыть диалог создания общих чатов
}

void MainWindow::onUserDoubleClicked(QListWidgetItem *item) {
 // кинуть в сервер сообщение о создании нового direct чата с юзером и открыть чат с ним/создать таб в открытом окне чатов
    CommonItem *commonItem = dynamic_cast<CommonItem*>(item);
    try {
        // Формируем JSON для логина
        nlohmann::json request = {
            {"cmd", "create_chat"},
            {"type", "direct"},
            {"user_ids", {m_user_id,commonItem->id}}
        };

        // Отправляем на сервер
        QString jsonStr = QString::fromStdString(request.dump() + "\n");
        m_socket->write(jsonStr.toUtf8());

        qDebug() << "Login request sent:" << jsonStr;
        // открываем окно уже в onReadyRead при получении ответа, что чат создан
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
                              QString("Failed to create request: %1").arg(e.what()));
    }
}

void MainWindow::onChatDoubleClicked(QListWidgetItem *item) {
// открыть окно чата/добавить tab в открытом окне чатов
    CommonItem *commonItem = dynamic_cast<CommonItem*>(item);
    try {
        // Формируем JSON для логина
        nlohmann::json request = {
            {"cmd", "fetch_chat"},
            {"chat_id", commonItem->id}
        };

        // Отправляем на сервер
        QString jsonStr = QString::fromStdString(request.dump() + "\n");
        m_socket->write(jsonStr.toUtf8());

        qDebug() << "Login request sent:" << jsonStr;
        // открываем окно уже в onReadyRead при получении ответа, что чат создан
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
                              QString("Failed to create request: %1").arg(e.what()));
    }
}

void MainWindow::onSocketReadyRead() {
    while (m_socket->canReadLine()) {
        QByteArray line = m_socket->readLine().trimmed();
        qDebug() << "Server response:" << line;

        try {
            auto response = nlohmann::json::parse(line.constData());
            if (response.contains("error")) {
                QString errorMsg = QString::fromStdString(response["error"].get<std::string>());
                QMessageBox::warning(this, "Server Error",
                    QString("Error: %1").arg(errorMsg));
            }
            else if (response.contains("cmd")) {

                QString cmd = QString::fromStdString(response["cmd"].get<std::string>());
                if (cmd == "chat_list") {
                    ui->chatsList->clear();
                    auto chatList = response["chats"].get<ChatList>();
                    for (auto& i : chatList.chats)
                    {
                        ui->chatsList->addItem(new CommonItem{ QString::fromStdString(i.name),i.id,i.name });
                    }
                }
                if (cmd == "user_list") {
                    ui->usersList->clear();
                    auto userList = response["users"].get<UserList>();
                    for (auto& i : userList.users)
                    {
                        auto newItem = new CommonItem{ QString::fromStdString(i.username),i.id,i.username };
                        if (response.contains("users_online"))
                        {
                            std::vector<uint64_t> users_online = response["users_online"];
                            for (auto& i : users_online) if (newItem->id == i) newItem->setForeground(Qt::green);
                        }
                        ui->usersList->addItem(newItem);
                    }
                }
                if (cmd == "chat") {
                    uint64_t chat_id = response["chat"]["id"].get<uint64_t>();
                    std::string chat_name = response["chat"]["name"].get<std::string>();
                    auto users = response["chat"]["users"].get<std::vector<UserPreview>>();

                    openChatInWindow(chat_id, QString::fromStdString(chat_name), users);
                }
                if (cmd == "new_user") {
                    sendRequest({ { "cmd","fetch_users" } });
                }
                if (cmd == "new_chat") {
                    sendRequest({ { "cmd","fetch_chats" } });
                }
                if (cmd == "new_message")
                {
                    emit NewMessage(response["chat_id"], response["message"]);
                }
                if (cmd == "message_history")
                {
                    emit HistoryFetch(response["message_history"]["chat_id"], response["message_history"]);
                }
            }
        }
        catch (const std::exception& e) {
            QMessageBox::critical(this, "Parse Error",
                QString("Failed to parse server response: %1").arg(e.what()));
        }
    }
}

void MainWindow::onChatsWindowClosed()
{
    qDebug() << "ChatsWindow уничтожен!";
    disconnect(m_chats_window, &ChatsWindow::closed, this, nullptr);
    if (m_chats_window)
    delete m_chats_window;
    m_chats_window = nullptr;
}

void MainWindow::onNewChatCreated() 
{

}

void MainWindow::onNewUserRegistered() 
{

}

void MainWindow::sendRequest(const nlohmann::json& request)
{
    QString jsonStr = QString::fromStdString(request.dump() + "\n");
    m_socket->write(jsonStr.toUtf8());

    qDebug() << "Request sent:" << jsonStr;

}

 void MainWindow::openChatInWindow(uint64_t chat_id, const QString& chat_name,
     const std::vector<UserPreview>& users) {
     if (!m_chats_window) {
             m_chats_window = new ChatsWindow(this);  // Передаём MainWindow!
             connect(m_chats_window, &ChatsWindow::closed, this, &MainWindow::onChatsWindowClosed);
             m_chats_window->show();
     }

     m_chats_window->openChat(chat_id, chat_name, users);
     m_chats_window->raise();  // Поднимаем на передний план
 }