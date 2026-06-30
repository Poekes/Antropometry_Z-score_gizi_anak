#include "ConsoleView.h"
#include <iostream>
#include <limits>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include "../calculator.h"
#include "../models/FoodRecommendation.h"

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
    std::cout << BOLD << CYAN << std::string(65, ' ') << "DETEKSI GIZI ANAK BERDASARKAN Z-SCORE" << RESET << std::endl;
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
        std::cout << std::string(65, ' ') << "RIWAYAT BALITA DIRUJUK (KONDISI GAWAT)" << std::endl;
    } else {
        std::cout << std::string(68, ' ') << "RIWAYAT PEMERIKSAAN GIZI BALITA" << std::endl;
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
                  
        // Label Detail
        std::cout << "| " << std::left << std::setw(3) << "" 
                  << " | " << BOLD << YELLOW << "Detail Status Gizi:" << RESET << std::endl;
        
        // Baris 2: BB/U & Rujukan
        std::cout << "| " << std::left << std::setw(3) << "" 
                  << " |   " << BOLD << "Berat Badan/Umur   : " << RESET << std::left << std::setw(43) << bbu
                  << " | " << BOLD << "Tindakan Medis: " << RESET << colRujukan << std::endl;
                  
        // Baris 3: PB/U
        std::cout << "| " << std::left << std::setw(3) << "" 
                  << " |   " << BOLD << "Tinggi Badan/Umur  : " << RESET << std::left << std::setw(43) << pbu
                  << " |" << std::endl;
                  
        // Baris 4: BB/PB
        std::cout << "| " << std::left << std::setw(3) << "" 
                  << " |   " << BOLD << "Berat/Tinggi Badan : " << RESET << std::left << std::setw(43) << bbh
                  << " |" << std::endl;
        
        std::cout << CYAN << LINE_DASH << RESET << std::endl;
    }
    
    std::cout << CYAN << LINE_EQ << RESET << std::endl;
}

void ConsoleView::printDetailDiagnosis(const ChildProfile& child, const ZScoreResult& resBBU, const ZScoreResult& resPBTBU, const ZScoreResult& resBBH) {
    clearScreen();
    std::cout << BLUE << LINE_EQ << RESET << std::endl;
    std::cout << BOLD << BLUE << std::string(70, ' ') << "HASIL DIAGNOSIS STATUS GIZI" << RESET << std::endl;
    std::cout << BLUE << LINE_EQ << RESET << std::endl;
    
    std::cout << BOLD << "PROFIL BALITA:" << RESET << std::endl;
    std::cout << "  - Nama Balita    : " << child.nama << std::endl;
    std::cout << "  - Jenis Kelamin  : " << (child.jenis_kelamin == 'L' ? "Laki-laki" : "Perempuan") << std::endl;
    std::cout << "  - Umur / Usia    : " << child.umur_bulan << " Bulan" << std::endl;
    std::cout << "  - Berat Badan    : " << std::fixed << std::setprecision(1) << child.berat_kg << " kg" << std::endl;
    std::cout << "  - Tinggi Badan   : " << child.tinggi_cm << " cm" 
              << " (Dibulatkan ke: " << (std::round(child.tinggi_cm * 2.0) / 2.0) << " cm)" << std::endl;
    std::cout << BLUE << LINE_DASH << RESET << std::endl << std::endl;

    std::cout << BOLD << "HASIL DIAGNOSIS 3 INDIKATOR UTAMA:" << RESET << std::endl << std::endl;

    // Indikator 1: BB/U
    std::string colorBBU = getStatusColor(resBBU.status);
    std::cout << BOLD << "1. Berat Badan (BB/U)" << RESET << std::endl;
    std::cout << "   - Nilai Z-Score : " << std::fixed << std::setprecision(2) << resBBU.z_score << " SD" << std::endl;
    std::cout << "   - Status Gizi   : " << colorBBU << BOLD << "[" << resBBU.nama_status << "]" << RESET << std::endl;
    std::cout << "   - Penjelasan    : " << resBBU.penjelasan << std::endl;
    std::cout << "   - Rekomendasi   : " << FoodRecommendation::getRekomendasiBBU(resBBU.nama_status) << std::endl << std::endl;

    // Indikator 2: TB/U
    std::string labelPBTBU = "Tinggi Badan (TB/U)";
    std::string colorPBTBU = getStatusColor(resPBTBU.status);
    std::cout << BOLD << "2. " << labelPBTBU << RESET << std::endl;
    std::cout << "   - Nilai Z-Score : " << resPBTBU.z_score << " SD" << std::endl;
    std::cout << "   - Status Gizi   : " << colorPBTBU << BOLD << "[" << resPBTBU.nama_status << "]" << RESET << std::endl;
    std::cout << "   - Penjelasan    : " << resPBTBU.penjelasan << std::endl;
    std::cout << "   - Rekomendasi   : " << FoodRecommendation::getRekomendasiTBU(resPBTBU.nama_status) << std::endl << std::endl;

    // Indikator 3: BB/TB
    std::string labelBBH = "Berat Badan menurut Tinggi Badan (BB/TB)";
    std::string colorBBH = getStatusColor(resBBH.status);
    std::cout << BOLD << "3. " << labelBBH << RESET << std::endl;
    if (resBBH.status != StatusGizi::TIDAK_TERDEFINISI) {
        std::cout << "   - Nilai Z-Score : " << resBBH.z_score << " SD" << std::endl;
    } else {
        std::cout << "   - Nilai Z-Score : N/A" << std::endl;
    }
    std::cout << "   - Status Gizi   : " << colorBBH << BOLD << "[" << resBBH.nama_status << "]" << RESET << std::endl;
    std::cout << "   - Penjelasan    : " << resBBH.penjelasan << std::endl;
    std::cout << "   - Rekomendasi   : " << FoodRecommendation::getRekomendasiBBTB(resBBH.nama_status) << std::endl << std::endl;

    std::cout << BLUE << LINE_EQ << RESET << std::endl;

    if (ZScoreCalculator::isCritical(resBBU.status) || 
        ZScoreCalculator::isCritical(resPBTBU.status) || 
        ZScoreCalculator::isCritical(resBBH.status)) {
        
        std::string tindakanRujukan = ZScoreCalculator::getReferralAction(resBBU, resPBTBU, resBBH);
        std::cout << RED << LINE_EQ << RESET << std::endl;
        std::cout << RED << BOLD << std::string(59, ' ') << "⚠️  REKOMENDASI RUJUKAN & TINDAKAN MEDIS GAWAT  ⚠️" << RESET << std::endl;
        std::cout << RED << LINE_EQ << RESET << std::endl;
        std::cout << tindakanRujukan << std::endl;
        std::cout << RED << LINE_EQ << RESET << std::endl << std::endl;
    }
}
