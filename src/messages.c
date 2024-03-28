#include "messages.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define screenMaxline 22
#define letter1 10
#define letter2 20

int count_newlines(const char *message)
{
    int    count  = 0;
    size_t length = strlen(message);

    for(size_t i = 0; i < length; ++i)
    {
        if(message[i] == '\n')
        {
            count++;
        }
    }

    return count;
}

void *receive_messages(void *arg)
{
    struct clientinfo *args   = (struct clientinfo *)arg;
    int                sockfd = args->sockfd;    // Access sockfd from the structure

    // int  sockfd = *((int *)arg);
    char     input_message[] = "Input message (Q to quit):";
    int      row;
    int      col;
    char     message[BUF_SIZE];
    uint8_t  received_version;
    uint16_t received_content_size;
    char     content_size_str[letter2];    // 컨텐츠 크기를 저장할 문자열

    getmaxyx(stdscr, row, col);

    //    char received_message[BUF_SIZE]; // MAX_MESSAGE_SIZE는 수신할 수 있는 최대 메시지 크기

    start_color();
    init_pair(2, COLOR_WHITE, COLOR_BLACK);    // 흰색 색상 쌍 초기화

    while(1)
    {
        int str_len;

        char version_str[letter1];    // 버전을 저장할 문자열
        int  newline_count;
        attron(COLOR_PAIR(2));    // 흰색 속성 켜기
        if(read(sockfd, &received_version, sizeof(received_version)) <= 0)
        {
            perror("read() error");
            break;
        }

        // 컨텐츠 크기 수신 (uint16_t)
        if(read(sockfd, &received_content_size, sizeof(received_content_size)) <= 0)
        {
            perror("read() error");
            break;
        }

        str_len = (int)read(sockfd, message, BUF_SIZE - 1);
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

        // 받은 정수를 문자열로 변환
        sprintf(version_str, "%u", received_version);

        addstr("Version: ");
        addstr(version_str);    // 문자열로 변환된 버전 정보 출력
        addch(' ');

        // received_content_size를 문자열로 변환하여 출력

        sprintf(content_size_str, "%d", ntohs(received_content_size));
        addstr("Content size: ");
        addstr(content_size_str);    // 문자열로 변환된 컨텐츠 크기 출력
        addch(' ');

        // message 출력
        addstr("message: ");
        addstr(message);
        addch(' ');

        //        printf("Received version: %u\n", received_version);
        //        printf("Received content size: %u\n", received_content_size);
        //        printf("Received message: %s\n", message);

        newline_count     = count_newlines(message);
        args->screenOrder = args->screenOrder + newline_count;
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