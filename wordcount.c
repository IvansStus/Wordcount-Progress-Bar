/*
Ivan Stus - 821201908
Kenny Kieu - 819180993
CS 570-05
Spring 2021
Assignment #2 - p thread progress bar
Description - Real time printing of progress bar to determine word count inside a file 
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sched.h>
#include <unistd.h>
#include <ctype.h>

typedef struct {
    long * CurrentStatus; // number of bytes read
    long InitialValue;
    long TerminationValue; // Total number of bytes
    } PROGRESS_STATUS;

    void * progress_monitor( void * VoidPtr )
    {
        int x = 0;      //Printing Loop Counter
        int i = 0;      //Holds progressBar previous value
        int progressBar = 0;    //Amount of characters to be filled in
        long value = 0;     //holds current status
        int barCount;   //Tracks how many hyphens have been printed
        PROGRESS_STATUS *data = (PROGRESS_STATUS *)VoidPtr;     //Access struct from main
        while (progressBar != 50) {     
            value = (long)data->CurrentStatus/8;            
            progressBar = ((((float)value)/(float)data->TerminationValue) * 50);    //Determines how many hyphens to be printed
            if (i != progressBar) {     //progressBar has changed since last byte read                
                for (x = 0; x < progressBar - i; x++) {
                    if (barCount == 9) {    //Every 10, print a '+'                        
                        barCount = 0;
                        printf("+");
                        fflush(stdout);
                    } else {
                        printf("-");
                        fflush(stdout);
                        barCount++;
                    }
                }
                i = progressBar;
            }            
        }
        return NULL;       
    }

    void wordcount(char *argv) {       
        FILE *filename;
        filename = fopen(argv, "r");    //Open file        
        if (filename == NULL) {
            printf("could not open file\n");
        }
        PROGRESS_STATUS *varCounter = malloc (sizeof(PROGRESS_STATUS)); //Initialize struct        
        varCounter->CurrentStatus = 0;  //Reference and ititialize pointer
        struct stat sb;     //struct used to find terminationvalue       
        int count = 0;      //Holds word count of file      
        char c;
        char temp;          //Holds extra whitespace check  
        void * p;       //pthread_join pointer
        if (stat(argv, &sb) == -1) {        //Find total amount of bytes in file
            printf("Unable to get ternmination value\n");
        }
        else {
            varCounter->TerminationValue = (long long) sb.st_size;
        }
        pthread_t pt;   //Create p thread        
        pthread_create(&pt, NULL, progress_monitor, varCounter);
        while ((c = fgetc(filename)) != EOF) {                              
            if (isspace(c) != 0) {     //White space delimiter for word count
                if (isspace(temp) == 0) {
                    count++;    //Increase count for every word found
                }                
            }    
            temp = c;                
            varCounter->CurrentStatus = varCounter->CurrentStatus + 1;  //Every byte read increments current status                           
        }
        if (isspace(temp) == 0) {
            count++;
        }    
        pthread_join(pt, &p);   //After p thread finishes, wait and end          
        printf("\nThere are %d words in %s.", count, argv);    
    }

    int main(int argc, char *argv[]){
        if (argc == 2){
            wordcount(argv[1]);
        }
        else {
            printf("no file specified\n");
        }
        return 0;
    }    