#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>

struct hotel
{
    int bill;
    bool bill_sent;
    bool waiter_active;
};

int main()
{
    int num_tables;
    printf("Enter the Total Number of Tables at the Hotel:");
    scanf("%d", &num_tables);

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
    
    if (!(*permission))
    {
		shmdt(permission);
		return 0;
    }
    
    key_t wkeys[num_tables];
    struct hotel *shmadd[num_tables];
	
    for (int i = 0; i < num_tables; i++)
    {
        wkeys[i] = ftok("admin.c", 5000 + i + 1);
        int shm_size = sizeof(struct hotel *);
        int shmid = shmget(wkeys[i], shm_size, IPC_CREAT | 0666);

        if (shmid == -1)
        {
            perror("shmget");
            exit(EXIT_FAILURE);
        }

        struct hotel *shmadd_i = (struct hotel *)shmat(shmid, NULL, 0);
        shmadd[i] = shmadd_i;
    }
    	
	
    FILE *file = fopen("earnings.txt", "w");
    while (true)
    {
    
    	int inactive[num_tables];
    	for(int i=0;i<num_tables;i++) inactive[i]=0;
    	int inactive_count=0;
        for (int i = 0; i < num_tables; i++)
        { 
       		if (!(*permission) && !shmadd[i]->waiter_active)
       		{
       			if (inactive[i]==0)
       				inactive_count++;
       			inactive[i]=1;
       			if (inactive_count==num_tables)
       			{
       				fclose(file);
       				
       				file = fopen("earnings.txt", "r");
       				
					char line[100];
					int total_bills=0;
					// Extract prices
					while (fgets(line, 100, file) != NULL) {
						// Check if the line starts with "Earning from Table"
						if (strstr(line, "Earning from Table") != NULL) {
							int current_price;
							
							int scanned = sscanf(line, "Earning from Table %*d: %d INR", &current_price);
							if (scanned == 1) {
								total_bills += current_price;
							} else {
								fprintf(stderr, "Error parsing line: %s", line);
							}
						} else {
							fprintf(stderr, "Error parsing line: %s", line);
						}
					}
					
					int wage=(int)(total_bills*0.4);
					int profit=(int)(total_bills*0.6);
					
					printf("Total Earnings of Hotel: %d INR\n",total_bills);
					printf("Total Wages of Waiters: %d INR\n",wage);
					printf("Total Profit: %d INR\n",profit);
					
					printf("Thank you for visiting the Hotel!\n");
					
					fclose(file);
					
					file = fopen("earnings.txt", "a");
					
					char txt[100];
		            sprintf(txt, "Total Earnings of Hotel: %d INR", total_bills);
		            fprintf(file, "%s\n", txt);
		            
		            sprintf(txt, "Total Wages of Waiters: %d INR", wage);
		            fprintf(file, "%s\n", txt);
					
					sprintf(txt, "Total Profit: %d INR", profit);
		            fprintf(file, "%s\n", txt);
		            
					fflush(file);
					
					
					shmdt(permission);
					return 0;
				}
       		}
       		
            if (shmadd[i]->bill_sent)
            {
                char txt[100];
                sprintf(txt, "Earning from Table %d: %d INR", i + 1, shmadd[i]->bill);
                fprintf(file, "%s\n", txt);
				fflush(file);
                shmadd[i]->bill_sent = false;
            }
            
        }

        
        sleep(1); 
    }
    fclose(file);
    shmdt(permission);
    return 0;
}


