#ifndef RIWAYAT_MODEL_H
#define RIWAYAT_MODEL_H

#include <string>
#include <vector>
#include "../zscore_data.h"

class RiwayatModel {
public:
    // Menyimpan hasil kalkulasi pemeriksaan ke file CSV eksternal
    static bool simpan(const ChildProfile& child, const ZScoreResult& bbu, const ZScoreResult& pbtbu, const ZScoreResult& bbh);
    
    // Mendapatkan semua riwayat pemeriksaan tanpa filter
    static std::vector<std::vector<std::string>> getAllRiwayat();
    
    // Mendapatkan riwayat yang sudah difilter berdasarkan rujukan dan/atau nama anak
    static std::vector<std::vector<std::string>> getFilteredRiwayat(bool hanyaRujukan, const std::string& filterNama);

private:
    static std::vector<std::string> splitCsvLine(const std::string& line);
};

#endif
