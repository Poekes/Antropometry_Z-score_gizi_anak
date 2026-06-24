#ifndef IBU_CONTROLLER_H
#define IBU_CONTROLLER_H

#include "../models/UserModel.h"

class IbuController {
public:
    IbuController(UserSession& session);
    void run();

private:
    UserSession& currentSession;

    void showMainMenu();
    void doPilihAnak();
    void doLihatRiwayat();
};

#endif
