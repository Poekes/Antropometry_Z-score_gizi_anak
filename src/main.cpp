#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <sstream>
#include "zscore_data.h"
#include "json_parser.h"
#include "calculator.h"

// Konstanta warna ANSI untuk CLI yang menarik
const std::string RESET   = "\033[0m";
const std::string BOLD    = "\033[1m";
const std::string RED     = "\033[31m";
const std::string GREEN   = "\033[32m";
const std::string YELLOW  = "\033[33m";
const std::string BLUE    = "\033[34m";
const std::string CYAN    = "\033[36m";
const std::string MAGENTA = "\033[35m";

// Fungsi untuk membersihkan buffer input C++
void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Fungsi portabel untuk membersihkan layar terminal
void clearScreen() {
    #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif
}

// Fungsi bantu untuk mendapatkan warna berdasarkan kategori status gizi
std::string getStatusColor(StatusGizi status) {
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

// Menampilkan header aplikasi
void tampilkanHeader() {
    std::cout << MAGENTA << "=====================================================================" << RESET << std::endl;
    std::cout << BOLD << CYAN << "       GIZI ANAK & DETEKSI Z-SCORE (KEMENKES RI 2020)      " << RESET << std::endl;
    std::cout << MAGENTA << "=====================================================================" << RESET << std::endl;
    std::cout << " Alat bantu cepat untuk Kader Posyandu & Bidan Desa dalam menentukan" << std::endl;
    std::cout << " status gizi balita usia 0 hingga 60 bulan secara akurat." << std::endl;
    std::cout << MAGENTA << "---------------------------------------------------------------------" << RESET << std::endl;
}

// Menyimpan hasil kalkulasi pemeriksaan ke file CSV eksternal
void simpanKeRiwayat(const ChildProfile& child, const ZScoreResult& bbu, const ZScoreResult& pbtbu, const ZScoreResult& bbh) {
    std::string filename = "riwayat_pemeriksaan.csv";
    
    // Periksa apakah berkas kosong/belum ada untuk menulis header
    std::ifstream testFile(filename);
    bool writeHeader = !testFile.is_open() || testFile.peek() == std::ifstream::traits_type::eof();
    testFile.close();
    
    std::ofstream outFile(filename, std::ios::app);
    if (!outFile.is_open()) {
        std::cerr << RED << "[ERROR] Gagal membuka file riwayat untuk mencatat data!" << RESET << std::endl;
        return;
    }
    
    if (writeHeader) {
        outFile << "Tanggal/Waktu,Nama,Jenis Kelamin,Usia (Bulan),Berat (kg),Tinggi (cm),"
                << "Z-Score BB/U,Status BB/U,"
                << "Z-Score PB_TB/U,Status PB_TB/U,"
                << "Z-Score BB/PB_TB,Status BB/PB_TB,Rujukan\n";
    }
    
    // Mengambil waktu lokal sistem saat ini
    std::time_t rawtime = std::time(nullptr);
    struct std::tm* timeinfo = std::localtime(&rawtime);
    char timeBuffer[80];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M", timeinfo);
    
    // Ganti koma di nama anak agar struktur CSV tidak rusak
    std::string cleanName = child.nama;
    std::replace(cleanName.begin(), cleanName.end(), ',', ' ');
    
    // Tentukan status rujukan kritis
    bool rujukan = ZScoreCalculator::isCritical(bbu.status) || 
                    ZScoreCalculator::isCritical(pbtbu.status) || 
                    ZScoreCalculator::isCritical(bbh.status);
    
    outFile << timeBuffer << ","
            << cleanName << ","
            << (child.jenis_kelamin == 'L' ? "Laki-laki" : "Perempuan") << ","
            << child.umur_bulan << ","
            << std::fixed << std::setprecision(1) << child.berat_kg << ","
            << std::fixed << std::setprecision(1) << child.tinggi_cm << ","
            << std::fixed << std::setprecision(2) << bbu.z_score << "," << bbu.nama_status << ","
            << std::fixed << std::setprecision(2) << pbtbu.z_score << "," << pbtbu.nama_status << ",";
            
    if (bbh.status != StatusGizi::TIDAK_TERDEFINISI) {
        outFile << std::fixed << std::setprecision(2) << bbh.z_score << "," << bbh.nama_status << ",";
    } else {
        outFile << "N/A," << bbh.nama_status << ",";
    }
    
    outFile << (rujukan ? "RUJUK" : "TIDAK") << "\n";
    
    outFile.close();
}

// Pemisah kolom CSV sederhana
std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(line);
    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }
    return tokens;
}

// Membaca file CSV dan menampilkannya sebagai tabel rapi di console
void tampilkanRiwayat(bool hanyaRujukan = false) {
    std::string filename = "riwayat_pemeriksaan.csv";
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cout << YELLOW << "Belum ada catatan riwayat pemeriksaan (berkas belum ada)." << RESET << std::endl;
        std::cout << "Silakan pilih menu [1] untuk mengisi pemeriksaan baru." << std::endl;
        return;
    }
    
    std::string line;
    // Lewati baris header CSV
    if (!std::getline(inFile, line)) {
        std::cout << YELLOW << "Berkas riwayat kosong." << RESET << std::endl;
        return;
    }
    
    std::vector<std::vector<std::string>> dataRows;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        dataRows.push_back(splitCsvLine(line));
    }
    inFile.close();
    
    if (dataRows.empty()) {
        std::cout << YELLOW << "Belum ada riwayat pemeriksaan anak terdaftar." << RESET << std::endl;
        return;
    }
    
    // Lakukan pemfilteran data jika hanya menampilkan rujukan
    std::vector<std::vector<std::string>> filteredRows;
    for (const auto& row : dataRows) {
        if (row.size() < 12) continue;
        
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
        
        if (hanyaRujukan) {
            if (rujukan == "RUJUK") {
                filteredRows.push_back(row);
            }
        } else {
            filteredRows.push_back(row);
        }
    }
    
    if (filteredRows.empty()) {
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
    for (const auto& row : filteredRows) {
        std::string jk = (row[2] == "Laki-laki") ? "L" : "P";
        std::string bbu = row[7];
        std::string pbu = row[9];
        std::string bbh = row[11];
        
        // Mempersingkat teks medis yang panjang agar muat di kolom
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

int main() {
    ReferenceData refData;
    std::string pathReference = "data/zscore_reference.json";

    clearScreen();
    tampilkanHeader();

    // Memuat data referensi JSON
    std::cout << "Memuat data referensi Z-Score dari [" << pathReference << "]... ";
    if (!JsonParser::loadReferenceData(pathReference, refData)) {
        pathReference = "../data/zscore_reference.json";
        std::cout << std::endl << "Mencoba jalur alternatif [" << pathReference << "]... ";
        if (!JsonParser::loadReferenceData(pathReference, refData)) {
            std::cout << RED << "GAGAL!" << RESET << std::endl;
            std::cout << "Pastikan file database 'zscore_reference.json' berada di direktori 'data/'." << std::endl;
            std::cout << "Tekan ENTER untuk keluar...";
            std::cin.get();
            return 1;
        }
    }
    std::cout << GREEN << "BERHASIL!" << RESET << std::endl;
    std::cout << "Tekan ENTER untuk masuk ke Menu Utama...";
    std::cin.get();

    bool mainLoop = true;
    while (mainLoop) {
        clearScreen();
        tampilkanHeader();
        
        std::cout << BOLD << "--- MENU UTAMA ---" << RESET << std::endl;
        std::cout << " [1] Ukur Antropometri Baru" << std::endl;
        std::cout << " [2] Lihat Semua Riwayat Pemeriksaan" << std::endl;
        std::cout << " [3] Lihat Riwayat Rujukan Medis (Gawat)" << std::endl;
        std::cout << " [4] Keluar Aplikasi" << std::endl;
        std::cout << MAGENTA << "---------------------------------------------------------------------" << RESET << std::endl;
        std::cout << "Pilih Menu (1-4): ";
        
        int menuPilihan = 0;
        if (!(std::cin >> menuPilihan)) {
            std::cout << RED << "Input tidak valid! Harap pilih angka 1-4." << RESET << std::endl;
            clearInput();
            std::cout << "\nTekan ENTER untuk melanjutkan...";
            std::cin.get();
            continue;
        }
        clearInput();

        if (menuPilihan == 1) {
            ChildProfile child;
            std::string errMsg = "";

            // 1. Input Nama
            while (true) {
                clearScreen();
                tampilkanHeader();
                std::cout << BOLD << "--- FORM INPUT DATA BALITA ---" << RESET << std::endl << std::endl;
                if (!errMsg.empty()) {
                    std::cout << RED << BOLD << "[!] Error: " << errMsg << RESET << std::endl << std::endl;
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
                clearScreen();
                tampilkanHeader();
                std::cout << BOLD << "--- FORM INPUT DATA BALITA ---" << RESET << std::endl << std::endl;
                std::cout << "  - Nama Balita    : " << child.nama << std::endl;
                if (!errMsg.empty()) {
                    std::cout << RED << BOLD << "\n[!] Error: " << errMsg << RESET << std::endl;
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
                clearInput();
            }

            // 3. Input Umur
            while (true) {
                clearScreen();
                tampilkanHeader();
                std::cout << BOLD << "--- FORM INPUT DATA BALITA ---" << RESET << std::endl << std::endl;
                std::cout << "  - Nama Balita    : " << child.nama << std::endl;
                std::cout << "  - Jenis Kelamin  : " << (child.jenis_kelamin == 'L' ? "Laki-laki" : "Perempuan") << std::endl;
                if (!errMsg.empty()) {
                    std::cout << RED << BOLD << "\n[!] Error: " << errMsg << RESET << std::endl;
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
                clearInput();
            }

            // 4. Input Berat Badan
            while (true) {
                clearScreen();
                tampilkanHeader();
                std::cout << BOLD << "--- FORM INPUT DATA BALITA ---" << RESET << std::endl << std::endl;
                std::cout << "  - Nama Balita    : " << child.nama << std::endl;
                std::cout << "  - Jenis Kelamin  : " << (child.jenis_kelamin == 'L' ? "Laki-laki" : "Perempuan") << std::endl;
                std::cout << "  - Umur / Usia    : " << child.umur_bulan << " Bulan" << std::endl;
                if (!errMsg.empty()) {
                    std::cout << RED << BOLD << "\n[!] Error: " << errMsg << RESET << std::endl;
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
                clearInput();
            }

            // 5. Input Tinggi/Panjang
            while (true) {
                clearScreen();
                tampilkanHeader();
                std::cout << BOLD << "--- FORM INPUT DATA BALITA ---" << RESET << std::endl << std::endl;
                std::cout << "  - Nama Balita    : " << child.nama << std::endl;
                std::cout << "  - Jenis Kelamin  : " << (child.jenis_kelamin == 'L' ? "Laki-laki" : "Perempuan") << std::endl;
                std::cout << "  - Umur / Usia    : " << child.umur_bulan << " Bulan" << std::endl;
                std::cout << "  - Berat Badan    : " << std::fixed << std::setprecision(1) << child.berat_kg << " kg" << std::endl;
                if (!errMsg.empty()) {
                    std::cout << RED << BOLD << "\n[!] Error: " << errMsg << RESET << std::endl;
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
                clearInput();
            }
            clearInput();

            // Kalkulasi
            ZScoreResult resBBU, resPBTBU, resBBH;
            ZScoreCalculator::calculateStatus(refData, child, resBBU, resPBTBU, resBBH);

            // Simpan Ke File CSV
            simpanKeRiwayat(child, resBBU, resPBTBU, resBBH);

            // Tampilkan Hasil Diagnosis
            clearScreen();
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
            std::string colorBBU = getStatusColor(resBBU.status);
            std::cout << BOLD << "1. Berat Badan menurut Umur (BB/U)" << RESET << std::endl;
            std::cout << "   - Nilai Z-Score : " << std::fixed << std::setprecision(2) << resBBU.z_score << " SD" << std::endl;
            std::cout << "   - Status Gizi   : " << colorBBU << BOLD << "[" << resBBU.nama_status << "]" << RESET << std::endl;
            std::cout << "   - Penjelasan    : " << resBBU.penjelasan << std::endl << std::endl;

            // Indikator 2: PB/U atau TB/U
            std::string labelPBTBU = (child.umur_bulan <= 24) ? "Panjang Badan menurut Umur (PB/U)" : "Tinggi Badan menurut Umur (TB/U)";
            std::string colorPBTBU = getStatusColor(resPBTBU.status);
            std::cout << BOLD << "2. " << labelPBTBU << RESET << std::endl;
            std::cout << "   - Nilai Z-Score : " << resPBTBU.z_score << " SD" << std::endl;
            std::cout << "   - Status Gizi   : " << colorPBTBU << BOLD << "[" << resPBTBU.nama_status << "]" << RESET << std::endl;
            std::cout << "   - Penjelasan    : " << resPBTBU.penjelasan << std::endl << std::endl;

            // Indikator 3: BB/PB atau BB/TB
            std::string labelBBH = (child.umur_bulan <= 24) ? "Berat Badan menurut Panjang Badan (BB/PB)" : "Berat Badan menurut Tinggi Badan (BB/TB)";
            std::string colorBBH = getStatusColor(resBBH.status);
            std::cout << BOLD << "3. " << labelBBH << RESET << std::endl;
            if (resBBH.status != StatusGizi::TIDAK_TERDEFINISI) {
                std::cout << "   - Nilai Z-Score : " << resBBH.z_score << " SD" << std::endl;
            } else {
                std::cout << "   - Nilai Z-Score : N/A" << std::endl;
            }
            std::cout << "   - Status Gizi   : " << colorBBH << BOLD << "[" << resBBH.nama_status << "]" << RESET << std::endl;
            std::cout << "   - Penjelasan    : " << resBBH.penjelasan << std::endl << std::endl;

            std::cout << BLUE << "=====================================================================" << RESET << std::endl;

            // Cek apakah ada kondisi kritis yang memerlukan rujukan segera
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

            std::cout << GREEN << BOLD << "Info: Data di atas telah otomatis disimpan ke file 'riwayat_pemeriksaan.csv'!" << RESET << std::endl;
            
            std::cout << "\nTekan ENTER untuk kembali ke Menu Utama...";
            std::cin.get();
        } 
        else if (menuPilihan == 2) {
            clearScreen();
            tampilkanRiwayat(false);
            std::cout << "\nTekan ENTER untuk kembali ke Menu Utama...";
            std::cin.get();
        } 
        else if (menuPilihan == 3) {
            clearScreen();
            tampilkanRiwayat(true);
            std::cout << "\nTekan ENTER untuk kembali ke Menu Utama...";
            std::cin.get();
        } 
        else if (menuPilihan == 4) {
            mainLoop = false;
        } 
        else {
            std::cout << RED << "Pilihan menu tidak terdaftar! Pilih 1, 2, 3, atau 4." << RESET << std::endl;
            std::cout << "\nTekan ENTER untuk melanjutkan...";
            std::cin.get();
        }
    }

    clearScreen();
    std::cout << std::endl << BOLD << GREEN << "Terima kasih telah menggunakan aplikasi ini. Jaga kesehatan anak Indonesia!" << RESET << std::endl << std::endl;
    return 0;
}
