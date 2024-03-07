#include "messages.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define screenMaxline 22

void *receive_messages(void *arg)
{
    struct clientinfo *args   = (struct clientinfo *)arg;
    int                sockfd = args->sockfd;    // Access sockfd from the structure

    // int  sockfd = *((int *)arg);
    char input_message[] = "Input message (Q to quit):";
    int  row;
    int  col;
    getmaxyx(stdscr, row, col);
    char message[BUF_SIZE];
    start_color();
    init_pair(2, COLOR_WHITE, COLOR_BLACK);    // 흰색 색상 쌍 초기화

    while(1)
    {
        attron(COLOR_PAIR(2));    // 흰색 속성 켜기
        int str_len = (int)read(sockfd, message, BUF_SIZE - 1);
        if(str_len <= 0)
        {
            perror("read() error");
            break;
        }

        message[str_len] = 0;
        printf("\r");

        // 사용자 입력 받기 전에 화면 초기화
        if(args->screenOrder == screenMaxline)
        {
            clear();
            move(row - 2, 0);
            for(int i = 0; i < col; ++i)
            {
                addch(ACS_HLINE);
            }
            move(row - 1, 0);
            addstr(input_message);
            args->screenOrder = 0;
        }
        move(args->screenOrder++, 0);
        addstr(message);
        refresh();
        move(args->inputPointer, (int)strlen(input_message));
        attroff(COLOR_PAIR(2));
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