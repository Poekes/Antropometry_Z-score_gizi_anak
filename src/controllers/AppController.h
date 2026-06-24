#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include "../models/UserModel.h"
#include "../zscore_data.h"

class AppController {
public:
    AppController();
    void run();

private:
    ReferenceData refData;
    UserSession currentSession;
};

#endif
