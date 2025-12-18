#pragma once

#include <QListWidgetItem>

// Возможно есть способ лучше доставать данные из полей виджетов. (Мб property)
struct ChatInfoItem : public QListWidgetItem
{
    uint64_t id;
    std::string name;

    ChatInfoItem(const QString& displayText, uint64_t _id, const std::string& _name)
        : QListWidgetItem(displayText),
        id(_id),                  
        name(_name)               
    {}
};
