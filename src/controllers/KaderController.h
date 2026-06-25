#ifndef KADER_CONTROLLER_H
#define KADER_CONTROLLER_H

#include "../models/UserModel.h"
#include "../zscore_data.h"

class KaderController {
public:
    KaderController(UserSession& session, ReferenceData& refData);
    void run();

private:
    UserSession& currentSession;
    ReferenceData& refData;

    void showMainMenu();
    void doPengukuranBaru();
    void doLihatRiwayat(bool hanyaRujukan);
    void doTambahIbuBalita();
    void doTambahAnakKeIbu();
};

#endif
