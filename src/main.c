#include "dict.h"
#include "miniaudio.h"
#include "stack.h"
#include <curses.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bigkey_wav.h"
#include "key_wav.h"
#include "space_wav.h"

char **divide_str(const char *str, size_t str_len, char delimiter,
                  size_t *divided_count) {
    // Upper bound on token count
    size_t max_tokens = 1;
    for (size_t i = 0; i < str_len; i++) {
        if (str[i] == delimiter)
            max_tokens++;
    }
    char **quotient = malloc(max_tokens * sizeof(char *));
    char *copy = strndup(str, str_len); // Prevent const error
    if (!quotient || !copy) {
        free(quotient);
        free(copy);
        *divided_count = 0;
        return NULL;
    }
    const char delim[2] = {delimiter, '\0'};
    size_t n = 0;
    for (char *tok = strtok(copy, delim); tok != NULL;
         tok = strtok(NULL, delim)) {
        quotient[n] = strdup(tok);
        if (!quotient[n])
            break;
        n++;
    }
    free(copy);
    *divided_count = n;
    return quotient;
}

#if RAND_MAX == 32767
#define RANDOM() ((rand() << 15) | rand())
#else
#define RANDOM() rand()
#endif
/**
 * @brief Makes a string of random words from a dictionary
 * @param[in]  dictionary The array of strings (words) to be used.
 * @param[in]  target_len The maximum length (inclusive) of the return string.
 * @param[out] wordlist_len The length of the wordlist.
 */
char *generate_word_list(char **dictionary, size_t dictionary_len,
                         size_t target_len, size_t *wordList_len) {
    Stack chosenWordIndices = {.data = malloc(1 * sizeof(size_t)),
                               .len = 0,
                               .capacity = 1,
                               .element_size = sizeof(size_t)};
    size_t char_count = 0;
    size_t next_char_count = 0;
    while (1) {
        size_t current_wordidx = RANDOM() % dictionary_len;
        const char *current_word = dictionary[current_wordidx];
        next_char_count += strlen(current_word) + 1;
        if (next_char_count > target_len)
            break;
        if (stk_push(&chosenWordIndices, &current_wordidx) != 0) {
            perror("Stack push failed");
            exit(EXIT_FAILURE); // Error checking
        }
        char_count = next_char_count;
    }
    char *wordList = calloc(char_count + 1, sizeof(char));
    // Snapshot
    size_t chosenWordsCount = chosenWordIndices.len;
    size_t wordListPos = 0;
    for (size_t i = 0; i < chosenWordsCount; i++) {
        size_t current_idx = *(size_t *)stk_pop(&chosenWordIndices);
        memcpy(wordList + wordListPos, dictionary[current_idx],
               strlen(dictionary[current_idx]));
        wordList[wordListPos + strlen(dictionary[current_idx])] = ' ';
        wordListPos += strlen(dictionary[current_idx]) + 1;
    }
    if (wordListPos > 0)
        wordListPos--; // Protect buffer underflow
    wordList[wordListPos] = '\0';
    free(chosenWordIndices.data);
    *wordList_len = wordListPos;
    return wordList;
}
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
    size_t dictionary_len;
    char **dictionary =
        divide_str((const char *)words, words_len, '\n', &dictionary_len);
    size_t wordlist_len;
    char *wordlist =
        generate_word_list(dictionary, dictionary_len, 50, &wordlist_len);
    size_t pos = 0;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(1); 
    start_color();
    enum Colors {
        COLOR_DEF,   // 0
        COLOR_RIGHT, // 1
        COLOR_WRONG, // 2
    };
    init_pair(COLOR_RIGHT, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_WRONG, COLOR_RED, COLOR_BLACK);

    mvprintw(1, 0, "%s", wordlist);
    move(1, 0);
    // allows 1 wrong char before locking input
    bool wrongChar = false; 
    int y, x;
    int ch;
    while ((ch = getch()) != ERR) {
        getyx(stdscr, y, x);
        if (ch >= 32 && ch < 127) {
            char input = inch() & A_CHARTEXT;
            if (ch == input) {
                wrongChar = false;
                pos++;
                chgat(1, A_UNDERLINE, COLOR_RIGHT, NULL);
                move(y, x + 1);
            } else if (!wrongChar) {
                wrongChar = true;
                pos++;
                chgat(1, A_UNDERLINE, COLOR_WRONG, NULL);
                move(y, x + 1);
            }
            if (ch == 32) {
                ma_engine_play_sound(&engine, "space.wav", NULL);
            } else {
                ma_engine_play_sound(&engine, "key.wav", NULL);
            }
        } else if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
            if (x != 0) {
                wrongChar = false;
                pos--;
                move(y, x - 1);
                chgat(1, A_NORMAL, COLOR_DEF, NULL);
            }
            ma_engine_play_sound(&engine, "bigkey.wav", NULL);
        }
    }

    endwin();
    ma_resource_manager *resource_man = ma_engine_get_resource_manager(&engine);
    ma_resource_manager_unregister_data(resource_man, "key.wav");
    ma_resource_manager_unregister_data(resource_man, "bigkey.wav");
    ma_resource_manager_unregister_data(resource_man, "space.wav");
    ma_engine_uninit(&engine);
    return 0;
}
