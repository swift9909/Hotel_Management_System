#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>

int main()
{
    key_t permit = ftok("admin.c",9909);
    int shm_size = 1024;
    int shmid = shmget(permit, shm_size, IPC_CREAT | 0666);

    if (shmid == -1)
    {
    	perror("shmget");
        exit(EXIT_FAILURE); 
    }

    bool *permission = (bool*)shmat(shmid, NULL, 0);
    *permission=true;
    char decision;
	while(true)
	{
		
		printf("Do you want to close the hotel? Enter Y for Yes and N for No.");
		scanf(" %c",&decision);
		
		if (decision=='Y')
		{
			*permission=false;
			break;
		}
	}
	
	return 0;
}

