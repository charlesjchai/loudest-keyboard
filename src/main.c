#include <curses.h>
#include "miniaudio.h"
#include "stack.h"
#include "dict.h"

int main(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    
    endwin();
    return 0;
}