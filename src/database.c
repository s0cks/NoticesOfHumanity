#include <noh/database.h>
#include <string.h>
#include <stdlib.h>
#include <noh/bot.h>
#include <stdio.h>

#define SELECT_USER "SELECT * FROM users WHERE name = '%s';"

void
noh_database_init(noh_database* db, char* path){
  if((db->code = sqlite3_open(path, &db->database))) {
    printf(stderr, "Can't open database %s\n", sqlite3_errmsg(db->database));
    abort();
  }
}

void
noh_database_destroy(noh_database* db){
  sqlite3_close(db->database);
}

noh_profile*
noh_get_profile(noh_database* db, char* name){
  db->sql = malloc(strlen(SELECT_USER) - 2 + strlen(name));
  snprintf(db->sql, strlen(SELECT_USER) - 2 + strlen(name), SELECT_USER, name);
  db->code = sqlite3_prepare_v2(db->database, db->sql, -1, &db->response, 0);
  if(db->code != SQLITE_OK){
    noh_send(db->owner, "Error");
    printf("%s\n", sqlite3_errmsg(db->database));
    return NULL;
  }

  int pFlags = -1;
  char* pName = "<null>";

  for(;;){
    if((db->code = sqlite3_step(db->response)) == SQLITE_DONE){
      break;
    } else if(db->code != SQLITE_ROW){
      noh_send(db->owner, "Error");
      printf("%s\n", sqlite3_errmsg(db->database));
      break;
    }

    pFlags = sqlite3_column_int(db->response, 1);
    pName = (char*) sqlite3_column_text(db->response, 0);
  }

  noh_profile* profile = malloc(sizeof(noh_profile));
  profile->name = strdup(pName);
  profile->flags = pFlags;
  free(db->sql);
  return profile;
}