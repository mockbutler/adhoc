/* Copyright (c) 2011, Marc Butler. All Rights Reserved. */
/*
 * Generate a password tabula recta as describe by John
 * Graham-Cumming.
 * 
 * http://blog.jgc.org/2010/12/write-your-passwords-down.html
 */

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

char hex(int i) 
{
    static char hexdigits[] = "0123456789ABCDEF";
    return hexdigits[i % 16];
}

int main(int argc, char **argv)
{
#define TBL_DIM 26
#define TBL_SIZE (TBL_DIM * TBL_DIM)
    
    char table[TBL_SIZE] = {};
    int i;
    int fd;
    ssize_t rc;
    char *datfile = "pwtable.dat";
    static char bubble[] = ".oO";

    if (argc == 1) {
	fd = open("/dev/random", O_RDONLY);
	if (fd < 0) {
	    perror("/dev/random");
	    exit(EXIT_FAILURE);
	}
	printf("Generating tabula recta data. This can take several minutes. |");
	i = 0;
	while (i < TBL_DIM * TBL_DIM) {        
	    char c;
	    rc = read(fd, &c, 1);
	    if (rc == 1) {
		if (isprint(c)) {
                    putchar(0x8);
                    putchar(bubble[i % 3]);
                    fflush(stdout);
		    table[i++] = c;
		}
	    } else if (rc < 0) {
		perror("reading from /dev/random");
		exit(EXIT_FAILURE);
	    } else {
		perror("failed to read from /dev/random");
	    }
	}
	rc = close(fd);
	if (rc < 0) {
	    perror("closing /dev/random");
	    exit(EXIT_FAILURE);
	}
	puts("done");

	fd = open(datfile, O_WRONLY|O_CREAT, 0644);
	if (fd < 0) {
	    perror(datfile);
	    exit(EXIT_FAILURE);
	}
	rc = write(fd, table, TBL_SIZE);
	if (rc != TBL_DIM * TBL_DIM) {
	    perror("writing pwtable.dat");
	    exit(EXIT_FAILURE);
	}
	close(fd);
    } else if (argc == 2) {
        datfile = argv[1];
    } else {
        puts("\n"
             "Usage: pwtable [datfile]\n"
             "\n"
             "If no datfile is specified then one will be generated\n"
             "and stored in pwtable.dat.\n"
             "A tabula recta will then be printed out in ASCII.\n"
             "\n"
             "http://blog.jgc.org/2010/12/write-your-passwords-down.html\n");
        exit(EXIT_FAILURE);
    }        

    fd = open(datfile, O_RDONLY);
    if (fd < 0) {
	perror(datfile);
	exit(EXIT_FAILURE);
    }
    rc = read(fd, table, TBL_SIZE);
    if (rc != TBL_SIZE) {
	perror("reading pwtable.dat");
	exit(EXIT_FAILURE);
    }
    close(fd);
    
    printf("    ");
    for (i = 0; i < 26; i++) {
        putchar(hex(i % 13));
        putchar(' ');
    }
    printf("\n    ");
    for (i = 0; i < 26; i++) {
        putchar('A' + i);
        putchar(' ');
    }
    printf("\n   +---------------------------------------------------");
    for (i = 0; i < TBL_DIM * TBL_DIM; i++) {
        if (i % 26 == 0) {
            putchar('\n');
            putchar(hex((i / 26) % 13));
            putchar(' ');
            putchar('A' + (i / 26));
            putchar('|');            
        }
        putchar(table[i]);
        putchar(' ');
    }
    putchar('\n');
    exit(EXIT_SUCCESS);
}
