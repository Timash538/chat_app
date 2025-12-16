#pragma once

#include <optional>
#include <nlohmann/json.hpp>
#include <Commontypes.h>

class ICommand
{
public:
	virtual ~ICommand() = default;
	virtual nlohmann::json execute(std::optional<UserID> userID, const nlohmann::json& args) = 0;
};