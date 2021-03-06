#include <sys/wait.h>
#include <unistd.h>
#include "project2.h"
#include "system.h"
#include "utility.h"

#define MAX_LEN 255

#define TICK {				\
int x;					\
for (x = 0; x < 100; ++x) {			\
interrupt->OneTick();				\
}}

#define TICK {currentThread->Yield();}

char*
readline(FILE* fp) {
  static char line[MAX_LEN + 1];
  
  int pos = 0;
  char c = fgetc(fp);
  TICK;
  while (c != '\n' && c != EOF) {
    TICK;
    line[pos++] = c;
    TICK;
    c = fgetc(fp);
  }
  line[pos] = '\0';
  return c == EOF ? NULL : line;
}

void
serve(char* url) {
  pid_t pid = fork();
  if (pid == 0) {
    char* const argv[] = { "/usr/ecs/bin/wget", "-q", "-p", "-k", url, NULL };
    execv(argv[0], argv);
  }
  else {
    wait(NULL);
  }
}
