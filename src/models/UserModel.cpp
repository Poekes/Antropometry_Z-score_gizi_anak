#include "UserModel.h"
#include <fstream>
#include <sstream>
#include <algorithm>

const std::string FILE_PENGGUNA = "data_pengguna.csv";
const std::string FILE_IBU_ANAK = "data_ibu_anak.csv";

UserSession UserModel::login(const std::string& username, const std::string& password) {
    UserSession session = {false, 0, 0, "", ""};
    
    std::ifstream inFile(FILE_PENGGUNA);
    if (!inFile.is_open()) return session;
    
    std::string line;
    while (std::getline(inFile, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string tId, tRole, tUser, tPass;
        std::getline(ss, tId, ',');
        std::getline(ss, tRole, ',');
        std::getline(ss, tUser, ',');
        std::getline(ss, tPass, ',');
        
        if (tUser == username && tPass == password) {
            session.isLoggedIn = true;
            session.userId = std::stoi(tId);
            session.role = std::stoi(tRole);
            session.username = username;
            
            // Jika role 2 (Ibu), coba dapatkan anak pertama (nanti bisa diubah di controller)
            if (session.role == 2) {
                std::vector<std::string> anakList = getAnakByIbu(session.userId);
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
    
    int newId = 1;
    std::ifstream inFile(FILE_PENGGUNA);
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string tId;
            std::getline(ss, tId, ',');
            if (!tId.empty() && std::isdigit(tId[0])) {
                int currentId = std::stoi(tId);
                if (currentId >= newId) {
                    newId = currentId + 1;
                }
            }
        }
        inFile.close();
    }
    
    std::ofstream outFile(FILE_PENGGUNA, std::ios::app);
    if (!outFile.is_open()) return false;
    
    outFile << newId << "," << role << "," << username << "," << password << "\n";
    outFile.close();
    return true;
}

bool UserModel::linkIbuToAnak(int ibuId, const std::string& namaAnak) {
    std::ofstream outFile(FILE_IBU_ANAK, std::ios::app);
    if (!outFile.is_open()) return false;
    
    outFile << ibuId << "," << namaAnak << "\n";
    outFile.close();
    return true;
}

std::vector<std::string> UserModel::getAnakByIbu(int ibuId) {
    std::vector<std::string> anakList;
    std::ifstream inFile(FILE_IBU_ANAK);
    if (!inFile.is_open()) return anakList;
    
    std::string line;
    std::string strIbuId = std::to_string(ibuId);
    while (std::getline(inFile, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string tUser, tAnak;
        std::getline(ss, tUser, ',');
        std::getline(ss, tAnak, ',');
        
        if (tUser == strIbuId) {
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
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string tId, tRole, tUser;
        std::getline(ss, tId, ',');
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

int UserModel::getUserIdByUsername(const std::string& username) {
    std::ifstream inFile(FILE_PENGGUNA);
    if (!inFile.is_open()) return -1;
    
    std::string line;
    while (std::getline(inFile, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string tId, tRole, tUser;
        std::getline(ss, tId, ',');
        std::getline(ss, tRole, ',');
        std::getline(ss, tUser, ',');
        
        if (tUser == username) {
            inFile.close();
            return std::stoi(tId);
        }
    }
    inFile.close();
    return -1;
}

std::vector<std::pair<int, std::string>> UserModel::getAllIbu() {
    std::vector<std::pair<int, std::string>> ibuList;
    std::ifstream inFile(FILE_PENGGUNA);
    if (!inFile.is_open()) return ibuList;
    
    std::string line;
    while (std::getline(inFile, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string tId, tRole, tUser;
        std::getline(ss, tId, ',');
        std::getline(ss, tRole, ',');
        std::getline(ss, tUser, ',');
        
        if (tRole == "2") {
            ibuList.push_back({std::stoi(tId), tUser});
        }
    }
    inFile.close();
    return ibuList;
}
