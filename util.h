#pragma once

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

static void signalHandler(int nSigno);
void ignSigpipe();
int setNonblocking(int fd);