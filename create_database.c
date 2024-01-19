#include <sqlite3.h>
#include <stdio.h>

int main(void) {
    
    sqlite3 *db;
    char *err_msg = 0;
    
    if(sqlite3_open("database.db", &db) != SQLITE_OK){
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    
       
    char *sql = "CREATE TABLE Score(id INTEGER PRIMARY KEY, pseudo TEXT, score INT);"
                "INSERT INTO Score (pseudo, score) VALUES ('Pierre', 82);";

    if(sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        return 1;
    } 
    
    sqlite3_close(db);
    
    return 0;
}