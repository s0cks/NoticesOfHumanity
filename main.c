#include <noh/bot.h>
#include <noh/database.h>

int
main(int argc, char** argv) {
  noh_bot bot;
  noh_init(&bot, "Elektra", argv[1]);
  noh_start(&bot, "6667");
  noh_database_destroy(bot.database);
  return 0;
}