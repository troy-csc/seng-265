#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char  magic_number_1[] = {0xBA, 0x5E, 0xBA, 0x11, 0x00};
char  magic_number_2[] = {0xBA, 0x5E, 0xBA, 0x12, 0x00};

// Node structure for linked list
typedef struct Node Node;
struct Node {
    char *word;
    int index;
    Node *next;
};

// String structure consisting of char array, dynamic
struct String {
    int nval;
    int max;
    char *wrd;
} word; // Global variable, word

void check_magic_numbers(FILE *);
char *string_duplicator(char *);
int findnum(int, FILE *);
int add(char);
int del(char);
void write_pos(Node *, int, FILE *, int);
void *emalloc(size_t);
Node *newnode(char *, int);
void print_word(char *, FILE *);
Node *move_to_top(Node *, int, int);
Node *append(Node *, Node *);
void freeall(Node *);
Node *delete(Node *, char *);

/*
 * The encode() function may remain unchanged for A#4.
 */

int encode(FILE *input, FILE *output) {
    return 0;
}

int decode(FILE *input, FILE *output) {
    // Check for magic numbers
    check_magic_numbers(input);
    
    // Initalizing variables
    int ch=0, status=1, num, num_words=0, pos, wc=0;
    Node *dictionary=NULL;

    // Infinite loop
    for(;;) {
        // Newline handling
        if(ch=='\n') {
            fputc('\n', output);
            status=1;
        }

        // Read next char if last word was followed by \n
        if(status) {
            ch=fgetc(input);
            status=0;
        }

        // Break at the end of file
        if(ch==EOF) break;

        // Check if byte is not a letter character
        if(ch>128) {
            // num stores decoded int of char read by file
            num=findnum(ch, input);
            
            // Reads next byte to look ahead for \n
            ch=fgetc(input);

            // Check if num has been encountered before i.e. old word
            if(num<=num_words) {
                pos=num_words-num;
                write_pos(dictionary, pos, output, ch);
                dictionary=move_to_top(dictionary, pos, num_words);
                continue;
            }

            // Loops through characters until a number or \n is encountered and stores it in word
            while(ch<128) {
                add(ch);
                wc++;
                ch=fgetc(input);
                if(ch=='\n') {
                    status=1;
                    break;
                }
            }
            wc=0;

            // Adds new word to dictionary and prints it
            Node *temp=newnode(word.wrd, num_words);
            dictionary=append(dictionary, temp);
            num_words++;
            write_pos(dictionary, num_words-1, output, ch); // Print function
            word.wrd=NULL; // Reset word
        }
    }

    free(word.wrd);
    freeall(dictionary);
    return 0;
}

// Function to check start of file for either of the precious magical number sequences
void check_magic_numbers(FILE *input) {
    int i;
    char ch;
    for(i=0; i<4; i++) {
        ch=fgetc(input);
        if(!(ch==magic_number_1[i] || ch==magic_number_2[i])) {
            fprintf(stderr, "Magic numbers not detected.");
            exit(1);
        }
    }
}

// Returns decoded value of char based on encoding scheme specified in the problem description
int findnum(int ch, FILE *input) {
    int num;
    num=ch-128;
    if(num==121) {
        ch=fgetc(input);
        num=ch+121;
    }
    else if(num==122) {
        ch=fgetc(input);
        num=(ch*256)+376;
        ch=fgetc(input);
        num+=(ch%256);
    }
    return num;
}

// Very slightly modified version of addname() function taken from 06_c_dynamic_memory.pdf on connex.csc.uvic.ca SENG265 Resources Slide 27
int add(char chr) {
    char *str;
    if(word.wrd==NULL) {
        word.wrd=(char *)malloc(1*sizeof(char));
        if(word.wrd==NULL) {
            return -1;
        }
        word.max=1;
        word.nval=0;
    }
    else if(word.nval>=word.max) {
        str=(char *)realloc(word.wrd, (2*word.max)*sizeof(char));
        if(str==NULL) {
            return -1;
        }
        word.max=2*word.max;
        word.wrd=str;
    }
    word.wrd[word.nval]=chr;
    return word.nval++;
}

// Very slightly modified version of delname() function taken from 06_c_dynamic_memory.pdf on connex.csc.uvic.ca SENG265 Resources Slide 29
int del(char chr) {
    int i;
    for(i=0; i<word.nval; i++) {
        if(word.wrd[i]==chr) {
            memmove(word.wrd+i, word.wrd+i+1, (word.nval-(i+1))*sizeof(char));
            word.nval--;
            return 1;
        }
    }
    return 0;
}

// string_duplicator() taken from 06_c_dynamic_memory.pdf on connex.csc.uvic.ca SENG265 resources Slide 30
char *string_duplicator(char *input) {
    char *copy;
    assert(input!=NULL);
    copy=(char *)emalloc(sizeof(char)*strlen(input)+1);
    strncpy(copy, input, strlen(input)+1);
    return copy;
}

// Function to print word corresponding to position, pos
void write_pos(Node *dictionary, int pos, FILE *output, int ch) {
    Node *p;
    char *word;
    for(p=dictionary; p!=NULL; p=p->next) {
        if(p->index==pos) {
            word=p->word;
            break;
        }
    }
    if(ch=='\n') {
        print_word(word, output);
    }
    else {
        print_word(word, output);
        fputc(' ', output);
    }
}

// Simple char array print function
void print_word(char *word, FILE *output) {
    int i;
    for(i=0; i<strlen(word); i++) {
        fputc(word[i], output);
    }
}

// Moves node corresponding to position, pos to the top of the dictionary
Node *move_to_top(Node *dictionary, int pos, int num_words) {
    char *word;
    Node *p;
    for(p=dictionary; p!=NULL; p=p->next) {
        if(p->index==pos) {
            word=p->word;
            break;
        }
    }
    dictionary=delete(dictionary, word);
    num_words--;
    int i=0;
    for(p=dictionary; p!=NULL; p=p->next) {
        p->index=i;
        i++;
    }
    Node *temp=newnode(word, num_words);
    dictionary=append(dictionary, temp);
    num_words++;
    return dictionary;
}

// emalloc() function taken from 06_c_dynamic_memory.pdf on connex.csc.uvic.ca SENG 265 resources Slide 38
void *emalloc(size_t n) {
    void *m;
    m=malloc(n);
    if(m==NULL) {
        fprintf(stderr, "malloc of %u bytes failed", n);
        exit(1);
    }
    return m;
}

// newnode() function taken from 06_c_dynamic_memory.pdf on connex.csc.uvic.ca SENG265 Resources Slide 39 
Node *newnode(char *word, int index) {
    Node *new;
    new=(Node *)emalloc(sizeof(Node));
    new->word=word;
    new->index=index;
    new->next=NULL;
    return new;
}

// append function taken from addend() function from 06_c_dynamic_memory.pdf on connex.csc.uvic.ca SENG265 Resources Slide 41
Node *append(Node *list, Node *new) {
    Node *p;
    if(list==NULL) {
        return new;
    }
    for(p=list; p->next != NULL; p=p->next);
    p->next=new;
    return list;
}

// delete() taken from delitem() function from 06_c_dynamic_memory.pdf on connex.csc.uvic.ca SENG265 Resources Slide 50
Node *delete(Node *list, char *word) {
    Node *curr, *prev;
    prev=NULL;
    for(curr=list; curr!=NULL; curr=curr->next) {
        if(strcmp(word, curr->word)==0) {
            if(prev==NULL) {
                list=curr->next;
            }
            else {
                prev->next=curr->next;
            }
            free(curr);
            return list;
        }
        prev=curr;
    }
    fprintf(stderr, "node %s not in list", word);
    exit(1);
}

// freeall() function taken from 06_c_dynamic_memory.pdf on connex.csc.uvic.ca SENG265 Resources Slide 48
void freeall(Node *list) {
    Node *next;
    for(; list!=NULL; list=next) {
        next=list->next;
        free(list);
    }
}
