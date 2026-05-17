#pragma once
#include <string>

// Репозиторий авторизации: интерфейс доступа к данным пользователей
class AuthRepository {
public:
    virtual ~AuthRepository() = default;
    virtual bool saveUser(const std::string& login, const std::string& pass, const std::string& role) = 0;
    virtual bool validateUser(const std::string& login, const std::string& pass) = 0;
    virtual bool userExists(const std::string& login) = 0;
};
