#ifndef IBU_CONTROLLER_H
#define IBU_CONTROLLER_H

#include "../models/UserModel.h"
#include "../zscore_data.h"

class IbuController {
public:
    IbuController(UserSession& session, ReferenceData& refData);
    void run();

private:
    UserSession& currentSession;
    ReferenceData& refData;

    void showMainMenu();
    void doPilihAnak();
    void doLihatRiwayat();
    void doLihatDetailDiagnosis();
};

#endif
