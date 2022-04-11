#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

// controller {language} {mode} {command} {working directory path}
// controller [-c|-cpp|-java|-python|-perl|-php] [1|2|3] [./cexec] [/tmp/PARIKSHAK/student/8008/]

// mode 0 => No restrictions, 1 => strict restriction with maximum restrictions, 2 => medium level restiction, 3 => low level restriction
// command any command you would run

// RLIMIT_AS 65,792 => maximum size of the process's virtual memory(address space).  The limit is specified in bytes
// RLIMIT_CORE 0 => maximum size of a core file in bytes that the process may dump.
// RLIMIT_CPU 5 10 => limit, in seconds, on the amount of CPU time that the process can consume
// RLIMIT_DATA 65,792 => maximum size of the process's data segment. limit is specified in bytes, and is rounded down to the system page size.
// RLIMIT_RTTIME 10(microsecond) cpu wait time for process for blocking function call
// RLIMIT_FSIZE 1,048,576 bytes(1MB) => maximum size in bytes of files that the process may create
// RLIMIT_NOFILE no of file to open min should be 3

void exitFunction(){
    fprintf(stderr, "Time Limit Exceeded!\n");
    exit(101);
}

int main(int argc, char *argv[]){

	struct rlimit rl_as, rl_core, rl_cpu, rl_data, rl_rttime, rl_fsize, rl_nofile;
	
	int pathLength = strlen(argv[4]);
	if(pathLength <= 1){
		fprintf(stderr, "Invalid Path! %s\n", argv[4]);
		return 104;
	}
	if( access( argv[4], F_OK ) != 0 ) {
    	fprintf(stderr, "Invalid Path Folder doesn't exists! %s\n", argv[4]);
		return 104;
	}
 
	char* inputFilePath = (char*) calloc(pathLength+10, sizeof(char));
	strcpy(inputFilePath,argv[4]);
	int inputfile = open(strcat(inputFilePath,"input.txt"), O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH);	
	if( access( inputFilePath, F_OK ) != 0 ) {
    	fprintf(stderr, "Input File doesn't exists! %s\n", inputFilePath);
		return 105;
	} 
	
	char* outputFilePath = (char*) calloc(pathLength+11, sizeof(char));
	strcpy(outputFilePath,argv[4]);
	char* errorFilePath = (char*) calloc(pathLength+10, sizeof(char));
	strcpy(errorFilePath,argv[4]);

    int outputfile = open(strcat(outputFilePath,"output.txt"), O_CREAT|O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    int errorfile = open(strcat(errorFilePath,"error.txt"), O_CREAT|O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	dup2(inputfile, 0);
    dup2(outputfile, 1);    
    dup2(errorfile, 2);

	switch(argc){
		case 1 :
				fprintf(stderr, "No arguments provided!, refer below command :\ncontroller [-c|-cpp|-java|-python|-perl|-php] [1|2|3] [command] [working directory path]\n");
				return 102;
				break;
		case 5 :
				break;
		default :
				fprintf(stderr, "Invalid arguments, refer below command :\ncontroller [-c|-cpp|-java|-python|-perl|-php] [1|2|3] [command] [working directory path]\n");
				return 102;
				break;
	}

	rlim_t rl_as_cur = 524288, rl_as_max = 1048576;
	rlim_t rl_core_cur = 0, rl_core_max = 0;
	rlim_t rl_cpu_cur = 4, rl_cpu_max = 5;
	rlim_t rl_data_cur = 4096000000, rl_data_max = 6553600000;
	rlim_t rl_rttime_cur = 10, rl_rttime_max = 100;
	rlim_t rl_fsize_cur = 524288, rl_fsize_max = 1048576;
	rlim_t rl_nofile_cur = 100, rl_nofile_max = 1024;

	unsigned int time_limit = 3;
	int mode = 1;

	if(strcmp(argv[2], "1") == 0 || strcmp(argv[2], "2") == 0 || strcmp(argv[2], "3") == 0 || strcmp(argv[2], "0") == 0){
		mode = atoi(argv[2]);
	}else{
		fprintf(stderr, "Invalid! mode should be [0|1|2|3], refer below command :\ncontroller [-c|-cpp|-java|-python|-perl|-php] [1|2|3] [command] [working directory path]\n");
		return 102;
	}

	switch(mode){
		case 0 :
			time_limit = 60; //sec
			break;
		case 2 :
			rl_cpu_cur = 4; //sec
			rl_cpu_max = 5;
			rl_rttime_cur = 30; // microseconds
			rl_rttime_max = 50;
			rl_data_cur = 2048000; // 2MB
			rl_data_max = 4096000; // 4MB
			rl_fsize_cur = 532480; // 500KB
			rl_fsize_max = 1024000; // 1MB
			rl_nofile_cur = 64; 
			rl_nofile_max = 512;
			time_limit = 5; //sec
			break;
		case 3 :
			rl_cpu_cur = 50; 
			rl_cpu_max = 100;
			rl_rttime_cur = 500; // microseconds
			rl_rttime_max = 1000;
			rl_data_cur = 4096000000; // 4GB
			rl_data_max = 6553600000; // 6GB
			rl_fsize_cur = 1024000000; // 1GB
			rl_fsize_max = 4096000000; // 4GB
			rl_nofile_cur = 512;
			rl_nofile_max = 1024;
			time_limit = 30; //sec
			break;
		default :
			rl_cpu_cur = 2; //sec
			rl_cpu_max = 3;
			rl_rttime_cur = 10; // microseconds
			rl_rttime_max = 30;
			rl_data_cur = 409600; // 400KB
			rl_data_max = 819200; // 800KB
			rl_fsize_cur = 10240; // 10KB
			rl_fsize_max = 16384; // 16KB
			rl_nofile_cur = 16;
			rl_nofile_max = 64;
			time_limit = 3; //sec
			break;
	}

	if(strcmp(argv[1],"-cpp") == 0 || strcmp(argv[1],"-c") == 0){
		
		
	}else if(strcmp(argv[1],"-java") == 0){

		switch(mode){
			case 2 :
				rl_data_cur = 1228800000; // 1.2GB
				rl_data_max = 1638400000; // 1.6GB
				break;
			case 3 :
				rl_data_cur = 4096000000; // 4GB
				rl_data_max = 6553600000; // 6GB
				break;
			default :
				rl_data_cur = 1024000000; // 1GB
				rl_data_max = 1228800000; // 1.2GB
				break;
		}
	}else if(strcmp(argv[1],"-python") == 0){

		switch(mode){
			case 2 :
				rl_data_cur = 102400000; // 100MB
				rl_data_max = 204800000; // 200MB
				break;
			case 3 :
				rl_data_cur = 1228800000; // 1.2GB
				rl_data_max = 1638400000; // 1.6GB
				break;
			default :
				rl_data_cur = 10240000; // 10MB
				rl_data_max = 20480000; // 20MB
				break;
		}
		
	}else if(strcmp(argv[1],"-perl") == 0){
		switch(mode){
			case 2 :
				rl_data_cur = 102400000; // 100MB
				rl_data_max = 204800000; // 200MB
				break;
			case 3 :
				rl_data_cur = 1228800000; // 1.2GB
				rl_data_max = 1638400000; // 1.6GB
				break;
			default :
				rl_data_cur = 10240000; // 10MB
				rl_data_max = 20480000; // 20MB
				break;
		}
	}else if(strcmp(argv[1],"-php") == 0){
		switch(mode){
			case 2 :
				rl_data_cur = 102400000; // 100MB
				rl_data_max = 204800000; // 200MB
				break;
			case 3 :
				rl_data_cur = 1228800000; // 1.2GB
				rl_data_max = 1638400000; // 1.6GB
				break;
			default :
				rl_data_cur = 10240000; // 10MB
				rl_data_max = 20480000; // 20MB
				break;
		}

	}else{
		fprintf(stderr, "Invalid arguments, refer below command :\ncontroller [-c|-cpp|-java|-python|-perl|-php] [1|2|3] [command]\n");
		return 102;
	}
	
    


 // Get old limits
   /* if( getrlimit(RLIMIT_AS, &old_lim) == 0)
        printf("RLIMIT_AS Old limits -> soft limit= %ld \t"
          " hard limit= %ld \n", old_lim.rlim_cur,
                               old_lim.rlim_max);
    else
        fprintf(stderr, "%s RLIMIT_AS\n", strerror(errno));
   */

   // Set limits
   // if(setrlimit(RLIMIT_AS, &rl_as) == -1)
   //     fprintf(stderr, "%s RLIMIT_AS\n", strerror(errno)); 

	if(mode != 0){

		rl_as.rlim_cur = rl_as_cur;
		rl_as.rlim_max = rl_as_max;

		rl_core.rlim_cur = rl_core_cur;
		rl_core.rlim_max = rl_core_max;

		rl_cpu.rlim_cur = rl_cpu_cur;
		rl_cpu.rlim_max = rl_cpu_max;

		rl_rttime.rlim_cur = rl_rttime_cur;
		rl_rttime.rlim_max = rl_rttime_max;
		
		rl_data.rlim_cur = rl_data_cur;
		rl_data.rlim_max = rl_data_max;
		
		rl_fsize.rlim_cur = rl_fsize_cur;
		rl_fsize.rlim_max = rl_fsize_max;

		rl_nofile.rlim_cur = rl_nofile_cur;
		rl_nofile.rlim_max = rl_nofile_max;

		if(setrlimit(RLIMIT_CORE, &rl_core) == -1){
		    fprintf(stderr, "%s RLIMIT_CORE Cannot set Limit!\n", strerror(errno));
			return 103;
		}
		if(setrlimit(RLIMIT_CPU, &rl_cpu) == -1){
		    fprintf(stderr, "%s RLIMIT_CPU Cannot set Limit!\n", strerror(errno));
			return 103;
		}
		
		if(setrlimit(RLIMIT_RTTIME, &rl_rttime) == -1){
		    fprintf(stderr, "%s RLIMIT_rttime Cannot set Limit!\n", strerror(errno));
			return 103;
		}
		if(setrlimit(RLIMIT_DATA, &rl_data) == -1){
		    fprintf(stderr, "%s RLIMIT_DATA Cannot set Limit!\n", strerror(errno));
			return 103;
		}
		if(setrlimit(RLIMIT_FSIZE, &rl_fsize) == -1){
		    fprintf(stderr, "%s RLIMIT_FSIZE Cannot set Limit!\n", strerror(errno));
			return 103;
		}
		if(setrlimit(RLIMIT_NOFILE, &rl_nofile) == -1){
		    fprintf(stderr, "%s RLIMIT_NOFILE Cannot set Limit!\n", strerror(errno));
			return 103;
		}
	}

	dup2(outputfile, 1);    
    dup2(outputfile, 2);

    signal(SIGALRM, exitFunction);
    alarm(time_limit);

    int exitStatus;
    exitStatus = system( argv[3] );

	dup2(errorfile, 2);
    fprintf(stderr,"\nExit Status: %d\n", exitStatus);

	close(inputfile);
    close(outputfile);
    close(errorfile);

	free(outputFilePath);
	free(errorFilePath);

	return 0;
}
