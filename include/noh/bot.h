#ifndef NOTICESOFHUMANITY_BOT_H
#define NOTICESOFHUMANITY_BOT_H

typedef struct _noh_bot noh_bot;

typedef void (*noh_command)(noh_bot* bot, int argc, char** argv);

typedef struct _noh_commands{
  struct _noh_commands* next;
  struct _noh_commands* prev;
  char* name;
  noh_command call;
} noh_commands;

struct _noh_bot{
  int sd;
  char* nick;
  noh_commands* commands;
  struct _noh_database* database;
};

void noh_init(noh_bot* bot, char* nick, char* db);
void noh_start(noh_bot* bot, char* port);
void noh_send(noh_bot* bot, char* msg);
void noh_add_command(noh_bot* bot, char* cmd, noh_command command);

#endif