#include <server/CommandRegistry.h>


void CommandRegistry::registerCommand(const std::string& name, std::unique_ptr<ICommand> command)
{
	m_commands[name] = std::move(command);
}

nlohmann::json CommandRegistry::execute(const std::string& cmd, std::optional<UserID> userID, const nlohmann::json& args) const
{
	auto it = m_commands.find(cmd);
	if (it == m_commands.end())
	{
		throw std::runtime_error("Bad command");
	}

	return it->second->execute(userID, args);
};