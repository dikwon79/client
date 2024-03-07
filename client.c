//
// Created by dongil on 06/03/24.
//
#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>
#include <ncurses.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define UNKNOWN_OPTION_MESSAGE_LEN 24
#define BUF_SIZE 30
#define BASE_TEN 10
//#define POSITION 100

static void           parse_arguments(int argc, char *argv[], char **address, char **port);
static void           handle_arguments(const char *binary_name, const char *address, const char *port_str, in_port_t *port);
static in_port_t      parse_in_port_t(const char *binary_name, const char *port_str);
static void           convert_address(const char *address, struct sockaddr_storage *addr);
static int            socket_create(int domain, int type, int protocol);
static void           socket_connect(int sockfd, struct sockaddr_storage *addr, in_port_t port);
static void          *receive_messages(void *arg);
static void           socket_close(int client_fd);
_Noreturn static void usage(const char *program_name, int exit_code, const char *message);

struct clientinfo
{
    int sockfd;
    int screenOrder;
    int inputPointer;
};

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

    // ncurses 초기화
    initscr();
    // noecho();

    getmaxyx(stdscr, row, col);
    printf("%d", col);
    int cur_position = 0;    //(col / 2) - (len / 2);

    move(row - 1, cur_position);

    start_color();    // 컬러 모드 활성화

    init_pair(1, COLOR_YELLOW, COLOR_BLACK);    // 노란색 색상 쌍 초기화

    attron(COLOR_PAIR(1));    // 노란색 속성 켜기

    // cbreak();

    pthread_t thread;

    ip_address = NULL;
    port_str   = NULL;
    parse_arguments(args, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);

    sockfd = socket_create(addr.ss_family, SOCK_STREAM, 0);
    socket_connect(sockfd, &addr, port);
    struct clientinfo clientinfo = {sockfd, 0, row - 1};

    while(1)
    {
        char user_name[]     = "dongil";
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

        // 사용자 입력 받기 전에 화면 초기화

        getnstr(message, BUF_SIZE - 1);

        refresh();    // 화면 업데이트
        // endwin();
        //  fputs("Input message (Q to quit): ", stdout);

        // fgets(message, BUF_SIZE, stdin);
        attroff(COLOR_PAIR(1));    // 속성 해제

        // Construct the message with user name
        size_t full_message_size = strlen(user_name) + strlen(": ") + strlen(message) + 1;    // +1 for null terminator
        char  *full_message      = (char *)malloc(full_message_size);
        if(full_message == NULL)
        {
            perror("malloc() error");
            exit(EXIT_FAILURE);
        }

        snprintf(full_message, full_message_size, "%s: %s", user_name, message);

        write(sockfd, full_message, strlen(full_message));

        free(full_message);
    }

    socket_close(sockfd);

    return 0;
}

static void parse_arguments(int argc, char *argv[], char **ip_address, char **port)
{
    int opt;

    opterr = 0;

    while((opt = getopt(argc, argv, "h")) != -1)
    {
        switch(opt)
        {
            case 'h':
            {
                usage(argv[0], EXIT_SUCCESS, NULL);
            }
            case '?':
            {
                char message[UNKNOWN_OPTION_MESSAGE_LEN];

                snprintf(message, sizeof(message), "Unknown option '-%c'.", optopt);
                usage(argv[0], EXIT_FAILURE, message);
            }
            default:
            {
                usage(argv[0], EXIT_FAILURE, NULL);
            }
        }
    }

    if(optind >= argc)
    {
        usage(argv[0], EXIT_FAILURE, "The IP address or hostname is required.");
    }

    if(optind < argc - 2)
    {
        usage(argv[0], EXIT_FAILURE, "Too many arguments.");
    }

    *ip_address = argv[optind];
    *port       = argv[optind + 1];
}

static void handle_arguments(const char *binary_name, const char *ip_address, const char *port_str, in_port_t *port)
{
    if(ip_address == NULL)
    {
        usage(binary_name, EXIT_FAILURE, "The ip address is required.");
    }

    if(port_str == NULL)
    {
        usage(binary_name, EXIT_FAILURE, "The port is required.");
    }

    *port = parse_in_port_t(binary_name, port_str);
}

in_port_t parse_in_port_t(const char *binary_name, const char *str)
{
    char     *endptr;
    uintmax_t parsed_value;

    errno        = 0;
    parsed_value = strtoumax(str, &endptr, BASE_TEN);

    if(errno != 0)
    {
        perror("Error parsing in_port_t");
        exit(EXIT_FAILURE);
    }

    // Check if there are any non-numeric characters in the input string
    if(*endptr != '\0')
    {
        usage(binary_name, EXIT_FAILURE, "Invalid characters in input.");
    }

    // Check if the parsed value is within the valid range for in_port_t
    if(parsed_value > UINT16_MAX)
    {
        usage(binary_name, EXIT_FAILURE, "in_port_t value out of range.");
    }

    return (in_port_t)parsed_value;
}

static void convert_address(const char *address, struct sockaddr_storage *addr)
{
    memset(addr, 0, sizeof(*addr));

    if(inet_pton(AF_INET, address, &(((struct sockaddr_in *)addr)->sin_addr)) == 1)
    {
        addr->ss_family = AF_INET;
    }
    else if(inet_pton(AF_INET6, address, &(((struct sockaddr_in6 *)addr)->sin6_addr)) == 1)
    {
        addr->ss_family = AF_INET6;
    }
    else
    {
        fprintf(stderr, "%s is not an IPv4 or an IPv6 address\n", address);
        exit(EXIT_FAILURE);
    }
}

_Noreturn static void usage(const char *program_name, int exit_code, const char *message)
{
    if(message)
    {
        fprintf(stderr, "%s\n", message);
    }

    fprintf(stderr, "Usage: %s [-h] <IP address or hostname> <port>\n", program_name);
    fputs("Options:\n", stderr);
    fputs("  -h  Display this help message\n", stderr);
    exit(exit_code);
}

static int socket_create(int domain, int type, int protocol)
{
    int sockfd;

    sockfd = socket(domain, type, protocol);

    if(sockfd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

static void socket_connect(int sockfd, struct sockaddr_storage *addr, in_port_t port)
{
    char      addr_str[INET6_ADDRSTRLEN];
    in_port_t net_port;
    socklen_t addr_len;

    if(inet_ntop(addr->ss_family, addr->ss_family == AF_INET ? (void *)&(((struct sockaddr_in *)addr)->sin_addr) : (void *)&(((struct sockaddr_in6 *)addr)->sin6_addr), addr_str, sizeof(addr_str)) == NULL)
    {
        perror("inet_ntop");
        exit(EXIT_FAILURE);
    }

    printf("Connecting to: %s:%u\n", addr_str, port);
    net_port = htons(port);

    if(addr->ss_family == AF_INET)
    {
        struct sockaddr_in *ipv4_addr;

        ipv4_addr           = (struct sockaddr_in *)addr;
        ipv4_addr->sin_port = net_port;
        addr_len            = sizeof(struct sockaddr_in);
    }
    else if(addr->ss_family == AF_INET6)
    {
        struct sockaddr_in6 *ipv6_addr;

        ipv6_addr            = (struct sockaddr_in6 *)addr;
        ipv6_addr->sin6_port = net_port;
        addr_len             = sizeof(struct sockaddr_in6);
    }
    else
    {
        fprintf(stderr, "Invalid address family: %d\n", addr->ss_family);
        exit(EXIT_FAILURE);
    }

    if(connect(sockfd, (struct sockaddr *)addr, addr_len) == -1)
    {
        char *msg;

        msg = strerror(errno);
        fprintf(stderr, "Error: connect (%d): %s\n", errno, msg);
        exit(EXIT_FAILURE);
    }

    printf("Connected to: %s:%u\n", addr_str, port);
}

static void socket_close(int client_fd)
{
    if(close(client_fd) == -1)
    {
        perror("Error closing socket");
        exit(EXIT_FAILURE);
    }
}

static void *receive_messages(void *arg)
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
        move(args->ginputPointer, BUF_SIZE - 4);
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
