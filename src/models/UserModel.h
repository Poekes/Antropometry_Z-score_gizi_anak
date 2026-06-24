#ifndef USER_MODEL_H
#define USER_MODEL_H

#include <string>
#include <vector>

struct UserSession {
    bool isLoggedIn;
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
    
    // Menautkan nama balita ke username ibu
    static bool linkIbuToAnak(const std::string& usernameIbu, const std::string& namaAnak);
    
    // Mendapatkan daftar nama anak dari seorang ibu
    static std::vector<std::string> getAnakByIbu(const std::string& usernameIbu);
    
    // Memeriksa apakah user ada
    static bool userExists(const std::string& username);
};

#endif
