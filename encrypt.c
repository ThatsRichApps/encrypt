/*
 * encrypt.c
 *
 *  Created on: Mar 6, 2017
 *      Author: Richard Humphrey
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

const int KEYWORDSIZE = 10;

void print_help() {
	printf("Usage:\n");
	printf("./encrypt <binary_file> <key1> <key2>\n");
}

void print_try_help() {
	printf("Try encrypt -h for help.\n");
}

char *validate_key(char *key) {

	// key should be KEYWORDSIZE characters with no duplicates
	// and only alphabetical

	// convert to uppercase
	for(char* c = key; (*c=toupper(*c)); ++c) ;

	char *validkey;
	validkey = malloc(sizeof(char) * (KEYWORDSIZE + 1));

	// in order to use the C str functions, it must have a null terminator
	validkey[0] = '\0';

	// require the keyword to be part of the uppercase alphabet
	char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0";

	int unique_characters = 0;

	for(int i = 0; key[i] != '\0'; i++) {
		// is ch already in validkey? ch in alphabet?
		if ((strchr(validkey, key[i]) == NULL) && (strchr(alphabet, key[i]) != NULL)){
			strncat(validkey, &key[i], 1);
			unique_characters++;
		}

		if (unique_characters == KEYWORDSIZE){
			break;
		}
	}


	// Key must be KEYWORDSIZE characters
	if (unique_characters < KEYWORDSIZE) {
		return NULL;
	}

	//printf ("validkey = %s - %d chars\n", validkey, unique_characters);

	return validkey;
}


int *encrypt_map (char *key) {

	// Given the key, return an array of integers with the
	// position of each letter for the transposition encryption
	// example: key = BADFECIHJG ; order = 1 0 5 2 4 3 9 7 6 8

	// this does not give the order of the characters in the keyword
	// but rather the order of the columns to be read in the transpo

	int *key_map;
	key_map = (int *)malloc(sizeof(int) * KEYWORDSIZE);

	int map = 0;

	//printf("key is %s\n", key);
	//printf ("trans = ");

	for (char alpha = 'A'; alpha <= 'Z'; alpha++) {

		for (int key_i = 0; key_i < KEYWORDSIZE; key_i++) {

			if (key[key_i] == alpha) {
				//printf("found %c at index %i\n", alpha, key_i);
				//printf(" %i ", key_i);
				key_map[map] = key_i;
				map++;
				break;

			}

		}

	}
	//printf ("\n");

	for (int j = 0; j < KEYWORDSIZE; j++) {
		printf ("%i ", key_map[j]);
	}
	printf ("\n");


	return (key_map);

}


char *encrypt_vigenere (char *buff, char *key, int bufferlength) {

	// given the buffer, key, and length of the buffer
	// encrypt as a vigenere cipher, bitwise by doing an xor
	// of each char with the successive keyword character

	char *encrypted;
	encrypted =(char *)malloc(sizeof(char) * bufferlength);

	// Do a bitwise vigenere encryption with key
	// key = KEYWORDSIZE character array
	//printf ("bufflength: %i\n", bufferlength );

	int char_location = 0;

	while (char_location < bufferlength) {

		for (int j = 0; j < KEYWORDSIZE; j++) {

			//printf ("encrypt %c with char = %c \n", buff[char_location], key[j]);

			if (char_location == bufferlength) {
				break;
			}

			encrypted[char_location] = buff[char_location] ^ key[j];

			char_location++;
		}

	}

	return encrypted;

}


char *columnar_transposition (char *buff, char *key, int bufferlength) {

	// columnar transposition
	// do a single bitwise columnar transposition using the given key

	char *transposed;
	transposed =(char *)malloc(sizeof(char) * bufferlength);

	// we will be breaking the chars into bits
	int bitbufferlength = bufferlength * 8;

	// we know the key is KEYWORDSIZE characters, divide
	// the input into KEYWORDSIZE columns, then concat the columns
	// based upon the key
	int colsize = bitbufferlength / KEYWORDSIZE;
	int remainder = bitbufferlength % KEYWORDSIZE;

	// if the remainder is not zero, we will be padding the transpositon
	if (remainder != 0) {
		colsize++;
	}

	printf ("bufferlength: %i\n", bufferlength);
	printf ("bitlength: %i\n", bitbufferlength );
	printf ("KEYWORDSIZE: %i\n", KEYWORDSIZE);
	printf ("colsize: %i\n", colsize);
	printf ("remainder: %i\n", remainder);

	int *columns[KEYWORDSIZE];

	for (int k = 0; k < KEYWORDSIZE; k++) {
		columns[k] = (int *)malloc(sizeof(int) * colsize);
	}

	int i = 0;				// location of current char in buff
	int bitlocation = 0;	// location of bit in bit buffer
	int column_index = 0;
	int column = 0;

	// This time we need to read buffer one bit at a time
	// buff[i] is stored as bytes (8 bits)

	// Split buff[i] into bits, and then populate each of the columns

	while (i < bufferlength) {

		for (int bitpos = 7; bitpos >= 0; bitpos--) {

			// distribute into columns from 0 - (KEYWORDSIZE - 1)
			column = bitlocation % KEYWORDSIZE;

			// get the bit
			int bit = ((buff[i] >> bitpos) & 1);

			//printf ("bitpos %i - %i\n", bitpos, bit);
			//printf ("put %i pos col = %i, index = %i \n", bit, column, column_index);
			printf ("%i ", bit);

			columns[column][column_index] = bit;

			if (column == (KEYWORDSIZE - 1)) {
				column_index++;
				printf("\n");
			}

			bitlocation++;

		}

		i++;

	}

	printf ("\n\n");

	// now read off the bits according to the key order
	// create a map - assume key is A-Z

	int *column_map = encrypt_map(key);

	// now we just have to read each bit in the columns, according to the
	// column map, and set each of the bits in an 8bit char

	char ch = '\0';			// the temporary character
	int location = 0;   	// the location of the bit in the bit string
	int bitposition = 0;	// the position of the bit in the pending character
	int chlocation = 0;		// the location of the character in the transposed string
	int map = 0;

	for (int i = 0; i < KEYWORDSIZE; i++) {

		map = column_map[i];
		//printf ("reading from column %i \n", map);

		for (int j = 0; j < colsize; j++) {

			// when we get to the end of the column, only include
			// the final column in remainder number of columns
			if ((remainder != 0) && (j == (colsize - 1)) && (map >= remainder)) {
				break;
			}

			bitposition = location % 8;

			//printf ("%i, %i = %i\n", map, j, columns[map][j]);
			printf ("%i ", columns[map][j]);

			ch |= columns[map][j];

			if (bitposition == 7) {
				//printf (" - ");
				//printf ("appending %c to transposed: in position %i %s\n", ch, chlocation, transposed);
				printf ("\n");
				transposed[chlocation] = ch;
				chlocation++;
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
			printf("Key 1 is too short, must be %i unique characters (A-Z).\n", KEYWORDSIZE);
			print_try_help();
			exit(0);
		}

		if (k2 == NULL) {
			printf("Key 2 is too short, must be %i unique characters (A-Z).\n", KEYWORDSIZE);
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
		char *first_encryption = encrypt_vigenere(buffer, k1, fileLen);

		free(buffer);

		// do columnar transposition with k1
		char *first_transpo = columnar_transposition(first_encryption, k1, fileLen);

		free(first_encryption);

		char *second_transpo = columnar_transposition(first_transpo, k2, fileLen);
		free(first_transpo);

		// write to output file
		char *output_filename = "encrypted";
		FILE *wp;
		wp = fopen(output_filename,"wb");
		fwrite(second_transpo,fileLen,1,wp);
		fclose(wp);

}


