#pragma once
#include <string>
#include <map>
#include <mutex>
#include <vector>
enum class Role { Guest, User, Admin };
struct User { std::string login, pass; Role role; bool blocked = false; };
class UserManager {
    mutable std::mutex mtx; 
    std::map<std::string, User> users;
    std::string currentUser;
    UserManager();
public:
    static UserManager& instance();
    UserManager(const UserManager&) = delete;
    UserManager& operator=(const UserManager&) = delete;
    bool login(const std::string& l, const std::string& p);
    void logout();
    Role role() const;
    std::string name() const;
    bool loggedIn() const;
    std::vector<User> listUsers() const;
    void toggleBlock(const std::string& l);
};
