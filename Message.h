#pragma once
#include <string>
#include <memory>
#include "User.h"

class Message
{
private:
	std::shared_ptr<User> _sender;
	std::string _msg;
public:
	// Конструкторы:
	Message() = default;
	Message(User, std::string);
	Message(const Message& other);
	Message(Message&& other) noexcept;
	Message& operator=(const Message& m);
	Message& operator=(Message&& m) noexcept;
	

	// Деструктор
	~Message() = default;

	// Сеттеры:
	void setMessage(std::string&);
	void setSender(User user);

	//Геттеры:
	std::string getMessage();
	const User &getSendFrom();

};

