#include "FoodRecommendation.h"

std::string FoodRecommendation::getRekomendasiBBU(const std::string& status_bbu) {
    if (status_bbu.find("Sangat Kurang") != std::string::npos) {
        return "Perlu makanan padat kalori dan protein hewani ekstra (telur, ikan, daging, ayam). Berikan porsi kecil tapi sering (5-6 kali sehari). Segera konsultasikan ke fasilitas kesehatan.";
    } 
    else if (status_bbu.find("Kurang") != std::string::npos) {
        return "Tingkatkan asupan karbohidrat dan protein. Tambahkan lemak sehat seperti minyak zaitun, alpukat, atau santan pada MPASI/makanan anak.";
    } 
    else if (status_bbu.find("Normal") != std::string::npos) {
        return "Pertahankan asupan gizi seimbang (Isi Piringku). Berikan variasi makanan pokok, lauk pauk (hewani & nabati), sayur, dan buah.";
    } 
    else if (status_bbu.find("Risiko") != std::string::npos) {
        return "Batasi makanan ringan yang manis-manis atau jajanan tinggi kalori/pengawet. Fokus pada buah segar sebagai camilan.";
    }
    return "Tetap berikan makanan bergizi dan pantau terus berat badannya.";
}

std::string FoodRecommendation::getRekomendasiTBU(const std::string& status_tbu) {
    if (status_tbu.find("Sangat Pendek") != std::string::npos) {
        return "Sangat butuh protein hewani berkualitas tinggi setiap hari (susu, telur, hati ayam, ikan). Rujuk ke puskesmas untuk kemungkinan terapi gizi medis khusus (stunting kronis).";
    } 
    else if (status_tbu.find("Pendek") != std::string::npos) {
        return "Wajib ada protein hewani di setiap jadwal makan. Pastikan kecukupan kalsium dari susu dan ikan teri. Hindari teh/kopi yang bisa menghambat penyerapan zat besi.";
    } 
    else if (status_tbu.find("Normal") != std::string::npos) {
        return "Tinggi badan ideal. Teruskan pemberian makanan kaya protein dan kalsium untuk mendukung masa pertumbuhan emasnya.";
    } 
    else if (status_tbu.find("Tinggi") != std::string::npos) {
        return "Pertumbuhan tinggi sangat bagus. Lanjutkan asupan gizi seimbang yang cukup agar berat badan seimbang dengan tinggi badannya.";
    }
    return "Pastikan anak mendapatkan nutrisi yang cukup setiap harinya.";
}

std::string FoodRecommendation::getRekomendasiBBTB(const std::string& status_bbtb) {
    if (status_bbtb.find("Gizi Buruk") != std::string::npos) {
        return "KONDISI DARURAT MEDIS. Butuh formula khusus (F-75 / F-100) atau RUTF di bawah pengawasan ketat dokter/ahli gizi. Jangan hanya mengandalkan makanan biasa.";
    } 
    else if (status_bbtb.find("Gizi Kurang") != std::string::npos) {
        return "Tambahkan porsi lauk hewani ganda (misal: ayam + telur dalam 1 porsi). Berikan camilan bergizi (kacang hijau, puding susu) di antara jam makan utama.";
    } 
    else if (status_bbtb.find("Gizi Baik") != std::string::npos) {
        return "Proporsi tubuh sangat ideal. Lanjutkan pola makan gizi seimbang dan penuhi kebutuhan cairan harian. Ajak anak aktif bermain fisik.";
    } 
    else if (status_bbtb.find("Berisiko Gizi Lebih") != std::string::npos) {
        return "Mulai kurangi susu kental manis, permen, dan cokelat. Hindari menggoreng makanan dengan minyak berlebih (deep fry).";
    } 
    else if (status_bbtb.find("Gizi Lebih") != std::string::npos || status_bbtb.find("Obesitas") != std::string::npos) {
        return "Ganti camilan manis/gorengan dengan buah-buahan potong. Kurangi karbohidrat berlebih. Tingkatkan aktivitas fisik anak (bermain lari/lompat) 60 menit sehari. Konsultasi diet anak.";
    }
    return "Pertahankan nutrisi seimbang, perbanyak air putih, dan jaga aktivitas fisiknya.";
}
