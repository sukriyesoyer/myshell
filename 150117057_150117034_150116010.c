#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <glob.h>
#include <dirent.h> 
#include<fcntl.h>
#include<sys/stat.h>
#include <string.h>

#define CREATE_FLAGS (O_WRONLY | O_CREAT |O_TRUNC)
#define CREATE_FLAGS1 (O_WRONLY | O_CREAT |O_APPEND)
#define CREATE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
 
#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
 
/* The setup function below will not return any value, but it will just: read
in the next command line; separate it into distinct arguments (using blanks as
delimiters), and set the args array entries to point to the beginning of what
will become null-terminated, C-style strings. */

int isoutput=0;
int isinput=0;
int isappend=0;
int inputfileindex=0;
int outputfileindex=0;
int iserror=0;

typedef  struct bookmark {
     char  comment[MAX_LINE/2 + 1];
     struct bookmark *next;
 }bookmark_t;
 typedef bookmark_t *bookmarkptr;
 bookmarkptr head=NULL; 

typedef  struct backgroundprocess{
   int pid;
 char  name[MAX_LINE/2 + 1];
    int job;
struct backgroundprocess *next;
     }backgroundprocess_t;
    typedef backgroundprocess_t *backgroundptr;
    backgroundptr backhead=NULL; 

void insert(bookmarkptr *head,char args[]){
         bookmarkptr newPtr =(bookmarkptr)malloc(sizeof(bookmark_t)); // create node
	if(newPtr!=NULL){
	bookmarkptr currentptr=*head;
	bookmarkptr previousptr=NULL;
	

	strcpy(newPtr->comment,args);
	
	newPtr->next=NULL; // node does not link to another node
	// loop to find the correct location in the list
	while (currentptr != NULL ) {
	previousptr=currentptr;
	currentptr=currentptr->next;
	}
	if(previousptr==NULL){
        
	newPtr->next=*head;
	*head=newPtr;}
        else{
       previousptr -> next = newPtr;
        } }
else { perror("new pointer oluşturulmadı.");
}
}





void delete(bookmarkptr *head_ref, int position) 
{ 
   // If linked list is empty 
   if (*head_ref == NULL) 
      return; 
  
   // Store head node 
  bookmarkptr temp = *head_ref; 
  
    // If head needs to be removed 
    if (position == 0) 
    { 
        *head_ref = temp->next;   // Change head 
        free(temp);               // free old head 
        return; 
    } 
  
    // Find previous node of the node to be deleted 
    for (int i=0; temp!=NULL && i<position-1; i++) 
         temp = temp->next; 
  
    // If position is more than number of nodes 
    if (temp == NULL || temp->next == NULL) {
    fprintf(stderr,"index geçersiz");
         return; }
         
  
    // Node temp->next is the node to be deleted 
    // Store pointer to the next of node to be deleted 
   bookmarkptr next = temp->next->next; 
  
    // Unlink the node from linked list 
    free(temp->next);  // Free memory 
  
    temp->next = next;  // Unlink the deleted node from list 
} 


void print(bookmarkptr *head){
     bookmarkptr currentptr=*head;
     int i=0;
     while(currentptr!=NULL){
     printf("%d   %s\n",i,currentptr->comment);
     currentptr=currentptr->next;
     i++;}
}


void insertback(backgroundptr *head,pid_t pid,int job,char args[]) {
        backgroundptr  newPtr =(backgroundptr)malloc(sizeof(backgroundprocess_t)); // create node
	if(newPtr!=NULL){
	backgroundptr  currentptr=*head;
	backgroundptr previousptr=NULL;
	newPtr->pid=pid;
        newPtr->job=job;
        strcpy(newPtr->name,args);	
	newPtr->next=NULL; // node does not link to another node
	// loop to find the correct location in the list
	while (currentptr != NULL ) {
	previousptr=currentptr;
	currentptr=currentptr->next;
	}
	if(previousptr==NULL){
        
	newPtr->next=*head;
	*head=newPtr;}
        else{
       previousptr -> next = newPtr;
        } }
else { perror("back new ptr oluşturulmadı");
}
}
void printback(backgroundptr *head){
    backgroundptr currentptr=*head;
    while(currentptr!=NULL){
    printf("[%d]  %s (pid=%d) \n",currentptr->job,currentptr->name,currentptr->pid);
    currentptr=currentptr->next;
 }
}


void findinfile(char *file,char * word);
void removeChar(char *str, char garbage);
void find (char * word) {  
    char garbage ='"';
    removeChar(word, garbage);
   DIR *d;
  struct dirent *dir;
  d = opendir(".");
  if (d) {
   while ((dir = readdir(d)) != NULL) {
if(strstr(dir->d_name,".h")||strstr(dir->d_name,".c")||strstr(dir->d_name,".C")||strstr(dir->d_name,".H")){
    findinfile(dir->d_name,word);
    }
    }
    closedir(d);
  }
  return;
}
void findrecursive (char *basePath,char *word){
    char garbage ='"';
    removeChar(word, garbage);
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    // Unable to open directory stream
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
          char filename[100];
          strcpy(filename, basePath);
          strcat(filename, "/");
         strcat(filename, dp->d_name);
if(strstr(dp->d_name,".h")||strstr(dp->d_name,".c")||strstr(dp->d_name,".C")||strstr(dp->d_name,".H")){
        
            findinfile(filename,word);
 }

            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

           findrecursive(path,word);
        }
    }

    closedir(dir);
}
void findinfile(char *file,char * word)
{
         
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

int linenum=0;
    fp = fopen( file,"r");
    if (fp == NULL){
       fprintf(stderr,"dosya açılamadı %s",file);
        exit(EXIT_FAILURE);}

    while ((read = getline(&line, &len, fp)) != -1) {
    linenum++;

if(strstr(line,word)!=NULL){
    printf("%d %s %s\n",linenum,file, line);
    }}

    fclose(fp);
    if (line)
        free(line);

}

void running(bookmarkptr *head_ref, int position) {

  bookmarkptr temp = *head_ref; 
      for(int i=0;i<position;i++){
           temp=temp->next;
            }
char *ars= strtok (temp->comment, "\"");
    int i= system(ars);
if(i<0){
fprintf(stderr,"system function failed.");
}

}



void removeChar(char *str, char garbage) {

    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}



void determine(char * args[]){
int i=0;
while(args[i]!=NULL){

if(strcmp(args[i],"<")==0){
    isinput=1;
    inputfileindex=i+1;}
    if(strcmp(args[i],"2>")==0){
    iserror=1;
    outputfileindex=i+1;}
    
    

if(strcmp(args[i],">")==0){
    isoutput=1;
   outputfileindex=i+1;}
if(strcmp(args[i],">>")==0){
    isappend=1;
   outputfileindex=i+1;}
i++;
}
}




void redirectinput(char * word){
int fd;
   fd = open(word, O_RDONLY);
   if (dup2(fd, STDIN_FILENO) == -1) {
   perror("Failed to redirect standard output");
     }
   if (close(fd) == -1) {
   perror("Failed to close the file");}
     }
   void printerror(char * word){
   int fd;
   fd = open(word, CREATE_FLAGS1, CREATE_MODE);
  if (dup2(fd, STDERR_FILENO) == -1) {
   perror("Failed to redirect standard output");
       }    
  if (close(fd) == -1) {
   perror("Failed to close the file");}
      }
void redirecoutput(char * word){
    int fd;
fd = open(word, CREATE_FLAGS, CREATE_MODE,0644);
   if (fd == -1) {
    perror("Failed to open file");
    }
if (dup2(fd, STDOUT_FILENO) == -1) {
     perror("Failed to redirect standard output");}
if (close(fd) == -1) {
   perror("Failed to close the file");
   }
}

void redirecoutputappend(char * word){
    int fd;
    fd = open(word, CREATE_FLAGS1, CREATE_MODE);
    if (fd == -1) {
    perror("Failed to open file");
}
   if (dup2(fd, STDOUT_FILENO) == -1) {
     perror("Failed to redirect standard output");}
    if (close(fd) == -1) {
    perror("Failed to close the file");
  }
}

void setup(char inputBuffer[], char *args[],int *background)
{
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start,  /* index where beginning of next command parameter is */
        ct;     /* index of where to place the next parameter into args[] */
    
    ct = 0;
        
    /* read what the user enters on the command line */
    length = read(STDIN_FILENO,inputBuffer,MAX_LINE);  

    /* 0 is the system predefined file descriptor for stdin (standard input),
       which is the user's screen in this case. inputBuffer by itself is the
       same as &inputBuffer[0], i.e. the starting address of where to store
       the command that is read, and length holds the number of characters
       read in. inputBuffer is not a null terminated C-string. */

    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */

/* the signal interrupted the read system call */
/* if the process is in the read() system call, read returns -1
  However, if this occurs, errno is set to EINTR. We can check this  value
  and disregard the -1 value */
    if ( (length < 0) && (errno != EINTR) ) {
        perror("error reading the command");
	exit(-1);           /* terminate with error code of -1 */
    }

	printf(">>%s<<",inputBuffer);
    for (i=0;i<length;i++){ /* examine every character in the inputBuffer */

        switch (inputBuffer[i]){
	    case ' ':
	    case '\t' :               /* argument separators */
		if(start != -1){
                    args[ct] = &inputBuffer[start];   /* set up pointer */
		    ct++;
		}
                inputBuffer[i] = '\0'; /* add a null char; make a C string */
		start = -1;
		break;

            case '\n':                 /* should be the final char examined */
		if (start != -1){
                    args[ct] = &inputBuffer[start];     
		    ct++;
		}
                inputBuffer[i] = '\0';
                args[ct] = NULL; /* no more arguments to this command */
		break;

	    default :             /* some other character */
		if (start == -1)
		    start = i;
                if (inputBuffer[i] == '&'){
		    *background  = 1;
                    inputBuffer[i-1] = '\0';
		}
	} /* end of switch */
     }    /* end of for */
     args[ct] = NULL; /* just in case the input line was > 80 */

	for (i = 0; i <= ct; i++)
		printf("args %d = %s \n",i,args[i]);
if(*background==1){
args[ct-1]=NULL;}
} /* end of setup routine */



int main(void)
{
          char inputBuffer[MAX_LINE]; /*buffer to hold command entered */
            int background; /* equals 1 if a command is followed by '&' */
            char *args[MAX_LINE/2 + 1]; /*command line arguments */
               int job=1;
            while (1){
              
                        background = 0;
                        printf("myshell: ");
                        /*setup() calls exit() when Control-D is entered */
                       setup(inputBuffer, args, &background);
                       
                        /** the steps are:
                        (1) fork a child process using fork()
                        (2) the child process will invoke execv()
		        (3) if background == 0, the parent will wait,
                        otherwise it will invoke the setup() function again. */




determine(args);




if((isoutput==1 || isinput==1|| isappend==1|| iserror==1) && background==0){

   pid_t childpid;
   childpid=fork();
if(childpid == 0){
    char *arg [MAX_LINE/2 +1];
if(isoutput==1){
  redirecoutput(args[outputfileindex]);}
if(isinput==1){ 
   redirectinput(args[inputfileindex]); 
}
if(isappend==1){
  redirecoutputappend(args[outputfileindex]);
 }
 if(iserror==1){
 printerror(args[outputfileindex]);
  }
 
if(isinput==1){
int c;
for(c=0;c<inputfileindex-1;c++){
arg[c]=args[c];}
arg[c]=NULL;}
else{
int c;
for(c=0;c<outputfileindex-1;c++){
arg[c]=args[c];}
arg[c]=NULL;}

execvp(arg[0],&arg[0]);
perror("exec func fail");
}
wait(NULL);

isoutput=0;
isinput=0;
outputfileindex=0;
inputfileindex=0;
isappend=0;
iserror=0;
}     
    else if(strcmp(args[0],"bookmark")==0){
            if(strcmp(args[1],"-l")==0){
                    print(&head);
                 }
              else if(strcmp(args[1],"-d")==0){
                       delete(&head,atoi(args[2]));
                     }
           else if(strcmp(args[1],"-i")==0){
               running(&head,atoi(args[2]));}
              else{
          int i=2;
         char arg[MAX_LINE/2 + 1];
          strcpy(arg,args[1]);
       while(args[i]!=NULL){  
               strcat(arg," ");
               strcat(arg,args[i]);
              
	           i++;}
	
           insert(&head,arg);}
           
            }
            
          else if(strcmp(args[0],"search")==0 ){ 
          if(args[2]==NULL){
          find(args[1]);
          }
          else{
           findrecursive(".",args[2]);}
                  }
            else if(strcmp(args[0],"ps_all")==0){ 
      
                    printback(&backhead);
            
            }
           
          else  if(strcmp(args[0],"exit")==0){
      int  childpidd=waitpid(-1,NULL,WNOHANG);
      printf("%d",childpidd);
          if( childpidd == 0 ){
         printf("There are background processes cant exit \n");
              }  
               else   exit(EXIT_SUCCESS);          
                   }
     else {
char* s = getenv("PATH");


char * arr=(char *)malloc(strlen(s)+1);
strcpy(arr,s);

arr[strlen(s)]='\0';
char * local;

char *path=strtok (arr, ":");//path variablei ayırmak için
while (path != NULL)  {
   int newSize = strlen(arr)  + strlen(path) + 2; 
   char * newBuffer = (char *)malloc(newSize);

    strcpy(newBuffer,path);
     strcat(newBuffer,"/"); //buldugu yerde okudugu commentle birlestiriyo
   strcat(newBuffer,args[0]); 
  
     path= strtok (NULL, ":");
   if (access(newBuffer,X_OK)==0){
   	
   int childpid=fork();
    if(childpid==0){
     	execv(newBuffer,args);} 
if(background==1){
     waitpid(-1,NULL,WNOHANG);
     char arg[MAX_LINE/2 + 1];
int d=0;
while(args[d]!=NULL){
     strcpy(arg,args[d]);
d++;}
  insertback(&backhead,childpid,job,arg);
job++;
}
else{
wait(NULL);
}
   break;
}

  free(newBuffer); }

s=NULL;
}  
    
}}
  


