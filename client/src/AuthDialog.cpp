#include <client/AuthDialog.h>
#include <QMessageBox>
#include <QDebug>
#include <string>

AuthDialog::AuthDialog(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Dialog),
    m_socket(new QTcpSocket(this)),
    m_userId(0)
{
    ui->setupUi(this);
    setWindowTitle("Authentication");
    // Подключаем кнопки
    connect(ui->L_togglePushButton, &QPushButton::clicked, this, &AuthDialog::onLToggleClicked);
    connect(ui->R_togglePushButton, &QPushButton::clicked, this, &AuthDialog::onRToggleClicked);
    connect(ui->L_sendPushButton, &QPushButton::clicked, this, &AuthDialog::onLoginButtonClicked);
    connect(ui->R_sendPushButton, &QPushButton::clicked, this, &AuthDialog::onRegisterButtonClicked);

    // Подключаем сетевые сигналы
    connect(m_socket, &QTcpSocket::connected, this, [this]() {
        qDebug() << "Connected to server";
    });

    connect(m_socket, &QTcpSocket::disconnected, this, [this]() {
        qDebug() << "Disconnected from server";
    });

    connect(m_socket, &QTcpSocket::readyRead, this, &AuthDialog::onSocketReadyRead);

    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, [this](QAbstractSocket::SocketError error) {
                QMessageBox::critical(this, "Connection Error",
                                      QString("Connection failed: %1").arg(m_socket->errorString()));
            });

    // Подключаемся к серверу
    connectToServer();

    // Начинаем с страницы логина
    ui->stackedWidget->setCurrentIndex(0);
}

AuthDialog::~AuthDialog() {
    delete ui;
}

void AuthDialog::connectToServer() {
    // Замени на адрес и порт твоего сервера
    m_socket->connectToHost("localhost", 9955);

    // Ждем подключения не более 3 секунд
    if (!m_socket->waitForConnected(3000)) {
        QMessageBox::critical(this, "Connection Error",
                              QString("Could not connect to server: %1").arg(m_socket->errorString()));
    }
}

void AuthDialog::onLToggleClicked() {
    ui->stackedWidget->setCurrentIndex(1); // Страница регистрации
}

void AuthDialog::onRToggleClicked() {
    ui->stackedWidget->setCurrentIndex(0); // Страница логина
}

void AuthDialog::onLoginButtonClicked() {
    QString login = ui->L_loginEdit->text().trimmed();
    QString password = ui->L_passwordEdit->text().trimmed();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Login and password cannot be empty");
        return;
    }

    try {
        // Формируем JSON для логина
        nlohmann::json request = {
            {"cmd", "login"},
            {"login", login.toStdString()},
            {"pass", password.toStdString()}
        };

        // Отправляем на сервер
        QString jsonStr = QString::fromStdString(request.dump() + "\n");
        m_socket->write(jsonStr.toUtf8());

        qDebug() << "Login request sent:" << jsonStr;

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
                              QString("Failed to create request: %1").arg(e.what()));
    }
}

void AuthDialog::onRegisterButtonClicked() {
    QString login = ui->R_loginEdit->text().trimmed();
    QString password = ui->R_passwordEdit->text().trimmed();
    QString username = ui->R_passwordEdit->text().trimmed();

    if (login.isEmpty() || password.isEmpty() || username.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Login, password and username cannot be empty");
        return;
    }

    try {
        // Формируем JSON для логина
        nlohmann::json request = {
            {"cmd", "register"},
            {"login", login.toStdString()},
            {"pass", password.toStdString()},
            {"username", username.toStdString()}
        };

        // Отправляем на сервер
        QString jsonStr = QString::fromStdString(request.dump() + "\n");
        m_socket->write(jsonStr.toUtf8());

        qDebug() << "Login request sent:" << jsonStr;

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
                              QString("Failed to create request: %1").arg(e.what()));
    }
}

void AuthDialog::onSocketReadyRead() {
    while (m_socket->canReadLine()) {
        QByteArray line = m_socket->readLine().trimmed();
        qDebug() << "Server response:" << line << "AUF";

        try {
            auto response = nlohmann::json::parse(line.constData());

            if (response.contains("error")) {
                QString errorMsg = QString::fromStdString(response["error"].get<std::string>());
                QMessageBox::warning(this, "Authentication Failed",
                                      QString("Error: %1").arg(errorMsg));
            }
            else if (response.contains("status") && response["status"] == "ok") {
                if (response.contains("user_id")) {
                    m_userId = std::stoull(response["user_id"].get<std::string>());
                    m_username = response["username"].get<std::string>();
                    // Закрываем диалог с результатом успеха
                    accept();
                    qDebug() << "Authentication successful! User ID:" << m_userId;
                }
                else
                {
                    ui->stackedWidget->setCurrentIndex(0);
                }
            }
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Parse Error",
                                  QString("Failed to parse server response: %1").arg(e.what()));
        }
    }
}
