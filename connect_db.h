#ifndef CONNECT_DB_H
#define CONNECT_DB_H

#include <stddef.h>
#include <sqlite3.h>

extern sqlite3 *db;

int connectDb();
void closeDb();

#endif
