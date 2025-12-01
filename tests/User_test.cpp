#include <gtest/gtest.h>
#include "User.h"

TEST(UserTest, Fields) {
    User u{ 42, "Alice", "alice_nick" };
    EXPECT_EQ(u.id, 42);
    EXPECT_EQ(u.login, "Alice");
    EXPECT_EQ(u.nickname, "alice_nick");
}

TEST(UserTest, FromToJSON) {
    User u{ 42, "Alice", "alice_nick" };
    nlohmann::json j;
    to_json(j, u);
    from_json(j, u);
    EXPECT_EQ(u.id, 42);
    EXPECT_EQ(u.login, "Alice");
    EXPECT_EQ(u.nickname, "alice_nick");
}