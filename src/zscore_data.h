#ifndef ZSCORE_DATA_H
#define ZSCORE_DATA_H

#include <string>
#include <vector>

// Menyimpan satu baris data referensi Z-score (baik berdasarkan umur maupun tinggi/panjang badan)
struct ZScoreRecord {
    double key_val; // Dapat menyimpan umur_bulan (misal 0, 1, 2) atau tinggi_cm (misal 45.0, 45.5)
    double sd_minus_3;
    double sd_minus_2;
    double sd_minus_1;
    double median;
    double sd_plus_1;
    double sd_plus_2;
    double sd_plus_3;
};

// Menyimpan seluruh tabel referensi untuk satu jenis kelamin
struct GenderData {
    std::vector<ZScoreRecord> bbu;  // Berat Badan menurut Umur
    std::vector<ZScoreRecord> pbu;  // Panjang Badan menurut Umur
    std::vector<ZScoreRecord> tbu;  // Tinggi Badan menurut Umur
    std::vector<ZScoreRecord> bbpb; // Berat Badan menurut Panjang Badan
    std::vector<ZScoreRecord> bbtb; // Berat Badan menurut Tinggi Badan
};

// Wadah utama untuk seluruh data referensi dari JSON
struct ReferenceData {
    GenderData laki_laki;
    GenderData perempuan;
};

// Profil anak yang diinputkan oleh kader
struct ChildProfile {
    std::string nama;
    char jenis_kelamin; // 'L' (Laki-laki) atau 'P' (Perempuan)
    int umur_bulan;     // Usia dalam bulan (0 - 60)
    double berat_kg;    // Berat badan dalam kg
    double tinggi_cm;   // Panjang/tinggi badan dalam cm
};

// Kategori status gizi berdasarkan standar Kemenkes RI 2020
enum class StatusGizi {
    // Indikator BB/U
    BB_SANGAT_KURANG,
    BB_KURANG,
    BB_NORMAL,
    BB_RISIKO_LEBIH,

    // Indikator PB/U atau TB/U
    TB_SANGAT_PENDEK,
    TB_PENDEK,
    TB_NORMAL,
    TB_TINGGI,

    // Indikator BB/PB atau BB/TB
    GIZI_BURUK,
    GIZI_KURANG,
    GIZI_BAIK,
    BERISIKO_GIZI_LEBIH,
    GIZI_LEBIH,
    OBESITAS,

    // Jika di luar batas tabel referensi
    TIDAK_TERDEFINISI
};

// Hasil detail kalkulasi Z-score per indikator
struct ZScoreResult {
    double z_score;
    StatusGizi status;
    std::string nama_status;
    std::string penjelasan;
};

#endif // ZSCORE_DATA_H
