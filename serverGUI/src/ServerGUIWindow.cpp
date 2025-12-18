#include <serverGUI/ServerGUIWindow.h>
#include <serverGUI/UserInfoItem.h>
#include <serverGUI/ChatInfoItem.h>
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <Models.h>

ServerGUIWindow::ServerGUIWindow(QWidget *parent)
    : QMainWindow(parent),
    m_socket(new QTcpSocket(this)),
    ui(new Ui::ServerGUIWindow)
{
    ui->setupUi(this);
    // Подключаем кнопки
    connect(ui->user_delete, &QPushButton::clicked, this, &ServerGUIWindow::onBanPushed);
    connect(ui->user_kick, &QPushButton::clicked, this, &ServerGUIWindow::onKickPushed);
    connect(ui->usersList, &QListWidget::itemClicked, this, &ServerGUIWindow::onUserClicked);
    connect(ui->chatsList, &QListWidget::itemClicked, this, &ServerGUIWindow::onChatClicked);

    connect(m_socket, &QTcpSocket::disconnected, this, [this]() {
        qDebug() << "Disconnected from server";
    });

    connect(m_socket, &QTcpSocket::readyRead, this, &ServerGUIWindow::onSocketReadyRead);

    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, [this](QAbstractSocket::SocketError error) {
                QMessageBox::critical(this, "Connection Error",
                                      QString("Connection failed: %1").arg(m_socket->errorString()));
            });
    connectToServer();
    try {
        // Формируем JSON для логина
        sendRequest({{ "cmd","admin_fetch_all" }});
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
                              QString("Failed to create request: %1").arg(e.what()));
    }
}

ServerGUIWindow::~ServerGUIWindow() {
    delete ui;
}

void ServerGUIWindow::sendRequest(const nlohmann::json& request)
{
    try {
        QString jsonStr = QString::fromStdString(request.dump() + "\n");
        m_socket->write(jsonStr.toUtf8());

        qDebug() << "Request sent:" << jsonStr;
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "Error",
                          QString("Failed to create request: %1").arg(e.what()));
    }
}

void ServerGUIWindow::onUserClicked(QListWidgetItem *item) {
    auto user = dynamic_cast<UserInfoItem*>(item);
    user_id_selected = user->user_id;
    (user->is_deleted) ? ui->user_delete->setText("Разбанить пользователя") : ui->user_delete->setText("Забанить пользователя");
}

void ServerGUIWindow::onChatClicked(QListWidgetItem* item) {
    auto chat = dynamic_cast<ChatInfoItem*>(item);
    sendRequest({ {"cmd","admin_fetch_chat"},{"chat_id", chat->id} });
}

void ServerGUIWindow::onBanPushed()
{
    if (user_id_selected == 0)
    {
        QMessageBox::warning(this, "Error", QString("Сначала выберите пользователя"));
        return;
    }
    sendRequest({ {"cmd","admin_disconnect_user"},{"user_id",user_id_selected} });
    sendRequest({ {"cmd","admin_banhammer"},{"user_id",user_id_selected} });
    auto user = dynamic_cast<UserInfoItem*>(ui->usersList->currentItem());
    user->is_deleted = !(user->is_deleted);
    (user->is_deleted) ? ui->user_delete->setText("Разбанить пользователя") : ui->user_delete->setText("Забанить пользователя");
}

void ServerGUIWindow::onKickPushed()
{
    if (user_id_selected == 0) 
    {
        QMessageBox::warning(this, "Error", QString("Сначала выберите пользователя"));
        return;
    }
    sendRequest({ {"cmd","admin_disconnect_user"},{"user_id",user_id_selected} }); 
}

void ServerGUIWindow::onSocketReadyRead() {
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
                if (cmd == "full_info") {
                    ui->chatsList->clear();
                    auto chatList = response["chats"].get<std::vector<ChatFull>>();
                    for (auto& i : chatList)
                    {
                        ui->chatsList->addItem(new ChatInfoItem{ QString::fromStdString(i.name),i.id,i.name });
                    }
                    ui->usersList->clear();
                    auto userList = response["users"].get<std::vector<UserFull>>();
                    for (auto& i : userList)
                    {
                        ui->usersList->addItem(new UserInfoItem{ QString::fromStdString(i.username),i.id,i.username,i.is_deleted });
                    }
                }
                if (cmd == "chat") {
                    uint64_t chat_id = response["chat"]["id"].get<uint64_t>();
                    std::string chat_name = response["chat"]["name"].get<std::string>();
                    auto users = response["chat"]["users"].get<std::vector<UserPreview>>();

                    ui->chat_name->clear();
                    ui->chat_name->setText(QString::fromStdString(chat_name));
                    std::string users_in_chat;
                    std::map<uint64_t, QString> user_map;
                    for (int i = 0; i<users.size();i++)
                    {
                        if (i > 0) users_in_chat.append(", ");
                        users_in_chat.append(users[i].username);
                        if (users[i].is_deleted) users_in_chat.append("[BANNED]");
                        user_map[users[i].id] = QString::fromStdString(users[i].username);
                    }
                    ui->users_in_chat->clear();
                    ui->users_in_chat->setText(QString("Users: %1").arg(QString::fromStdString(users_in_chat)));
                    ui->textEdit->clear();
                    auto& msg_history = response["message_history"].get<MessageHistory>();
                    for (auto& msg : msg_history.messages)
                    {

                        QString utc_str = QString::fromStdString(msg.created_at);

                        utc_str = utc_str.left(19);


                        QDateTime utc_time = QDateTime::fromString(utc_str, "yyyy-MM-dd hh:mm:ss");
                        utc_time.setTimeSpec(Qt::UTC);

                        QDateTime local_time = utc_time.toLocalTime();

                        QString time_str = local_time.toString("yyyy-MM-dd hh:mm:ss");

                        QString line = QString("[%1] %2: %3")
                            .arg(time_str)
                            .arg(user_map[msg.sender_id])
                            .arg(QString::fromStdString(msg.content));

                        // дописываем в конец
                        ui->textEdit->append(line);
                    }
                    if (cmd == "new_chat" || cmd == "new_user")
                        user_id_selected = 0;
                        sendRequest({ { "cmd","admin_fetch_all" } });
                    {

                    }
                }
                
            }
        }
        catch (const std::exception& e) {
            QMessageBox::critical(this, "Parse Error",
                QString("Failed to parse server response: %1").arg(e.what()));
        }
    }
}

void ServerGUIWindow::connectToServer()
{
    m_socket->connectToHost("localhost", 9955);

    // Ждем подключения не более 3 секунд
    if (!m_socket->waitForConnected(3000)) {
        QMessageBox::critical(this, "Connection Error",
            QString("Could not connect to server: %1").arg(m_socket->errorString()));
    }
}