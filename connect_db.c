#include <sqlite3.h>
#include <stdio.h>
#include "connect_db.h"
#include "open_config.h"

sqlite3 *db;

int connectDb() {
    if (sqlite3_open(config->database_name, &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    return 1;
}

void closeDb(){
    sqlite3_close(db);
}