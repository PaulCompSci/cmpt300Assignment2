#include <unistd.h>
#include <stdlib.h>
#include <curses.h>

int main() {
    char buf[1024];  // Increase buffer size to accommodate more input
    int n = 0;
    char ch;  // A variable to store each character

    initscr();              // Initialize the curses mode
    cbreak();               // Enable cbreak mode, where no buffering is performed
    noecho();               // Do not echo input characters

    // Use curses functions to print to window
    printw("Type some text and press Enter: ");
    refresh();              // Refresh to update the screen

    // Read input from the user one character at a time
    while ((ch = getch()) != '\n' && n < sizeof(buf) - 1) {  // Stop at Enter key
        buf[n] = ch;
        n++;
    }
    buf[n] = '\0';  // Null-terminate the string

    // Write the read content to the screen using curses
    printw("You wrote: %s", buf);
    refresh();

    // Wait for user input before exiting
    printw("\nPress any key to exit");
    refresh();
    getch();                // Get a single character from the user

    endwin();               // End curses mode

    return 0;
}
