#include "login.h"
#include <ncurses.h>
#include <string.h>

#define width 40
#define height 10
#define size 50

void show_login_form()
{
    char username[size];
    char password[size];

    initscr();    // ncurses 초기화
    cbreak();     // 키 입력을 즉시 반응하도록 설정
    // noecho();     // 키 입력을 화면에 표시하지 않음
    // 색상 모드 활성화
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);    // 흰색 텍스트, 파란색 배경
    init_pair(2, COLOR_RED, COLOR_BLACK);     // 빨간색 텍스트, 검은색 배경

    // 화면 크기 얻기
    int rows;
    int cols;
    getmaxyx(stdscr, rows, cols);

    // 로그인 폼 크기 및 위치 계산
    int form_height = height;
    int form_width  = width;
    int starty      = (rows - form_height) / 2;    // 폼 시작 y 위치
    int startx      = (cols - form_width) / 2;     // 폼 시작 x 위치

    // 새로운 윈도우 생성
    WINDOW *my_win = newwin(4, form_width, starty - 4, startx);

    // 박스 그리기

    // 텍스트 출력
    attron(A_BOLD);    // 굵은 텍스트
    mvwprintw(my_win, 2, 4, "Welcome to Oset Chatting System");
    attroff(A_BOLD);

    // 윈도우에 적용
    wrefresh(my_win);
    // 로그인 폼 박스 생성
    WINDOW *login_form = newwin(form_height, form_width, starty, startx);

    box(login_form, 0, 0);    // 박스 그리기

    // 로그인 폼 내용 출력
    mvwprintw(login_form, 2, 2, "Username:");
    mvwprintw(login_form, 3, 2, "Password:");
    wrefresh(login_form);    // 화면에 반영

    // 사용자명 입력 받기
    mvwgetstr(login_form, 2, 12, username);

    // 패스워드 입력 받기
    // 입력 받은 문자를 화면에 표시
    wbkgd(login_form, COLOR_PAIR(2));    // 폼의 배경색 변경
    mvwgetstr(login_form, 3, 12, password);
    noecho();    // 입력 받은 문자를 화면에 표시하지 않음

    // 입력 받은 정보 확인
    attron(COLOR_PAIR(1));    // 텍스트 색상 변경
    mvprintw(rows - 3, 0, "Your username: %s", username);
    attron(COLOR_PAIR(2));    // 텍스트 색상 변경
    mvprintw(rows - 2, 0, "Your password: %s", password);
    refresh();    // 변경 사항 반영

    // 아무 키나 누를 때까지 대기
    getch();

    // ncurses 종료
    endwin();
}
