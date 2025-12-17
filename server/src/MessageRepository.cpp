#include <server/MessageRepository.h>

MessageHistory MessageRepository::fetchHistory(const uint64_t& chat_id)
{
	auto result = m_db.query(
		"SELECT id, chat_id, sender_id, content, created_at "
		"FROM messages WHERE chat_id = $1 ORDER BY id",
		{std::to_string(chat_id)}
	);

	if (result.empty()) {
		throw std::runtime_error("There is no messages yet.");
	}

	std::vector<Message> messages;
	messages.reserve(result.size());

	for (const auto& row : result)
	{
		messages.push_back({
			std::stoull(row[0]),
			std::stoull(row[1]),
			std::stoull(row[2]),
			row[3],
			row[4]
			});
	}
	return { chat_id, messages };
}

Message MessageRepository::saveMessage(const uint64_t& chat_id,const uint64_t& sender_id, const std::string& content)
{
	try {
		auto result = m_db.query(
			"INSERT INTO messages (chat_id, sender_id, content) "
			"VALUES ($1, $2, $3) RETURNING id, created_at ",
			{ std::to_string(chat_id), std::to_string(sender_id), content }
		);

		auto row = result[0];
		return 
		{
			std::stoull(row[0]),
			chat_id,
			sender_id,
			content,
			row[1]
		};
	}
	catch (const DatabaseError& e) 
	{
		throw std::runtime_error("Error on sending message");
	}
}
