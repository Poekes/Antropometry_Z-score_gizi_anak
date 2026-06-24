#ifndef CONSOLE_VIEW_H
#define CONSOLE_VIEW_H

#include <string>
#include <vector>
#include "../zscore_data.h"

extern const std::string RESET;
extern const std::string BOLD;
extern const std::string RED;
extern const std::string GREEN;
extern const std::string YELLOW;
extern const std::string BLUE;
extern const std::string CYAN;
extern const std::string MAGENTA;

class ConsoleView {
public:
    static void clearInput();
    static void clearScreen();
    static std::string getStatusColor(StatusGizi status);
    static void tampilkanHeader();
    
    // Menampilkan tabel riwayat (hasil dari RiwayatModel)
    static void printRiwayatTable(const std::vector<std::vector<std::string>>& data, bool hanyaRujukan);
    
    static void printError(const std::string& errorMsg);
    static void printSuccess(const std::string& successMsg);
};

#endif
