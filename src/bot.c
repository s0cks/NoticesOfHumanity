#include <noh/bot.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <noh/database.h>

#define MAX_DATA_SIZE 256
#define PING "PING "

int
send_data(noh_bot* bot, char* msg){
  ssize_t sent = send(bot->sd, msg, strlen(msg), 0);
  printf(">> %s\n", msg);
  return sent != 0;
}

int
find(char* toSearch, char* searchFor){
  for(int i = 0; i < strlen(toSearch); i++){
    if(searchFor[0] == toSearch[i]){
      int found = 1;
      for(int x = 1; x < strlen(searchFor); x++){
        if(toSearch[i + x] != searchFor[x]){
          found = 0;
          break;
        }
      }

      if(found){
        return 1;
      }
    }
  }

  return 0;
}

void
pong(noh_bot* bot, char* buff){
  for(int i = 0; i < strlen(buff); i++){
    if(buff[i] == PING[0]){
      int found = 1;
      for(int x = 1; x < 4; x++){
        if(buff[i + x] != PING[x]){
          found = 0;
          break;
        }
      }

      if(found){
        char ret[strlen(buff)];
        sprintf(ret, "PONG %s\r\n", strstr(buff, " ") + 1);
        send_data(bot, ret);
      }
    }
  }
}

char*
get_caller(char* buffer){
  int caller_size = 0;
  for(int i = 1; i < strlen(buffer); i++){
    if(buffer[i] == '!') break;
    else caller_size++;
  }

  char* caller = malloc((size_t) (caller_size + 1));
  memcpy(caller, &buffer[1], (size_t) caller_size);
  caller[caller_size] = '\0';
  return caller;
}


int
is_superuser(noh_bot* bot, char* buffer){
  char* caller = get_caller(buffer);
  noh_profile* profile = noh_get_profile(bot->database, caller);
  if(profile->flags == FLAG_SUPERUSER){
    free(caller);
    free(profile);
    return 1;
  } else{
    free(caller);
    free(profile);
    return 0;
  }
}

void
noh_init(noh_bot* bot, char* nick, char* db) {
  bot->nick = strdup(nick);
  bot->commands = NULL;
  bot->database = malloc(sizeof(noh_database));
  bot->database->owner = bot;
  printf("Opening Database @%s\n", db);
  noh_database_init(bot->database, db);
}

void
noh_start(noh_bot* bot, char* port){
  printf("--- Connecting ---\n");
  struct addrinfo hints, *server_info;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  int res;
  if((res = getaddrinfo("irc.esper.net", port, &hints, &server_info)) != 0){
    fprintf(stderr, "[noh_start] getaddrinfo: %s\n", gai_strerror(res));
    return;
  }

  if((bot->sd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol)) == -1){
    fprintf(stderr, "[noh_start] socket\n");
    return;
  }

  if(connect(bot->sd, server_info->ai_addr, server_info->ai_addrlen) == -1){
    close(bot->sd);
    fprintf(stderr, "[noh_start] connect\n");
    return;
  }

  char nick[7 + strlen(bot->nick) + 1];
  snprintf(nick, 7 + strlen(bot->nick) + 3, "NICK %s\r\n", bot->nick);

  char user[12 + (strlen(bot->nick) * 2)];
  snprintf(user, 12 + (strlen(bot->nick) * 2) + 3, "USER %s 8 * : %s\r\n", bot->nick, bot->nick);

  send_data(bot, nick);
  send_data(bot, user);

  char buffer[MAX_DATA_SIZE];
  int count = 0;
  ssize_t bytes;

  while(1){
    count++;
    switch(count){
      case 7: send_data(bot, "JOIN #iWin\r\n"); break;
      default: break;
    }

    bytes = read(bot->sd, buffer, MAX_DATA_SIZE - 1);
    buffer[bytes + 1] = '\0';

    printf("<< %s\n", buffer);

    if(find(buffer, "PING ")){
      pong(bot, buffer);
    } else if(find(buffer, "PRIVMSG #iWin")){
      char msg[0];
      if(is_superuser(bot, buffer)) {
        noh_send(bot, "Hello");
      }
    }

    if(bytes == 0){
      printf("--- Connection Closed ---\n");
      break;
    }
  }
}

void
noh_send(noh_bot* bot, char* m){
  char msg[18 + strlen(m)];
  snprintf(msg, sizeof(msg) / sizeof(char), "PRIVMSG #iWin :%s\r\n", m);
  send_data(bot, msg);
}

void
noh_add_command(noh_bot* bot, char* cmd, noh_command command){
  noh_commands* cmds = malloc(sizeof(noh_commands));
  cmds->call = command;
  cmds->name = strdup(cmd);
  cmds->next = bot->commands;
  if(bot->commands != NULL){
    bot->commands->prev = cmds;
  }
  bot->commands = cmds;
}