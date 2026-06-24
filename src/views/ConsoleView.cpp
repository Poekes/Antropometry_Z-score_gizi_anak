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
    std::cout << MAGENTA << "=====================================================================" << RESET << std::endl;
    std::cout << BOLD << CYAN << "          DETEKSI GIZI ANAK BERDASARKAN Z-SCORE         " << RESET << std::endl;
    std::cout << MAGENTA << "=====================================================================" << RESET << std::endl;
    std::cout << " Alat bantu cepat untuk Kader Posyandu & Bidan Desa dalam menentukan" << std::endl;
    std::cout << " status gizi balita usia 0 hingga 60 bulan secara akurat." << std::endl;
    std::cout << MAGENTA << "---------------------------------------------------------------------" << RESET << std::endl;
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
    std::cout << "=============================================================================================================================" << std::endl;
    if (hanyaRujukan) {
        std::cout << "                                         RIWAYAT BALITA DIRUJUK (KONDISI GAWAT)                                              " << std::endl;
    } else {
        std::cout << "                                              RIWAYAT PEMERIKSAAN GIZI BALITA                                                " << std::endl;
    }
    std::cout << "=============================================================================================================================" << RESET << std::endl;
    std::cout << BOLD;
    std::cout << "| " << std::left << std::setw(3) << "No"
              << " | " << std::left << std::setw(16) << "Waktu Periksa"
              << " | " << std::left << std::setw(15) << "Nama Balita"
              << " | " << std::left << std::setw(3) << "JK"
              << " | " << std::left << std::setw(4) << "Usia"
              << " | " << std::left << std::setw(5) << "Berat"
              << " | " << std::left << std::setw(6) << "Tinggi"
              << " | " << std::left << std::setw(13) << "Status BB/U"
              << " | " << std::left << std::setw(13) << "Status PB-TB/U"
              << " | " << std::left << std::setw(15) << "Status BB/PB-TB"
              << " | " << std::left << std::setw(7) << "Rujukan"
              << " | " << RESET << std::endl;
    std::cout << CYAN << "-----------------------------------------------------------------------------------------------------------------------------" << RESET << std::endl;
    
    int no = 1;
    for (const auto& row : dataRows) {
        if (row.size() < 12) continue;
        
        std::string jk = (row[2] == "Laki-laki") ? "L" : "P";
        std::string bbu = row[7];
        std::string pbu = row[9];
        std::string bbh = row[11];
        
        auto cleanStatus = [](const std::string& status) {
            size_t pos = status.find(" (");
            if (pos != std::string::npos) {
                return status.substr(0, pos);
            }
            return status;
        };
        
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
        
        std::cout << "| " << std::left << std::setw(3) << no++
                  << " | " << std::left << std::setw(16) << row[0]
                  << " | " << std::left << std::setw(15) << (row[1].length() > 15 ? row[1].substr(0, 12) + "..." : row[1])
                  << " | " << std::left << std::setw(3) << jk
                  << " | " << std::left << std::setw(4) << row[3] + " bln"
                  << " | " << std::left << std::setw(5) << row[4] + " kg"
                  << " | " << std::left << std::setw(6) << row[5] + " cm"
                  << " | " << std::left << std::setw(13) << cleanStatus(bbu)
                  << " | " << std::left << std::setw(13) << cleanStatus(pbu)
                  << " | " << std::left << std::setw(15) << cleanStatus(bbh)
                  << " | " << colRujukan << "  "
                  << " |" << std::endl;
    }
    
    std::cout << CYAN << "=============================================================================================================================" << RESET << std::endl;
}
