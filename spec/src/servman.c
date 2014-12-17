
//
// spec helper for bocla
//
// Sun Sep 21 14:58:20 JST 2014
//

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "servman.h"


pid_t server_pid = -1;


void server_start()
{
  if (server_pid > 0) return;

  server_pid = fork();

  if (server_pid == 0)
  {
    char *v = getenv("VALSERV");

    if (v != NULL && (strcmp(v, "1") == 0 || strcmp(v, "true") == 0))
    {
      char *env[] = { "FGAJ_HOST=g", "FGAJ_LEVEL=20", NULL };
      execle(
        "/usr/bin/valgrind", "val_shv_tst_webapp",
        "--leak-check=full", "-v",
        "../tst/webapp", NULL, env);
    }
    else
    {
      char *env[] = { "FGAJ_HOST=g", "FGAJ_LEVEL=20", NULL };
      execle(
        "../tst/webapp", "shv_tst_webapp",
        NULL, env);
    }

    perror("execle failed"); exit(1);
  }
  else
  {
    sleep(1);
  }
}

void server_stop()
{
  if (server_pid < 1) return;

  printf("stopping %i...\n", server_pid);
  kill(server_pid, SIGTERM);

  int status;
  waitpid(server_pid, &status, 0);
}

