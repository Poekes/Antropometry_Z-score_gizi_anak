#include "KaderSeeder.h"
#include "../models/UserModel.h"

void KaderSeeder::seed() {
    if (!UserModel::userExists("kader")) {
        UserModel::registerUser(1, "kader", "123");
    }
}
