#include "./BeautySalon/ui/Menu.h"
#include "./BeautySalon/database/DatabaseManager.h"
#include <iostream>

int main() {
    DatabaseManager* db = DatabaseManager::getInstance();
    db->execute("SET client_min_messages = warning");
    try {
        Menu menu;
        menu.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
    DatabaseManager::destroyInstance();
    return 0;
}
