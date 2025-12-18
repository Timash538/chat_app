#include <gtest/gtest.h>
#include "Models.h"

TEST(MessageTest, Fields) {
    auto now = std::chrono::system_clock::now();
    Message msg{ 100,5,42,"Hello"};

    EXPECT_EQ(msg.id, 100);
    EXPECT_EQ(msg.content, "Hello");
}

TEST(MessageTest, FromToJSON) {
    Message msg{ 100,5,42,"Hello" };
    nlohmann::json j;
    to_json(j, msg);
    from_json(j, msg);
    EXPECT_EQ(msg.id, 100);
    EXPECT_EQ(msg.chat_id, 5);
    EXPECT_EQ(msg.sender_id, 42);
    EXPECT_EQ(msg.content, "Hello");
}