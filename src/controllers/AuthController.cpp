#include "AuthController.h"
#include "../views/ConsoleView.h"
#include <iostream>

bool AuthController::showLoginScreen(UserSession& session) {
    ConsoleView::clearScreen();
    ConsoleView::tampilkanHeader();
    
    std::cout << BOLD << "--- SILAKAN LOGIN ---" << RESET << std::endl;
    std::cout << " (Ketik 'exit' pada Username untuk keluar aplikasi)" << std::endl;
    std::cout << GREEN << ConsoleView::LINE_DASH << RESET << std::endl;
    
    std::string username, password;
    std::cout << "Username: ";
    std::getline(std::cin >> std::ws, username);

    if (username == "exit") {
        return false; // Berhenti
    }

    std::cout << "Password: ";
    std::getline(std::cin, password);

    session = UserModel::login(username, password);
    if (!session.isLoggedIn) {
        ConsoleView::printError("Username atau password salah!");
        std::cout << "\nTekan ENTER untuk kembali...";
        std::cin.get();
        return true;
    } 

    if (session.role == 2) {
        ConsoleView::printSuccess("Selamat datang, Ibu " + session.username);
        std::cout << "\nTekan ENTER untuk melanjutkan...";
        std::cin.get();
    }
    return true; // Lanjut
}
