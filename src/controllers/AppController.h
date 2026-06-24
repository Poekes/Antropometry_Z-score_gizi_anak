#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include "../models/AuthModel.h"
#include "../zscore_data.h"

class AppController {
public:
    AppController();
    void run();

private:
    ReferenceData refData;
    UserSession currentSession;

    // Tampilan Menu
    void showLoginScreen();
    void showMainMenuKader();
    void showMainMenuIbu();

    // Aksi / Fitur
    void doPengukuranBaru();
    void doLihatRiwayat(bool hanyaRujukan);
};

#endif
