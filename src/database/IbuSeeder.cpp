#include "IbuSeeder.h"
#include "../models/UserModel.h"

void IbuSeeder::seed() {
    if (!UserModel::userExists("ibu_budi")) {
        UserModel::registerUser(2, "ibu_budi", "123");
        UserModel::linkIbuToAnak("ibu_budi", "Budi");
    }
    if (!UserModel::userExists("ibu_siti")) {
        UserModel::registerUser(2, "ibu_siti", "123");
        UserModel::linkIbuToAnak("ibu_siti", "Siti");
    }
    if (!UserModel::userExists("ibu_andi")) {
        UserModel::registerUser(2, "ibu_andi", "123");
        UserModel::linkIbuToAnak("ibu_andi", "Andi");
    }
}
