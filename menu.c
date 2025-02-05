#include <ctype.h>
#include <ncursesw/ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include "menu.h"
#include "UserFileCreator.h"
#include "main.h"
#include <locale.h>

#define MAP_HEIGHT 30
#define MAP_WIDTH 80

// Function declarations
int valid_password(const char *password);
int valid_email(const char *str);
int authenticate_user(const char *username, const char *password);
void create_account();
void guest_mode();
int login();
int menu();
int bunny();

char usernameext[50] = "";
char passwordext[50] = "";

// Screen setup
void screen_setup()
{
    initscr();
    cbreak();
    noecho();
    curs_set(false);
    keypad(stdscr, TRUE);
    setlocale(LC_ALL, "");
}

// int main() {
//     screen_setup();
//     menu();
//     endwin(); // Exit ncurses mode
//     return 0;
// }

int menu()
{
    screen_setup();
    curs_set(false);

    const char *options[] = {"Log in", "Create account", "Guest mode", "Exit"};
    int n_options = sizeof(options) / sizeof(options[0]);
    int choice = -1;
    int highlight = 0;

    while (1)
    {
        clear();

        bunny();

        for (int i = 0; i < n_options; i++)
        {
            if (i == highlight)
            {
                attron(A_REVERSE); // Highlight the current option
            }
            mvprintw(i, 0, "%s", options[i]); // Print option
            if (i == highlight)
            {
                attroff(A_REVERSE); // Turn off highlight
            }
        }

        int key = getch();
        switch (key)
        {
        case KEY_UP:
            highlight = (highlight - 1 + n_options) % n_options; // Move up
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % n_options; // Move down
            break;
        case 10: // Enter
            choice = highlight;
            break;
        }

        if (choice == 0)
        {
            return login();
        }
        else if (choice == 1)
        {
            create_account();
            return 1;
        }
        else if (choice == 2)
        {
            return 2;
        }
        else if (choice == 3)
        {
            return 3; // Exit
        }

        choice = -1;
    }
}

int login()
{
    clear();
    char username[50] = "";
    char password[50] = "";

    echo();
    mvprintw(0, 0, "Please enter your username: ");
    scanw("%s", username);

    mvprintw(1, 0, "Please enter your password: ");
    scanw("%s", password);

    if (authenticate_user(username, password))
    {
        mvprintw(3, 0, "Login successful! Welcome, %s!", username);
    }
    else
    {
        mvprintw(3, 0, "Invalid username or password.");
        mvprintw(5, 0, "Press any key to continue. ");
        getch();
        return -1;
    }

    strcpy(usernameext, username);
    strcpy(passwordext, password);

    mvprintw(5, 0, "Press any key to continue. ");
    getch();
    return 0;
}

void create_account()
{
    clear();
    char username[100] = "";
    char email[100] = "";
    char password[100] = "";

    echo();
    mvprintw(0, 0, "Please enter a username: ");
    scanw(" %s", username);
    while (taken_username(username))
    {
        mvprintw(0, 0, "We can't let you have this username. Please enter a new one: ");
        scanw("%s", username);
    }

    mvprintw(1, 0, "Please enter your email address: ");
    scanw("%s", email);
    while (!valid_email(email))
    {
        mvprintw(2, 0, "Invalid email. Please re-enter: ");
        clrtoeol();
        scanw("%s", email);
    }

    mvprintw(3, 0, "Please enter your password: ");
    scanw("%s", password);
    while (!valid_password(password))
    {
        mvprintw(4, 0, "Password must be 8+ chars, 1 upper, 1 lower, 1 digit. Re-enter: ");
        clrtoeol();
        scanw("%s", password);
    }

    FILE *file = fopen("user_data.txt", "a");
    if (file)
    {
        fprintf(file, "Username: %s\nEmail: %s\nPassword: %s\n", username, email, password);
        fclose(file);
        mvprintw(6, 0, "Account created successfully!");
    }
    else
    {
        mvprintw(6, 0, "Error: Could not save account.");
    }

    strcpy(usernameext, username);
    strcpy(passwordext, password);

    mvprintw(8, 0, "Press any key continue.");
    getch();
}

void guest_mode()
{
    clear();
    mvprintw(0, 0, "Welcome to Guest Mode! Enjoy exploring the application.");
    mvprintw(2, 0, "Press any key to return to the menu.");
    getch();
}

int valid_password(const char *password)
{
    int has_upper = 0, has_lower = 0, has_digit = 0;

    if (strlen(password) < 8)
    {
        return 0; // Password is too short
    }

    while (*password)
    {
        if (isupper(*password))
        {
            has_upper = 1;
        }
        else if (islower(*password))
        {
            has_lower = 1;
        }
        else if (isdigit(*password))
        {
            has_digit = 1;
        }

        if (has_upper && has_lower && has_digit)
        {
            return 1;
        }
        password++;
    }
    return 0;
}

int valid_email(const char *str)
{
    const char *at = strchr(str, '@');
    if (at == NULL)
        return 0;

    const char *dot = strchr(at, '.');
    if (dot == NULL)
        return 0;

    if (at - str < 1 || dot - at < 2 || strlen(dot) < 2)
        return 0;

    return 1;
}

int taken_username(char *username)
{
    if (strlen(username) < 2)
        return 1;

    FILE *file = fopen("user_data.txt", "r");
    if (file == NULL)
    {
        return 0; // File not found
    }

    char file_username[50];
    char file_email[100];
    char file_password[50];

    while (fscanf(file, "Username: %99s\nEmail: %99s\nPassword: %99s\n", file_username, file_email, file_password) == 3)
    {
        if (strcmp(file_username, username) == 0)
        {
            fclose(file);
            return 1;
        }
    }

    return 0;
}

int authenticate_user(const char *username, const char *password)
{
    FILE *file = fopen("user_data.txt", "r");
    if (file == NULL)
    {
        return 0; // File not found
    }

    char file_username[50];
    char file_email[100];
    char file_password[50];

    while (fscanf(file, "Username: %99s\nEmail: %99s\nPassword: %99s\n", file_username, file_email, file_password) == 3)
    {
        if (strcmp(file_username, username) == 0 && strcmp(file_password, password) == 0)
        {
            fclose(file);
            return 1; // Authentication successful
        }
    }

    fclose(file);
    return 0; // Authentication failed
}

int NewGameChoices()
{
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();

    int choice = -1;
    int highlight = 0;
    const char *options[] = {"Continue", "NewGame", "Score Board", "Back"};
    int n_options = sizeof(options) / sizeof(options[0]);

    while (1)
    {
        clear();
        for (int i = 0; i < n_options; i++)
        {
            if (i == highlight)
            {
                attron(A_REVERSE); // Highlight the current option
            }
            mvprintw(i, 0, "%s", options[i]); // Print option

            if (i == highlight)
            {
                attroff(A_REVERSE); // Turn off highlight
            }
        }

        int key = getch();
        switch (key)
        {
        case KEY_UP:
            highlight = (highlight - 1 + n_options) % n_options; // Move up
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % n_options; // Move down
            break;
        case 10: // Enter key
            choice = highlight;
            break;
        }

        if (choice < n_options && choice > -1)
        { // Exit option
            return choice;
        }
    }

    endwin(); // Exit ncurses mode
    return 0;
}

int bunny()
{
    // for (int i = 0; i < 10; i++) {
    srand(time(NULL));

    char *bunny[] = {
        " /\\_/\\",
        "(^ - ^)",
        "(    <3",
        "  = = "};
    int bunny_height = 4;
    int bunny_width = 9;

    int max_y = 30 - bunny_height;
    int max_x = 80 - bunny_width;
    int v = 8 + rand() % max_y;
    int b = 80 + rand() % max_x;

    for (int i = 0; i < bunny_height; i++)
    {
        mvprintw(v + i, b, "%s\n", bunny[i]);
    }
    //}
}

int settingMenu()
{
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();

    int choice = -1;
    int highlight = 0;
    const char *options[] = {"Food", "Weapons", "Score Board", "Save", "Difficulty", "Heros", "Hero Colors", "Enchant", "Back"};
    int n_options = sizeof(options) / sizeof(options[0]);

    while (1)
    {
        clear();
        for (int i = 0; i < n_options; i++)
        {
            if (i == highlight)
            {
                attron(A_REVERSE); // Highlight the current option
            }
            mvprintw(i, 0, "%s", options[i]); // Print option

            if (i == highlight)
            {
                attroff(A_REVERSE); // Turn off highlight
            }
        }

        int key = getch();
        switch (key)
        {
        case KEY_UP:
            highlight = (highlight - 1 + n_options) % n_options; // Move up
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % n_options; // Move down
            break;
        case 10: // Enter key
            choice = highlight;
            printw("Chose\n");
            getchar();
            break;
        }

        if (choice < n_options && choice > -1)
        { // Exit option
            return choice;
        }
        // RECENTLY I REMOVE LOADBOAR() FROM HERE
    }

    endwin();
    return 0;
}

int Difficulty()
{
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();

    int choice = -1;
    int highlight = 0;
    const char *options[] = {"IRAN MODE", "EXTREME", "MEDIUM"};
    int n_options = sizeof(options) / sizeof(options[0]);

    while (1)
    {
        clear();
        for (int i = 0; i < n_options; i++)
        {
            if (i == highlight)
            {
                attron(A_REVERSE); // Highlight the current option
            }
            mvprintw(i, 0, "%s", options[i]); // Print option

            if (i == highlight)
            {
                attroff(A_REVERSE); // Turn off highlight
            }
        }

        int key = getch();
        switch (key)
        {
        case KEY_UP:
            highlight = (highlight - 1 + n_options) % n_options; // Move up
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % n_options; // Move down
            break;
        case 10: // Enter key
            choice = highlight;
            printw("Chose\n");
            getchar();
            break;
        }

        if (choice < n_options && choice > -1)
        { // Exit option
            return choice;
        }
    }

    endwin();
    return 0;
}

int HeroColors()
{
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();

    int choice = -1;
    int highlight = 0;
    const char *options[] = {"WHITE", "RED", "GREEN", "YELLOW", "BLUE", "MAGENTA", "CYAN"};
    int n_options = sizeof(options) / sizeof(options[0]);

    while (1)
    {
        clear();
        for (int i = 0; i < n_options; i++)
        {
            if (i == highlight)
            {
                attron(A_REVERSE); // Highlight the current option
            }
            mvprintw(i, 0, "%s", options[i]); // Print option

            if (i == highlight)
            {
                attroff(A_REVERSE); // Turn off highlight
            }
        }

        int key = getch();
        switch (key)
        {
        case KEY_UP:
            highlight = (highlight - 1 + n_options) % n_options; // Move up
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % n_options; // Move down
            break;
        case 10: // Enter key
            choice = highlight;
            printw("Chose\n");
            getchar();
            break;
        }

        if (choice < n_options && choice > -1)
        { // Exit option
            return choice;
        }
    }

    endwin();
    return 0;
}

char heroCharacters()
{
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();

    int choice = -1;
    int highlight = 0;
    const char *options[] = {"0", "1", "2", "3", "4", "5", "6"};
    int n_options = sizeof(options) / sizeof(options[0]);

    while (1)
    {
        clear();
        for (int i = 0; i < n_options; i++)
        {
            if (i == highlight)
            {
                attron(A_REVERSE); // Highlight the current option
            }
            mvprintw(i, 0, "%s", options[i]); // Print option

            if (i == highlight)
            {
                attroff(A_REVERSE); // Turn off highlight
            }
        }

        int key = getch();
        switch (key)
        {
        case KEY_UP:
            highlight = (highlight - 1 + n_options) % n_options; // Move up
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % n_options; // Move down
            break;
        case 10: // Enter key
            choice = highlight;
            printw("Chose\n");
            getchar();
            break;
        }

        if (choice < n_options && choice > -1)
        { // Exit option
            return options[choice][0];
        }
    }

    endwin();
    return 0;
}

void boardDisplayer()
{
    screen_setup();
    setlocale(LC_ALL, "");
    start_color();
    keypad(stdscr, TRUE); // Enable arrow keys

    init_pair(1, COLOR_YELLOW, COLOR_BLACK);  // Yellow text
    init_pair(2, COLOR_CYAN, COLOR_BLACK);    // Cyan text
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK); // Magenta text
    wchar_t trophy[] = L"\U0001F3C6";         // Trophy
    wchar_t medal1[] = L"\U0001F947";         // 1st place medal
    wchar_t medal2[] = L"\U0001F948";         // 2nd place medal
    wchar_t medal3[] = L"\U0001F949";         // 3rd place medal
    wchar_t star[] = L"\u2B50";               // Star

    int firstLine = 0;

    while (1)
    {
        clear();
        bunny();

        attron(A_REVERSE);
        mvwprintw(stdscr, 0, 0, "%s (YOU) ", board[myhero.user.rank - 1]);
        wprintw(stdscr, "%ls", star);
        attroff(A_REVERSE);

        for (int i = firstLine; i < MAP_HEIGHT - 1 + firstLine && board[i][0] != '\0'; i++)
        {
            char tempBoard[256];
            strcpy(tempBoard, board[i]);
            char *temp = strtok(tempBoard, ".");
            char *username = strtok(NULL, ":");

            user_data user;
            strcpy(user.username, username);
            user = readUserInfo(user);

            // Display user info
            mvprintw(i + 1 - firstLine, 0, "%s, Gold:%d, CountGames:%d, Experience:%d", board[i], user.gold, user.count_games, user.experience);

            // Display medals with colors for top 3 players
            if (i == 0)
            {
                attron(COLOR_PAIR(1));
                wprintw(stdscr, " (GOAT) %ls", medal1);
                attroff(COLOR_PAIR(1));
            }
            else if (i == 1)
            {
                attron(COLOR_PAIR(2));
                wprintw(stdscr, " (LEGEND) %ls", medal2);
                attroff(COLOR_PAIR(2));
            }
            else if (i == 2)
            {
                attron(COLOR_PAIR(3));
                wprintw(stdscr, " (PRO) %ls", medal3);
                attroff(COLOR_PAIR(3));
            }
        }

        refresh();

        // Input handling
        int c = getch();
        if (c == KEY_DOWN && board[firstLine + MAP_HEIGHT][0] != '\0')
        {
            firstLine++;
        }
        else if (c == KEY_UP && firstLine != 0)
        {
            firstLine--;
        }
        else if (c == 'q' || c == 'Q')
        {
            break;
        }
    }

    endwin();
}

int weaponDisplayer() {
    if (myhero.weaponIndex < 0)
        return -1;

    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    start_color();

    int choice = -1;
    int highlight = 0;

    while (1) {
        int j;
        clear();
        for (int i = 0; i < myhero.weaponIndex; i++) {
            if (i == highlight) {
                attron(A_REVERSE);  // Highlight current option
            }

            // Display weapon based on type
            switch (mygame.weapon[myhero.weapon[i]].type) {
                case 0:
                    mvwprintw(stdscr, i, 0, "Mace");
                    break;
                case 1:
                    mvwprintw(stdscr, i, 0, "Dagger");
                    break;
                case 2:
                    mvwprintw(stdscr, i, 0, "Wand");
                    break;
                case 3:
                    mvwprintw(stdscr, i, 0, "Arrow");
                    break;
                case 4:
                    mvwprintw(stdscr, i, 0, "Sword");
                    break;
            }

            if (i == highlight) {
                attroff(A_REVERSE);  // Turn off highlight
            }
            j = i + 1;
        }

        refresh();  // Refresh screen after printing

        int key = getch();
        switch (key) {
            case KEY_UP:
                highlight = (highlight - 1 + j) % j;  // Move up
                break;
            case KEY_DOWN:
                highlight = (highlight + 1) % j;  // Move down
                break;
            case 10:  // Enter key
                choice = highlight;
                break;
        }

        if (choice >= 0 && choice < j) {  // Valid choice
            endwin();
            return choice;
        }
    }

    endwin();  // Close ncurses
    return 0;
}

int spellDisplayer() {
    if (myhero.weaponIndex < 0)
        return -1;

    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    start_color();

    int choice = -1;
    int highlight = 0;

    while (1) {
        int j;
        clear();
        for (int i = 0; i < myhero.spellIndex; i++) {
            if (i == highlight) {
                attron(A_REVERSE);  // Highlight current option
            }

            // Display weapon based on type
            switch (mygame.weapon[myhero.weapon[i]].type) {
                case 0:
                    mvwprintw(stdscr, i, 0, "Health");
                    break;
                case 1:
                    mvwprintw(stdscr, i, 0, "Speed");
                    break;
                case 2:
                    mvwprintw(stdscr, i, 0, "Damage");
                    break;
            }

            if (i == highlight) {
                attroff(A_REVERSE);  // Turn off highlight
            }
            j = i + 1;
        }

        refresh();  // Refresh screen after printing

        int key = getch();
        switch (key) {
            case KEY_UP:
                highlight = (highlight - 1 + j) % j;  // Move up
                break;
            case KEY_DOWN:
                highlight = (highlight + 1) % j;  // Move down
                break;
            case 10:  // Enter key
                choice = highlight;
                break;
        }

        if (choice >= 0 && choice < j) {  // Valid choice
            endwin();
            return choice;
        }
    }

    endwin();  // Close ncurses
    return 0;
}
