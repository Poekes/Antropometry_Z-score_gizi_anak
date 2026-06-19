#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <cmath>
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
    std::cout << BOLD << CYAN << "      KALKULATOR GIZI ANAK & DETEKSI Z-SCORE (KEMENKES RI 2020)      " << RESET << std::endl;
    std::cout << MAGENTA << "=====================================================================" << RESET << std::endl;
    std::cout << " Alat bantu cepat untuk Kader Posyandu & Bidan Desa dalam menentukan" << std::endl;
    std::cout << " status gizi balita usia 0 hingga 60 bulan secara akurat." << std::endl;
    std::cout << MAGENTA << "---------------------------------------------------------------------" << RESET << std::endl;
}

int main() {
    ReferenceData refData;
    std::string pathReference = "data/zscore_reference.json";

    tampilkanHeader();

    // Memuat data referensi JSON
    std::cout << "Memuat data referensi Z-Score dari [" << pathReference << "]... ";
    if (!JsonParser::loadReferenceData(pathReference, refData)) {
        // Coba jalur alternatif jika dijalankan dari subfolder berbeda
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
    std::cout << GREEN << "BERHASIL!" << RESET << std::endl << std::endl;

    bool mainLoop = true;
    while (mainLoop) {
        ChildProfile child;
        
        std::cout << BOLD << "--- INPUT DATA BALITA ---" << RESET << std::endl;

        // 1. Input Nama Balita
        while (true) {
            std::cout << "Masukkan Nama Balita: ";
            std::getline(std::cin >> std::ws, child.nama);
            if (!child.nama.empty()) {
                break;
            }
            std::cout << RED << "Nama tidak boleh kosong!" << RESET << std::endl;
        }

        // 2. Input Jenis Kelamin
        while (true) {
            std::cout << "Masukkan Jenis Kelamin (L = Laki-laki, P = Perempuan): ";
            char jk;
            std::cin >> jk;
            jk = std::toupper(static_cast<unsigned char>(jk));
            if (jk == 'L' || jk == 'P') {
                child.jenis_kelamin = jk;
                break;
            }
            std::cout << RED << "Input tidak valid! Masukkan 'L' atau 'P'." << RESET << std::endl;
            clearInput();
        }

        // 3. Input Umur dalam Bulan
        while (true) {
            std::cout << "Masukkan Umur Anak (0 - 60 Bulan): ";
            int umur;
            if (std::cin >> umur) {
                if (umur >= 0 && umur <= 60) {
                    child.umur_bulan = umur;
                    break;
                }
            }
            std::cout << RED << "Umur harus berupa angka antara 0 hingga 60 bulan!" << RESET << std::endl;
            clearInput();
        }

        // 4. Input Berat Badan (kg)
        while (true) {
            std::cout << "Masukkan Berat Badan (kg) [contoh: 8.5]: ";
            double berat;
            if (std::cin >> berat) {
                if (berat > 0.0 && berat < 100.0) {
                    child.berat_kg = berat;
                    break;
                }
            }
            std::cout << RED << "Berat badan harus berupa angka positif yang valid!" << RESET << std::endl;
            clearInput();
        }

        // 5. Input Tinggi/Panjang Badan (cm)
        while (true) {
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
                    break;
                }
            }
            std::cout << RED << "Tinggi/panjang badan harus berupa angka positif yang valid!" << RESET << std::endl;
            clearInput();
        }

        // Bersihkan buffer input setelah semua data terisi
        clearInput();

        // 6. Jalankan Perhitungan Z-score
        ZScoreResult resBBU, resPBTBU, resBBH;
        ZScoreCalculator::calculateStatus(refData, child, resBBU, resPBTBU, resBBH);

        // 7. Tampilkan Hasil
        std::cout << std::endl;
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

        // Tampilkan Indikator 1: BB/U
        std::string colorBBU = getStatusColor(resBBU.status);
        std::cout << BOLD << "1. Berat Badan menurut Umur (BB/U)" << RESET << std::endl;
        std::cout << "   - Nilai Z-Score : " << std::fixed << std::setprecision(2) << resBBU.z_score << " SD" << std::endl;
        std::cout << "   - Status Gizi   : " << colorBBU << BOLD << "[" << resBBU.nama_status << "]" << RESET << std::endl;
        std::cout << "   - Penjelasan    : " << resBBU.penjelasan << std::endl << std::endl;

        // Tampilkan Indikator 2: PB/U atau TB/U
        std::string labelPBTBU = (child.umur_bulan <= 24) ? "Panjang Badan menurut Umur (PB/U)" : "Tinggi Badan menurut Umur (TB/U)";
        std::string colorPBTBU = getStatusColor(resPBTBU.status);
        std::cout << BOLD << "2. " << labelPBTBU << RESET << std::endl;
        std::cout << "   - Nilai Z-Score : " << resPBTBU.z_score << " SD" << std::endl;
        std::cout << "   - Status Gizi   : " << colorPBTBU << BOLD << "[" << resPBTBU.nama_status << "]" << RESET << std::endl;
        std::cout << "   - Penjelasan    : " << resPBTBU.penjelasan << std::endl << std::endl;

        // Tampilkan Indikator 3: BB/PB atau BB/TB
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

        // Menanyakan apakah ingin menghitung lagi
        while (true) {
            std::cout << "Apakah Anda ingin menghitung data anak lain? (y/n): ";
            char pilihan;
            std::cin >> pilihan;
            pilihan = std::tolower(static_cast<unsigned char>(pilihan));
            clearInput();
            
            if (pilihan == 'y') {
                std::cout << std::endl;
                break;
            } else if (pilihan == 'n') {
                mainLoop = false;
                break;
            }
            std::cout << RED << "Input tidak valid! Masukkan 'y' atau 'n'." << RESET << std::endl;
        }
    }

    std::cout << std::endl << BOLD << GREEN << "Terima kasih telah menggunakan aplikasi ini. Jaga kesehatan anak Indonesia!" << RESET << std::endl;
    return 0;
}
