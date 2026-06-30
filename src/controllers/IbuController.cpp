#include "IbuController.h"
#include "../views/ConsoleView.h"
#include "../models/RiwayatModel.h"
#include "../models/FoodRecommendation.h"
#include "../calculator.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

IbuController::IbuController(UserSession& session, ReferenceData& refData) : currentSession(session), refData(refData) {}

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
    
    if (!currentSession.childName.empty()) {
        std::vector<std::vector<std::string>> riwayat = RiwayatModel::getFilteredRiwayat(false, currentSession.childName);
        if (!riwayat.empty()) {
            auto latest = riwayat.back();
            std::cout << "\n" << BOLD << CYAN << " === INFORMASI STATUS GIZI TERAKHIR (" << latest[0] << ") ===" << RESET << std::endl;
            std::cout << CYAN << ConsoleView::LINE_DASH << RESET << std::endl;
            std::cout << " " << BOLD << std::left << std::setw(26) << "Indikator" << " | " << "Status & Rekomendasi" << RESET << std::endl;
            std::cout << CYAN << ConsoleView::LINE_DASH << RESET << std::endl;
            
            std::cout << " " << std::left << std::setw(26) << "Berat Badan (BB/U)" << " | " << BOLD << latest[7] << RESET << std::endl;
            std::cout << " " << std::setw(26) << "" << " | -> " << FoodRecommendation::getRekomendasiBBU(latest[7]) << std::endl;
            std::cout << CYAN << ConsoleView::LINE_DASH << RESET << std::endl;
            
            std::cout << " " << std::left << std::setw(26) << "Tinggi Badan (TB/U)" << " | " << BOLD << latest[9] << RESET << std::endl;
            std::cout << " " << std::setw(26) << "" << " | -> " << FoodRecommendation::getRekomendasiTBU(latest[9]) << std::endl;
            std::cout << CYAN << ConsoleView::LINE_DASH << RESET << std::endl;
            
            std::cout << " " << std::left << std::setw(26) << "Berat thdp Tinggi Badan" << " | " << BOLD << latest[11] << RESET << std::endl;
            std::cout << " " << std::setw(26) << "" << " | -> " << FoodRecommendation::getRekomendasiBBTB(latest[11]) << std::endl;
            std::cout << CYAN << ConsoleView::LINE_DASH << RESET << std::endl;
            
            if (latest.size() >= 13) {
                std::cout << " " << std::left << std::setw(26) << "Tindakan Medis" << " | " << (latest[12] == "RUJUK" ? (RED + BOLD + "RUJUK KELAINAN/GIZI BURUK" + RESET) : (GREEN + "TIDAK (Normal)" + RESET)) << std::endl;
                std::cout << CYAN << ConsoleView::LINE_DASH << RESET << std::endl;
            }
        } else {
            std::cout << YELLOW << " [Belum ada riwayat pemeriksaan]" << RESET << std::endl;
        }
    }
    
    std::cout << MAGENTA << ConsoleView::LINE_DASH << RESET << std::endl;
    std::cout << " [1] Lihat Data & Riwayat Pemeriksaan Anak" << std::endl;
    std::cout << " [2] Pilih Anak Balita" << std::endl;
    std::cout << " [3] Lihat Detail Diagnosis Terakhir" << std::endl;
    std::cout << " [4] Logout" << std::endl;
    std::cout << MAGENTA << ConsoleView::LINE_DASH << RESET << std::endl;
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
        doLihatDetailDiagnosis();
    } else if (menuPilihan == 4) {
        currentSession.isLoggedIn = false;
    } else {
        ConsoleView::printError("Pilihan menu tidak terdaftar!");
        std::cout << "\nTekan ENTER untuk melanjutkan...";
        std::cin.get();
    }
}

void IbuController::doPilihAnak() {
    std::vector<std::string> anakList = UserModel::getAnakByIbu(currentSession.userId);
    if (anakList.empty()) {
        ConsoleView::clearScreen();
        ConsoleView::tampilkanHeader();
        std::cout << BOLD << "--- PILIH ANAK AKTIF ---" << RESET << std::endl;
        ConsoleView::printError("Belum ada data anak terdaftar untuk akun Anda. Silakan hubungi Kader Posyandu.");
        std::cout << "\nTekan ENTER untuk kembali...";
        std::cin.get();
        return;
    }
    
    ConsoleView::clearScreen();
    ConsoleView::tampilkanHeader();
    std::cout << BOLD << "--- PILIH ANAK AKTIF ---" << RESET << std::endl;
    std::cout << " Balita Aktif saat ini: " << (currentSession.childName.empty() ? "[Belum Dipilih]" : currentSession.childName) << std::endl;
    std::cout << MAGENTA << ConsoleView::LINE_DASH << RESET << std::endl;
    for (size_t i = 0; i < anakList.size(); ++i) {
        std::cout << " [" << (i + 1) << "] " << anakList[i] << std::endl;
    }
    std::cout << " [" << (anakList.size() + 1) << "] Kembali ke Menu Utama" << std::endl;
    std::cout << MAGENTA << ConsoleView::LINE_DASH << RESET << std::endl;
    std::cout << "Pilih (1-" << (anakList.size() + 1) << "): ";
    int sel = 0;
    if (std::cin >> sel) {
        if (sel >= 1 && sel <= (int)anakList.size()) {
            currentSession.childName = anakList[sel - 1];
            ConsoleView::printSuccess("Berhasil mengganti anak aktif ke " + currentSession.childName);
        } else if (sel == (int)anakList.size() + 1) {
            // Kembali ke menu utama
        } else {
            ConsoleView::printError("Pilihan tidak valid!");
        }
    } else {
        ConsoleView::printError("Input tidak valid!");
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

void IbuController::doLihatDetailDiagnosis() {
    if (currentSession.childName.empty()) {
        ConsoleView::printError("Silakan pilih balita terlebih dahulu!");
        std::cout << "\nTekan ENTER untuk kembali...";
        std::cin.get();
        return;
    }
    
    std::vector<std::vector<std::string>> riwayat = RiwayatModel::getFilteredRiwayat(false, currentSession.childName);
    if (riwayat.empty()) {
        ConsoleView::printError("Belum ada riwayat pemeriksaan untuk balita ini!");
        std::cout << "\nTekan ENTER untuk kembali...";
        std::cin.get();
        return;
    }

    auto latest = riwayat.back();
    ChildProfile child;
    child.nama = latest[1];
    child.jenis_kelamin = (latest[2] == "Laki-laki") ? 'L' : 'P';
    
    try {
        child.umur_bulan = std::stoi(latest[3]);
        child.berat_kg = std::stod(latest[4]);
        child.tinggi_cm = std::stod(latest[5]);
    } catch (...) {
        ConsoleView::printError("Gagal membaca data riwayat yang tersimpan.");
        std::cout << "\nTekan ENTER untuk kembali...";
        std::cin.get();
        return;
    }

    ZScoreResult resBBU, resPBTBU, resBBH;
    ZScoreCalculator::calculateStatus(refData, child, resBBU, resPBTBU, resBBH);
    
    ConsoleView::printDetailDiagnosis(child, resBBU, resPBTBU, resBBH);
    
    std::cout << "\nTekan ENTER untuk kembali ke Menu Utama...";
    std::cin.get();
}
