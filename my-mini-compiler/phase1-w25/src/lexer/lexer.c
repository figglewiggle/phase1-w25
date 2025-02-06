/* lexer.c */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../../include/tokens.h"

// Line tracking
static int current_line = 1;
static char last_token_type = 'x'; // For checking consecutive operators

/* Print error messages for lexical errors */
void print_error(ErrorType error, int line, const char *lexeme) {
    printf("Lexical Error at line %d: ", line);
    switch (error) {
        case ERROR_INVALID_CHAR:
            printf("Invalid character '%s'\n", lexeme);
            break;
        case ERROR_INVALID_NUMBER:
            printf("Invalid number format\n");
            break;
        case ERROR_CONSECUTIVE_OPERATORS:
            printf("Consecutive operators not allowed\n");
            break;
        case ERROR_INVALID_OPERATOR:
            printf("Invalid operator '%s'\n", lexeme);
            break;
        default:
            printf("Unknown error\n");
    }
}

/* Print token information
 *
 *  TODO Update your printing function accordingly
 */

void print_token(Token token) {
    if (token.error != ERROR_NONE) {
        print_error(token.error, token.line, token.lexeme);
        return;
    }

    printf("Token: ");
    switch (token.type) {
        case TOKEN_NUMBER:
            printf("NUMBER");
            break;
        case TOKEN_OPERATOR:
            printf("OPERATOR");
            break;
        case TOKEN_EOF:
            printf("EOF");
            break;
        case TOKEN_IF:
            printf("IF");
            break;
        case TOKEN_REPEAT:
            printf("REPEAT");
            break;
        case TOKEN_UNTIL:
            printf("UNTIL");
            break;
        case TOKEN_IDENTIFIER:
            printf("IDENTIFIER");
            break;
        case TOKEN_DELIMITER:
            printf("DELIMITER");
            break;
        default:
            printf("UNKNOWN");
    }
    printf(" | Lexeme: '%s' | Line: %d\n",
           token.lexeme, token.line);
}


int is_operator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '='|| c == '!'|| 
            c == '&' || c == '|' || c == '<' || c == '>' || c == '%'|| c == '=' );
}

int is_delimiter(char c) {
    return (c == '(' || c == ')' || c == '{' || c == '}' || c == '['|| c == ']' ||
            c == ',' || c == ';');
}

/* Get next token from input */
Token get_next_token(const char *input, int *pos) {
    Token token = {TOKEN_ERROR, "", current_line, ERROR_NONE};
    char c;

    // Skip whitespace and track line numbers
    while ((c = input[*pos]) != '\0' && (c == ' ' || c == '\n' || c == '\t')) {
        if (c == '\n') {
            current_line++;
        }
        (*pos)++;
    }

    if (input[*pos] == '\0') {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }

    c = input[*pos];

    // TODO: Add comment handling here

    if ((c == '/') && (input[*pos + 1] == '/')){ // Inline comment

        int all_clear = 0;
        while (all_clear == 0){

            // Get to end of line
            (*pos)++; // second /
            do {
                (*pos)++;
                c = input[*pos];
            } while ((c != '\n') && (c != '\0'));

            // Filter out any whitespace on subsequent lines
            while ((c = input[*pos]) != '\0' && (c == ' ' || c == '\n' || c == '\t')) {
                if (c == '\n') {
                current_line++;
                }
                (*pos)++;
            }

            // Did the file end after a comment and blankspace?
            if (input[*pos] == '\0') {
                token.type = TOKEN_EOF;
                strcpy(token.lexeme, "EOF");
                return token;
            }

            // Did we hit a new inline comment?
            if ((c != '/') || (input[*pos + 1] != '/')){ // If not, all clear!
                all_clear++;
            }
        }
    }


    // Handle operators

    if (is_operator(c)){
        int i = 0;
        token.lexeme[i++] = c; //store character in lexeme
        (*pos)++; // go to next character
        char c_next = input[*pos]; // get next character

        if (c == '<') {
            // Check for shift operator << or the <= operator
            if (c_next == '<') {
                // Detected <<
                token.lexeme[i++] = c_next;
                (*pos)++;
            } else if (c_next == '=') {
                // Detected <=
                token.lexeme[i++] = c_next;
                (*pos)++;
            }
        } else if (c == '>') {
            // Check for shift operator >> or the >= operator
            if (c_next == '>') {
                // Detected >>
                token.lexeme[i++] = c_next;
                (*pos)++;
            } else if (c_next == '=') {
                // Detected >=
                token.lexeme[i++] = c_next;
                (*pos)++;
            }
        } else if (c == '=' || c == '!') {
            // For = and !, only the equality operator is allowed (== or !=)
            if (c_next == '=') {
                token.lexeme[i++] = c_next;
                (*pos)++;
            }
        }

        else if (c == '&'){
            if (c_next == '&'){ // if next character is &, then we have logical AND
                token.lexeme[i++] = c_next; // store second character in lexeme
                (*pos)++; // go to next character
            }
            else{
                // if next character is not &, then we have an invalid operator
                // don't move to next character
                token.type = TOKEN_ERROR;
                token.error = ERROR_INVALID_OPERATOR;
                last_token_type = 'x'; // reset last_token_type
                token.lexeme[i] = '\0';
                return token;
            }
        }

        else if (c == '|'){
            if (c_next == '|'){ // if next character is |, then we have logical OR
                token.lexeme[i++] = c_next; // store second character in lexeme
                (*pos)++; // go to next character
            }
            else{
                // if next character is not |, then we have an invalid operator
                // don't move to next character
                token.type = TOKEN_ERROR;
                token.error = ERROR_INVALID_OPERATOR;
                last_token_type = 'x'; // reset last_token_type
                token.lexeme[i] = '\0';
                return token;
            }
        }

        token.type = TOKEN_OPERATOR;
        token.lexeme[i] = '\0';
        
        if (last_token_type == 'o') {
            // Check for consecutive operators
            token.error = ERROR_CONSECUTIVE_OPERATORS;
            return token;
        }

        last_token_type = 'o';
        return token;

    }

    last_token_type = 'x'; // reset last_token_type


    // Handle numbers
    if (isdigit(c)) {
        int i = 0;
        do {
            token.lexeme[i++] = c;
            (*pos)++;
            c = input[*pos];
        } while (isdigit(c) && i < sizeof(token.lexeme) - 1);

        token.lexeme[i] = '\0';
        token.type = TOKEN_NUMBER;
        return token;
    }

    // TODO: Add keyword and identifier handling here
    // Hint: You'll have to add support for keywords and identifiers, and then string literals
     if (isalpha(c)) {  // checks if the first letter of the keyword/identifier is a letter (which it should be)
        int i = 0; 
        do {
            token.lexeme[i++] = c;
            (*pos)++;
            c = input[*pos];
        } while ((isalnum(c) || c == '_') && i < sizeof(token.lexeme) - 1); // keeps going as long as the next token is a letter or a number or an underscore, and stops at end of token regardless
        token.lexeme[i] = '\0'; // makes the token into a string by adding \0
        if (strcmp(token.lexeme, "if") == 0) { // keyword checking is done here
            token.type = TOKEN_IF;
        } else if (strcmp(token.lexeme, "repeat") == 0) {
            token.type = TOKEN_REPEAT;
        } else if (strcmp(token.lexeme, "until") == 0) {
            token.type = TOKEN_UNTIL;
        } else {
            token.type = TOKEN_IDENTIFIER; // if it is not a keyword, it is an identifier
        }
        return token;
    }


    // String literal handling
    if (c == '\"') {
        int i = 0;
        char c_prev; // edge case: \" escape. Need to keep track of previous char
        do {
            token.lexeme[i++] = c;
            (*pos)++;
            c_prev = c;
            c = input[*pos];

            // String doesn't get closed
            if ((c == '\n') or (c == '\0')) {
                token.type = ERROR_OPEN_STR;
                strcpy(token.lexeme, "OPEN_STR");
                return token;
            }
            
        } while (!((c == '\"') && (c_prev != '\\')) && i < sizeof(token.lexeme) - 1);

        // Complete the token
        token.lexeme[i] = '\0';
        token.type = TOKEN_LITERAL;
        return token;
    }

    // TODO: Add delimiter handling here
    if (is_delimiter(c)) {
        token.type = TOKEN_DELIMITER;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        (*pos)++;
        return token;
    }

    // Handle invalid characters
    token.error = ERROR_INVALID_CHAR;
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    (*pos)++;
    return token;
}


// This is a basic lexer that handles numbers (e.g., "123", "456"), basic operators (+ and -), consecutive operator errors, whitespace and newlines, with simple line tracking for error reporting.

int main() {
    const char *input = "123 + 456 - 789\n1 ++ 2 != 34 && 3 && = 4 &| 5 << & 3 { 0 + 1; } [1, 2, 3] test();"; // Test with multi-line input
    int position = 0;
    Token token;

    printf("Analyzing input:\n%s\n\n", input);

    do {
        token = get_next_token(input, &position);
        print_token(token);
    } while (token.type != TOKEN_EOF);

    return 0;
}
