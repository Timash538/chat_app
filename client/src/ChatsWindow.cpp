#include <client/ChatsWindow.h>
#include <client/MainWindow.h>
#include <QKeyEvent>
#include <QDateTime>

void ChatsWindow::onMessageEntryReturnPressed(uint64_t chat_id, QString msg) 
{
    m_main_window->sendRequest({ {"cmd","send_message"},{"chat_id",chat_id},{ "content",msg.toStdString() }});
}

void ChatsWindow::onNewMessage(uint64_t chat_id, Message msg) 
{

    QTextEdit* history = nullptr;
    uint tab;
    for (auto& [t, c] : m_chat_tabs)
    {
        if (c.id == chat_id)
        {
            history = ui->tabWidget->widget(t)->findChild<QTextEdit*>("messageHistory");
            tab = t;
        }
    }
    if (!history) return;

    std::map<uint64_t, std::string> users;

    for (auto& u : m_chat_tabs[tab].users)
    {
        users[u.id] = u.username;
    }

    QString utc_str = QString::fromStdString(msg.created_at);

    utc_str = utc_str.left(19);


    QDateTime utc_time = QDateTime::fromString(utc_str, "yyyy-MM-dd hh:mm:ss");
    utc_time.setTimeSpec(Qt::UTC);

    QDateTime local_time = utc_time.toLocalTime();

    QString time_str = local_time.toString("yyyy-MM-dd hh:mm:ss");

    QString line = QString("[%1] %2: %3")
        .arg((time_str))
        .arg(QString::fromStdString(users[msg.sender_id]))
        .arg(QString::fromStdString(msg.content));

    // дописываем в конец
    history->append(line);
}

void ChatsWindow::onHistoryFetch(uint64_t chat_id, MessageHistory msg_history) 
{
    QTextEdit* history = nullptr;
    uint tab;
    for (auto& [t,c] : m_chat_tabs)
    {
        if (c.id == chat_id)
        {
            history = ui->tabWidget->widget(t)->findChild<QTextEdit*>("messageHistory");
            tab = t;
        }
    }
    if (!history) return;

    std::map<uint64_t, std::string> users;
    for (auto& u : m_chat_tabs[tab].users)
    {
        users[u.id] = u.username;
    }

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
            .arg(QString::fromStdString(users[msg.sender_id]) )
            .arg(QString::fromStdString(msg.content));   

        // дописываем в конец
        history->append(line);
    }
}

void ChatsWindow::closeEvent(QCloseEvent* event) {
    emit closed();
    QMainWindow::closeEvent(event);
}



ChatsWindow::ChatsWindow(MainWindow* main_window, QWidget* parent)
    : QMainWindow(parent), m_main_window(main_window), ui(new Ui::ChatsWindow)
{
    ui->setupUi(this);
    ui->tabWidget->clear();
    connect(m_main_window, &MainWindow::HistoryFetch, this, &ChatsWindow::onHistoryFetch);
    connect(m_main_window, &MainWindow::NewMessage, this, &ChatsWindow::onNewMessage);
}

ChatsWindow::~ChatsWindow() {
    delete ui;
}

QWidget* ChatsWindow::createNewTabFromPrototype() {
    QWidget* new_tab = new QWidget();
    new_tab->setObjectName("chat_tab_" + QString::number(m_chat_tabs.size()));
    auto* layout = new QVBoxLayout(new_tab);

    QTextEdit* history = new QTextEdit(new_tab);
    history->setObjectName("messageHistory");
    history->setReadOnly(true);
    layout->addWidget(history);

    QTextEdit* entry = new QTextEdit(new_tab);
    entry->setObjectName("messageEntry");
    entry->setMaximumHeight(100);
    layout->addWidget(entry);

    return new_tab;
}

void ChatsWindow::openChat(uint64_t chat_id, const QString& chat_name,
    const std::vector<UserPreview>& users) {
    for (auto& [t,c] : m_chat_tabs)
    {
        if (c.id == chat_id)
        {
            ui->tabWidget->setCurrentIndex(t); //x
            m_current_chat_id = chat_id; //x
            return;
        }
    }

    QWidget* new_tab = createNewTabFromPrototype();//x
    new_tab->setProperty("chat_id", chat_id);
    int tab_index = ui->tabWidget->insertTab(m_chat_tabs.size(), new_tab, chat_name);

    m_chat_tabs[tab_index].id = chat_id;
    m_chat_tabs[tab_index].name = chat_name;
    m_chat_tabs[tab_index].users = users;
    m_current_chat_id = chat_id;

    ui->tabWidget->setCurrentIndex(tab_index);

    QTextEdit* history = new_tab->findChild<QTextEdit*>("messageHistory");
    QTextEdit* entry = new_tab->findChild<QTextEdit*>("messageEntry");
    if (history) history->clear();
    if (entry) 
    {
        entry->clear();
        entry->setProperty("chat_id", chat_id);
        entry->installEventFilter(this);
    }
    nlohmann::json req = {
        {"cmd", "fetch_history"},
        {"chat_id", chat_id}
    };
    m_main_window->sendRequest(req);
}

bool ChatsWindow::eventFilter(QObject* obj, QEvent* event) {
    // Фильтруем только наши QTextEdit'ы ввода
    if (event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);

        // проверяем, что событие от поля ввода
        if (auto* edit = qobject_cast<QTextEdit*>(obj)) {
            // Enter / Return без модификаторов = отправка
            if ((keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) &&
                (keyEvent->modifiers() == Qt::NoModifier)) {

                QString msg = edit->toPlainText();
                if (!msg.trimmed().isEmpty()) {
                    onMessageEntryReturnPressed(obj->property("chat_id").toULongLong(), msg);
                    edit->clear();
                }

                // не даём QTextEdit добавить перевод строки
                return true;
            }

            // Shift+Enter — разрешаем перенос строки
            if ((keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) &&
                (keyEvent->modifiers() & Qt::ShiftModifier)) {
                return false; // стандартное поведение
            }
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

//int ChatsWindow::findTabIndex(uint64_t chat_id) {
//    // Логика поиска индекса таба по chat_id
//    return 0;  // Пока заглушка
//}

//void ChatsWindow::sendMessage() {
//    if (m_current_chat_id == 0 || ui->messageEntry->toPlainText().isEmpty()) return;
//
//    nlohmann::json msg = {
//        {"cmd", "send_message"},
//        {"chat_id", m_current_chat_id},
//        {"content", ui->messageEntry->toPlainText().toStdString()}
//    };
//
//    m_main_window->getSocket()->write((msg.dump() + "\n").c_str());
//    ui->messageEntry->clear();
//}