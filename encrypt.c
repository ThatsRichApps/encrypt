/*
 * encrypt.c
 *
 *  Created on: Mar 6, 2017
 *      Author: RJHumphrey
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

void print_help() {
	printf("Usage:\n");
	printf("./encrypt <binary_file> <key1> <key2>\n");
}

void print_try_help() {
	printf("Try encrypt -h for help.\n");
}

char *validate_key(char *key) {

	// key should be 10 characters with no duplicates
	// convert to uppercase

	char *validkey;
	validkey = malloc(sizeof(char) * 11);
	validkey[0] = '\0';
	int keychars = 0;

	for(int i = 0; key[i] != '\0'; i++) {
		// is ch already in validkey?
		// ** make sure strncat puts the \0 on the end
		if (strchr(validkey, key[i])==NULL){
			strncat(validkey, &key[i], 1);
			keychars++;
		}

		if (keychars == 10){
			break;
		}
	}


	// Key must be 10 characters
	if (keychars < 10) {
		return NULL;
	}

	for(char* c=validkey; (*c=toupper(*c)); ++c) ;

	//printf ("validkey = %s - %d chars\n", validkey, keychars);

	return validkey;
}

char *encrypt_vigenere (char *buff, char *key) {

	char *encrypted;

	size_t bufflength = strlen(buff);

	encrypted =(char *)malloc(bufflength);

	// Do a bitwise vigenere encryption with key
	// key = 10 character array

	printf ("bufflength: %li\n", bufflength );

	int i = 0;

	while (i < bufflength) {

		for (int j = 0; key[j] != '\0'; j++) {

			//printf ("encrypt %c with char = %c \n", buff[i], key[j]);

			if (i == bufflength) {
				break;
			}

			encrypted[i] = buff[i] ^ key[j];

			i++;
		}

	}

	encrypted[bufflength] = '\0';

	return encrypted;

}


char *columnar_transposition (char *buff, char *key) {

	char *transposed;

	int buffcharlen = strlen(buff);

	size_t bufflength = strlen(buff) * 8;

	transposed =(char *)malloc(bufflength);
	transposed[0] = '\0';

	// we know the key is 10 characters, divide
	// the input into 10 columns, then concat the columns
	// based upon the key

	size_t keysize = strlen(key);

	int colsize = bufflength / keysize;
	int remainder = bufflength % keysize;

	if (remainder != 0) {
		printf("the remainder %d is NOT zero!\n", remainder);
		colsize++;
	}

	printf ("bitlength: %li\n", bufflength );
	printf ("keysize: %li\n", keysize);
	printf ("colsize: %i\n", colsize);
	printf ("remainder: %i\n", remainder);

	int *columns[keysize];

	for (int k = 0; k < keysize; k++) {
		columns[k] = (int *)malloc(colsize);
	}

	int i = 0;
	int bitlocation = 0;
	int column_index = 0;
	int column = 0;

	// This time we need to read buffer one bit at a time
	// buff[i] is stored as bytes (8 bits)

	// Split buff[i] into bits, and then populate each of the columns

	// can't stop at '\0' because a binary file might not care about null bit = 0000 0000

	while (i < buffcharlen) {

		for (int bitpos = 7; bitpos >= 0; bitpos--) {

			column = bitlocation % keysize;

			int bit = ((buff[i] >> bitpos) & 1);

			//printf ("bitpos %i - %i\n", bitpos, bit);
			//printf ("put %i position col = %i, index = %i \n", bit, column, column_index);
			printf ("%i ", bit);

			columns[column][column_index] = bit;

			if (column == (keysize - 1)) {
				column_index++;
				printf("\n");
			}

			bitlocation++;

		}

		i++;

	}

	// now read off the bits according to the key order
	// create a map - assume key is A-Z

	int column_map[keysize];

	int map = 0;

	printf("\n");
	printf("key is %s\n", key);

	printf ("trans = ");

	for (char alpha = 'A'; alpha <= 'Z'; alpha++) {

		for (int key_i = 0; key_i < keysize; key_i++) {

			if (key[key_i] == alpha) {
				//printf("found %c at index %i\n", alpha, key_i);
				printf(" %i ", key_i);
				column_map[map] = key_i;
				map++;
				break;

			}

		}

	}
	printf ("\n");

	/*
	for (int i = 0; i < keysize; i++) {
		printf(" %i ", column_map[i]);
	}
	printf ("\n");
	*/

	// now we just have to read each bit in the columns, according to the
	// column map, and set each of the bits in an 8bit char

	char ch = 0;
	int location = 0;
	int bitposition = 0;
	int chlocation = 0;

	for (int i = 0; i< keysize; i++) {

		int map = column_map[i];
		//printf ("reading from column %i \n", map);

		for (int j = 0; j < colsize; j++) {
			bitposition = location % 8;

			printf ("%i", columns[map][j]);

			ch |= columns[map][j];

			if (bitposition == 7) {
				//printf (" - ");
				//printf ("appending %c to transposed: in position %i %s\n", ch, chlocation, transposed);
				printf ("\n");
				transposed[chlocation] = ch;
				chlocation++;
				transposed[chlocation] = '\0';
				ch = 0;
			} else {
				ch <<= 1;
			}
			location++;
		}

	}
	printf("\n");

	return transposed;

}


int main (int argc, char **argv) {

	char *plaintext_file;
	char *key1;
	char *key2;
	int c;

	if (argc == 1) {
			printf("Error: Provide binary filename and keys.\n");
			print_try_help();
			exit(0);
		}

		while ((c = getopt (argc, argv, "h")) != -1) {
			// Option argument
			switch (c) {
			case 'h':
				print_help();
				exit(1);
			default:
				break;
			}
		}

		// plaintext file should be non option argument
		plaintext_file = argv[optind];

		if (!plaintext_file) {
			printf("Error: Provide the name of the file to encrypt.\n");
			print_try_help();
			exit(0);
		}

		optind++;

		// key1 should be non option argument
		key1 = argv[optind];

		if (!key1) {
			printf("Error: Provide key1.\n");
			print_try_help();
			exit(0);
		}
		optind++;

		// key2 file should be non option argument
		key2 = argv[optind];

		if (!key2) {
			printf("Error: Provide key2.\n");
			print_try_help();
			exit(0);
		}

		printf ("file: %s\n", plaintext_file);
		//printf ("key1: %s\n", key1);
		//printf ("key2: %s\n", key2);

		// test keys for appropriate length
		char *k1 = validate_key(key1);
		char *k2 = validate_key(key2);

		if (k1 == NULL) {
			printf("Key 1 is too short, must be 10 unique characters (A-Z).\n");
			print_try_help();
			exit(0);
		}

		if (k2 == NULL) {
			printf("Key 2 is too short, must be 10 unique characters (A-Z).\n");
			print_try_help();
			exit(0);
		}

		printf ("k1 = %s\n", k1);
		printf ("k2 = %s\n", k2);

		// Read from the binary file into a buffer
		char *buffer;
		unsigned long fileLen;

		//Open the file
		FILE *fp = fopen(plaintext_file,"rb");
		if(!fp) {
			printf ("Unable to read file");
			return 0;
		}

		//Get file length
		fseek(fp, 0, SEEK_END);
		fileLen=ftell(fp);
		fseek(fp, 0, SEEK_SET);

		//Allocate memory
		buffer=(char *)malloc(fileLen+1);
		if (!buffer) {
			printf("Memory error!");
	        fclose(fp);
			return 0;
		}

		//Read file contents into buffer
		fread(buffer, fileLen, 1, fp);
		fclose(fp);

		buffer[fileLen] = '\0';

		printf ("Plaintext from file: |%s|\n", buffer);

		// encrypt the buffer with key1
		char *first_encryption = encrypt_vigenere(buffer, k1);

		free(buffer);

		//printf ("first encryption = %s\n", first_encryption);

		// read temp file, do columnar transposition with k1

		// do columnar transposition with k1
		char *first_transpo = columnar_transposition(first_encryption, k1);
		free(first_encryption);

		return 0;

		char *second_transpo = columnar_transposition(first_transpo, k2);
		free(first_transpo);

		// write to output file
		char *output_filename = "encrypted";
		FILE *wp;
		wp = fopen(output_filename,"wb");
		fwrite(second_transpo,fileLen,1,wp);
		fclose(wp);

}


