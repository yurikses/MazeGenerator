#include "UserManager.h"

UserManager::UserManager() {
    users["guest"] = {"guest", "", Role::Guest, false};
    users["user"]  = {"user", "123", Role::User, false};
    users["admin"] = {"admin", "123", Role::Admin, false};
}
UserManager& UserManager::instance() { static UserManager i; return i; }
bool UserManager::login(const std::string& l, const std::string& p) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = users.find(l);
    if(it!=users.end() && !it->second.blocked && (it->second.pass==p || it->second.role==Role::Guest)) {
        currentUser = l; return true;
    } return false;
}
void UserManager::logout() { currentUser.clear(); }
Role UserManager::role() const { return users.count(currentUser) ? users.at(currentUser).role : Role::Guest; }
std::string UserManager::name() const { return currentUser; }
bool UserManager::loggedIn() const { return !currentUser.empty(); }
std::vector<User> UserManager::listUsers() const { 
    std::lock_guard<std::mutex> lock(mtx); 
    std::vector<User> r; for(auto& p:users) r.push_back(p.second); return r; 
}
void UserManager::toggleBlock(const std::string& login) {
    std::lock_guard<std::mutex> lock(mtx); 
    auto it = users.find(login);
    if(it != users.end() && it->second.role != Role::Admin)
        it->second.blocked = !it->second.blocked;
}