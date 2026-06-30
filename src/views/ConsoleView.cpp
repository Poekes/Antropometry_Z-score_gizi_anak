#include "ConsoleView.h"
#include <iostream>
#include <limits>
#include <iomanip>
#include <cstdlib>

const std::string RESET   = "\033[0m";
const std::string BOLD    = "\033[1m";
const std::string RED     = "\033[31m";
const std::string GREEN   = "\033[32m";
const std::string YELLOW  = "\033[33m";
const std::string BLUE    = "\033[34m";
const std::string CYAN    = "\033[36m";
const std::string MAGENTA = "\033[35m";

const std::string ConsoleView::LINE_EQ = std::string(168, '=');
const std::string ConsoleView::LINE_DASH = std::string(168, '-');

void ConsoleView::clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void ConsoleView::clearScreen() {
    #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif
}

std::string ConsoleView::getStatusColor(StatusGizi status) {
    switch (status) {
        case StatusGizi::BB_NORMAL:
        case StatusGizi::TB_NORMAL:
        case StatusGizi::GIZI_BAIK:
            return GREEN;
        case StatusGizi::BB_KURANG:
        case StatusGizi::TB_PENDEK:
        case StatusGizi::BB_RISIKO_LEBIH:
        case StatusGizi::BERISIKO_GIZI_LEBIH:
        case StatusGizi::GIZI_KURANG:
            return YELLOW;
        case StatusGizi::BB_SANGAT_KURANG:
        case StatusGizi::TB_SANGAT_PENDEK:
        case StatusGizi::GIZI_BURUK:
        case StatusGizi::GIZI_LEBIH:
        case StatusGizi::OBESITAS:
            return RED;
        case StatusGizi::TB_TINGGI:
            return CYAN;
        default:
            return RESET;
    }
}

void ConsoleView::tampilkanHeader() {
    std::cout << MAGENTA << LINE_EQ << RESET << std::endl;
    std::cout << BOLD << CYAN << std::string(65, ' ') << "DETEKSI GIZI ANAK BERDASARKAN Z-SCORE" << std::string(66, ' ') << RESET << std::endl;
    std::cout << MAGENTA << LINE_EQ << RESET << std::endl;
    std::cout << " Alat bantu cepat untuk Kader Posyandu & Bidan Desa dalam menentukan" << std::endl;
    std::cout << " status gizi balita usia 0 hingga 60 bulan secara akurat." << std::endl;
    std::cout << MAGENTA << LINE_DASH << RESET << std::endl;
}

void ConsoleView::printError(const std::string& errorMsg) {
    std::cout << RED << BOLD << "[!] Error: " << errorMsg << RESET << std::endl;
}

void ConsoleView::printSuccess(const std::string& successMsg) {
    std::cout << GREEN << BOLD << "[✓] " << successMsg << RESET << std::endl;
}

void ConsoleView::printRiwayatTable(const std::vector<std::vector<std::string>>& dataRows, bool hanyaRujukan) {
    if (dataRows.empty()) {
        if (hanyaRujukan) {
            std::cout << GREEN << BOLD << "\n[✓] Kabar Baik! Tidak ada riwayat balita yang memerlukan rujukan medis saat ini." << RESET << std::endl;
        } else {
            std::cout << YELLOW << "Belum ada riwayat pemeriksaan anak terdaftar." << RESET << std::endl;
        }
        return;
    }
    
    std::cout << BOLD << CYAN;
    std::cout << LINE_EQ << std::endl;
    if (hanyaRujukan) {
        std::cout << std::string(65, ' ') << "RIWAYAT BALITA DIRUJUK (KONDISI GAWAT)" << std::string(65, ' ') << std::endl;
    } else {
        std::cout << std::string(68, ' ') << "RIWAYAT PEMERIKSAAN GIZI BALITA" << std::string(69, ' ') << std::endl;
    }
    std::cout << LINE_EQ << RESET << std::endl;
    std::cout << BOLD;
    std::cout << "| " << std::left << std::setw(3) << "No"
              << " | " << std::left << std::setw(16) << "Waktu Periksa"
              << " | " << std::left << std::setw(25) << "Nama Balita"
              << " | " << std::left << std::setw(3) << "JK"
              << " | " << std::left << std::setw(6) << "Usia"
              << " | " << std::left << std::setw(8) << "Berat"
              << " | " << std::left << std::setw(8) << "Tinggi"
              << " |" << RESET << std::endl;
    std::cout << CYAN << LINE_DASH << RESET << std::endl;
    
    int no = 1;
    for (const auto& row : dataRows) {
        if (row.size() < 12) continue;
        
        std::string jk = (row[2] == "Laki-laki") ? "L" : "P";
        std::string bbu = row[7];
        std::string pbu = row[9];
        std::string bbh = row[11];
        
        std::string rujukan = "TIDAK";
        if (row.size() >= 13) {
            rujukan = row[12];
        } else {
            if (bbu.find("Sangat Kurang") != std::string::npos || 
                pbu.find("Sangat Pendek") != std::string::npos || 
                bbh.find("Gizi Buruk") != std::string::npos ||
                bbh.find("Obesitas") != std::string::npos) {
                rujukan = "RUJUK";
            }
        }
        
        std::string colRujukan = (rujukan == "RUJUK") ? (RED + BOLD + "RUJUK" + RESET) : (GREEN + "TIDAK" + RESET);
        
        // Baris 1: Informasi Dasar
        std::cout << "| " << std::left << std::setw(3) << no++
                  << " | " << std::left << std::setw(16) << row[0]
                  << " | " << std::left << std::setw(25) << row[1]
                  << " | " << std::left << std::setw(3) << jk
                  << " | " << std::left << std::setw(6) << row[3] + " bln"
                  << " | " << std::left << std::setw(8) << row[4] + " kg"
                  << " | " << std::left << std::setw(8) << row[5] + " cm"
                  << " |" << std::endl;
        
        // Baris 2: BB/U & Rujukan
        std::cout << "| " << std::left << std::setw(3) << "" 
                  << " | " << BOLD << "BB/U    : " << RESET << std::left << std::setw(40) << bbu
                  << " | " << BOLD << "Rujukan: " << RESET << colRujukan << std::endl;
                  
        // Baris 3: PB/U
        std::cout << "| " << std::left << std::setw(3) << "" 
                  << " | " << BOLD << "PB-TB/U : " << RESET << std::left << std::setw(40) << pbu
                  << " |" << std::endl;
                  
        // Baris 4: BB/PB
        std::cout << "| " << std::left << std::setw(3) << "" 
                  << " | " << BOLD << "BB/PB-TB: " << RESET << std::left << std::setw(40) << bbh
                  << " |" << std::endl;
        
        std::cout << CYAN << LINE_DASH << RESET << std::endl;
    }
    
    std::cout << CYAN << LINE_EQ << RESET << std::endl;
}
