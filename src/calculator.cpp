#include "calculator.h"
#include <cmath>
#include <algorithm>
#include <iostream>

// Menghitung Z-score tunggal berdasarkan rumus standar WHO/Kemenkes
double ZScoreCalculator::computeSingleZScore(double realVal, const ZScoreRecord& rec) {
    if (std::abs(realVal - rec.median) < 0.0001) {
        return 0.0;
    }
    if (realVal > rec.median) {
        double sd = rec.sd_plus_1 - rec.median;
        if (sd <= 0.0) return 0.0;
        return (realVal - rec.median) / sd;
    } else {
        double sd = rec.median - rec.sd_minus_1;
        if (sd <= 0.0) return 0.0;
        return (realVal - rec.median) / sd;
    }
}

// Mencari record referensi berdasarkan umur anak (0-60 bulan)
bool ZScoreCalculator::findRecordByAge(const std::vector<ZScoreRecord>& records, int age, ZScoreRecord& match) {
    for (const auto& rec : records) {
        // Karena key_val untuk bbu/pbu/tbu adalah nilai umur (integer), kita periksa dengan toleransi kecil
        if (static_cast<int>(rec.key_val + 0.5) == age) {
            match = rec;
            return true;
        }
    }
    return false;
}

// Mencari record referensi berdasarkan tinggi anak (dibulatkan ke 0.5 cm terdekat)
bool ZScoreCalculator::findRecordByHeight(const std::vector<ZScoreRecord>& records, double height, ZScoreRecord& match) {
    // Bulatkan tinggi ke kelipatan 0.5 terdekat (misal 74.3 -> 74.5, 74.1 -> 74.0)
    double rounded_height = std::round(height * 2.0) / 2.0;
    
    for (const auto& rec : records) {
        if (std::abs(rec.key_val - rounded_height) < 0.01) {
            match = rec;
            return true;
        }
    }
    return false;
}

// Klasifikasi Z-score ke kategori status gizi BB/U
StatusGizi ZScoreCalculator::classifyBBU(double z) {
    if (z < -3.0) return StatusGizi::BB_SANGAT_KURANG;
    if (z < -2.0) return StatusGizi::BB_KURANG;
    if (z <= 1.0) return StatusGizi::BB_NORMAL;
    return StatusGizi::BB_RISIKO_LEBIH;
}

// Klasifikasi Z-score ke kategori status gizi PB/U atau TB/U
StatusGizi ZScoreCalculator::classifyPBTBU(double z) {
    if (z < -3.0) return StatusGizi::TB_SANGAT_PENDEK;
    if (z < -2.0) return StatusGizi::TB_PENDEK;
    if (z <= 2.0) return StatusGizi::TB_NORMAL;
    return StatusGizi::TB_TINGGI;
}

// Klasifikasi Z-score ke kategori status gizi BB/PB atau BB/TB
StatusGizi ZScoreCalculator::classifyBBH(double z) {
    if (z < -3.0) return StatusGizi::GIZI_BURUK;
    if (z < -2.0) return StatusGizi::GIZI_KURANG;
    if (z <= 1.0) return StatusGizi::GIZI_BAIK;
    if (z <= 2.0) return StatusGizi::BERISIKO_GIZI_LEBIH;
    if (z <= 3.0) return StatusGizi::GIZI_LEBIH;
    return StatusGizi::OBESITAS;
}

// Menghitung status gizi anak secara lengkap untuk 3 indikator sekaligus
void ZScoreCalculator::calculateStatus(const ReferenceData& refData, const ChildProfile& child,
                                      ZScoreResult& resultBBU, ZScoreResult& resultPBTBU, ZScoreResult& resultBBH) {
    // 1. Tentukan GenderData yang sesuai
    char gender = std::toupper(static_cast<unsigned char>(child.jenis_kelamin));
    const GenderData& gData = (gender == 'L') ? refData.laki_laki : refData.perempuan;

    // --- Indikator 1: BB/U (Berat Badan menurut Umur) ---
    ZScoreRecord recordBBU;
    if (findRecordByAge(gData.bbu, child.umur_bulan, recordBBU)) {
        resultBBU.z_score = computeSingleZScore(child.berat_kg, recordBBU);
        resultBBU.status = classifyBBU(resultBBU.z_score);
        resultBBU.nama_status = getStatusName(resultBBU.status);
        resultBBU.penjelasan = getStatusExplanation(resultBBU.status);
    } else {
        resultBBU.status = StatusGizi::TIDAK_TERDEFINISI;
        resultBBU.z_score = 0.0;
        resultBBU.nama_status = "Tidak Terdefinisi";
        resultBBU.penjelasan = "Data umur anak di luar rentang standar data referensi (0-60 bulan).";
    }

    // --- Indikator 2: PB/U atau TB/U (Panjang/Tinggi menurut Umur) ---
    ZScoreRecord recordPBTBU;
    bool foundPBTBU = false;
    
    // Tentukan apakah menggunakan PB/U (<= 24 bulan) atau TB/U (> 24 bulan)
    if (child.umur_bulan <= 24) {
        foundPBTBU = findRecordByAge(gData.pbu, child.umur_bulan, recordPBTBU);
    } else {
        foundPBTBU = findRecordByAge(gData.tbu, child.umur_bulan, recordPBTBU);
    }

    if (foundPBTBU) {
        resultPBTBU.z_score = computeSingleZScore(child.tinggi_cm, recordPBTBU);
        resultPBTBU.status = classifyPBTBU(resultPBTBU.z_score);
        resultPBTBU.nama_status = getStatusName(resultPBTBU.status);
        resultPBTBU.penjelasan = getStatusExplanation(resultPBTBU.status);
    } else {
        resultPBTBU.status = StatusGizi::TIDAK_TERDEFINISI;
        resultPBTBU.z_score = 0.0;
        resultPBTBU.nama_status = "Tidak Terdefinisi";
        resultPBTBU.penjelasan = "Data umur anak di luar rentang standar data referensi.";
    }

    // --- Indikator 3: BB/PB atau BB/TB (Berat menurut Panjang/Tinggi) ---
    ZScoreRecord recordBBH;
    bool foundBBH = false;

    // Tentukan apakah menggunakan BB/PB (<= 24 bulan) atau BB/TB (> 24 bulan)
    if (child.umur_bulan <= 24) {
        foundBBH = findRecordByHeight(gData.bbpb, child.tinggi_cm, recordBBH);
    } else {
        foundBBH = findRecordByHeight(gData.bbtb, child.tinggi_cm, recordBBH);
    }

    if (foundBBH) {
        resultBBH.z_score = computeSingleZScore(child.berat_kg, recordBBH);
        resultBBH.status = classifyBBH(resultBBH.z_score);
        resultBBH.nama_status = getStatusName(resultBBH.status);
        resultBBH.penjelasan = getStatusExplanation(resultBBH.status);
    } else {
        resultBBH.status = StatusGizi::TIDAK_TERDEFINISI;
        resultBBH.z_score = 0.0;
        resultBBH.nama_status = "Tidak Dapat Ditentukan";
        resultBBH.penjelasan = "Tinggi badan anak berada di luar rentang standar (45-110 cm untuk PB, 65-120 cm untuk TB) untuk penentuan berat badan ideal.";
    }
}

// Mendapatkan teks nama kategori status gizi resmi
std::string ZScoreCalculator::getStatusName(StatusGizi status) {
    switch (status) {
        case StatusGizi::BB_SANGAT_KURANG:    return "Sangat Kurang (Severely Underweight)";
        case StatusGizi::BB_KURANG:           return "Kurang (Underweight)";
        case StatusGizi::BB_NORMAL:           return "Normal";
        case StatusGizi::BB_RISIKO_LEBIH:     return "Risiko Berat Badan Lebih";
        
        case StatusGizi::TB_SANGAT_PENDEK:    return "Sangat Pendek (Severely Stunted)";
        case StatusGizi::TB_PENDEK:           return "Pendek (Stunted)";
        case StatusGizi::TB_NORMAL:           return "Normal";
        case StatusGizi::TB_TINGGI:           return "Tinggi";
        
        case StatusGizi::GIZI_BURUK:          return "Gizi Buruk (Severely Wasted)";
        case StatusGizi::GIZI_KURANG:         return "Gizi Kurang (Wasted)";
        case StatusGizi::GIZI_BAIK:           return "Gizi Baik (Normal)";
        case StatusGizi::BERISIKO_GIZI_LEBIH: return "Berisiko Gizi Lebih (Risk of Overweight)";
        case StatusGizi::GIZI_LEBIH:          return "Gizi Lebih (Overweight)";
        case StatusGizi::OBESITAS:            return "Obesitas (Obese)";
        
        default:                              return "Tidak Diketahui / Di luar batas";
    }
}

// Mendapatkan penjelasan konsep yang ramah kader (tidak terlalu teknis)
std::string ZScoreCalculator::getStatusExplanation(StatusGizi status) {
    switch (status) {
        case StatusGizi::BB_SANGAT_KURANG:
            return "Berat badan balita berada jauh di bawah rentang normal untuk usianya. Anak membutuhkan perhatian medis segera dan penanganan gizi yang intensif.";
        case StatusGizi::BB_KURANG:
            return "Berat badan balita kurang untuk usianya. Disarankan untuk menambah asupan makanan bergizi dan memantau tumbuh kembangnya secara berkala.";
        case StatusGizi::BB_NORMAL:
            return "Berat badan balita sesuai dan proporsional untuk usianya. Pertahankan pola makan bergizi seimbang dan lanjutkan posyandu setiap bulan.";
        case StatusGizi::BB_RISIKO_LEBIH:
            return "Berat badan balita berada di atas rata-rata anak seusianya. Perlu diperhatikan agar berat badan tidak terus naik melebihi batas ideal.";
        
        case StatusGizi::TB_SANGAT_PENDEK:
            return "Tinggi badan balita jauh di bawah standar tinggi anak seusianya. Kondisi ini menunjukkan adanya potensi masalah gizi jangka panjang (kronis) atau stunting yang parah. Segera rujuk ke dokter/Puskesmas.";
        case StatusGizi::TB_PENDEK:
            return "Tinggi badan balita berada di bawah rata-rata anak seusianya. Hal ini mengindikasikan potensi stunting ringan hingga sedang. Perlu perbaikan gizi harian anak.";
        case StatusGizi::TB_NORMAL:
            return "Pertumbuhan tinggi/panjang badan balita berjalan sangat baik dan sesuai untuk usianya. Pertahankan asupan kalsium dan nutrisi anak!";
        case StatusGizi::TB_TINGGI:
            return "Balita memiliki tinggi badan di atas rata-rata anak seusianya. Ini adalah pertumbuhan yang bagus dan normal, biasanya dipengaruhi oleh faktor genetik keluarga.";
        
        case StatusGizi::GIZI_BURUK:
            return "Berat badan anak sangat tidak sebanding dengan tinggi badannya (sangat kurus). Ini adalah kondisi darurat medis yang memerlukan penanganan klinis segera oleh dokter atau Puskesmas.";
        case StatusGizi::GIZI_KURANG:
            return "Berat badan anak kurang proporsional dengan tinggi badannya (kurus). Perlu perbaikan asupan gizi berkualitas (seperti protein hewani dan kalori) agar berat badannya naik secara sehat.";
        case StatusGizi::GIZI_BAIK:
            return "Berat badan anak sangat ideal dan proporsional dengan panjang/tinggi badannya. Kondisi fisik anak dalam keadaan sangat baik dan seimbang.";
        case StatusGizi::BERISIKO_GIZI_LEBIH:
            return "Berat badan anak mulai melebihi proporsi tinggi badannya. Perlu sedikit membatasi makanan manis atau berlemak tinggi serta tingkatkan aktivitas fisik bermainnya.";
        case StatusGizi::GIZI_LEBIH:
            return "Anak masuk kategori kelebihan berat badan (gemuk). Kurangi asupan gula/lemak berlebih dan konsultasikan menu makanan sehat dengan petugas kesehatan agar tidak berlanjut ke obesitas.";
        case StatusGizi::OBESITAS:
            return "Berat badan anak jauh melebihi tinggi badannya (sangat gemuk/obesitas). Ini berisiko bagi kesehatan jangka panjang anak. Disarankan berkonsultasi ke dokter anak atau ahli gizi.";
        
        default:
            return "Tidak ada penjelasan.";
    }
}

// Memeriksa apakah status gizi masuk kategori kritis/gawat yang butuh rujukan
bool ZScoreCalculator::isCritical(StatusGizi status) {
    return (status == StatusGizi::BB_SANGAT_KURANG ||
            status == StatusGizi::TB_SANGAT_PENDEK ||
            status == StatusGizi::GIZI_BURUK ||
            status == StatusGizi::OBESITAS);
}

// Mendapatkan rencana tindakan rujukan medis jika terdeteksi gawat
std::string ZScoreCalculator::getReferralAction(const ZScoreResult& bbu, const ZScoreResult& pbtbu, const ZScoreResult& bbh) {
    bool hasBbuCritical = (bbu.status == StatusGizi::BB_SANGAT_KURANG);
    bool hasTbuCritical = (pbtbu.status == StatusGizi::TB_SANGAT_PENDEK);
    bool hasWastingCritical = (bbh.status == StatusGizi::GIZI_BURUK);
    bool hasObesity = (bbh.status == StatusGizi::OBESITAS);

    if (!hasBbuCritical && !hasTbuCritical && !hasWastingCritical && !hasObesity) {
        return ""; // Tidak kritis
    }

    std::string action = "";
    if (hasWastingCritical) {
        action += "[!] RUJUKAN SEGERA (GIZI BURUK):\n"
                  "  1. Segera rujuk anak ke Puskesmas, Klinik, atau Rumah Sakit terdekat untuk penanganan klinis.\n"
                  "  2. Konsultasikan pemberian PMT Pemulihan intensif seperti RUTF (Ready-to-Use Therapeutic Food) atau susu F100.\n"
                  "  3. Pastikan asupan cairan cukup untuk mencegah dehidrasi berat dan periksa tanda infeksi klinis.\n\n";
    }
    if (hasBbuCritical) {
        action += "[!] PERHATIAN MEDIS (BERAT BADAN SANGAT KURANG):\n"
                  "  1. Rujuk anak ke Puskesmas atau dokter anak untuk mencari tahu penyebab berat badan sangat rendah.\n"
                  "  2. Lakukan evaluasi asupan makanan harian (apakah kurang ASI, kurang kalori, atau ada penyakit penyerta).\n"
                  "  3. Berikan makanan tambahan yang padat gizi secara bertahap.\n\n";
    }
    if (hasTbuCritical) {
        action += "[!] RUJUKAN SPESIALISTIK (SANGAT PENDEK / CHRONIC STUNTING):\n"
                  "  1. Anak terindikasi stunting tingkat parah. Rujuk ke fasilitas kesehatan tingkat lanjut untuk evaluasi Dokter Spesialis Anak.\n"
                  "  2. Berikan asupan kaya protein hewani (telur, susu, ikan, daging) dan zat besi secara teratur.\n"
                  "  3. Evaluasi pola asuh, sanitasi air bersih di rumah, serta riwayat imunisasi dasar anak.\n\n";
    }
    if (hasObesity) {
        action += "[!] KONSULTASI NUTRISI (OBESITAS ANAK):\n"
                  "  1. Konsultasikan dengan ahli gizi untuk pengaturan menu makan rendah kalori namun padat nutrisi.\n"
                  "  2. Batasi konsumsi susu formula manis, makanan olahan tinggi lemak, atau jajanan manis.\n"
                  "  3. Dorong aktivitas fisik anak (bermain aktif merangkak/berjalan/berlari) minimal 60 menit sehari.\n\n";
    }

    action += "PEMANTAUAN UMUM POSYANDU:\n"
              "  - Lakukan penimbangan berat badan dan pengukuran tinggi badan setiap 1-2 minggu di bawah pengawasan Bidan Desa.\n"
              "  - Bantu orang tua untuk mencatat perkembangan tumbuh kembang anak di Buku KIA.";
              
    return action;
}
