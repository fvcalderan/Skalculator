/* __                _     _
  / _|_   _____ __ _| | __| | ___ _ __ __ _ _ __
 | |_\ \ / / __/ _` | |/ _` |/ _ \ '__/ _` | '_ \
 |  _|\ V / (_| (_| | | (_| |  __/ | | (_| | | | |
 |_|   \_/ \___\__,_|_|\__,_|\___|_|  \__,_|_| |_|

BSD 3-Clause License
Copyright (c) 2024, Felipe V. Calderan
All rights reserved.
See the full license inside LICENSE.txt file */

// Import necessary libraries
#include <X11/Xlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define important constants
#define WIDTH 200 // Window width
#define HEIGHT 370 // Window height
#define BUTTON_WIDTH 50 // Button width
#define BUTTON_HEIGHT 50 // Button height
#define DISPLAY_LEN 256 // Maximum display length
#define STACK_SIZE 256 // Maximum stack size
#define MAX_TOKENS 1024 // Maximum number of non-interactive tokens
#define MAX_BUF_SIZE 4096 // Maximum size of input file

// Data structure to store tokenized strings
typedef struct {
    char** tokens;
    int token_count;
} Tokenized;

// X11 variables
Display* display;
Window window;
GC gc;
XEvent event;
int screen;

// State variables
char display_text[DISPLAY_LEN] = "0";
char display_text_aux[DISPLAY_LEN] = "0";
double stack[STACK_SIZE] = { 0 };
int head = 0;

// Keypad layout
const char* buttons[6][4] = {
    { "C", "AC", "POP", "SWAP" },
    { "%", "^", "1/x", "+/-" },
    { "7", "8", "9", "+" },
    { "4", "5", "6", "-" },
    { "1", "2", "3", "*" },
    { ".", "0", "ENTER", "/" },
};

// Draw the screen
void draw_screen()
{
    // Clear window
    XClearWindow(display, window);

    // Print stack
    for (int i = 2; i >= 0; i--) {

        // Only print if index exists
        if (head - i >= 0) {
            snprintf(
                display_text_aux, sizeof(display_text), "%g", stack[head - i]);

            // Otherwise default to 0
        } else {
            strcpy(display_text_aux, "0");
        }

        // Write to screen
        XDrawString(display, window, gc, 20, 45 - 15 * i, display_text_aux,
            strlen(display_text_aux));
    }

    // Write display to screen
    XDrawString(
        display, window, gc, 20, 60, display_text, strlen(display_text));

    // Draw buttons
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 4; ++j) {
            int x = j * BUTTON_WIDTH;
            int y = 70 + i * BUTTON_HEIGHT;
            size_t len = strlen(buttons[i][j]);
            XDrawRectangle(
                display, window, gc, x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
            XDrawString(display, window, gc, x + 20 - (len * 1.5), y + 30,
                buttons[i][j], len);
        }
    }
}

// Update current display state
void update_display(char* text)
{
    if (strcmp(display_text, "0") == 0) {
        strncpy(display_text, text, sizeof(display_text) - 1);
    } else {
        strncat(display_text, text,
            sizeof(display_text) - strlen(display_text) - 1);
    }
    draw_screen();
}

// Clear the stack
void clear_stack()
{
    for (int i = 0; i < STACK_SIZE; i++) {
        stack[i] = 0.0;
    }
}

// Push current display element to the stack
void push()
{
    head++;
    stack[head] = atof(display_text);
    strcpy(display_text, "0");
    draw_screen();
}

// Pop head of the stack
void pop()
{
    if (head > 0) {
        head--;
        snprintf(display_text, sizeof(display_text), "%g", stack[head + 1]);
    } else {
        clear_stack();
        snprintf(display_text, sizeof(display_text), "%g", stack[head + 1]);
    }
    draw_screen();
}

// Swap current display element with head of the stack
void swap()
{
    double currHead = stack[head];
    stack[head] = atof(display_text);
    snprintf(display_text, sizeof(display_text), "%g", currHead);
    draw_screen();
}

// Add decimal point
void decimal()
{
    // Only add decimal point if one isn't already present
    if (strchr(display_text, '.') == NULL) {
        // Find the length of the string
        size_t len = strlen(display_text);
        // Add a period at the end
        display_text[len] = '.';
        // Null-terminate the string
        display_text[len + 1] = '\0';
    }
    draw_screen();
}

// Compute operation using display and stack items
void compute_operation(char op)
{
    // Process operation
    double result = 0.0;
    switch (op) {
    case '+': // Addition
        result = stack[head] + atof(display_text);
        break;
    case '-': // Subtraction
        result = stack[head] - atof(display_text);
        break;
    case '*': // Multiplication
        result = stack[head] * atof(display_text);
        break;
    case '/': // Division
        result = stack[head] / atof(display_text);
        break;
    case '^': // Power
        result = pow(stack[head], atof(display_text));
        break;
    }

    // Move head, or clear the stack if there is nowhere to move
    if (head > 0) {
        head--;
    } else {
        clear_stack();
    }

    // Update the screen
    snprintf(display_text, sizeof(display_text), "%g", result);
    draw_screen();
}

// Calculate percentage based on the head of the stack and current display
void percentage()
{
    double result = stack[head] * (atof(display_text) / 100.0);
    snprintf(display_text, sizeof(display_text), "%g", result);
    draw_screen();
}

// Calcula 1/display
void inverse()
{
    double result = 1.0 / atof(display_text);
    snprintf(display_text, sizeof(display_text), "%g", result);
    draw_screen();
}

// Clear the display and the whole stack
void all_clear()
{
    strcpy(display_text, "0");
    for (int i = 0; i < STACK_SIZE; i++) {
        stack[i] = 0.0;
    }
    draw_screen();
}

// Clear the display
void clear()
{
    strcpy(display_text, "0");
    draw_screen();
}

// Switch between +display and -display
void plus_minus()
{

    // Ignore if the value is 0
    if (strcmp(display_text, "0") == 0) {
        return;
    }

    size_t length = strlen(display_text);

    // If the value is negative, remove the minus sign
    if (display_text[0] == '-') {
        for (int i = 0; i < length; i++) {
            display_text[i] = display_text[i + 1];
        }

        // Otherwise, add the minus sign
    } else {
        if (length + 1 < sizeof(display_text)) {
            for (int i = length; i >= 0; i--) {
                display_text[i + 1] = display_text[i];
            }
            display_text[0] = '-';
        }
    }

    // Update screen
    draw_screen();
}

// Process pressed button
void process_label(const char* label)
{
    if (strcmp(label, "C") == 0) {
        clear();
    } else if (strcmp(label, "AC") == 0) {
        all_clear();
    } else if (strcmp(label, "POP") == 0) {
        pop();
    } else if (strcmp(label, "SWAP") == 0) {
        swap();
    } else if (strcmp(label, "%") == 0) {
        percentage();
    } else if (strcmp(label, "1/x") == 0) {
        inverse();
    } else if (strcmp(label, "+/-") == 0) {
        plus_minus();
    } else if (strcmp(label, ".") == 0) {
        decimal();
    } else if (strcmp(label, "ENTER") == 0) {
        push();
    } else if (strcmp(label, "+") == 0 || strcmp(label, "-") == 0
        || strcmp(label, "*") == 0 || strcmp(label, "/") == 0
        || strcmp(label, "^") == 0) {
        compute_operation(label[0]);
    } else {
        update_display((char*)label);
    }
}

// Tokenize string
Tokenized tokenize(char* str)
{
    // Alocate necessary variables
    char** tokens = malloc(MAX_TOKENS * sizeof(char*));
    int token_count = 0;

    // Define the delimiters: space, tab, and newline
    const char* delimiters = " \t\n";

    // Split the string by the specified delimiters
    char* token = strtok(str, delimiters);
    while (token != NULL && token_count < MAX_TOKENS) {
        tokens[token_count++] = token;
        token = strtok(NULL, delimiters);
    }

    // return tokens
    return (Tokenized) { .tokens = tokens, .token_count = token_count };
}

// Load file and return buffer
char* load_file(char* filename)
{
    // Create buffer
    char* buffer = malloc(MAX_BUF_SIZE * sizeof(char));

    // Try to open requested file
    FILE* file = fopen(filename, "r");

    // File doesn't exist or is inaccessible
    if (file == NULL) {
        fprintf(stderr, "ERROR: Failed doesn't exist or is inaccessible.\n");
        exit(EXIT_FAILURE);
    }

    // Read the contents of the file into the buffer
    if (fgets(buffer, MAX_BUF_SIZE, file) == NULL) {
        fprintf(stderr, "ERROR: Failed to read file.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Close file and return buffer
    fclose(file);
    return buffer;
}

// Process arguments and return tokens (if any)
Tokenized process_args(int argc, char** argv)
{
    // If there is no arguments, just open Skalculator
    if (argc < 2) {
        return (Tokenized) { .tokens = NULL, .token_count = 0 };
    }

    // If there is one argument, and it's -h or --help, show usage
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        printf("Skalculator v1.0\n");
        printf("Usage: %s [-h, --help] [-f FILE_NAME, --file FILE_NAME]\n",
            argv[0]);

        exit(EXIT_SUCCESS);
    }

    // Read from file
    if (strcmp(argv[1], "-f") == 0 || strcmp(argv[1], "--file") == 0) {
        // File name is present
        if (argc > 2) {
            return tokenize(load_file(argv[2]));

            // File name is missing
        } else {
            fprintf(stderr, "ERROR: --file (-f) expects a file name.\n");
            exit(EXIT_FAILURE);
        }

        // Read from CLI
    } else {
        return tokenize(argv[1]);
    }
}

// Main function
int main(int argc, char** argv)
{
    // Get non-interactive tokens
    Tokenized tk = process_args(argc, argv);

    // Start display
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "ERROR: Cannot open display.\n");
        exit(EXIT_FAILURE);
    }

    // Set screen and window
    screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10,
        WIDTH, HEIGHT, 1, WhitePixel(display, screen),
        BlackPixel(display, screen));

    // Setup input
    XSelectInput(
        display, window, ExposureMask | KeyPressMask | ButtonPressMask);
    XMapWindow(display, window);

    // Create context
    gc = XCreateGC(display, window, 0, 0);

    // Set foreground color
    XSetForeground(display, gc, WhitePixel(display, screen));

    // Run non-interactive things
    if (tk.tokens != NULL) {
        for (int i = 0; i < tk.token_count; i++) {
            // If argument is a valid number, automatically press ENTER
            if (strcmp(tk.tokens[i], "+") != 0 && strcmp(tk.tokens[i], "-") != 0
                && strspn(tk.tokens[i], "0123456789e+-.")
                    == strlen(tk.tokens[i])) {
                process_label("ENTER");
            }
            process_label(tk.tokens[i]);
        }
    }

    // Loop
    while (1) {
        // Process next event
        XNextEvent(display, &event);

        // Redraw the screen
        if (event.type == Expose) {
            draw_screen();
        }

        // Check for mouse button press
        if (event.type == ButtonPress) {
            int x = event.xbutton.x;
            int y = event.xbutton.y;

            // Check for valid click position
            if (y > 50) {
                // Get row and column clicked
                int row = (y - 70) / BUTTON_HEIGHT;
                int col = x / BUTTON_WIDTH;

                // Check for valid row/col position
                if (row < 6 && col < 4) {
                    // Process button label
                    process_label(buttons[row][col]);
                }
            }
        }
    }

    // Close the display and the application
    XCloseDisplay(display);
    return EXIT_SUCCESS;
}
