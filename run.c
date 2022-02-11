#include <stdio.h>
#include <stdlib.h>

// Define tokens
#define POINTER_RIGHT 10
#define POINTER_LEFT 11
#define INCREMENT 12
#define DECREMENT 13
#define OUTPUT 14
#define INPUT 15
#define LEFT_BRACKET 16
#define RIGHT_BRACKET 17
#define ZERO_LOOP 18

// Integer List struct
typedef struct IntList {
    int* arr;
    size_t size;
    size_t used;
} IntList;

// Global variables
char nextChar;
int nextToken;
IntList tokenList;

// Function headers
void createList(IntList* list, int initialSize);
void appendList(IntList* list, int item);
void freeList(IntList* list);
void lex();
char* lookup(int token);
void printMem(IntList* list);

int main(int argc, char* argv[]) {
    // Check for argument length
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(2);
    }

    // Open file for reading
    char* filename = argv[1];
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "File cannot be opened\n");
        exit(2);
    }

    // Lexer =========================================

    // Default list of size 100
    createList(&tokenList, 100);

    // Iterate through input until EOF
    while ((nextChar = getc(fp)) != EOF) {
        lex();
    }

    // Add EOF char to end of tokenList
    appendList(&tokenList, EOF);

    // Optimization ==================================

    // Replace [+] and [-] with ZERO_LOOP { O(n) }
    IntList tempTokenList;
    createList(&tempTokenList, 100);
    size_t curr = 0;
    if (tokenList.used > 3) {
        while (curr + 2 < tokenList.used) {
            // Extract [+] and [-] and set to ZERO_LOOP
            // Check using A[curr], A[curr + 1], A[curr + 2]
            if (tokenList.arr[curr] == LEFT_BRACKET &&
                (tokenList.arr[curr + 1] == INCREMENT ||
                 tokenList.arr[curr + 1] == DECREMENT) &&
                tokenList.arr[curr + 2] == RIGHT_BRACKET) {
                appendList(&tempTokenList, ZERO_LOOP);

                // Skip the next 2 tokens
                curr += 3;
            } else {
                // If not a zero loop, simply add token
                appendList(&tempTokenList, tokenList.arr[curr++]);
            }
        }
    }

    // Add on the remaining token loops
    while (curr != tokenList.used) {
        appendList(&tempTokenList, tokenList.arr[curr++]);
    }

    // Set new token list with optimizations
    freeList(&tokenList);
    tokenList.arr = tempTokenList.arr;
    tokenList.size = tempTokenList.size;
    tokenList.used = tempTokenList.used;

    // DEBUGGING: Print the tokens
    // printf("Size: %ld, Used: %ld\n", tokenList.size, tokenList.used);
    // for (int i = 0; i < tokenList.used; i++) {
    //     printf("%d ", tokenList.arr[i]);
    // }
    // printf("\n");

    // Interpreter ===================================

    // Next token and pos refer to the code itself
    nextToken = 0;
    size_t pos = 0;
    int bracketCount = 0;

    // The mem list structure is for the actual BF program to use
    IntList mem;
    createList(&mem, 100);

    // Iterate until EOF
    while ((nextToken = tokenList.arr[pos++]) != EOF) {
        // DEBUGGING STATEMENTS
        // printMem(&mem);
        // printf("%s | %ld\n", lookup(nextToken), mem.used);
        switch (nextToken) {
            case POINTER_RIGHT: {
                if (mem.size == mem.used) {
                    mem.size *= 2;
                    mem.arr = realloc(mem.arr, mem.size * sizeof(int));
                }
                mem.used++;
                break;
            }
            case POINTER_LEFT: {
                if (mem.used == 0) {
                    fprintf(stderr,
                            "Unable to access negative memory addresses\n");
                    exit(2);
                }
                mem.used--;
                break;
            }
            case INCREMENT: {
                mem.arr[mem.used]++;
                break;
            }
            case DECREMENT: {
                mem.arr[mem.used]--;
                break;
            }
            case OUTPUT: {
                printf("%c", mem.arr[mem.used]);
                break;
            }
            case INPUT: {
                char in;
                scanf("%c", &in);
                mem.arr[mem.used] = in;
                break;
            }
            case ZERO_LOOP: {
                mem.arr[mem.used] = 0;
                break;
            }
            case LEFT_BRACKET: {
                if (mem.arr[mem.used] == 0) {
                    // printf("LOOP SKIP: %ld\n", mem.used);
                    for (bracketCount = 1; bracketCount > 0; pos++) {
                        if (tokenList.arr[pos] == EOF) {
                            fprintf(stderr, "Unmatched left bracket :(\n");
                            exit(2);
                        } else if (tokenList.arr[pos] == RIGHT_BRACKET) {
                            bracketCount--;
                        } else if (tokenList.arr[pos] == LEFT_BRACKET) {
                            bracketCount++;
                        }
                    }
                }
                break;
            }
            case RIGHT_BRACKET: {
                if (mem.arr[mem.used] != 0) {
                    // printf("LOOP BACK: %ld\n", mem.used);
                    for (bracketCount = 1, pos -= 2; bracketCount > 0; pos--) {
                        if (pos < 0) {
                            fprintf(stderr, "Unmatched left bracket :(\n");
                            exit(2);
                        } else if (tokenList.arr[pos] == LEFT_BRACKET) {
                            bracketCount--;
                        } else if (tokenList.arr[pos] == RIGHT_BRACKET) {
                            bracketCount++;
                        }
                    }
                    pos++;
                }
                break;
            }
        }
    }
}

// Create dynamic int list
void createList(IntList* list, int initialSize) {
    list->arr = (int*)calloc(initialSize, sizeof(int));
    list->size = initialSize;
    list->used = 0;
}

// Insert item into int list
void appendList(IntList* list, int item) {
    if (list->used == list->size) {
        list->size *= 2;
        list->arr = realloc(list->arr, list->size * sizeof(int));
    }
    list->arr[list->used++] = item;
}

// Free up int list space
void freeList(IntList* list) {
    free(list->arr);
    list->arr = NULL;
    list->size = 0;
    list->used = 0;
}

// Lex tokens
void lex() {
    switch (nextChar) {
        case '>':
            nextToken = POINTER_RIGHT;
            break;
        case '<':
            nextToken = POINTER_LEFT;
            break;
        case '+':
            nextToken = INCREMENT;
            break;
        case '-':
            nextToken = DECREMENT;
            break;
        case '.':
            nextToken = OUTPUT;
            break;
        case ',':
            nextToken = INPUT;
            break;
        case '[':
            nextToken = LEFT_BRACKET;
            break;
        case ']':
            nextToken = RIGHT_BRACKET;
            break;
        default:
            return;
    }

    appendList(&tokenList, nextToken);
}

// Debug using lookup table
char* lookup(int token) {
    switch (token) {
        case POINTER_LEFT:
            return "<";
        case POINTER_RIGHT:
            return ">";
        case INCREMENT:
            return "+";
        case DECREMENT:
            return "-";
        case OUTPUT:
            return ".";
        case INPUT:
            return ",";
        case LEFT_BRACKET:
            return "[";
        case RIGHT_BRACKET:
            return "]";
        case ZERO_LOOP:
            return "[+]";
        defualt:
            return "ERROR";
    }
}

void printMem(IntList* list) {
    printf("0   1   2   3   4   5   6\n");
    for (int i = 0; i < 7; i++) {
        printf("%-3d ", list->arr[i]);
    }
    printf("\n");
}