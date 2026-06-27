#ifndef FOOD_RECOMMENDATION_H
#define FOOD_RECOMMENDATION_H

#include <string>

class FoodRecommendation {
public:
    // Mendapatkan rekomendasi makanan berdasarkan status gizi Berat Badan terhadap Umur (BB/U)
    static std::string getRekomendasiBBU(const std::string& status_bbu);
    
    // Mendapatkan rekomendasi makanan berdasarkan status gizi Tinggi Badan terhadap Umur (TB/U)
    static std::string getRekomendasiTBU(const std::string& status_tbu);
    
    // Mendapatkan rekomendasi makanan berdasarkan status gizi Berat Badan terhadap Tinggi Badan (BB/TB)
    static std::string getRekomendasiBBTB(const std::string& status_bbtb);
};

#endif
