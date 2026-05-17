#pragma once
#include <string>
#include <mutex>
#include <memory>
#include "AuthRepository.h"

// Singleton для управления авторизацией
class AuthManager {
public:
    static AuthManager& instance();
    bool registerUser(const std::string& login, const std::string& pass);
    bool login(const std::string& login, const std::string& pass);
    void logout();
    bool isLoggedIn() const;
    std::string currentUser() const;
    void setRepository(std::shared_ptr<AuthRepository> repo);
private:
    AuthManager();
    std::mutex mtx;
    std::shared_ptr<AuthRepository> repo;
    std::string curUser;
};
