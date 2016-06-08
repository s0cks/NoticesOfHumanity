#ifndef NOTICESOFHUMANITY_DATABASE_H
#define NOTICESOFHUMANITY_DATABASE_H

#include <sqlite3.h>

typedef enum{
  FLAG_USER = 1,
  FLAG_SUPERUSER = 2,
} noh_profile_flags;

typedef struct _noh_database{
  sqlite3* database;
  sqlite3_stmt* response;
  int code;
  char* sql;
  struct _noh_bot* owner;
} noh_database;

typedef struct{
  char* name;
  int flags;
} noh_profile;

void noh_database_init(noh_database* db, char* path);
void noh_database_destroy(noh_database* db);
noh_profile* noh_get_profile(noh_database* db, char* name);

#endif