#include "arguments.h"
#include "login.h"
#include "messages.h"
#include "socket.h"
#include "utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define screenMaxline 22
#define screenStart 0

int main(int args, char *argv[])
{
    char                   *ip_address;
    char                   *port_str;
    in_port_t               port;
    struct sockaddr_storage addr;
    int                     sockfd;
    int                     row;
    int                     col;

    // char                    text[] = "hello world";
    //  int                     len    = (int)strlen(text);

    show_login_form();

    // ncurses 초기화
    initscr();
    clear();
    echo();

    getmaxyx(stdscr, row, col);
    printf("%d", col);
    int cur_position = 0;    //(col / 2) - (len / 2);

    move(row - 1, cur_position);

    // cbreak();

    pthread_t thread;

    ip_address = NULL;
    port_str   = NULL;
    parse_arguments(args, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);

    sockfd = socket_create(addr.ss_family, SOCK_STREAM, 0);
    socket_connect(sockfd, &addr, port);
    struct clientinfo clientinfo = {sockfd, screenStart, row - 1};

    start_color();                              // 컬러 모드 활성화
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);    // 노란색 색상 쌍 초기화

    while(1)
    {
        attron(COLOR_PAIR(1));    // 노란색 속성 켜기
        // char user_name[]     = "dongil";
        char input_message[] = "Input message (Q to quit):";
        char message[BUF_SIZE];
        memset(message, 0, sizeof(message));
        // 사용자에게 메시지 입력 요청
        // mvprintw(0, 0, "Input message (Q to quit):");

        if(pthread_create(&thread, NULL, receive_messages, (void *)&clientinfo) != 0)
        {
            perror("Thread creation failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        clrtoeol();    // 현재 행의 커서 위치부터 끝까지 지우기
        // 수평선 출력
        move(row - 2, 0);
        for(int i = 0; i < col; ++i)
        {
            addch(ACS_HLINE);
        }
        addstr(input_message);
        refresh();
        move(row - 1, (int)strlen(input_message));
        // refresh();

        getnstr(message, BUF_SIZE - 1);

        // 사용자 입력 받기 전에 화면 초기화
        if(clientinfo.screenOrder == screenMaxline)
        {
            clear();
            move(row - 2, 0);
            for(int i = 0; i < col; ++i)
            {
                addch(ACS_HLINE);
            }
            move(row - 1, 0);
            addstr(input_message);
            clientinfo.screenOrder = 0;
            move(0, 0);
        }
        if(!strcmp(message, "q") || !strcmp(message, "Q"))
        {
            endwin();
            pthread_cancel(thread);
            exit(0);
        }

        move(clientinfo.screenOrder++, 0);
        printw("Me> %s", message);
        move(clientinfo.inputPointer, 4);

        refresh();                 // 화면 업데이트
        attroff(COLOR_PAIR(1));    // 속성 해제
        // endwin();
        //  fputs("Input message (Q to quit): ", stdout);

        // fgets(message, BUF_SIZE, stdin);

        // Construct the message with user name
        size_t full_message_size = strlen(message) + 1;    // +1 for null terminator
        char  *full_message      = (char *)malloc(full_message_size);
        if(full_message == NULL)
        {
            perror("malloc() error");
            exit(EXIT_FAILURE);
        }

        snprintf(full_message, full_message_size, "%s", message);

        write(sockfd, full_message, strlen(full_message));

        free(full_message);
    }
    endwin();
    socket_close(sockfd);
    pthread_cancel(thread);
    pthread_join(thread, NULL);
    return 0;
}