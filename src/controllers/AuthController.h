#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H

#include "../models/UserModel.h"

class AuthController {
public:
    static bool showLoginScreen(UserSession& session);
};

#endif
