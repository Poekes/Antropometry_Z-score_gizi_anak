#include "FoodRecommendation.h"
#include <vector>
#include <random>
#include <ctime>

// Fungsi pembantu untuk memilih rekomendasi acak dari daftar
static std::string getRandomRec(const std::vector<std::string>& recs) {
    if (recs.empty()) return "";
    static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<std::size_t> dist(0, recs.size() - 1);
    return recs[dist(rng)];
}

std::string FoodRecommendation::getRekomendasiBBU(const std::string& status_bbu) {
    if (status_bbu.find("Sangat Kurang") != std::string::npos) {
        return getRandomRec({
            "Perlu makanan padat kalori dan protein hewani (telur, ikan, ayam). Berikan porsi kecil tapi sering (5-6x sehari). Konsultasikan ke dokter segera.",
            "Fokus pada asupan tinggi kalori. Tambahkan mentega/margarin pada bubur/makanan anak. Wajib kontrol ke Puskesmas untuk evaluasi.",
            "Berikan ekstra protein seperti hati ayam matang atau daging cincang. Jangan paksakan makan banyak sekaligus, tapi lebih sering."
        });
    } 
    else if (status_bbu.find("Kurang") != std::string::npos) {
        return getRandomRec({
            "Tingkatkan karbohidrat dan protein. Tambahkan lemak sehat seperti minyak zaitun, alpukat, atau santan pada makanannya.",
            "Berikan selingan/camilan padat gizi seperti bubur kacang hijau atau puding susu di antara jadwal makan utama.",
            "Pastikan anak makan 3x sehari dengan lauk hewani (ikan/telur) dan nabati (tempe/tahu). Evaluasi tekstur makanan jika anak susah makan."
        });
    } 
    else if (status_bbu.find("Normal") != std::string::npos) {
        return getRandomRec({
            "Pertahankan asupan gizi seimbang (Isi Piringku). Berikan variasi lauk pauk, sayur, dan buah setiap harinya.",
            "Pertumbuhan sangat baik. Coba variasikan menu seperti sup sayur telur puyuh atau perkedel tahu agar anak selalu semangat makan.",
            "Berat badan ideal. Lanjutkan pemberian makan dengan porsi seimbang antara nasi, lauk, dan sayur. Perbanyak minum air putih."
        });
    } 
    else if (status_bbu.find("Risiko") != std::string::npos) {
        return getRandomRec({
            "Batasi makanan manis (permen, cokelat) atau jajanan tinggi pengawet. Fokus pada buah segar sebagai camilan sore.",
            "Ganti camilan kerupuk/biskuit dengan buah-buahan potong (pepaya, pisang, semangka). Pastikan anak tetap aktif bergerak.",
            "Kurangi minuman manis seperti teh kotak atau susu kental manis. Ajak anak bermain fisik lebih banyak."
        });
    }
    return "Tetap berikan makanan bergizi dan pantau terus berat badannya.";
}

std::string FoodRecommendation::getRekomendasiTBU(const std::string& status_tbu) {
    if (status_tbu.find("Sangat Pendek") != std::string::npos) {
        return getRandomRec({
            "Sangat butuh protein hewani setiap hari (susu, telur, ikan). Rujuk ke puskesmas untuk kemungkinan terapi gizi stunting kronis.",
            "Tinggi badan sangat tertinggal. Berikan olahan ikan lele, belut, atau hati ayam yang kaya zat gizi mikro. Pantau kesehatan dari infeksi.",
            "Diperlukan evaluasi medis segera. Berikan lauk pauk tinggi kalsium dan zinc (daging sapi sapi, keju) untuk menstimulasi tulang."
        });
    } 
    else if (status_tbu.find("Pendek") != std::string::npos) {
        return getRandomRec({
            "Wajib ada protein hewani di setiap jadwal makan (ikan teri, telur, ayam). Hindari teh yang bisa menghambat zat besi.",
            "Anak perlu tambahan kalsium dan vitamin D. Rutin berjemur pagi dan berikan susu atau yogurt anak.",
            "Kejar pertumbuhan tingginya dengan menu sup tulang kaldu ayam atau ikan teri basah yang sangat baik untuk tulang anak."
        });
    } 
    else if (status_tbu.find("Normal") != std::string::npos) {
        return getRandomRec({
            "Tinggi badan ideal. Teruskan pemberian makanan kaya protein dan kalsium untuk mendukung masa pertumbuhan emasnya.",
            "Postur tubuh anak normal. Pertahankan variasi sayuran berdaun hijau gelap dan lauk hewani agar tumbuh kembangnya maksimal.",
            "Tinggi anak sesuai umurnya. Variasikan lauk dengan tahu, tempe, dan ikan laut segar."
        });
    } 
    else if (status_tbu.find("Tinggi") != std::string::npos) {
        return getRandomRec({
            "Pertumbuhan tinggi sangat bagus. Lanjutkan asupan gizi seimbang yang cukup agar berat badan proporsional dengan tinggi badannya.",
            "Tinggi badannya unggul. Pastikan porsi kalori dan protein harian tercukupi agar anak tidak terlihat kurus.",
            "Sangat baik. Teruskan pola makan sehat dan dukung dengan aktivitas fisik seperti melompat dan berlari."
        });
    }
    return "Pastikan anak mendapatkan nutrisi yang cukup setiap harinya.";
}

std::string FoodRecommendation::getRekomendasiBBTB(const std::string& status_bbtb) {
    if (status_bbtb.find("Gizi Buruk") != std::string::npos) {
        return getRandomRec({
            "KONDISI DARURAT MEDIS. Butuh formula khusus (F-75/F-100) atau RUTF di bawah pengawasan ketat dokter. Jangan asal beri makan.",
            "Segera bawa ke rumah sakit. Jangan berikan makanan padat secara mendadak; anak butuh stabilisasi dengan cairan gizi khusus dari medis.",
            "Gizi buruk berisiko komplikasi. Perawatan medis adalah wajib. Patuhi arahan ahli gizi puskesmas untuk fase rehabilitasi."
        });
    } 
    else if (status_bbtb.find("Gizi Kurang") != std::string::npos) {
        return getRandomRec({
            "Tambahkan porsi lauk hewani ganda (misal: ayam + telur). Berikan camilan bergizi (kacang hijau) di sela jam makan.",
            "Coba strategi 'double kalori', campurkan mentega/santan/minyak kelapa pada buburnya. Tingkatkan frekuensi ngemil sehat.",
            "Fokus pada peningkatan berat badan. Sediakan makanan favorit anak namun divariasikan dengan tambahan keju atau telur puyuh rebus."
        });
    } 
    else if (status_bbtb.find("Gizi Baik") != std::string::npos) {
        return getRandomRec({
            "Proporsi tubuh sangat ideal. Lanjutkan pola makan gizi seimbang dan penuhi kebutuhan cairan harian. Ajak anak aktif bermain fisik.",
            "Gizi seimbang tercapai. Terus kreasikan menu MPASI/makanan anak dengan bentuk yang menarik agar nafsu makannya terjaga.",
            "Luar biasa! Pertahankan pola makan saat ini: sarapan bergizi, makan siang lengkap, dan makan malam secukupnya."
        });
    } 
    else if (status_bbtb.find("Berisiko Gizi Lebih") != std::string::npos) {
        return getRandomRec({
            "Mulai kurangi susu kental manis dan cokelat. Hindari menggoreng makanan dengan minyak berlebih (deep fry).",
            "Ganti jajanan gurih bermesin dengan rebus-rebusan seperti jagung rebus, ubi, atau pisang kukus.",
            "Perbanyak minum air putih dan kurangi minuman kemasan manis. Tingkatkan aktivitas bermain di luar rumah."
        });
    } 
    else if (status_bbtb.find("Gizi Lebih") != std::string::npos || status_bbtb.find("Obesitas") != std::string::npos) {
        return getRandomRec({
            "Ganti camilan manis/gorengan dengan buah potong. Kurangi nasi berlebih. Tingkatkan aktivitas fisik (berlari) 60 menit sehari.",
            "Terapkan diet seimbang (bukan puasa). Penuhi piring dengan sayur dan kurangi tepung-tepungan. Konsultasi dengan ahli gizi.",
            "Batasi fast food, sosis, dan nugget. Fokus pada olahan makanan rebus, panggang, atau kukus murni."
        });
    }
    return "Pertahankan nutrisi seimbang, perbanyak air putih, dan jaga aktivitas fisiknya.";
}
