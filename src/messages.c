#include "messages.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *receive_messages(void *arg)
{
    struct clientinfo *args   = (struct clientinfo *)arg;
    int                sockfd = args->sockfd;    // Access sockfd from the structure

    // int  sockfd = *((int *)arg);
    char message[BUF_SIZE];

    while(1)
    {
        int str_len = (int)read(sockfd, message, BUF_SIZE - 1);
        if(str_len <= 0)
        {
            perror("read() error");
            break;
        }

        message[str_len] = 0;
        printf("\r");
        args->screenOrder++;
        move(args->screenOrder, 0);
        addstr(message);
        refresh();
        move(args->inputPointer, BUF_SIZE - 4);
        refresh();
        //  Clear the entire line
        //  printf("\033[K");'

        //        move(args->inputPointer, 0);
        //
        //        addstr("Input message (Q to quit):");
        //
        //        getnstr(message, BUF_SIZE - 1);

        // printf("%s Input message (Q to quit): ", message);
        // fflush(stdout);
    }
    return NULL;
}