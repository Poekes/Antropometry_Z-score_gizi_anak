#ifndef USER_MODEL_H
#define USER_MODEL_H

#include <string>
#include <vector>
#include <utility>

struct UserSession {
    bool isLoggedIn;
    int userId; // ID unik pengguna
    int role; // 1 = Kader Posyandu, 2 = Ibu Balita
    std::string username;
    std::string childName; // Balita yang sedang dipilih
};

class UserModel {
public:
    // Mengecek kredensial dan melakukan login
    static UserSession login(const std::string& username, const std::string& password);
    
    // Mendaftarkan pengguna (Kader atau Ibu)
    static bool registerUser(int role, const std::string& username, const std::string& password);
    
    // Menautkan nama balita ke ID ibu
    static bool linkIbuToAnak(int ibuId, const std::string& namaAnak);
    
    // Mendapatkan daftar nama anak dari ID ibu
    static std::vector<std::string> getAnakByIbu(int ibuId);
    
    // Memeriksa apakah user ada
    static bool userExists(const std::string& username);

    // Mendapatkan User ID berdasarkan username
    static int getUserIdByUsername(const std::string& username);

    // Mendapatkan daftar semua Ibu (role == 2) - pair<ID, Username>
    static std::vector<std::pair<int, std::string>> getAllIbu();
};

#endif
