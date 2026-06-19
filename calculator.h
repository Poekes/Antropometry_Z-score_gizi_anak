#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "zscore_data.h"

// Mesin hitung Z-score dan penentu klasifikasi status gizi
class ZScoreCalculator {
public:
    // Menghitung status gizi anak untuk 3 indikator sekaligus
    static void calculateStatus(const ReferenceData& refData, const ChildProfile& child,
                                ZScoreResult& resultBBU, ZScoreResult& resultPBTBU, ZScoreResult& resultBBH);

    // Mendapatkan representasi teks ramah pengguna untuk kategori status gizi
    static std::string getStatusName(StatusGizi status);

    // Mendapatkan teks penjelasan konsep sederhana untuk kategori status gizi
    static std::string getStatusExplanation(StatusGizi status);

private:
    // Menghitung Z-score tunggal berdasarkan rumus standar WHO/Kemenkes
    static double computeSingleZScore(double realVal, const ZScoreRecord& rec);

    // Mencari record referensi berdasarkan umur anak (0-60 bulan)
    static bool findRecordByAge(const std::vector<ZScoreRecord>& records, int age, ZScoreRecord& match);

    // Mencari record referensi berdasarkan tinggi anak (dibulatkan ke 0.5 cm terdekat)
    static bool findRecordByHeight(const std::vector<ZScoreRecord>& records, double height, ZScoreRecord& match);

    // Fungsi klasifikasi kategori berdasarkan Z-score
    static StatusGizi classifyBBU(double z);
    static StatusGizi classifyPBTBU(double z);
    static StatusGizi classifyBBH(double z);
};

#endif // CALCULATOR_H
