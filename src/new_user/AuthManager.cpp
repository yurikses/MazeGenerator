#include "AuthManager.h"
#include <fstream>
#include <sstream>

class FileAuthRepo : public AuthRepository {
    std::string path = "users.db";
public:
    bool saveUser(const std::string& login, const std::string& pass, const std::string& role) override {
        std::ofstream f(path, std::ios::app);
        if(!f) return false;
        f << login << " " << pass << " " << role << "\n";
        return true;
    }
    bool validateUser(const std::string& login, const std::string& pass) override {
        std::ifstream f(path);
        if(!f) return false;
        std::string l,p,r;
        while(f >> l >> p >> r) {
            if(l==login && p==pass) return true;
        }
        return false;
    }
    bool userExists(const std::string& login) override {
        std::ifstream f(path);
        if(!f) return false;
        std::string l,p,r;
        while(f >> l >> p >> r) if(l==login) return true;
        return false;
    }
};

AuthManager::AuthManager() { repo = std::make_shared<FileAuthRepo>(); }
AuthManager& AuthManager::instance() { static AuthManager i; return i; }
void AuthManager::setRepository(std::shared_ptr<AuthRepository> r) { std::lock_guard<std::mutex> g(mtx); repo = r; }

bool AuthManager::registerUser(const std::string& login, const std::string& pass) {
    std::lock_guard<std::mutex> g(mtx);
    if(repo->userExists(login)) return false;
    return repo->saveUser(login, pass, "User");
}

bool AuthManager::login(const std::string& login, const std::string& pass) {
    std::lock_guard<std::mutex> g(mtx);
    if(repo->validateUser(login, pass)) { curUser = login; return true; }
    return false;
}

void AuthManager::logout() { std::lock_guard<std::mutex> g(mtx); curUser.clear(); }
bool AuthManager::isLoggedIn() const { return !curUser.empty(); }
std::string AuthManager::currentUser() const { return curUser; }
