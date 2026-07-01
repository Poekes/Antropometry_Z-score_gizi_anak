#include "KaderController.h"
#include "../views/ConsoleView.h"
#include "../models/RiwayatModel.h"
#include "../calculator.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include "../models/FoodRecommendation.h"

KaderController::KaderController(UserSession& session, ReferenceData& refData) 
    : currentSession(session), refData(refData) {}

void KaderController::run() {
    bool kaderLoop = true;
    while (kaderLoop && currentSession.isLoggedIn) {
        showMainMenu();
    }
}

void KaderController::showMainMenu() {
    ConsoleView::clearScreen();
    ConsoleView::tampilkanHeader();
    
    std::cout << BOLD << "--- MENU UTAMA (KADER POSYANDU: " << currentSession.username << ") ---" << RESET << std::endl;
    std::cout << " [1] Ukur Antropometri Baru" << std::endl;
    std::cout << " [2] Lihat Semua Riwayat Pemeriksaan" << std::endl;
    std::cout << " [3] Lihat Riwayat Rujukan Medis (Gawat)" << std::endl;
    std::cout << " [4] Registrasi Ibu Balita Baru" << std::endl;
    std::cout << " [5] Tambah Anak untuk Ibu Terdaftar" << std::endl;
    std::cout << " [6] Logout" << std::endl;
    std::cout << GREEN << ConsoleView::LINE_DASH << RESET << std::endl;
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
        doPengukuranBaru();
    } else if (menuPilihan == 2) {
        doLihatRiwayat(false);
    } else if (menuPilihan == 3) {
        doLihatRiwayat(true);
    } else if (menuPilihan == 4) {
        doTambahIbuBalita();
    } else if (menuPilihan == 5) {
        doTambahAnakKeIbu();
    } else if (menuPilihan == 6) {
        currentSession.isLoggedIn = false;
    } else {
        ConsoleView::printError("Pilihan menu tidak terdaftar!");
        std::cout << "\nTekan ENTER untuk melanjutkan...";
        std::cin.get();
    }
}

void KaderController::doTambahIbuBalita() {
    ConsoleView::clearScreen();
    ConsoleView::tampilkanHeader();
    std::cout << BOLD << "--- REGISTRASI IBU BALITA BARU ---" << RESET << std::endl << std::endl;
    
    std::string username, password;
    std::cout << "Masukkan Username Ibu baru: ";
    std::getline(std::cin >> std::ws, username);
    
    if (UserModel::userExists(username)) {
        ConsoleView::printError("Username '" + username + "' sudah terdaftar!");
        std::cout << "\nTekan ENTER untuk kembali...";
        std::cin.get();
        return;
    }
    
    std::cout << "Masukkan Password: ";
    std::getline(std::cin, password);
    
    if (UserModel::registerUser(2, username, password)) {
        ConsoleView::printSuccess("Ibu Balita '" + username + "' berhasil didaftarkan!");
    } else {
        ConsoleView::printError("Gagal mendaftarkan Ibu Balita baru.");
    }
    
    std::cout << "\nTekan ENTER untuk kembali...";
    std::cin.get();
}

void KaderController::doTambahAnakKeIbu() {
    ConsoleView::clearScreen();
    ConsoleView::tampilkanHeader();
    std::cout << BOLD << "--- TAMBAH ANAK KE IBU TERDAFTAR ---" << RESET << std::endl << std::endl;
    
    std::string username, namaAnak;
    std::cout << "Masukkan Username Ibu: ";
    std::getline(std::cin >> std::ws, username);
    
    if (!UserModel::userExists(username)) {
        ConsoleView::printError("Username Ibu '" + username + "' tidak ditemukan!");
        std::cout << "\nTekan ENTER untuk kembali...";
        std::cin.get();
        return;
    }
    
    std::cout << "Masukkan Nama Anak Baru: ";
    std::getline(std::cin, namaAnak);
    
    int ibuId = UserModel::getUserIdByUsername(username);
    if (UserModel::linkIbuToAnak(ibuId, namaAnak)) {
        ConsoleView::printSuccess("Anak '" + namaAnak + "' berhasil ditambahkan ke Ibu '" + username + "'!");
    } else {
        ConsoleView::printError("Gagal menambahkan data anak.");
    }
    
    std::cout << "\nTekan ENTER untuk kembali...";
    std::cin.get();
}

void KaderController::doPengukuranBaru() {
    ChildProfile child;
    std::string errMsg = "";

    // Pemilihan Balita
    while (true) {
        ConsoleView::clearScreen();
        ConsoleView::tampilkanHeader();
        std::cout << BOLD << "--- FORM INPUT DATA BALITA ---" << RESET << std::endl << std::endl;
        
        std::vector<ChildProfile> children = RiwayatModel::getUniqueChildren();
        if (children.empty()) {
            std::cout << "Belum ada data balita yang terdaftar." << std::endl;
            std::cout << "Silakan gunakan menu 'Tambah Anak untuk Ibu Terdaftar' terlebih dahulu." << std::endl;
            std::cout << "\nTekan ENTER untuk kembali...";
            std::cin.get();
            return;
        }

        std::cout << "Pilih Balita yang akan diukur:" << std::endl;
        
        int idx = 1;
        for (const auto& c : children) {
            std::cout << " [" << idx << "] " << c.nama << " (" << (c.jenis_kelamin == 'L' ? "Laki-laki" : "Perempuan") << ")" << std::endl;
            idx++;
        }
        
        if (!errMsg.empty()) {
            std::cout << std::endl;
            ConsoleView::printError(errMsg);
        }
        std::cout << "\nPilihan (1-" << (idx - 1) << "): ";
        
        int balitaPilihan = 0;
        if (std::cin >> balitaPilihan) {
            ConsoleView::clearInput();
            if (balitaPilihan >= 1 && balitaPilihan < idx) {
                child.nama = children[balitaPilihan - 1].nama;
                child.jenis_kelamin = children[balitaPilihan - 1].jenis_kelamin;
                child.umur_bulan = children[balitaPilihan - 1].umur_bulan;
                errMsg = "";
                break;
            } else {
                errMsg = "Pilihan tidak valid! Harap pilih angka 1-" + std::to_string(idx - 1) + ".";
            }
        } else {
            errMsg = "Input tidak valid! Harap masukkan angka.";
            ConsoleView::clearInput();
        }
    }


    // 3. Umur sudah didapatkan dari data sebelumnya (via getUniqueChildren)
    // Tidak perlu meminta input umur lagi.

    // 4. Input Berat Badan
    while (true) {
        ConsoleView::clearScreen();
        ConsoleView::tampilkanHeader();
        std::cout << BOLD << "--- FORM INPUT DATA BALITA ---" << RESET << std::endl << std::endl;
        std::cout << "  - Nama Balita    : " << child.nama << std::endl;
        std::cout << "  - Jenis Kelamin  : " << (child.jenis_kelamin == 'L' ? "Laki-laki" : "Perempuan") << std::endl;
        std::cout << "  - Umur / Usia    : " << child.umur_bulan << " Bulan" << std::endl;
        if (!errMsg.empty()) {
            std::cout << std::endl;
            ConsoleView::printError(errMsg);
        }
        std::cout << std::endl;

        std::cout << "Masukkan Berat Badan (kg) [contoh: 8.5]: ";
        double berat;
        if (std::cin >> berat) {
            if (berat > 0.0 && berat < 100.0) {
                child.berat_kg = berat;
                errMsg = "";
                break;
            }
        }
        errMsg = "Berat badan harus berupa angka positif yang valid (di bawah 100 kg)!";
        ConsoleView::clearInput();
    }

    // 5. Input Tinggi/Panjang
    while (true) {
        ConsoleView::clearScreen();
        ConsoleView::tampilkanHeader();
        std::cout << BOLD << "--- FORM INPUT DATA BALITA ---" << RESET << std::endl << std::endl;
        std::cout << "  - Nama Balita    : " << child.nama << std::endl;
        std::cout << "  - Jenis Kelamin  : " << (child.jenis_kelamin == 'L' ? "Laki-laki" : "Perempuan") << std::endl;
        std::cout << "  - Umur / Usia    : " << child.umur_bulan << " Bulan" << std::endl;
        std::cout << "  - Berat Badan    : " << std::fixed << std::setprecision(1) << child.berat_kg << " kg" << std::endl;
        if (!errMsg.empty()) {
            std::cout << std::endl;
            ConsoleView::printError(errMsg);
        }
        std::cout << std::endl;

        std::cout << "Masukkan Tinggi Badan (cm): ";
        
        double tinggi;
        if (std::cin >> tinggi) {
            if (tinggi > 0.0 && tinggi < 200.0) {
                child.tinggi_cm = tinggi;
                errMsg = "";
                break;
            }
        }
        errMsg = "Tinggi badan harus berupa angka positif yang valid (di bawah 200 cm)!";
        ConsoleView::clearInput();
    }
    ConsoleView::clearInput();

    // Kalkulasi
    ZScoreResult resBBU, resPBTBU, resBBH;
    ZScoreCalculator::calculateStatus(refData, child, resBBU, resPBTBU, resBBH);

    // Simpan Ke File CSV
    if (RiwayatModel::simpan(child, resBBU, resPBTBU, resBBH)) {
        ConsoleView::printSuccess("Data berhasil disimpan!");
    } else {
        ConsoleView::printError("Gagal menyimpan data ke riwayat_pemeriksaan.csv!");
    }

    // Tampilkan Hasil Diagnosis
    ConsoleView::printDetailDiagnosis(child, resBBU, resPBTBU, resBBH);
    
    std::cout << "\nTekan ENTER untuk kembali ke Menu Utama...";
    std::cin.get();
}

void KaderController::doLihatRiwayat(bool hanyaRujukan) {
    ConsoleView::clearScreen();
    std::vector<std::vector<std::string>> data = RiwayatModel::getFilteredRiwayat(hanyaRujukan, "");
    ConsoleView::printRiwayatTable(data, hanyaRujukan);
    
    std::cout << "\nTekan ENTER untuk kembali ke Menu Utama...";
    std::cin.get();
}
