#pragma once
#include <string>
#include <memory>
#include <chrono>
#include <nlohmann/json.hpp>

struct Message {
    int id;
    int chat_id;
    int sender_id;
    std::string text;
    std::chrono::system_clock::time_point timestamp;
};

void to_json(nlohmann::json& j, const Message& m) {
    auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(m.timestamp);
    auto ms_int = ms.time_since_epoch().count();
    j = nlohmann::json{
        {"id", m.id},
        {"chat_id", m.chat_id},
        {"sender_id", m.sender_id},
        {"text", m.text},
        {"timestamp", ms_int}
    };
}

void from_json(const nlohmann::json& j, Message& m) {
    j.at("id").get_to(m.id);
    j.at("chat_id").get_to(m.chat_id);
    j.at("sender_id").get_to(m.sender_id);
    j.at("text").get_to(m.text);
    auto ms = j.at("timestamp").get<std::int64_t>();
    auto tp = std::chrono::time_point<std::chrono::system_clock,
        std::chrono::milliseconds>(std::chrono::milliseconds(ms));
    m.timestamp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(tp);
}