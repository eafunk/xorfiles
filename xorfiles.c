#include	<stdio.h>
#include	<string.h>

/*
 Copyright (c) 2019 Ethan Funk
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
 documentation files (the "Software"), to deal in the Software without restriction, including without limitation 
 the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
 and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial portions 
 of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED 
 TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 DEALINGS IN THE SOFTWARE.
*/

int main(int argn, char **args)
{
    FILE *in, *key, *out;
    int fval, kval;
	char scramb, res;
	long offset;
	unsigned long count;
	
	if(argn != 5){
		fprintf(stdout, "USAGE:[input file path] [key file path] [output file path] [key file byte offset (decimal)]\n");
		return 1;
	}
	if((offset = atol(args[4])) < 0){
		fprintf(stdout, "Key file byte offset needs to be a positive number or zero.\n");
		return 1;
	}
	if((strlen(args[2]) > 0) && ((key = fopen(args[2], "rb")) != NULL)){
		if(fseek(key, offset, SEEK_SET)){
			fprintf(stdout, "Key file byte offset error: larger than the keyfile size.\n");
			fclose(key);
			return 1;
		}
		if((strlen(args[1]) > 0) && ((in = fopen(args[1], "rb")) != NULL)){
			if((strlen(args[3]) > 0) && ((out = fopen(args[3], "wb")) != NULL)){
				count = 0;
				scramb = 0xf0 + (char)(offset & 0xff);	/* prime the key bit scrambler */
				while(1){
					if((fval = fgetc(in)) == EOF){
						if(ferror(in))
							fprintf(stdout, "Error reading the input file.\n");
						else
							fprintf(stdout, "\nDone. %lu bytes writen.\n", count);
						fclose(in);
						fclose(key);
						fclose(out);
						return 0;
					}
					if((kval = fgetc(key)) == EOF){
						// key file is too short... rewind and use again.
						fprintf(stdout, "Warning: key file is shorter than the data file... key file is being repeated.\n");
						rewind(key);
						kval = fgetc(key);
					}
					/* recursive bit scramble using previous value and next key byte (modified BSD hash) */
					scramb = (scramb >> 1) + ((scramb & 1) << 7);
					scramb += (char)(kval & 0xff);
					/* xor scrambled key byte with input file byte */
					res = scramb ^ (char)(fval & 0xff);
					fputc(res, out);
					count++;
				}
			}
			fprintf(stdout, "Failed to create output file.\n");
			return 1;
		}
		fprintf(stdout, "Bad input file specified.\n");
		return 1;
	}
	fprintf(stdout, "Bad key file specified.\n");
	return 1;
}
			
