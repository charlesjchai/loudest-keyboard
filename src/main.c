#include "dict.h"
#include "miniaudio.h"
#include "stack.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bigkey_wav.h"
#include "key_wav.h"
#include "space_wav.h"

int main(void) {
    ma_engine engine;
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        printf("Failed to init engine\n");
        return -1;
    }
    ma_result result;
    if ((result = ma_resource_manager_register_encoded_data(
             ma_engine_get_resource_manager(&engine), "key.wav", key_wav,
             key_wav_len)) != MA_SUCCESS ||
        (result = ma_resource_manager_register_encoded_data(
             ma_engine_get_resource_manager(&engine), "bigkey.wav", bigkey_wav,
             bigkey_wav_len)) != MA_SUCCESS ||
        (result = ma_resource_manager_register_encoded_data(
             ma_engine_get_resource_manager(&engine), "space.wav", space_wav,
             space_wav_len)) != MA_SUCCESS) { // Initialize sound
        perror("Failed to register data");
        return -1;
    }
    srand(time(NULL));
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    int y, x;
    char ch;
    while ((ch = getch())) {
        getyx(stdscr, y, x);
        if (ch == 127) {
            move(y, x - 1);
            delch();
        } else if (ch >= 32 && ch < 127) {
            addch(ch);
        }
    }

    endwin();
    ma_resource_manager_unregister_data(ma_engine_get_resource_manager(&engine),
                                        "key.wav");
    ma_resource_manager_unregister_data(ma_engine_get_resource_manager(&engine),
                                        "bigkey.wav");
    ma_resource_manager_unregister_data(ma_engine_get_resource_manager(&engine),
                                        "space.wav");
    ma_engine_uninit(&engine);
    return 0;
}