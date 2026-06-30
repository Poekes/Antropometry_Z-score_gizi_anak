#include "ChildSeeder.h"
#include "../models/RiwayatModel.h"
#include "../calculator.h"
#include <fstream>

#include <ctime>
#include <sstream>

// Helper untuk menghitung umur dalam bulan dari format YYYY-MM-DD
static int hitungUmurBulan(const std::string& tanggalLahir) {
    int tahunLahir, bulanLahir, hariLahir;
    char sep1, sep2;
    std::stringstream ss(tanggalLahir);
    ss >> tahunLahir >> sep1 >> bulanLahir >> sep2 >> hariLahir;

    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    int tahunSekarang = now->tm_year + 1900;
    int bulanSekarang = now->tm_mon + 1;
    int hariSekarang = now->tm_mday;

    int umurBulan = (tahunSekarang - tahunLahir) * 12 + (bulanSekarang - bulanLahir);
    if (hariSekarang < hariLahir) {
        umurBulan--;
    }
    return umurBulan > 0 ? umurBulan : 0;
}

void ChildSeeder::seed(const ReferenceData& refData) {
    std::string filename = "riwayat_pemeriksaan.csv";
    
    // Periksa apakah file sudah ada dan tidak kosong
    std::ifstream checkFile(filename);
    if (checkFile.is_open()) {
        std::string firstLine;
        if (std::getline(checkFile, firstLine)) {
            // File sudah berisi data (minimal header), batalkan seeding
            checkFile.close();
            return;
        }
        checkFile.close();
    }

    // Jika file tidak ada atau kosong, lakukan seeding data
    
    // Data 1: Budi
    ChildProfile child1;
    child1.nama = "Budi";
    child1.jenis_kelamin = 'L';
    // Gunakan fungsi hitungUmurBulan, misalnya lahir 12 bulan yang lalu (2025-06-30 asumsi sekarang 2026-06-30)
    child1.umur_bulan = hitungUmurBulan("2025-06-30"); 
    child1.berat_kg = 9.5;
    child1.tinggi_cm = 75.0;
    
    ZScoreResult r1BBU, r1PBTBU, r1BBH;
    ZScoreCalculator::calculateStatus(refData, child1, r1BBU, r1PBTBU, r1BBH);
    RiwayatModel::simpan(child1, r1BBU, r1PBTBU, r1BBH);

    // Data 2: Siti
    ChildProfile child2;
    child2.nama = "Siti";
    child2.jenis_kelamin = 'P';
    // Lahir 24 bulan yang lalu (2024-06-30)
    child2.umur_bulan = hitungUmurBulan("2024-06-30");
    child2.berat_kg = 11.0;
    child2.tinggi_cm = 85.0;
    
    ZScoreResult r2BBU, r2PBTBU, r2BBH;
    ZScoreCalculator::calculateStatus(refData, child2, r2BBU, r2PBTBU, r2BBH);
    RiwayatModel::simpan(child2, r2BBU, r2PBTBU, r2BBH);

    // Data 3: Andi
    ChildProfile child3;
    child3.nama = "Andi";
    child3.jenis_kelamin = 'L';
    // Lahir 8 bulan yang lalu (2025-10-30)
    child3.umur_bulan = hitungUmurBulan("2025-10-30");
    child3.berat_kg = 7.0; // Sedikit kurang
    child3.tinggi_cm = 68.0;
    
    ZScoreResult r3BBU, r3PBTBU, r3BBH;
    ZScoreCalculator::calculateStatus(refData, child3, r3BBU, r3PBTBU, r3BBH);
    RiwayatModel::simpan(child3, r3BBU, r3PBTBU, r3BBH);
}
