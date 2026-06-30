#include "RiwayatModel.h"
#include "../calculator.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <cctype>

bool RiwayatModel::simpan(const ChildProfile& child, const ZScoreResult& bbu, const ZScoreResult& pbtbu, const ZScoreResult& bbh) {
    std::string filename = "riwayat_pemeriksaan.csv";
    
    // Periksa apakah berkas kosong/belum ada untuk menulis header
    std::ifstream testFile(filename);
    bool writeHeader = !testFile.is_open() || testFile.peek() == std::ifstream::traits_type::eof();
    testFile.close();
    
    std::ofstream outFile(filename, std::ios::app);
    if (!outFile.is_open()) {
        return false;
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
    return true;
}

std::vector<std::string> RiwayatModel::splitCsvLine(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(line);
    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<std::vector<std::string>> RiwayatModel::getAllRiwayat() {
    std::vector<std::vector<std::string>> dataRows;
    std::string filename = "riwayat_pemeriksaan.csv";
    std::ifstream inFile(filename);
    
    if (!inFile.is_open()) {
        return dataRows;
    }
    
    std::string line;
    // Lewati baris header CSV
    if (!std::getline(inFile, line)) {
        return dataRows;
    }
    
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        dataRows.push_back(splitCsvLine(line));
    }
    inFile.close();
    return dataRows;
}

std::vector<std::vector<std::string>> RiwayatModel::getFilteredRiwayat(bool hanyaRujukan, const std::string& filterNama) {
    std::vector<std::vector<std::string>> allData = getAllRiwayat();
    std::vector<std::vector<std::string>> filteredRows;
    
    for (const auto& row : allData) {
        if (row.size() < 12) continue;

        // Filter berdasarkan nama jika parameter filterNama tidak kosong
        if (!filterNama.empty()) {
            std::string n1 = filterNama;
            std::string n2 = row[1];
            std::transform(n1.begin(), n1.end(), n1.begin(), [](unsigned char c){ return std::tolower(c); });
            std::transform(n2.begin(), n2.end(), n2.begin(), [](unsigned char c){ return std::tolower(c); });
            if (n2.find(n1) == std::string::npos) {
                continue;
            }
        }
        
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
    
    return filteredRows;
}

std::vector<ChildProfile> RiwayatModel::getUniqueChildren() {
    std::vector<ChildProfile> children;
    std::vector<std::string> seenNames;
    auto allData = getAllRiwayat();
    
    for (const auto& row : allData) {
        if (row.size() < 4) continue;
        std::string nama = row[1];
        
        // Cek apakah nama sudah ada dalam daftar
        auto it = std::find(seenNames.begin(), seenNames.end(), nama);
        if (it == seenNames.end()) {
            seenNames.push_back(nama);
            ChildProfile cp;
            cp.nama = nama;
            cp.jenis_kelamin = (row[2] == "Laki-laki") ? 'L' : 'P';
            try {
                cp.umur_bulan = std::stoi(row[3]);
            } catch (...) {
                cp.umur_bulan = 0;
            }
            children.push_back(cp);
        } else {
            // Jika sudah ada, update dengan data umur yang paling baru (paling bawah di CSV)
            int index = std::distance(seenNames.begin(), it);
            try {
                children[index].umur_bulan = std::stoi(row[3]);
            } catch (...) {}
        }
    }
    
    return children;
}
