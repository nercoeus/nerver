#pragma once

#include "util.h"
#include "def.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>
#include <stdio.h>

int serverfdInit(int port);

int acceptConnect(int fd);