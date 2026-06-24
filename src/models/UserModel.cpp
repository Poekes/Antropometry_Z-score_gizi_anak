#include "UserModel.h"
#include <fstream>
#include <sstream>
#include <algorithm>

const std::string FILE_PENGGUNA = "data_pengguna.csv";
const std::string FILE_IBU_ANAK = "data_ibu_anak.csv";

UserSession UserModel::login(const std::string& username, const std::string& password) {
    UserSession session = {false, 0, "", ""};
    
    std::ifstream inFile(FILE_PENGGUNA);
    if (!inFile.is_open()) return session;
    
    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string tRole, tUser, tPass;
        std::getline(ss, tRole, ',');
        std::getline(ss, tUser, ',');
        std::getline(ss, tPass, ',');
        
        if (tUser == username && tPass == password) {
            session.isLoggedIn = true;
            session.role = std::stoi(tRole);
            session.username = username;
            
            // Jika role 2 (Ibu), coba dapatkan anak pertama (nanti bisa diubah di controller)
            if (session.role == 2) {
                std::vector<std::string> anakList = getAnakByIbu(username);
                if (!anakList.empty()) {
                    session.childName = anakList[0];
                }
            }
            break;
        }
    }
    inFile.close();
    
    return session;
}

bool UserModel::registerUser(int role, const std::string& username, const std::string& password) {
    if (userExists(username)) return false;
    
    std::ofstream outFile(FILE_PENGGUNA, std::ios::app);
    if (!outFile.is_open()) return false;
    
    outFile << role << "," << username << "," << password << "\n";
    outFile.close();
    return true;
}

bool UserModel::linkIbuToAnak(const std::string& usernameIbu, const std::string& namaAnak) {
    std::ofstream outFile(FILE_IBU_ANAK, std::ios::app);
    if (!outFile.is_open()) return false;
    
    outFile << usernameIbu << "," << namaAnak << "\n";
    outFile.close();
    return true;
}

std::vector<std::string> UserModel::getAnakByIbu(const std::string& usernameIbu) {
    std::vector<std::string> anakList;
    std::ifstream inFile(FILE_IBU_ANAK);
    if (!inFile.is_open()) return anakList;
    
    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string tUser, tAnak;
        std::getline(ss, tUser, ',');
        std::getline(ss, tAnak, ',');
        
        if (tUser == usernameIbu) {
            // Hindari duplikasi
            if (std::find(anakList.begin(), anakList.end(), tAnak) == anakList.end()) {
                anakList.push_back(tAnak);
            }
        }
    }
    inFile.close();
    return anakList;
}

bool UserModel::userExists(const std::string& username) {
    std::ifstream inFile(FILE_PENGGUNA);
    if (!inFile.is_open()) return false;
    
    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string tRole, tUser;
        std::getline(ss, tRole, ',');
        std::getline(ss, tUser, ',');
        
        if (tUser == username) {
            inFile.close();
            return true;
        }
    }
    inFile.close();
    return false;
}
