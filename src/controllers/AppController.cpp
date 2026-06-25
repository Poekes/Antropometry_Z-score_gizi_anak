#include "AppController.h"
#include "AuthController.h"
#include "KaderController.h"
#include "IbuController.h"
#include "../database/KaderSeeder.h"
#include "../database/IbuSeeder.h"
#include "../database/ChildSeeder.h"
#include "../views/ConsoleView.h"
#include "../json_parser.h"
#include <iostream>

AppController::AppController() {
    currentSession = {false, 0, "", ""};
}

void AppController::run() {
    std::string pathReference = "data/zscore_reference.json";

    ConsoleView::clearScreen();
    ConsoleView::tampilkanHeader();

    // Memuat data referensi JSON
    if (!JsonParser::loadReferenceData(pathReference, refData)) {
        pathReference = "../data/zscore_reference.json";
        if (!JsonParser::loadReferenceData(pathReference, refData)) {
            ConsoleView::printError("GAGAL memuat data referensi Z-Score!");
            std::cout << "Pastikan file database 'zscore_reference.json' berada di direktori 'data/'." << std::endl;
            std::cout << "Tekan ENTER untuk keluar...";
            std::cin.get();
            return;
        }
    }

    KaderSeeder::seed();
    IbuSeeder::seed();
    ChildSeeder::seed(refData);

    bool appLoop = true;
    while (appLoop) {
        if (!currentSession.isLoggedIn) {
            if (!AuthController::showLoginScreen(currentSession)) {
                appLoop = false;
            }
        } else {
            if (currentSession.role == 1) {
                KaderController kaderCtrl(currentSession, refData);
                kaderCtrl.run();
            } else if (currentSession.role == 2) {
                IbuController ibuCtrl(currentSession);
                ibuCtrl.run();
            } else {
                // Failsafe
                currentSession.isLoggedIn = false;
            }
        }
    }

    ConsoleView::clearScreen();
    std::cout << std::endl << BOLD << GREEN << "Terima kasih telah menggunakan aplikasi ini. Jaga kesehatan anak Indonesia!" << RESET << std::endl << std::endl;
}
