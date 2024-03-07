#ifndef MESSAGES_H
#define MESSAGES_H
#define BUF_SIZE 2048

#include <ncurses.h>

struct clientinfo
{
    int sockfd;
    int screenOrder;
    int inputPointer;
};

void *receive_messages(void *arg);
int   count_newlines(const char *message);

#endif /* MESSAGES_H */
