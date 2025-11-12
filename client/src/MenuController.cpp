#include "MenuController.h"
#include <iostream>

bool isStringNum(std::string);
void pressEnter(std::string);

MenuController::MenuController(std::unique_ptr<UserManager> uM,std::unique_ptr<ChatManager> cM) 
{
    _uM = std::move(uM);
    _cM = std::move(cM);
}

void MenuController::startMenu()
{
    
	char choice = '0';
	while (choice != 3)
	{
        system("cls");
		std::cout << "Выберите один из возможных режимов программы:\n1 - Регистрация нового пользователя.\n2 - Вход зарегистрированного пользователя.\n3 - Завершить работу программы.\nВведите номер планируемого действия: ";

		std::cin >> choice;

		switch (choice)
		{
		case '1':
			userRegistration();
			break;
		case '2':
			login();
			break;
		case '3':
			return;
		default:
            pressEnter("Ошибка ввода.");
		}
	}
}

void MenuController::userRegistration()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    system("cls");
    std::cout << "\n\nРегистрация нового пользователя:\n\n";
    std::string log{ "" };
    std::string name{ "" };
    std::string pass{ "" };
    std::string currentString{ "" };

    std::cout << "Для выхода из режима регистрации, введите '!' и 'ENTER'.\n";
    std::cout << "Логин (латинские буквы, цифры и знак нижнего подчёркивания): ";
    bool badInput = false;
    while (true)
    {
        badInput = false;
        getline(std::cin, currentString);
        if (currentString == "!")
        {
            return;
        }

        if (currentString.length() == 0)
        {
            std::cout << "\n\nПустая строка не может быть логином! Повторите попытку или выйдите из регистрации.\n\n";
            continue;
        }
        for (auto& c : currentString)                             // контроль правильности введенных символов
        {
            if (!((c <= 57 && c >= 48) ||                     // символ - цифра
                (c <= 90 && c >= 65) ||                     // символ заглавная латинская буква
                (c <= 122 && c >= 97) ||                     // символ прописная латинская буква
                (c == 95)))                                  // символ нижнее подчеркивание
            {
                std::cout << "\n\nК сожалению, в логине нашлись недопустимые символы. \nПопытайтесь ввести логин заново.\n\n";
                badInput = true;
                break;
            }
        }                             //Все символы правильные, проверяем на уникальность логина
        if (_uM->findUserByLogin(move(currentString)))
        {
            std::cout << "Пользователь с этим логином уже зарегистрирован, попробуйте другой логин.\n\n";
            continue;
        }
        if (!badInput)
        {
            log = currentString;
            break;
        }
    }
    while (true)
    {
        badInput = false;
        std::cout << "Имя (русские буквы, цифры и знак нижнего подчёркивания): ";

        getline(std::cin, currentString);
        if (currentString == "!")
        {
            return;
        }

        if (currentString.length() == 0)
        {
            std:: cout << "\n\nПустая строка не может быть именем! Повторите попытку или выйдите из регистрации.\n\n";
            continue;
        }

        for (auto& c : currentString)                             // контроль правильности введенных символов
        {
            if (!((c <= 57 && c >= 48) ||                     // символ - цифра
                (c <= -38 && c >= -64) ||                     // символ русская буква А-Ъ
                (c <= -6 && c >= -36) ||                     // символ русская буква Ь-ъ
                (c <= -1 && c >= -4) ||                     // символ русская буква ь-я
                c == -88 || c == -72 ||                     // символы Ё, ё
                c == -5 || c == -37 ||                     // символы ы, Ы
                (c == 95)))                                   // символ нижнее подчеркивание
            {
                std::cout << "К сожалению, в имени нашлись недопустимые символы. \nПопробуйте ввести еще раз.\n\n";
                badInput = true;
                break;
            }
        }
        if (_uM->findUserByNickname(move(currentString)))
        {
            std::cout << "Пользователь с этим именем уже зарегистрирован, попробуйте другое имя:)))))).\n";
            continue;
        }
        if (!badInput)
        {
            name = currentString;
            break;
        }
    }

    while (true)
    {
        badInput = false;
        std::cout << "Пароль (латинские буквы, цифры и \nспециальные знаки, кроме пробела и восклицательного знака): ";

        getline(std::cin, currentString);
        if (currentString == "!")
        {
            return;
        }

        if (currentString.length() == 0)
        {
            std::cout << "\n\nПустая строка не может быть паролем! Повторите попытку или выйдите из регистрации.\n\n";
            continue;
        }

        for (auto& c : currentString)                             // контроль правильности введенных символов
        {
            if (!(c <= 126 && c >= 34) || c == '!' || c == ' ')                  // Все печатные символы первой страницы ASCII кроме ! и пробела
            {
                std::cout << "\nБудьте внимательны! При наборе пароля допущена ошибка.\nПопытайтесь ввести пароль заново: ";
                badInput = true;
                break;
            }

        }
        if (!badInput)
        {
            pass = currentString;
            break;
        }
    }

    _uM->addUser(User(log, name, pass));
    system("cls");
    std::cout << "Новый пользователь '" << name << "' успешно зарегистрирован.\n\n";
    _cM->addUserToChat(_uM->getCount() - 1, 0); // Добавим пользователя в общий чат.
}

void MenuController::login()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    system("cls");
    std::cout << "Вход в систему\n";
    std::string log{ "" };
    std::string pass{ "" };
    std::string currentString{ "" };

    std::cout << "Для выхода из режима входа, введите '!' и 'ENTER'.\n";
    while (true)
    {

        std::cout << "Логин: ";
        getline(std::cin, currentString);
        if (currentString == "!") return;
        log = currentString;

        std::cout << "Пароль: ";
        getline(std::cin, currentString);
        if (currentString == "!") return;
        pass = currentString;

        if (_uM->loginUser(move(log), move(pass)))
        {
            userMenu();
            return;
        }
    }
}

void MenuController::userMenu()
{
    
    while (true)
    {
        system("cls");
        _cM->showAllUserChats(_uM->getCurrentUserID(), *_uM);

        std::cout << "\n\n" << _uM->getCurrentUser().getNickname() << ", Возможные действия:\n";
        std::cout << "Введите ID чата, чтобы войти в него.\nВведите N, чтобы создать новый чат.\nВведите ! чтобы разлогиниться.\n";

        std::string currentString;
        getline(std::cin, currentString);
        if (currentString == "!")
        {
            return;
        }
        if (currentString == "N")
        {
            newChatMenu();
            continue;
        }

        if (!isStringNum(currentString)) continue;
        int idx = -1;
        _cM->findUserToChatLink(_uM->getCurrentUserID(), idx);
        unsigned int id = atoi(currentString.c_str());
        if (id >= _cM->getLinks()[idx]._chats.size())
        {
            pressEnter("Нет такого ID.");
            continue;
        }
        
        chatMenu(id);
    }
}

void MenuController::newChatMenu()
{
    while (true)
    {
        system("cls");
        std::cout << "Выберите тип нового чата.\n1 - групповой чат\n2 - приватный чат.\n! - Вернуться в меню пользователя.\n";
        char choice = '0';
        std::cin >> choice;
        if (choice != '1' && choice != '2')
        {
            std::cout << "Выберите 1 или 2. Нажмите ENTER, чтобы продолжить.\n";
            continue;
        }

        unsigned int type = (unsigned int)choice - (int)'0';
        std::cout << "Выберите ID пользователя, с которым хотите начать чат.\n! - Вернуться в меню пользователя.\nb - Вернуться к выбору типа чата.\n";
        _uM->showUsersExcludingCurrent();
        std::string strID;
        std::cin >> strID;
        if (strID == "!") return;
        if (strID == "b") continue;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (!isStringNum(strID)) continue;
        int id = atoi(strID.c_str());
        if (id == _uM->getCurrentUserID() || id >= _uM->getCount())
        {
            pressEnter("Нет такого ID.");
            continue;
        }
        _cM->addNewChat(ChatType(type), _uM->getCurrentUserID());
        _cM->addUserToLastChat(id);
       
        
        std::cout << "Создан " << ((type == 1) ? "групповой " : "приватный ") << "чат с пользователем " << _uM->getUser(id).getNickname() << ".\n";
        pressEnter("");
        return;
    }
}

void MenuController::chatMenu(unsigned int id)
{
    int chatID = getRealChatID(id);
    while (true)
    {
        system("cls");
        _cM->updateUserPtrToLastMsg(_uM->getCurrentUserID(), id);

        std::cout << "Открыт ";
        switch (_cM->getChat(chatID)._type)
        {
        case 0:
            std::cout << "общий ";
            break;
        case 1:
            std::cout << "групповой ";
            break;
        case 2:
            std::cout << "личный ";
            break;
        default:
            break;
        }
        std::cout << "чат.\n";
        std::cout << "Участники чата: ";

        for (int i = 0; i < _cM->getChat(chatID)._users.size(); i++)
        {
            std::cout << _uM->getUser(_cM->getChat(chatID)._users.at(i)).getNickname() << ((i == _cM->getChat(chatID)._users.size() - 1) ? ".\n\n" : ", ");
        }
        if (_cM->getChat(chatID)._messages.empty())
        {
            std::cout << "Нет сообщений, попробуйте что-нибудь написать.\n\n";
        }
        _cM->showMessages(chatID);
        std::cout << "\n";

        std::cout << "Введите сообщение. \nДля выхода из чата, введите '!' и 'ENTER'.\n";
        if (_cM->getChat(chatID)._type == GROUP) std::cout << "Чтобы добавить нового пользователя введите /add\n\n";
        std::string message;
        std::getline(std::cin, message, '\n');
        if (message == "!") return;
        else if (message == "/add" && _cM->getChat(chatID)._type == GROUP)
        {
            addUserToChatMenu(chatID);
            continue;
        }
        else
        {
            _cM->getChat(chatID)._messages.push_back(Message(_uM->getCurrentUser(), message));
            _cM->updateUserPtrToLastMsg(_uM->getCurrentUserID(), id);
        }
    }
}

void MenuController::addUserToChatMenu(unsigned int chatID)
{
    while (true)
    {
        std::cout << "Выберите ID пользователя, с которым хотите начать чат.\n! - Вернуться в меню пользователя.\n";
        std::vector<int> usersIDInChat = _cM->getUsersIDFromChat(chatID);
        _uM->showUsersExcluding(std::move(usersIDInChat));
        std::string strID;
        getline(std::cin, strID);
        if (strID == "!") return;
        if (!isStringNum(strID)) continue;
        int id = atoi(strID.c_str());
        bool f = false;
        for (int i = 0; i < usersIDInChat.size(); i++)
        {
            if (id == usersIDInChat[i])
            {
                f = true;
            }
            if (f) break;
        }
        if (id >= _uM->getCount() || f)
        {
            pressEnter("Нет такого ID.");
            continue;
        }
        else
        _cM->addUserToChat(id, chatID);
        continue;
    }
}

int MenuController::getRealChatID(unsigned int id) //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Объединить linkи и chats в ChatManager!!!!!!!
{
    int idx = -1;
    _cM->findUserToChatLink(_uM->getCurrentUserID(), idx);
    return _cM->getLinks()[idx]._chats[id];
}

UserManager& MenuController::getUM()
{
    return *_uM;
}

ChatManager& MenuController::getCM()
{
    return *_cM;
}

void pressEnter(std::string str)
{
    std::cout << str << " Нажмите ENTER, чтобы продолжить.";
    getline(std::cin, str);
}

bool isStringNum(std::string str)
{
    for (auto& c : str)
    {
        if (!(c <= 57 && c >= 48))
        {
            pressEnter("Ошибка в вводе.");
            return false;
        }
    }
    return true;
}

