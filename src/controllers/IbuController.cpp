#include "IbuController.h"
#include "../views/ConsoleView.h"
#include "../models/RiwayatModel.h"
#include <iostream>
#include <algorithm>

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
    std::cout << " [2] Pilih / Tautkan Anak Balita" << std::endl;
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
    bool subLoop = true;
    while (subLoop) {
        ConsoleView::clearScreen();
        ConsoleView::tampilkanHeader();
        std::cout << BOLD << "--- MENU PILIH / TAUTKAN BALITA ---" << RESET << std::endl;
        std::cout << " Balita Aktif saat ini: " << (currentSession.childName.empty() ? "[Belum Dipilih]" : currentSession.childName) << std::endl;
        std::cout << MAGENTA << "---------------------------------------------------------------------" << RESET << std::endl;
        std::cout << " [1] Pilih Anak Aktif (dari yang sudah ditautkan)" << std::endl;
        std::cout << " [2] Tautkan Anak dari Data Posyandu" << std::endl;
        std::cout << " [3] Tautkan Anak Baru (Input Manual)" << std::endl;
        std::cout << " [4] Kembali ke Menu Utama" << std::endl;
        std::cout << MAGENTA << "---------------------------------------------------------------------" << RESET << std::endl;
        std::cout << "Pilih Menu: ";
        
        int pilihan = 0;
        if (!(std::cin >> pilihan)) {
            ConsoleView::printError("Input tidak valid!");
            ConsoleView::clearInput();
            std::cout << "\nTekan ENTER untuk melanjutkan...";
            std::cin.get();
            continue;
        }
        ConsoleView::clearInput();
        
        if (pilihan == 1) {
            std::vector<std::string> anakList = UserModel::getAnakByIbu(currentSession.username);
            if (anakList.empty()) {
                ConsoleView::printError("Anda belum menautkan data anak. Silakan pilih menu [2] atau [3] terlebih dahulu.");
                std::cout << "\nTekan ENTER untuk kembali...";
                std::cin.get();
                continue;
            }
            
            ConsoleView::clearScreen();
            ConsoleView::tampilkanHeader();
            std::cout << BOLD << "--- PILIH ANAK AKTIF ---" << RESET << std::endl;
            for (size_t i = 0; i < anakList.size(); ++i) {
                std::cout << " [" << (i + 1) << "] " << anakList[i] << std::endl;
            }
            std::cout << "Pilih (1-" << anakList.size() << "): ";
            int sel = 0;
            if (std::cin >> sel && sel >= 1 && sel <= (int)anakList.size()) {
                currentSession.childName = anakList[sel - 1];
                ConsoleView::printSuccess("Berhasil mengganti anak aktif ke " + currentSession.childName);
            } else {
                ConsoleView::printError("Pilihan tidak valid!");
            }
            ConsoleView::clearInput();
            std::cout << "\nTekan ENTER untuk melanjutkan...";
            std::cin.get();
            
        } else if (pilihan == 2) {
            std::vector<ChildProfile> allChildren = RiwayatModel::getUniqueChildren();
            if (allChildren.empty()) {
                ConsoleView::printError("Belum ada data anak terdaftar di sistem Posyandu.");
                std::cout << "\nTekan ENTER untuk kembali...";
                std::cin.get();
                continue;
            }
            
            ConsoleView::clearScreen();
            ConsoleView::tampilkanHeader();
            std::cout << BOLD << "--- TAUTKAN ANAK DARI DATA POSYANDU ---" << RESET << std::endl;
            for (size_t i = 0; i < allChildren.size(); ++i) {
                std::cout << " [" << (i + 1) << "] " << allChildren[i].nama 
                          << " (" << (allChildren[i].jenis_kelamin == 'L' ? "Laki-laki" : "Perempuan") << ")" << std::endl;
            }
            std::cout << "Pilih anak untuk ditautkan ke akun Anda: ";
            int sel = 0;
            if (std::cin >> sel && sel >= 1 && sel <= (int)allChildren.size()) {
                std::string namaAnak = allChildren[sel - 1].nama;
                
                std::vector<std::string> anakList = UserModel::getAnakByIbu(currentSession.username);
                if (std::find(anakList.begin(), anakList.end(), namaAnak) != anakList.end()) {
                    ConsoleView::printError("Anak '" + namaAnak + "' sudah ditautkan ke akun Anda!");
                } else {
                    if (UserModel::linkIbuToAnak(currentSession.username, namaAnak)) {
                        ConsoleView::printSuccess("Berhasil menautkan anak '" + namaAnak + "' ke akun Anda!");
                        if (currentSession.childName.empty()) {
                            currentSession.childName = namaAnak;
                        }
                    } else {
                        ConsoleView::printError("Gagal menautkan anak.");
                    }
                }
            } else {
                ConsoleView::printError("Pilihan tidak valid!");
            }
            ConsoleView::clearInput();
            std::cout << "\nTekan ENTER untuk melanjutkan...";
            std::cin.get();
            
        } else if (pilihan == 3) {
            ConsoleView::clearScreen();
            ConsoleView::tampilkanHeader();
            std::cout << BOLD << "--- TAUTKAN ANAK BARU (INPUT MANUAL) ---" << RESET << std::endl;
            std::cout << "Masukkan Nama Anak: ";
            std::string namaAnak;
            std::getline(std::cin >> std::ws, namaAnak);
            
            std::vector<std::string> anakList = UserModel::getAnakByIbu(currentSession.username);
            if (std::find(anakList.begin(), anakList.end(), namaAnak) != anakList.end()) {
                ConsoleView::printError("Anak '" + namaAnak + "' sudah ditautkan ke akun Anda!");
            } else {
                if (UserModel::linkIbuToAnak(currentSession.username, namaAnak)) {
                    ConsoleView::printSuccess("Berhasil menautkan anak '" + namaAnak + "' ke akun Anda!");
                    if (currentSession.childName.empty()) {
                        currentSession.childName = namaAnak;
                    }
                } else {
                    ConsoleView::printError("Gagal menautkan anak.");
                }
            }
            std::cout << "\nTekan ENTER untuk melanjutkan...";
            std::cin.get();
            
        } else if (pilihan == 4) {
            subLoop = false;
        } else {
            ConsoleView::printError("Pilihan tidak valid!");
            std::cout << "\nTekan ENTER untuk melanjutkan...";
            std::cin.get();
        }
    }
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
