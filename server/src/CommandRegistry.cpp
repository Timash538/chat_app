#include <server/CommandRegistry.h>

void CommandRegistry::registerCommand(const std::string& name, CommandFactory factory)
{
	m_factories[name] = factory;
}

nlohmann::json CommandRegistry::execute(const std::string& cmd, UserID executor_id, const nlohmann::json& args)
{
	auto it = m_factories.find(cmd);
	if (it == m_factories.end())
	{
		return { {"error", "unknown command: " + cmd} };
	}

	auto command = it->second();
	return command->execute(executor_id, args);
}