#ifndef AUTH_MODEL_H
#define AUTH_MODEL_H

#include <string>

struct UserSession {
    bool isLoggedIn;
    int role; // 1 = Kader Posyandu, 2 = Ibu Balita
    std::string username;
    std::string childName; // Hanya untuk Ibu Balita
};

class AuthModel {
public:
    // Fungsi untuk verifikasi login
    static UserSession login(const std::string& username, const std::string& password);
};

#endif
