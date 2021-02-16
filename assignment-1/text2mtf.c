#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define max_words 400
#define max_word_len 20
#define max_lines 100
#define max_line_len 80
#define max_filename_len 25

/*
 * Initializing global variables
 */
int num_words = 0;
int num_lines = 0;
char lines[max_lines][max_line_len];
char words[max_words][max_word_len];
char dictionary[max_words][max_word_len];

void tokenize_line(char *);
void construct_out(char *, char *);
void clear_string(int);

int main(int argc, char *argv[]) {
	if(argc<2) {
		fprintf(stderr, "Error. No filename detected.\n");
		exit(1);
	}

	/*
	 * Reading and constructing infile name
 	 */
	int l = strlen(argv[1])+1;
	char filename[l];
	strncpy(filename, argv[1], l);

	/*
	 * Constructing outfile name
	 */ 
	char outfilename[l];
	construct_out(filename, outfilename);

	/*
	 * Opening files
	 */ 
	FILE *infile = fopen(filename, "r");
	FILE *outfile = fopen(outfilename, "w");

	if(infile==NULL) {		
		fprintf(stderr, "Unable to open %s\n", filename);
	}

	/*
	 * Magic number sequence for mtf
	 */
	fputc(0xba, outfile);
	fputc(0x5e, outfile);
	fputc(0xba, outfile);
	fputc(0x11, outfile);

	char buffer[max_line_len];

	/*
	 * Reading infile line-by-line
	 */ 
	while(fgets(buffer, max_line_len, infile) > 0) {
		strncpy(lines[num_lines], buffer, max_line_len);
		num_lines++;
	}
	
	char ch, temp[max_word_len];
	int i=0, j=0, k=0, dc=0, t=0, c=0, pos=0, m=0;
	while(i<num_lines) {
		/*
		 * Empty line handling
		 */ 
		if(lines[i][0]=='\n') {
			fputc(10, outfile);
			i++;
			continue;
		}
		tokenize_line(lines[i]);	
		while(j<num_words) {
			while(k<num_words) {
				if((strncmp(words[j], dictionary[k], max_word_len))==0) {
					/*
					 * Old word
					 */ 
					pos=dc-k;
					fputc(pos+128, outfile);

					/*
					 * Rearranging dictionary
					 */ 
					for(int i=0; i<max_word_len; i++) {
						temp[i]=0;
					}
					strncpy(temp, dictionary[k], strlen(dictionary[k]));
					while(c<pos) {
						clear_string(c+k);
						strncpy(dictionary[c+k], dictionary[c+k+1], strlen(dictionary[c+k+1]));
						c++;
					}
					strncpy(dictionary[dc-1], temp, strlen(temp));

					c=0;
					t=1;
					k=0;
					break;
				}
				k++;
			}
			k=0;
			if(t==0) {
				/*
				 * New word
				 */ 
				strncpy(dictionary[dc], words[j], strlen(words[j]));
				dc++;
				fputc(dc+128, outfile);

				while(m<strlen(words[j])) {
					ch=words[j][m];
					fputc(ch, outfile);
					m++;
				}
				m=0;
			}
			t=0;
			j++;
		}
		i++;
		fputc(10, outfile);
	}
	/*
	 * Closing files
	 */ 
	fclose(infile);
	fclose(outfile);	
	exit(0);
}

/*
 * Breaking up line into words and removing the '\n' at the end of the line for easier word handing
 * Code by Zastre. Taken from lecture slides
 */
void tokenize_line(char *input_line) { 
	char *t;

	t = strtok(input_line, " ");
	while(t && num_words < max_words) {
		strncpy(words[num_words], t, max_word_len);
		num_words++;
		t = strtok(NULL, " ");
	}
	/*
	 * Following line removes the '\n' character at the end of the last word of each line
	 */ 
	words[num_words-1][strlen(words[num_words-1])-1]=0;
	return;
}

/*
 * Constructs outfile name
 */
void construct_out(char *filename, char *outfilename) {
	int i;

	for(i=0; filename[i]!='.'; i++) {
		outfilename[i]=filename[i];
	}
	strncat(outfilename, ".mtf", 5);

	return;
}

/*
 * Clears string in array with index, i
 */
void clear_string(int n) {
	int i;

	for(i=0; i<max_word_len; i++) {
		dictionary[n][i]=0;
	}
}
