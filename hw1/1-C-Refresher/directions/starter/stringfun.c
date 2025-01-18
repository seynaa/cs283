#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
void reverse_string(char *, int);
void word_print(char *);

void reverse_string(char *buff, int len) {
    int start = 0;
    int end = len - 1;
    while (start < end) {
        char temp = buff[start];
        buff[start] = buff[end];
        buff[end] = temp;
        start++;
        end--;
    }
}

void word_print(char *buff) {
    int len = strlen(buff);
    int word_count = 0;
    int word_length = 0;

    for (int i = 0; i <= len; i++) {
        if (!isspace(buff[i]) && buff[i] != '\0') {
            word_length++;  
        } else if (word_length > 0) {
            word_count++;  
            printf("%d. ", word_count);
            for (int j = i - word_length; j < i; j++) {
                putchar(buff[j]);
            }
            printf(" (%d)\n", word_length);
            word_length = 0; 
        }
    }
}



int setup_buff(char *buff, char *user_str, int len){
    int str_len = strlen(user_str);
    if (str_len > len) {
        fprintf(stderr, "Error: Input string is too large for the buffer.\n");
        return -1; 
    }

    strncpy(buff, user_str, len);

    if (str_len < len) {
        memset(buff + str_len, '\0', len - str_len);
    }
    return str_len; 
}
    

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    int word_count = 0;
    int in_word = 0;

    for (int i = 0; i < str_len; i++) {
        if (!isspace(buff[i])) {
            if (!in_word) {
                word_count++;  
                in_word = 1;
            }
        } else {
            
            in_word = 0;
        }
    }

    return word_count;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING

    //if arv[1] does not exist it would mean that there would be no way to 
    //reference it, which would lead to a crash. It makes sure that the first argument
    //is a valid option. 
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING

    //if there is not at least 3 arguments, the program will not run because 
    //the string to process would be missing. The if statement checks that the program
    //has all the required inputs
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3

    buff = (char *)malloc(BUFFER_SZ);
    if (!buff) { 
        fprintf(stderr, "Memory allocation failed\n");
        exit(99);
}


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        free(buff);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options

        case 'r':
            reverse_string(buff, user_str_len);
            printf("Reversed String: %s\n", buff);
            break;

        case 'w':
    
            word_print(buff);  
            break;

        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE

//providing both pointer and the length is good pratice because it helps avoid butter overflows,
//by passing the length the function can tell if the buffer size has been exceeded and will work to 
//fix that. 