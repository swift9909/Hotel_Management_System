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

struct hotel
{
	int bill;
	bool bill_sent;
	bool waiter_active;
	bool hotel_active;
};


int main()
{
	int wid;
	printf("Enter Waiter ID:");
	scanf("%d",&wid);
	
	key_t key_manager=ftok("admin.c", 5000 + wid ); 
        int shm_size=sizeof(struct hotel*); 
	int shmid = shmget(key_manager, shm_size, IPC_CREAT | 0666);

	if (shmid == -1)
	{
		perror("shmget");
       	exit(EXIT_FAILURE);
	}

	struct hotel *shmadd_manager = (struct hotel *)shmat(shmid, NULL, 0);
	if (shmadd_manager == (struct hotel *)(-1))
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
	
	if (!shmadd_manager->hotel_active)
	{
		sleep(1);
	}
		
    shmadd_manager->waiter_active=true;
		
	int key = ftok("admin.c",1000+ wid);
		
	char menu_file_path[] = "menu.txt";
	
    FILE *file = fopen(menu_file_path, "r");
    	
    char line[100];
    int prices[10];
    int price_count = 0;
    // Extract prices
    while (fgets(line, 100, file) != NULL && price_count < 10) {
        // Skip numeric prefix
        
        char *item_start = strchr(line, '.');
        if (item_start != NULL) {
            item_start++;  // Move past the dot
            int scanned = sscanf(item_start, "%*[^0-9]%d INR", &prices[price_count]);
            if (scanned == 1) {
                price_count++;
            } else {
                fprintf(stderr, "Error parsing line: %s", line);
            }
        } else {
            fprintf(stderr, "Error parsing line: %s", line);
        }
    }

        
        
	
    shm_size = sizeof(struct table_order); 
    shmid = shmget(key, shm_size, IPC_CREAT | 0666);

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
        
    
    //shmadd->table_active=true;
    while(true)
        {
        	while (shmadd->table_active)
		    sleep(1);
		    if (shmadd->terminate)
        	    	break;
        	 
		    int bill=0;
		    
		    for(int i=0; i<shmadd->num_cust; i++)
		    {
		    	if (bill==-1)
		    		break;
		    	for(int j=0; j<11; j++)
		    	{
		    		if(shmadd->orders[i][j]==-1)
		    		{
		    			break;
		    		} 
		    		else if(shmadd->orders[i][j]<1 || shmadd->orders[i][j]>price_count)
		    		{
		    			bill=-1;
		    			break;
		    		} 
		    		else
		    		{
		    			
		    			bill+=prices[shmadd->orders[i][j]-1];
		    		}
		    	}
		    }
		    
		    if (bill!=-1)
		    {	
			    printf("Bill Amount for Table X: %d INR\n", bill);   
			    shmadd_manager->bill=bill;
			    shmadd_manager->bill_sent=true;
		    }
		    
		    	    
		    shmadd->bill=bill;
		    shmadd->table_active=true;

        }
        
        shmadd_manager->waiter_active=false;
        
        shmdt(shmadd);
        shmdt(shmadd_manager);
   	return 0;
	
	
}
        

