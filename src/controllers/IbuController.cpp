#include "IbuController.h"
#include "../views/ConsoleView.h"
#include "../models/RiwayatModel.h"
#include <iostream>

IbuController::IbuController(UserSession& session) : currentSession(session) {}

void IbuController::run() {
    bool ibuLoop = true;
    while (ibuLoop && currentSession.isLoggedIn) {
        showMainMenu();
    }
}

void IbuController::showMainMenu() {
    ConsoleView::clearScreen();
    ConsoleView::tampilkanHeader();
    
    std::cout << BOLD << "--- MENU UTAMA (IBU BALITA: " << currentSession.username << ") ---" << RESET << std::endl;
    std::cout << " Balita Aktif saat ini: " << (currentSession.childName.empty() ? "[Belum Dipilih]" : currentSession.childName) << std::endl;
    std::cout << MAGENTA << "---------------------------------------------------------------------" << RESET << std::endl;
    std::cout << " [1] Lihat Data & Riwayat Pemeriksaan Anak" << std::endl;
    std::cout << " [2] Ganti Balita (Pilih Anak)" << std::endl;
    std::cout << " [3] Logout" << std::endl;
    std::cout << MAGENTA << "---------------------------------------------------------------------" << RESET << std::endl;
    std::cout << "Pilih Menu: ";
    
    int menuPilihan = 0;
    if (!(std::cin >> menuPilihan)) {
        ConsoleView::printError("Input tidak valid!");
        ConsoleView::clearInput();
        std::cout << "\nTekan ENTER untuk melanjutkan...";
        std::cin.get();
        return;
    }
    ConsoleView::clearInput();

    if (menuPilihan == 1) {
        doLihatRiwayat();
    } else if (menuPilihan == 2) {
        doPilihAnak();
    } else if (menuPilihan == 3) {
        currentSession.isLoggedIn = false;
    } else {
        ConsoleView::printError("Pilihan menu tidak terdaftar!");
        std::cout << "\nTekan ENTER untuk melanjutkan...";
        std::cin.get();
    }
}

void IbuController::doPilihAnak() {
    std::vector<std::string> anakList = UserModel::getAnakByIbu(currentSession.username);
    if (anakList.empty()) {
        ConsoleView::printError("Anda belum menautkan data anak. Hubungi Kader Posyandu.");
        std::cout << "\nTekan ENTER untuk kembali...";
        std::cin.get();
        return;
    }

    ConsoleView::clearScreen();
    ConsoleView::tampilkanHeader();
    std::cout << BOLD << "--- PILIH BALITA ---" << RESET << std::endl;
    
    int idx = 1;
    for (const auto& anak : anakList) {
        std::cout << " [" << idx++ << "] " << anak << std::endl;
    }
    std::cout << "Pilihan: ";
    
    int pilihan = 0;
    if (std::cin >> pilihan && pilihan >= 1 && pilihan <= (int)anakList.size()) {
        currentSession.childName = anakList[pilihan - 1];
        ConsoleView::printSuccess("Berhasil mengganti anak aktif ke " + currentSession.childName);
    } else {
        ConsoleView::printError("Pilihan tidak valid!");
    }
    ConsoleView::clearInput();
    std::cout << "\nTekan ENTER untuk melanjutkan...";
    std::cin.get();
}

void IbuController::doLihatRiwayat() {
    if (currentSession.childName.empty()) {
        ConsoleView::printError("Silakan pilih balita terlebih dahulu!");
        std::cout << "\nTekan ENTER untuk kembali...";
        std::cin.get();
        return;
    }
    
    ConsoleView::clearScreen();
    std::vector<std::vector<std::string>> data = RiwayatModel::getFilteredRiwayat(false, currentSession.childName);
    ConsoleView::printRiwayatTable(data, false);
    
    std::cout << "\nTekan ENTER untuk kembali ke Menu Utama...";
    std::cin.get();
}
