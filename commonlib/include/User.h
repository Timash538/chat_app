#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct User	{
	int id;
	std::string login;
	std::string nickname;
	std::string password;
};

void to_json(nlohmann::json& j, const User& u) {
    j = nlohmann::json{
        {"id", u.id},
        {"login", u.login},
        {"nickname", u.nickname},
        {"password", u.password}
    };
}

void from_json(const nlohmann::json& j, User& u) {
    j.at("id").get_to(u.id);
    j.at("login").get_to(u.login);
    j.at("nickname").get_to(u.nickname);
    j.at("password").get_to(u.password);
}