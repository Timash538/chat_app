#pragma once

#include <optional>
#include <nlohmann/json.hpp>

//Вспомогательный класс для CommandRegistry
class ICommand
{
public:
	virtual ~ICommand() = default;
	virtual nlohmann::json execute(std::optional<uint64_t> userID, const nlohmann::json& args) = 0;
};