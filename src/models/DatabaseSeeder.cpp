#include "DatabaseSeeder.h"
#include "RiwayatModel.h"
#include "../calculator.h"
#include <fstream>
#include <iostream>

void DatabaseSeeder::run(const ReferenceData& refData) {
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
    child1.umur_bulan = 12;
    child1.berat_kg = 9.5;
    child1.tinggi_cm = 75.0;
    
    ZScoreResult r1BBU, r1PBTBU, r1BBH;
    ZScoreCalculator::calculateStatus(refData, child1, r1BBU, r1PBTBU, r1BBH);
    RiwayatModel::simpan(child1, r1BBU, r1PBTBU, r1BBH);

    // Data 2: Siti
    ChildProfile child2;
    child2.nama = "Siti";
    child2.jenis_kelamin = 'P';
    child2.umur_bulan = 24;
    child2.berat_kg = 11.0;
    child2.tinggi_cm = 85.0;
    
    ZScoreResult r2BBU, r2PBTBU, r2BBH;
    ZScoreCalculator::calculateStatus(refData, child2, r2BBU, r2PBTBU, r2BBH);
    RiwayatModel::simpan(child2, r2BBU, r2PBTBU, r2BBH);

    // Data 3: Andi
    ChildProfile child3;
    child3.nama = "Andi";
    child3.jenis_kelamin = 'L';
    child3.umur_bulan = 8;
    child3.berat_kg = 7.0; // Sedikit kurang
    child3.tinggi_cm = 68.0;
    
    ZScoreResult r3BBU, r3PBTBU, r3BBH;
    ZScoreCalculator::calculateStatus(refData, child3, r3BBU, r3PBTBU, r3BBH);
    RiwayatModel::simpan(child3, r3BBU, r3PBTBU, r3BBH);

    std::cout << "[INFO] Database Seeder: Data dummy berhasil ditambahkan!\n";
}
