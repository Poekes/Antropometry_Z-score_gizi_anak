#ifndef DATABASE_SEEDER_H
#define DATABASE_SEEDER_H

#include "../zscore_data.h"

class DatabaseSeeder {
public:
    static void run(const ReferenceData& refData);
};

#endif
