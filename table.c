#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>


struct table_order
{
	int num_cust;
	int orders[5][11];
	int bill;
	bool table_active;
	bool terminate;
	bool hotel_active;
};

int main()
{
    pid_t pid;
    int tnum;
    int num_cust;
    
    
    printf("Enter Table Number:");
    scanf("%d", &tnum);
    
    int wkey = ftok("admin.c", tnum+1000);
    if (wkey == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    
        
    int shm_size = sizeof(struct table_order);
    int shmid = shmget(wkey, shm_size, IPC_CREAT | 0666);

    if (shmid == -1)
   	{
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    struct table_order *shmadd = (struct table_order *)shmat(shmid, NULL, 0);
    if (shmadd == (struct table_order *)(-1))
        {
            perror("shmat");
            exit(EXIT_FAILURE);
        }
        
    if (!shmadd->hotel_active)
	{
		sleep(1);
	}

    
    shmadd->table_active=true;
    
    int cust_key = ftok("admin.c", tnum);
    if (cust_key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    
    shm_size = sizeof(int*);
    shmid = shmget(cust_key, shm_size, IPC_CREAT | 0666);

    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    int* customer= (int*)shmat(shmid, NULL, 0);
    if (customer == (int*)(-1))
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    shmadd->terminate=false;
    while(true)
    {
		printf("Enter Number of Customers at Table (maximum no. of customers can be 5):");
		scanf("%d", &num_cust);
			
		shmadd->num_cust=num_cust;
		
		if (num_cust==-1)
		{
			shmadd->terminate=true;
			shmadd->table_active=false;
			break;
		}
		
		FILE *file = fopen("menu.txt", "r");
	    	
	    char line[100];
	    
	    while (fgets(line, 100, file) != NULL) {
	    	printf("%s",line);
	    }
		
		int fd[num_cust][2];
		for (int i = 0; i < shmadd->num_cust; i++)
		{
		    if (pipe(fd[i]) == -1)
				{
			        perror("Pipe creation failed");
				    exit(EXIT_FAILURE);
				}
		}
		    
		
		
		for (int i=0;i<num_cust;i++)
		{
			pid=fork();
			if (pid < 0) {
				// Error occurred
				perror("fork failed");
				return 1;
			}
			else if (pid==0)
			{
				customer[i]=1;
				
				while(true)
				{
					if (i==0)
					{
						while(customer[num_cust-1]!=1)
							sleep(1);
						
					}
					else
					{
						while(customer[i-1]!=2)
							sleep(1);
					}
				
				
					printf("Enter the serial number(s) of the item(s) to order from the menu. Enter -1 when done: ");
				    close(fd[i][0]);

				    for (int j = 0; j < 10; j++)
				    {

				        int val;
				        scanf("%d", &val);

				        shmadd->orders[i][j] = val;

				        if (val == -1)
				            break;
				    }
				 
				    shmadd->orders[i][10]=-1;
				    	
				    
				    if (write(fd[i][1], shmadd->orders[i], sizeof(shmadd->orders[i])) == -1) {
						perror("Error writing to pipe");
						exit(EXIT_FAILURE);
					}

				    
				    
				    customer[i]=2;
				    while(customer[i]==2)
				    	sleep(1);
				    if(customer[i]==3)
				    {
				    	close(fd[i][1]);
				    	return 1;
				    }
				    
		        }
				
			}
			
		}
		
		while(true)
		{
			while(customer[num_cust-1]!=2)
				sleep(1);
			
			for(int i=0;i<num_cust;i++)
			{
				if (read(fd[i][0], shmadd->orders[i], sizeof(shmadd->orders[i])) == -1)
				   {
				        perror("Error reading from pipe");
				        exit(EXIT_FAILURE);
				   }
			}
			
			shmadd->table_active=false;
			while (!shmadd->table_active)
				    sleep(1);

				int result = shmadd->bill;

				for(int i=0;i<num_cust;i++)
		 		{
						if(result!=-1) customer[i]=3;
						else customer[i]=1;
				}
				
				if (result == -1)
				{
				    continue;
				}


				printf("The total bill amount is %d INR.\n", result);
				break;
		}
	}
	shmdt(customer);
	shmdt(shmadd);
    return 0;

}

