#pragma once

#include <QListWidgetItem>

struct CommonItem : public QListWidgetItem
{
    uint64_t id;
    std::string name;

    CommonItem(const QString& displayText, uint64_t itemId, const std::string& itemName)
        : QListWidgetItem(displayText),
        id(itemId),                  
        name(itemName)               
    {
    }
};
