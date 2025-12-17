#pragma once

#include <QListWidgetItem>

struct ChatInfoItem : public QListWidgetItem
{
    uint64_t id;
    std::string name;

    ChatInfoItem(const QString& displayText, uint64_t _id, const std::string& _name)
        : QListWidgetItem(displayText), // Инициализируем базовый класс QListWidgetItem
        id(_id),                  // Инициализируем ваше поле id
        name(_name)               // Инициализируем ваше поле name
    {}
};
