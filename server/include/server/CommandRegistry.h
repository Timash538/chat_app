#pragma once

#include <nlohmann/json.hpp>
#include <functional>

using UserID = uint64_t;

class ICommand
{
public:
	virtual ~ICommand() = default;
	virtual nlohmann::json execute(UserID executor_id, const nlohmann::json& args) = 0;
};

using CommandFactory = std::function<std::unique_ptr<ICommand>()>;

class CommandRegistry
{
public:
	void registerCommand(const std::string& name, CommandFactory factory);
	nlohmann::json execute(const std::string& name, UserID executor, const nlohmann::json& args);
private:
	std::unordered_map<std::string, CommandFactory> m_factories;
};