#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <grp.h>
#include <pwd.h>
#include <errno.h>
#include <signal.h>


#define COMMAND_CH_LIMIT 512
#define DIRNAME_CH_LIMIT 20

void lsCommand(char *directoryName);
void lsWithl(char *directoryName);
void catCommand(char *fileName);
void grepCommand(char *statement, char *fileName);

int main(int argc, char const *argv[]){
	
	FILE *commandsFile;

	char command[COMMAND_CH_LIMIT];
	char directoryName[DIRNAME_CH_LIMIT];

	char *sub;
	char statement[512];
	char substrC[512];
	char substrG[512];
	
	int i,j,m;
	int index;
	int sizeOfCommand;
	int tempsize;
	char last;
	int isEnter = 0;

	if(argc == 1){          //Interactive mode
		printf("Interactive Mode\n");
		
		do{
			index=0;

			printf("prompt> ");

			scanf("%[^\n]", command);   // reads user input with whitespace
			scanf("%c", &last);			
			
			if(command[0] == '@'){
				printf("prompt> ");
   				scanf("%[^\n]", command);   // reads user input with whitespace
   				scanf("%c", &last);
   			}


   			if(strlen(command) > COMMAND_CH_LIMIT){
				perror("Too long command!\n");
				
			}
   			
   			if(!strcmp(command, "ls"))    
   				lsCommand(".");
   			if(strstr(command, "/bin/ls"))
   				lsCommand("/bin");

   			if(strstr(command, "ls -l")){
   				lsWithl(".");
   			}

   			if(sub = strstr(command, "cat")){

            		index = sub-command;

   					for(i=0;i<strlen(sub); i++){

   						if(command[index+4] == ' ' ||  command[index+4] == ';'){						
   							substrC[i] = '\0';
   							break;
   						}	
   						substrC[i] = command[index+4];
   						index++;
   					}

   					catCommand(substrC);	
   								
   			}			

   			if(sub = strstr(command, "grep")){

   				index = sub-command;

   				for(i=0;i<strlen(sub); i++){
   					substrG[i] = command[index+5];
   					index++;
   				}
   				substrG[i] = '\0';

   				j=0;
   				for(i=0; i< strlen(substrG); i++){
   					while(substrG[i] != ' '){
   						statement[j] = substrG[i];
   						j++;
   						i++;
   					}
   					statement[j] = '\0';
   					i++;
   					break;
   				}

   				m=i;
   				for(; i< strlen(substrG); i++){
   					if(substrG[i] == ' ' || substrG[i] == ';'){						
   						substrG[i] = '\0';
   						break;
   					}	
   				}								
   				grepCommand(statement,&substrG[m]);
   			}

   			if(command[0] != 'q')
   				command[0] = ' ';
		} while(strcmp(command, "quit") != 0);   // + CTRL-D option

		return 0;
	}

	if(argc == 2){          //Batch mode
		printf("Batch Mode\n");
		
		if ((commandsFile = fopen(argv[1], "r" )) == NULL) {
			fprintf(stderr, "Can not open file: %s!", argv[1]);
			exit(EXIT_FAILURE);
		}

		while(fgets(command, COMMAND_CH_LIMIT, commandsFile)){     // reading commands in file line by line

			index=0;

			sizeOfCommand= strlen(command)-1;
			if(command[sizeOfCommand] == '\n')			
				command[sizeOfCommand] = '\0';

			if(sizeOfCommand > COMMAND_CH_LIMIT){
				perror("Too long command!\n");
			}

			printf("Command: %s\n\n",command);

			if(!strcmp(command, "")){
				fprintf(stderr, "Command can not execute!\n");
				continue;
			}
			

			if(!strcmp(command, "quit"))
				break;
			if(!strcmp(command, "ls")) {   
				lsCommand(".");
			}	
			if(strstr(command, "/bin/ls"))
				lsCommand("/bin");

			if(strstr(command, "ls –l") || strstr(command, "ls -l")){ 
				lsWithl(".");
			}

			if(sub = strstr(command, "cat")){

				index = sub-command;

				for(i=0;i<strlen(sub); i++){

					if(command[index+4] == ' ' ||  command[index+4] == ';'){						
						substrC[i] = '\0';
						break;
					}	
					substrC[i] = command[index+4];
					index++;
				}

				catCommand(substrC);					
			}			

			if(sub = strstr(command, "grep")){

				index = sub-command;

				for(i=0;i<strlen(sub); i++){
					substrG[i] = command[index+5];
					index++;
				}
				substrG[i] = '\0';
				
				j=0;
				for(i=0; i< strlen(substrG); i++){
					while(substrG[i] != ' '){
						statement[j] = substrG[i];
						j++;
						i++;
					}
					statement[j] = '\0';
					i++;
					break;
				}

				m=i;
				for(; i< strlen(substrG); i++){
					if(substrG[i] == ' ' || substrG[i] == ';'){						
						substrG[i] = '\0';
						break;
					}	
				}			

				grepCommand(statement,&substrG[m]);

			}	
		}	

		fclose(commandsFile);	
	}	

	else{
		fprintf(stderr, "Wrong number of arguments entered!");
		exit(EXIT_FAILURE);
	}	

	return 0;
}

void lsCommand(char *directoryName){

	DIR *p;
	struct dirent *d;

	p=opendir(directoryName);

	if(p==NULL) {
		perror("Can not find directory!");
		exit(-1);
	}

	while(d=readdir(p))
		printf("%s\n",d->d_name);
}

void catCommand(char *fileName){

	FILE *readFile;
	char c;

	if ((readFile = fopen(fileName, "r" )) == NULL) {
		fprintf(stderr, "Can not open file: %s!", fileName);
		exit(EXIT_FAILURE);
	}

	while((c = getc(readFile)) != EOF)
		fprintf(stdout, "%c", c);

	printf("\n\n");

	fclose(readFile);
}

void lsWithl(char *directoryName){

	DIR *dp;
	struct dirent *dirp;
	struct passwd *pw;
	struct group *gp;

	char *c;
	int i;

	if ((dp = opendir(directoryName)) == NULL){
		perror("Can not find directory!");
		exit(-1);
	}

	while ((dirp = readdir(dp)) != NULL){
		struct stat fileStat;
		stat(dirp->d_name,&fileStat);   

		printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
		printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
		printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
		printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
		printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
		printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
		printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
		printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
		printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
		printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
		printf(" ");

		printf(" %zu ",fileStat.st_nlink);

		pw=getpwuid(fileStat.st_uid);
		printf("%s ",pw->pw_name);
		gp=getgrgid(fileStat.st_gid);
		printf("%s ",gp->gr_name);

		c=ctime(&fileStat.st_mtime);
		for(i=4;i<=15;i++)
			printf("%c",c[i]);
		printf(" ");

		printf(" %zu  ",fileStat.st_size);
		printf("%s\n", dirp->d_name);

	} 
	printf("\n");
	
}

void grepCommand(char *statement, char *fileName){

	FILE *readFile;
	int r,j=0; 
	char c,line[100]; 

	if ((readFile = fopen(fileName, "r" )) == NULL) {
		fprintf(stderr, "Can not open file: %s!", fileName);
		exit(EXIT_FAILURE);
	}

	else { 
		do{ 

			c = getc(readFile);      	

			if(c!='\n' && c!= EOF){ 
				line[j]=c;               
				j++; 
			} 

			else { 

				if(strstr(line,statement)) 
					printf("%s\n",line); 
				memset(line,0,sizeof(line)); 
				j=0; 
			} 

		} while(c  != EOF) ;
	}   

	fclose(readFile);
}
