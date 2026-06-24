#include "AppController.h"
#include "../views/ConsoleView.h"
#include "../models/RiwayatModel.h"
#include "../json_parser.h"
#include "../calculator.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <cctype>

AppController::AppController() {
    currentSession = {false, 0, "", ""};
}

void AppController::run() {
    std::string pathReference = "data/zscore_reference.json";

    ConsoleView::clearScreen();
    ConsoleView::tampilkanHeader();

    // Memuat data referensi JSON
    if (!JsonParser::loadReferenceData(pathReference, refData)) {
        pathReference = "../data/zscore_reference.json";
        if (!JsonParser::loadReferenceData(pathReference, refData)) {
            ConsoleView::printError("GAGAL memuat data referensi Z-Score!");
            std::cout << "Pastikan file database 'zscore_reference.json' berada di direktori 'data/'." << std::endl;
            std::cout << "Tekan ENTER untuk keluar...";
            std::cin.get();
            return;
        }
    }

    bool appLoop = true;
    while (appLoop) {
        if (!currentSession.isLoggedIn) {
            showLoginScreen();
            // Jika setelah login dipanggil masih belum login (karena pilih keluar)
            if (!currentSession.isLoggedIn) {
                appLoop = false;
            }
        } else {
            if (currentSession.role == 1) {
                showMainMenuKader();
            } else if (currentSession.role == 2) {
                showMainMenuIbu();
            }
        }
    }

    ConsoleView::clearScreen();
    std::cout << std::endl << BOLD << GREEN << "Terima kasih telah menggunakan aplikasi ini. Jaga kesehatan anak Indonesia!" << RESET << std::endl << std::endl;
}

void AppController::showLoginScreen() {
    ConsoleView::clearScreen();
    ConsoleView::tampilkanHeader();
    
    std::cout << BOLD << "--- MENU LOGIN ---" << RESET << std::endl;
    std::cout << " [1] Login sebagai Kader Posyandu" << std::endl;
    std::cout << " [2] Login sebagai Ibu Balita" << std::endl;
    std::cout << " [3] Keluar Aplikasi" << std::endl;
    std::cout << MAGENTA << "---------------------------------------------------------------------" << RESET << std::endl;
    std::cout << "Pilih Role (1-3): ";
    
    int rolePilihan = 0;
    if (!(std::cin >> rolePilihan)) {
        ConsoleView::printError("Input tidak valid! Harap pilih angka 1-3.");
        ConsoleView::clearInput();
        std::cout << "\nTekan ENTER untuk melanjutkan...";
        std::cin.get();
        return;
    }
    ConsoleView::clearInput();

    if (rolePilihan == 3) {
        return; // Kembali ke run, loop akan berhenti
    }

    std::string username, password;

    if (rolePilihan == 1) {
        std::cout << "Username Kader: ";
        std::getline(std::cin, username);
        std::cout << "Password (default: 123): ";
        std::getline(std::cin, password);
        
        currentSession = AuthModel::login(1, username, password);
        if (!currentSession.isLoggedIn) {
            ConsoleView::printError("Username atau password Kader salah!");
            std::cout << "\nTekan ENTER untuk kembali...";
            std::cin.get();
        }
    } else if (rolePilihan == 2) {
        std::cout << "Username Ibu (contoh: ibu_budi / ibu_siti): ";
        std::getline(std::cin, username);
        std::cout << "Password (default: 123): ";
        std::getline(std::cin, password);

        currentSession = AuthModel::login(2, username, password);
        if (!currentSession.isLoggedIn) {
            ConsoleView::printError("Username atau password Ibu salah!");
            std::cout << "\nTekan ENTER untuk kembali...";
            std::cin.get();
        } else {
            ConsoleView::printSuccess("Selamat datang Ibu dari " + currentSession.childName);
            std::cout << "\nTekan ENTER untuk melanjutkan...";
            std::cin.get();
        }
    } else {
        ConsoleView::printError("Pilihan role tidak terdaftar!");
        std::cout << "\nTekan ENTER untuk melanjutkan...";
        std::cin.get();
    }
}

void AppController::showMainMenuKader() {
    ConsoleView::clearScreen();
    ConsoleView::tampilkanHeader();
    
    std::cout << BOLD << "--- MENU UTAMA (KADER POSYANDU: " << currentSession.username << ") ---" << RESET << std::endl;
    std::cout << " [1] Ukur Antropometri Baru" << std::endl;
    std::cout << " [2] Lihat Semua Riwayat Pemeriksaan" << std::endl;
    std::cout << " [3] Lihat Riwayat Rujukan Medis (Gawat)" << std::endl;
    std::cout << " [4] Logout" << std::endl;
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
        doPengukuranBaru();
    } else if (menuPilihan == 2) {
        doLihatRiwayat(false);
    } else if (menuPilihan == 3) {
        doLihatRiwayat(true);
    } else if (menuPilihan == 4) {
        currentSession.isLoggedIn = false;
    } else {
        ConsoleView::printError("Pilihan menu tidak terdaftar!");
        std::cout << "\nTekan ENTER untuk melanjutkan...";
        std::cin.get();
    }
}

void AppController::showMainMenuIbu() {
    ConsoleView::clearScreen();
    ConsoleView::tampilkanHeader();
    
    std::cout << BOLD << "--- MENU UTAMA (IBU BALITA: " << currentSession.childName << ") ---" << RESET << std::endl;
    std::cout << " [1] Lihat Data & Riwayat Pemeriksaan Anak" << std::endl;
    std::cout << " [2] Logout" << std::endl;
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
        doLihatRiwayat(false);
    } else if (menuPilihan == 2) {
        currentSession.isLoggedIn = false;
    } else {
        ConsoleView::printError("Pilihan menu tidak terdaftar!");
        std::cout << "\nTekan ENTER untuk melanjutkan...";
        std::cin.get();
    }
}

void AppController::doPengukuranBaru() {
    ChildProfile child;
    std::string errMsg = "";

    // 1. Input Nama
    while (true) {
        ConsoleView::clearScreen();
        ConsoleView::tampilkanHeader();
        std::cout << BOLD << "--- FORM INPUT DATA BALITA ---" << RESET << std::endl << std::endl;
        if (!errMsg.empty()) {
            ConsoleView::printError(errMsg);
            std::cout << std::endl;
        }
        
        std::cout << "Masukkan Nama Balita: ";
        std::getline(std::cin >> std::ws, child.nama);
        if (!child.nama.empty()) {
            errMsg = "";
            break;
        }
        errMsg = "Nama tidak boleh kosong!";
    }

    // 2. Input Jenis Kelamin
    while (true) {
        ConsoleView::clearScreen();
        ConsoleView::tampilkanHeader();
        std::cout << BOLD << "--- FORM INPUT DATA BALITA ---" << RESET << std::endl << std::endl;
        std::cout << "  - Nama Balita    : " << child.nama << std::endl;
        if (!errMsg.empty()) {
            std::cout << std::endl;
            ConsoleView::printError(errMsg);
        }
        std::cout << std::endl;

        std::cout << "Masukkan Jenis Kelamin (L = Laki-laki, P = Perempuan): ";
        char jk;
        std::cin >> jk;
        jk = std::toupper(static_cast<unsigned char>(jk));
        if (jk == 'L' || jk == 'P') {
            child.jenis_kelamin = jk;
            errMsg = "";
            break;
        }
        errMsg = "Input tidak valid! Harap masukkan 'L' atau 'P'.";
        ConsoleView::clearInput();
    }

    // 3. Input Umur
    while (true) {
        ConsoleView::clearScreen();
        ConsoleView::tampilkanHeader();
        std::cout << BOLD << "--- FORM INPUT DATA BALITA ---" << RESET << std::endl << std::endl;
        std::cout << "  - Nama Balita    : " << child.nama << std::endl;
        std::cout << "  - Jenis Kelamin  : " << (child.jenis_kelamin == 'L' ? "Laki-laki" : "Perempuan") << std::endl;
        if (!errMsg.empty()) {
            std::cout << std::endl;
            ConsoleView::printError(errMsg);
        }
        std::cout << std::endl;

        std::cout << "Masukkan Umur Anak (0 - 60 Bulan): ";
        int umur;
        if (std::cin >> umur) {
            if (umur >= 0 && umur <= 60) {
                child.umur_bulan = umur;
                errMsg = "";
                break;
            }
        }
        errMsg = "Umur harus berupa angka antara 0 hingga 60 bulan!";
        ConsoleView::clearInput();
    }

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

        std::cout << "Masukkan ";
        if (child.umur_bulan <= 24) {
            std::cout << "Panjang Badan (cm) [0-24 bulan diukur telentang]: ";
        } else {
            std::cout << "Tinggi Badan (cm) [25-60 bulan diukur berdiri]: ";
        }
        
        double tinggi;
        if (std::cin >> tinggi) {
            if (tinggi > 0.0 && tinggi < 200.0) {
                child.tinggi_cm = tinggi;
                errMsg = "";
                break;
            }
        }
        errMsg = "Tinggi/panjang badan harus berupa angka positif yang valid (di bawah 200 cm)!";
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
    ConsoleView::clearScreen();
    std::cout << BLUE << "=====================================================================" << RESET << std::endl;
    std::cout << BOLD << BLUE << "                    HASIL DIAGNOSIS STATUS GIZI                      " << RESET << std::endl;
    std::cout << BLUE << "=====================================================================" << RESET << std::endl;
    
    std::cout << BOLD << "PROFIL BALITA:" << RESET << std::endl;
    std::cout << "  - Nama Balita    : " << child.nama << std::endl;
    std::cout << "  - Jenis Kelamin  : " << (child.jenis_kelamin == 'L' ? "Laki-laki" : "Perempuan") << std::endl;
    std::cout << "  - Umur / Usia    : " << child.umur_bulan << " Bulan" 
              << " (" << (child.umur_bulan <= 24 ? "Diukur telentang / Panjang Badan" : "Diukur berdiri / Tinggi Badan") << ")" << std::endl;
    std::cout << "  - Berat Badan    : " << std::fixed << std::setprecision(1) << child.berat_kg << " kg" << std::endl;
    std::cout << "  - Tinggi/Panjang : " << child.tinggi_cm << " cm" 
              << " (Dibulatkan ke: " << (std::round(child.tinggi_cm * 2.0) / 2.0) << " cm)" << std::endl;
    std::cout << BLUE << "---------------------------------------------------------------------" << RESET << std::endl << std::endl;

    std::cout << BOLD << "HASIL DIAGNOSIS 3 INDIKATOR UTAMA:" << RESET << std::endl << std::endl;

    // Indikator 1: BB/U
    std::string colorBBU = ConsoleView::getStatusColor(resBBU.status);
    std::cout << BOLD << "1. Berat Badan menurut Umur (BB/U)" << RESET << std::endl;
    std::cout << "   - Nilai Z-Score : " << std::fixed << std::setprecision(2) << resBBU.z_score << " SD" << std::endl;
    std::cout << "   - Status Gizi   : " << colorBBU << BOLD << "[" << resBBU.nama_status << "]" << RESET << std::endl;
    std::cout << "   - Penjelasan    : " << resBBU.penjelasan << std::endl << std::endl;

    // Indikator 2: PB/U atau TB/U
    std::string labelPBTBU = (child.umur_bulan <= 24) ? "Panjang Badan menurut Umur (PB/U)" : "Tinggi Badan menurut Umur (TB/U)";
    std::string colorPBTBU = ConsoleView::getStatusColor(resPBTBU.status);
    std::cout << BOLD << "2. " << labelPBTBU << RESET << std::endl;
    std::cout << "   - Nilai Z-Score : " << resPBTBU.z_score << " SD" << std::endl;
    std::cout << "   - Status Gizi   : " << colorPBTBU << BOLD << "[" << resPBTBU.nama_status << "]" << RESET << std::endl;
    std::cout << "   - Penjelasan    : " << resPBTBU.penjelasan << std::endl << std::endl;

    // Indikator 3: BB/PB atau BB/TB
    std::string labelBBH = (child.umur_bulan <= 24) ? "Berat Badan menurut Panjang Badan (BB/PB)" : "Berat Badan menurut Tinggi Badan (BB/TB)";
    std::string colorBBH = ConsoleView::getStatusColor(resBBH.status);
    std::cout << BOLD << "3. " << labelBBH << RESET << std::endl;
    if (resBBH.status != StatusGizi::TIDAK_TERDEFINISI) {
        std::cout << "   - Nilai Z-Score : " << resBBH.z_score << " SD" << std::endl;
    } else {
        std::cout << "   - Nilai Z-Score : N/A" << std::endl;
    }
    std::cout << "   - Status Gizi   : " << colorBBH << BOLD << "[" << resBBH.nama_status << "]" << RESET << std::endl;
    std::cout << "   - Penjelasan    : " << resBBH.penjelasan << std::endl << std::endl;

    std::cout << BLUE << "=====================================================================" << RESET << std::endl;

    if (ZScoreCalculator::isCritical(resBBU.status) || 
        ZScoreCalculator::isCritical(resPBTBU.status) || 
        ZScoreCalculator::isCritical(resBBH.status)) {
        
        std::string tindakanRujukan = ZScoreCalculator::getReferralAction(resBBU, resPBTBU, resBBH);
        std::cout << RED << "=====================================================================" << RESET << std::endl;
        std::cout << RED << BOLD << "            ⚠️  REKOMENDASI RUJUKAN & TINDAKAN MEDIS GAWAT  ⚠️" << RESET << std::endl;
        std::cout << RED << "=====================================================================" << RESET << std::endl;
        std::cout << tindakanRujukan << std::endl;
        std::cout << RED << "=====================================================================" << RESET << std::endl << std::endl;
    }
    
    std::cout << "\nTekan ENTER untuk kembali ke Menu Utama...";
    std::cin.get();
}

void AppController::doLihatRiwayat(bool hanyaRujukan) {
    ConsoleView::clearScreen();
    std::string filter = "";
    if (currentSession.role == 2) {
        filter = currentSession.childName;
    }
    
    std::vector<std::vector<std::string>> data = RiwayatModel::getFilteredRiwayat(hanyaRujukan, filter);
    ConsoleView::printRiwayatTable(data, hanyaRujukan);
    
    std::cout << "\nTekan ENTER untuk kembali ke Menu Utama...";
    std::cin.get();
}
