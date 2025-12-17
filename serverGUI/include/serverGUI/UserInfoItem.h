#pragma once

#include <QListWidgetItem>

struct UserInfoItem : public QListWidgetItem
{
    uint64_t user_id;
    std::string username;
    bool is_deleted;

    UserInfoItem(const QString& displayText, uint64_t _user_id, const std::string& _username, const bool& _is_deleted)
        : QListWidgetItem(displayText), // Инициализируем базовый класс QListWidgetItem
        user_id(_user_id),                  // Инициализируем ваше поле id
        username(_username),               // Инициализируем ваше поле name
        is_deleted(_is_deleted)
    {}
};
