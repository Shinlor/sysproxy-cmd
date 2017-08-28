#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "common.h"

static const char* proxyHost;
static const char* proxyPort;

void usage(const char* binName)
{
  printf("Usage: %s [show | on | off | wait-and-cleanup <proxy host> <proxy port>]\n", binName);
  exit(INVALID_FORMAT);
}

void handleSignals(int signal)
{
  toggleProxy(false, proxyHost, proxyPort);
}

void setupSignals()
{
  // Register signal handlers to make sure we turn proxy off no matter what
  signal(SIGABRT, handleSignals);
  signal(SIGFPE, handleSignals);
  signal(SIGILL, handleSignals);
  signal(SIGINT, handleSignals);
  signal(SIGSEGV, handleSignals);
  signal(SIGTERM, handleSignals);
  signal(SIGSEGV, handleSignals);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    usage(argv[0]);
  }

#ifdef DARWIN
  if (strcmp(argv[1], "setuid") == 0) {
    return setUid();
  }
#endif

  if (strcmp(argv[1], "show") == 0) {
    return show();
  } else {
    if (argc < 4) {
      usage(argv[0]);
    }
    proxyHost = argv[2];
    proxyPort = argv[3];
    if (strcmp(argv[1], "on") == 0) {
      return toggleProxy(true, proxyHost, proxyPort);
    } else if (strcmp(argv[1], "off") == 0) {
      return toggleProxy(false, proxyHost, proxyPort);
    } else if (strcmp(argv[1], "wait-and-cleanup") == 0) {
      setupSignals();
#ifdef _WIN32
      setupSystemShutdownHandler();
#endif
      // wait for input from stdin (or close), then toggle off
      getchar();
      return toggleProxy(false, proxyHost, proxyPort);
    } else {
      usage(argv[0]);
    }
  }
  // code never reaches here, just avoids compiler from complaining.
  return RET_NO_ERROR;
}
