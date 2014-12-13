#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define COMMAND_SIZE 70
#define CWD_SIZE 140
#define MAX_PARAMETERS 5
 

pid_t pid;

int main(int argc, char **argv)
{

	printf("\n\nStartar shellJPH VERSION PERROR2.0\n\n");
	printf("-------------ENJOY------------------\n\n");
	
	char command_buf[COMMAND_SIZE];				/* Array att lägga in inmatningssträngen i */
	char *first_token, *next_token;				/* Tokens att spara delsträngar i */
	char *arguments[MAX_PARAMETERS];			/* Array med parametrarna */
	char *cwd;                                  /* Pekare till "current working directory" */
	char cwd_buf[CWD_SIZE];                     /* Array att lägga i "current working directory" */
	char *home;                                 /* Pekare till hemkatalogen */
	char *path;                                 /* Används för cd:s argmument "path" */
	int length;                                 /* Variabel för längden på strängar */
	int i;                                      /* While-loop variabel */
	int status;                                 /* Returvärde (status) från barnprocesserna */
    int return_value;                           /* Returvärde från funktioner ex: execvp */
	int ochTeckenTal;                           /* Hjälpvariabel för hantering av bakgrundsprocesser */

	int buf_counter;                            /* Hjälpvariabel Räknare */

	cwd = getcwd(cwd_buf, CWD_SIZE);            /* Sparar "current working directory" som getcwd (inbyggd funktion) returnerar  */
	printf("%s$ ", cwd);                        /* Skriver ut "current working directory" på terminalen */

	struct timeval start, end;                  /* ------------------------------------------- */
	int start_t, end_t;                         /* Variaber för att mäta processernas körtid   */
	double time_dif, start_time, end_time;      /* ------------------------------------------- */

	while(NULL != fgets(command_buf, COMMAND_SIZE, stdin))	/* Sparar inläst sträng (läses in med fgets (inbyggd funktion) från stdin (användarens input) */
	{
		buf_counter = 0;                                    /* Initialiterar räknare och hjälpvariabler */
		ochTeckenTal = 0;
        
		while(1)                                            /* Loopar igenom den inmatade bufferten */
		{
			if(command_buf[buf_counter] == '\n')            /* Om vi i bufferten har "newline" tecken (vilket vi har då användarne trycker enter */
			{
				command_buf[buf_counter] = '\0';            /* Byter ut "newline" tecknet mot \0 för att kunna köra execvp (se längre ner) */
				break;
			}
			buf_counter++;
		}
        
		first_token = strtok(command_buf, " ");             /* Sparar första delsträngen i inmatningen som genereras av strtok (inbyggd funktion) */
		if(first_token == NULL)                             /* Om delsträngen är NULL */
		{
                                                            /* Gör vi ingenting */
		}
        
		else if(0 == strncmp(first_token, "exit", 4))       /* Kollar om "exit" finns i första delsträngen */
		{
			exit(0);                                        /* Programmet avslutas normalt */
		}
        
		else if(0 == strncmp(first_token, "cd", 2))         /* Jämför första delsträngen med "cd" */
		{		
			
        /* ------------------------ Hanterar _ _ cd _ _ och cd ----------------------- */
			
            next_token = strtok(NULL, " ");			/* Nästa delsträng */
			
			if(next_token == NULL)					/* Bara cd, inget mer */
			{
				home = getenv("HOME");				/* Hämta HOME variablen. */	
				chdir(home);						/* Byt katalog till hemkatalogen */
			}
            
		/* ------------------------- Hanterar cd dir ----------------------------------*/
		
            while(next_token != NULL)               /* Så länge der finns fler arg. */
			{
				chdir(next_token);                  /* Byt katalog till det arg. */
				next_token = strtok(NULL, " ");     /* Hämta nästa arg */
			}
		}
        
		/* ----- Här hanterar vi resten av alla världens kommandon (ex: ls -l, pwd, et.c) -------*/
		else
		{
			next_token = strtok(NULL, " ");         /* Hämtar (parsar) nästa delsträng */
			arguments[0] = first_token;             /* Tillderlar argumentarrayen sina bas delsträngar */
			arguments[1] = next_token;
            
			if(next_token != NULL)                  /* Så längde delsträngen inte är tom */
			{
				
				
				if(0 == strncmp(next_token, "&", 1))    /* Om nästa delsträng är "&" */
				{
					ochTeckenTal = 1;					/* , 1-ställ hjälpvariabeln  */
					arguments[1] = NULL;                /* och NULL-ställ första argumentet i arrayen.*/
				}
                
                                                /* Om de finns fler delsträngar att läsas in så gör det*/
                i = 2;
				while((arguments[i] = strtok(NULL, " ")) != NULL && i < MAX_PARAMETERS)
				{
					i++;	
				}
			}
            
            
			start_t = gettimeofday(&start, NULL);	/* Klockar och sparar tiden i variabeln start */

			pid = fork();                           /* Skapar en ny process */
            
			if(-1 == pid)                           /* Fork misslyckades */
			{
				perror("Fork misslyckades");        /* Skriv ut felmeddelande */
				exit(1);                            /* Skicka statut onormalt avslut */
			}
            
			if(0 == pid)                            /* Barnet */
			{
                /* Här körs endast barnprocessens kod */
                
				if(1 == ochTeckenTal)               /* Kollar om det är en bakgrundsprocess */
				{
					printf("Gav upphov till en bakgrundsprocess pid: %d\n", getpid());
				}
				else
				{
					printf("Gav upphov till en förgrundsprocess pid %d\n", getpid());
				}
                
				return_value = execvp(arguments[0], arguments); /* Exekverar */
                
                if (-1 == return_value) {                       /* Execvp misslyckades */
                    perror("Kunde inte köra execvp");
                    exit(1);
                }
			}
            
			/*
			 *Om tecknet var 1-ställt tidigare så förekom det ett &-tecken, annars
			 *vänta på barnprocessen, klocka tiden och spara i slutvariabeln,
       		 *skriva ut rätt värden på tiderna
			 */
			if(1 != ochTeckenTal)
			{
				wait(&status);
				end_t = gettimeofday(&end, NULL);

				unsigned int start_sec = start.tv_sec;
				unsigned int start_usec = start.tv_usec;
				unsigned int end_sec = end.tv_sec;
				unsigned int end_usec = end.tv_usec;

				if(start_t == 0 && end_t == 0)
				{
					start_time = start_sec + (start_usec/100000.0);
					end_time = end_sec + (end_usec/100000.0);
					time_dif = (end_time - start_time);
					printf("Förgrundsprocess %d terminerade\nwallclock time: %.3f msec\n", pid, time_dif*1000);
				}
			}
			/* Föräldern sover för snyggare utskrift */
			usleep(10);
		}
		cwd = getcwd(cwd_buf, CWD_SIZE);
		printf("%s$ ", cwd);
	}
	perror("fgets\n");
	exit(1);
}
