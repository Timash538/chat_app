#include <gtest/gtest.h>
#include "Message.h"

TEST(MessageTest, Fields) {
    auto now = std::chrono::system_clock::now();
    Message msg{ 100,5,42,"Hello",now };

    EXPECT_EQ(msg.id, 100);
    EXPECT_EQ(msg.text, "Hello");
}

TEST(MessageTest, FromToJSON) {
    auto now = std::chrono::system_clock::now();
    now = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    Message msg{ 100,5,42,"Hello",now };
    nlohmann::json j;
    to_json(j, msg);
    from_json(j, msg);
    EXPECT_EQ(msg.id, 100);
    EXPECT_EQ(msg.chat_id, 5);
    EXPECT_EQ(msg.sender_id, 42);
    EXPECT_EQ(msg.text, "Hello");
    EXPECT_EQ(msg.timestamp, now);
}