#include "Hairdresser.h"
#include <iostream>

Hairdresser::Hairdresser()
    : Employee(-1, "", "парикмахер", 0, 0.0, 0.0, 0.0, "работает", false) {
    std::cout << "\033[1;31mСоздан парикмахер\033[0m\n";
}

Hairdresser::Hairdresser(const std::string& fullName, int experience, double rating,
                         double salaryBase, double salaryPercent)
    : Employee(-1, fullName, "парикмахер", experience, rating, salaryBase, salaryPercent, "работает", false) {
    std::cout << "\033[1;31mСоздан парикмахер " << fullName << "\033[0m\n";
}

Hairdresser::Hairdresser(int id, const std::string& fullName, int experience, double rating,
                         double salaryBase, double salaryPercent, const std::string& status, bool isDeleted)
    : Employee(id, fullName, "парикмахер", experience, rating, salaryBase, salaryPercent, status, isDeleted) {
    std::cout << "\033[1;31mСоздан парикмахер " << fullName << "\033[0m\n";
}

Hairdresser::Hairdresser(const Hairdresser& other)
    : Employee(other) {
    std::cout << "\033[1;31mСкопирован парикмахер " << fullName << "\033[0m\n";
}

Hairdresser::~Hairdresser() {
    std::cout << "\033[1;31mУдален парикмахер " << fullName << "\033[0m\n";
}

int Hairdresser::getMaxClientsPerDay() const {
    return 8;
}

int Hairdresser::getAvgServiceTime() const {
    return 45;
}

std::string Hairdresser::getType() const {
    return "hairdresser";
}
