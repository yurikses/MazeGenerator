#include "MazeApp.h"
#include "../core/AppLogger.h"
#include "../core/UserManager.h"
#include "../core/MazeStorage.h"

MazeApp::MazeApp() : win(sf::VideoMode({1100u, 700u}), "Maze System") {
    win.setFramerateLimit(60);
    ImGui::CreateContext();
    ImGui::SFML::Init(win);
    selfPtr = std::shared_ptr<IObserver>(this, [](auto*){});
    ctrl.setObserver(selfPtr);
    AppLogger::instance().log("System initialized");
}

void MazeApp::onStep(const VisData& d) {}

void MazeApp::run() {
    while(win.isOpen()) {
        while(auto event = win.pollEvent()) { // Корректная обработка std::optional
            (void)ImGui::SFML::ProcessEvent(win, *event); // Убрано [[nodiscard]]
            if(event->is<sf::Event::Closed>()) { win.close(); return; }
            if(!showAuth && event->is<sf::Event::MouseButtonPressed>()) {
                auto mb = event->getIf<sf::Event::MouseButtonPressed>();
                if(!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
                    auto p = sf::Mouse::getPosition(win);
                    int x = p.x * ctrl.getGrid().W / win.getSize().x;
                    int y = p.y * ctrl.getGrid().H / win.getSize().y;
                    if(x>=0 && x<ctrl.getGrid().W && y>=0 && y<ctrl.getGrid().H) {
                        if(mb->button == sf::Mouse::Button::Left)
                            ctrl.setPoints(x, y, ctrl.getGrid().W/2, ctrl.getGrid().H/2);
                        else if(mb->button == sf::Mouse::Button::Right) {
                            isPathMode = true;
                            ctrl.setPoints(ctrl.getGrid().W/2, ctrl.getGrid().H/2, x, y);
                        }
                    }
                }
            }
        }
        float dt = clk.restart().asSeconds();
        if(running && !ImGui::GetIO().WantCaptureKeyboard) {
            for(int i=0; i<speed; ++i) if(!ctrl.step()) { running=false; break; }
        }
        mazeList = MazeStorage::load();

        ImGui::SFML::Update(win, sf::seconds(dt));
        uiAuth();
        if(showAuth) { win.clear(); ImGui::Render(); ImGui::SFML::Render(win); win.display(); continue; }
        uiMain(); uiAdmin();
        win.clear(sf::Color(30,30,30));
        drawMaze();
        ImGui::SFML::Render(win);
        win.display();
    }
    ImGui::SFML::Shutdown();
}

void MazeApp::uiAuth() {
    ImVec2 sz = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos({sz.x/2.0f - 150.0f, sz.y/2.0f - 100.0f});
    ImGui::SetNextWindowSize({300, 200});
    ImGui::Begin("Authorization", nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse);
    ImGui::InputText("Login", login, sizeof(login));
    ImGui::InputText("Password", pass, sizeof(pass), ImGuiInputTextFlags_Password);
    if(ImGui::Button("Login")) {
        std::string l(login), p(pass);
        if(UserManager::instance().login(l, p)) {
            showAuth = false;
            AppLogger::instance().log("Login: " + l);
        } else {
            ImGui::OpenPopup("Error");
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Exit")) win.close();
    if(ImGui::BeginPopup("Error")) {
        ImGui::Text("Invalid credentials or account blocked.");
        ImGui::EndPopup();
    }
    ImGui::End();
}

void MazeApp::uiMain() {
    ImGui::Begin("Controls");
    auto& um = UserManager::instance();
    ImGui::Text("User: %s (%s)", um.name().c_str(),
        um.role()==Role::Admin ? "Admin" : (um.role()==Role::User ? "User" : "Guest"));
    if(ImGui::Button("Logout")) { um.logout(); showAuth=true; }
    if(um.role()==Role::Admin && ImGui::Button("Admin Panel")) showAdmin=!showAdmin;
    ImGui::Separator();

    if(um.role() != Role::Guest) {
        ImGui::SliderInt("Width", &mazeW, 5, 50);
        ImGui::SliderInt("Height", &mazeH, 5, 50);
        const char* gens[] = {"DFS", "Kruskal"};
        ImGui::Combo("Generator", (int*)&genType, gens, 2);
        if(ImGui::Button("Generate")) {
            ctrl.resize(mazeW, mazeH);
            ctrl.run(genType);
            running = true; isPathMode = false;
        }
        ImGui::Separator();
        const char* paths[] = {"BFS", "A*"};
        ImGui::Combo("Pathfinder", (int*)&pathType, paths, 2);
        if(isPathMode && ImGui::Button("Find Path")) {
            ctrl.run(pathType); running = true;
        }
    }
    ImGui::SliderInt("Speed", &speed, 1, 100);
    ImGui::Checkbox("Auto Run", &running);
    ImGui::Separator();

    if(um.role() != Role::Guest) {
        ImGui::InputText("Save ID", newMazeId, sizeof(newMazeId));
        ImGui::Checkbox("Publish", &isPathMode);
        if(ImGui::Button("Save Maze")) ctrl.save(std::string(newMazeId), isPathMode);
    }
    ImGui::End();

    ImGui::Begin("Published Mazes");
    for(const auto& m : mazeList) {
        if(m.published) ImGui::Text("[%s] %s (%dx%d)", m.owner.c_str(), m.id.c_str(), m.W, m.H);
    }
    ImGui::End();
}

void MazeApp::uiAdmin() {
    if(!showAdmin || UserManager::instance().role()!=Role::Admin) return;
    ImGui::Begin("Admin Panel");
    if(ImGui::CollapsingHeader("Users")) {
        auto users = UserManager::instance().listUsers();
        for(const auto& u : users) {
            ImGui::Text("%s [%s]", u.login.c_str(), u.blocked ? "BLOCKED" : "ACTIVE");
            if(u.role != Role::Admin && ImGui::SmallButton(u.blocked ? "Unblock" : "Block"))
                UserManager::instance().toggleBlock(u.login);
        }
    }
    if(ImGui::CollapsingHeader("Maze Management")) {
        for(const auto& m : mazeList) {
            ImGui::Text("%s by %s", m.id.c_str(), m.owner.c_str());
            if(ImGui::SmallButton("Delete")) MazeStorage::remove(m.id);
        }
    }
    ImGui::End();
}

void MazeApp::drawMaze() {
    float cw = win.getSize().x / static_cast<float>(ctrl.getGrid().W);
    float ch = win.getSize().y / static_cast<float>(ctrl.getGrid().H);
    sf::VertexArray walls(sf::PrimitiveType::Lines);
    for(int y=0; y<ctrl.getGrid().H; ++y) {
        for(int x=0; x<ctrl.getGrid().W; ++x) {
            sf::Vector2f p(static_cast<float>(x)*cw, static_cast<float>(y)*ch);
            sf::Color bg = sf::Color(240,240,240);
            const auto& c = ctrl.getGrid().at(x,y);
            if(c.status==1) bg=sf::Color(255,210,0);
            else if(c.status==2) bg=sf::Color(120,140,255);
            else if(c.status==3) bg=sf::Color(60,220,60);

            sf::RectangleShape r(sf::Vector2f(cw,ch));
            r.setPosition(p); r.setFillColor(bg);
            win.draw(r);

            sf::Vertex v1{p, sf::Color(100,100,100)};
            sf::Vertex v2{sf::Vector2f(p.x+cw,p.y), sf::Color(100,100,100)};
            sf::Vertex v3{sf::Vector2f(p.x+cw,p.y+ch), sf::Color(100,100,100)};
            sf::Vertex v4{sf::Vector2f(p.x,p.y+ch), sf::Color(100,100,100)};

            if(c.walls[0]) { walls.append(v1); walls.append(v2); }
            if(c.walls[1]) { walls.append(v2); walls.append(v3); }
            if(c.walls[2]) { walls.append(v3); walls.append(v4); }
            if(c.walls[3]) { walls.append(v4); walls.append(v1); }
        }
    }
    win.draw(walls);
}