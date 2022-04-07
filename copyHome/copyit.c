// Author: German Parra
// Operating Systems Project 1

// Directives
#include<stdio.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<signal.h>
#include<string.h>

// Function Prototypes 
void display_message(int signum);

int main(int argc, char* argv[]) {

	// Check to see if there are two many arguments	
	if(argc > 3) {
		printf("copyit: Too many arguments!\n");
		printf("usage: copyit <sourcefile> <targetfile>\n");
		exit(1);
	}

	else if(argc < 3) {
		printf("copyit: Too few arguments!\n");
		printf("usage: copyit <sourcefile> <targetfile>\n");
		exit(1);
	}

	//set up the periodic message
	signal(SIGALRM, display_message);
	alarm(1); // scheduled alarm after 1 second 

	// Opening the source file, is O_CREAT neccesary?
	// open the source file or exit with an error
	int fd = open(argv[1], O_RDWR);

	// if you need to check the fd when not failed 
	//printf("%d\n", fd);

	// Check if the file is errno, give error number
	if(fd < 0) {
		
		//printf("ERROR NUMBER: %d\n", errno);
		//perror("Error Number");
		printf("Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}	

	// create the target file or exit with an error
	int createFile = creat(argv[2], O_RDWR | S_IRWXU);

	// check for errors with the creation of the new file 
	if(createFile < 0) {

		printf("Unable to create %s: %s\n", argv[2], strerror(errno));
		exit(1);
	}

	/* 
	loop over {
     read a bit of data from the source file
     if the read was interrupted, try it again
     if there was an error reading, exit with an error
     if no data left, end the loop

     write a bit of data to the target file
     if the write was interrupted, try it again
     if not all the data was written, exit with an error
	}
	*/
	int result;
	char buf[BUFSIZ]; 
	int bytes;
	int counter = 0;

	while((result = read(fd, buf, BUFSIZ)) > 0) {

		// Check to see if there is an interrupt 
		if(errno == EINTR) {
			continue;
		}

		// Check if result has errors 
		if(result < 0) {
			printf("copyit: %s\n", strerror(errno));
			exit(1);
		}

		// Want to call write before checking the errno and errors again
		bytes = write(createFile, buf, result);

		counter = counter + bytes;
		// Different than first EINTR, since write isn't argument in the while loop 
		if(errno == EINTR) {
			bytes = write(createFile, buf, result);
		}
		
		// Check for errors here 
		if(bytes < 0) {
			printf("copyit: %s\n", strerror(errno));
			exit(1);
		}
		//result = read(fd, buf, BUFSIZ);
	}

	// print success message
	printf("copyit: Copied %d bytes from file %s to %s.\n", counter, argv[1], argv[2]);
	
	// close both files
	close(fd);
	close(createFile);

	return 0;
}

// Function Definitions 
void display_message(int signum) {
	printf("copyit: still copying...\n");	
	alarm(1);
	//sleep(1);
}
