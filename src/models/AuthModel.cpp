#include "AuthModel.h"

UserSession AuthModel::login(const std::string& username, const std::string& password) {
    UserSession session = {false, 0, "", ""};
    
    // Data Dummy Kader Posyandu
    if (username == "kader" && password == "123") {
        session.isLoggedIn = true;
        session.role = 1;
        session.username = username;
    } 
    // Data Dummy Ibu Balita
    else if (username == "ibu_budi" && password == "123") {
        session.isLoggedIn = true;
        session.role = 2;
        session.username = username;
        session.childName = "Budi";
    } else if (username == "ibu_siti" && password == "123") {
        session.isLoggedIn = true;
        session.role = 2;
        session.username = username;
        session.childName = "Siti";
    } else if (username == "ibu_andi" && password == "123") {
        session.isLoggedIn = true;
        session.role = 2;
        session.username = username;
        session.childName = "Andi";
    }
    
    return session;
}
