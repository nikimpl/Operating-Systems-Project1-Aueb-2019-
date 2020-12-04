#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <string.h>
#include "res1.h"

//total available seats
int seats = Nseat;
//available telephones
int tele = Ntel;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *Client(void *a) {

    int *tid = (int *)a;
	
	waiting_time = (rand_r(&seedp)%Tseathigh) + Tseatlow;
	
	int tickets = (rand_r(&seedp)%Nseathigh) + Nseatlow;
	
	int success = ((rand_r(&seedp) % 10)+1);
	printf("%d : success %d\n",*tid, success);
	
	int rc;
	
	//wait till tel is available
	while (tele == 0) {
		rc = pthread_cond_wait(&cond, &tel_mutex);
	}
	
	rc = pthread_mutex_lock(&tel_mutex);
	if (rc != 0) {	
		printf("ERROR in %d: Return code from pthread_mutex_lock(&tel_mutex) is %d\n",*tid, rc);
		pthread_exit(&rc);
	}
	tele--;
	
	//waiting waiting_time simulation
	sleep(waiting_time);
	rc = pthread_mutex_lock(&time_mutex);
	if (rc != 0) {	
		printf("ERROR in %d: Return code from pthread_mutex_lock(&time_mutex) is %d\n",*tid, rc);
		pthread_exit(&rc);
	}
	total_waiting_time+=waiting_time;
	rc = pthread_mutex_unlock(&time_mutex);
	
	tele++;
	rc = pthread_mutex_unlock(&tel_mutex);
	rc = pthread_cond_signal(&cond);
	
	clock_gettime(CLOCK_REALTIME, &start);
	
	//initialization of Return
	for(int i=0;i<5;i++) {
		Return[i] = -1;
	}
	
	//full theater
	if(seats==0) {
		rc = pthread_mutex_lock(&tel_mutex);
		if (rc != 0) {	
			printf("ERROR in %d: Return code from pthread_mutex_lock(&tel_mutex) is %d\n",*tid, rc);
			pthread_exit(&rc);
		}
		tele--;
		
		rc = pthread_mutex_lock(&screen_mutex);
		if (rc != 0) {	
			printf("ERROR in %d: Return code from pthread_mutex_lock(&screen_mutex) is %d\n",*tid, rc);
			pthread_exit(&rc);
		}
		printf("%d: Your reservation was canceled because the theater is full.\n",*tid);
		rc = pthread_mutex_unlock(&screen_mutex);
		
		tele++;
		rc = pthread_mutex_unlock(&tel_mutex);
		rc = pthread_cond_signal(&cond);
		
		clock_gettime(CLOCK_REALTIME, &stop);
		
		rc = pthread_mutex_lock(&time_mutex);
		if (rc != 0) {	
			printf("ERROR in %d: Return code from pthread_mutex_lock(&time_mutex) is %d\n",*tid, rc);
			pthread_exit(&rc);
		}

        total_service_time = total_service_time + (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec);
		rc = pthread_mutex_unlock(&time_mutex);
		
		pthread_exit(NULL);
	}
	
	//check for available seats and reserve them
	rc = pthread_mutex_lock(&seats_mutex);
	if (rc != 0) {	
		printf("ERROR in %d: Return code from pthread_mutex_lock(&seats_mutex) is %d\n",*tid, rc);
		pthread_exit(&rc);
	}
	
	int j = 0;
	int t = tickets;
	for(int i=0;i<Nseat;i++) {
		if(t!= 0 && seats!=0) {
			if(Seats[i]==0) {
				Seats[i]=*tid;
				t--;
				seats--;
				Return[j] = i;
				j++;
			}
		}
		else break;
	}
	
	rc = pthread_mutex_unlock(&seats_mutex);
	
	//lack of available seats
	if(t!=0){
		for(int i =0;i<j;i++){
			Seats[Return[i]]=0;
			seats++;
		}
		
		rc = pthread_mutex_unlock(&seats_mutex);
		
		//wait till tel is available
		while (tele == 0) {
			rc = pthread_cond_wait(&cond, &tel_mutex);
		}
		
		rc = pthread_mutex_lock(&tel_mutex);
		if (rc != 0) {	
			printf("ERROR in %d: Return code from pthread_mutex_lock(&tel_mutex) is %d\n",*tid, rc);
			pthread_exit(&rc);
		}
		tele--;
		
		rc = pthread_mutex_lock(&screen_mutex);
		if (rc != 0) {	
			printf("ERROR in %d: Return code from pthread_mutex_lock(&screen_mutex) is %d\n",*tid, rc);
			pthread_exit(&rc);
		}
		printf("%d: Your reservation was canceled due to lack of available seats.\n",*tid);
		rc = pthread_mutex_unlock(&screen_mutex);
		
		tele++;
		rc = pthread_mutex_unlock(&tel_mutex);
		rc = pthread_cond_signal(&cond);
		
		clock_gettime(CLOCK_REALTIME, &stop);
		rc = pthread_mutex_lock(&time_mutex);
		if (rc != 0) {	
			printf("ERROR in %d: Return code from pthread_mutex_lock(&time_mutex) is %d\n",*tid, rc);
			pthread_exit(&rc);
		}
        total_service_time = total_service_time + (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec);
		rc = pthread_mutex_unlock(&time_mutex);
		pthread_exit(NULL);
	}
	
	//payment
	if(success <= Pcardsuccess) {
		
		cost = Cseat*tickets;
		
		//wait till tel is available
		while (tele == 0) {
			rc = pthread_cond_wait(&cond, &tel_mutex);
		}
		
		rc = pthread_mutex_lock(&tel_mutex);
		if (rc != 0) {	
			printf("ERROR in %d: Return code from pthread_mutex_lock(&tel_mutex) is %d\n",*tid, rc);
			pthread_exit(&rc);
		}
		tele--;
		
		rc = pthread_mutex_lock(&screen_mutex);
		if (rc != 0) {	
			printf("ERROR in %d: Return code from pthread_mutex_lock(&screen_mutex) is %d\n",*tid, rc);
			pthread_exit(&rc);
		}
		printf("%d: Your reservation is complete. Your transaction number is <%d>, your seats are <",*tid, *tid);
		for(int i=0;i<5;i++) {
		    if(Return[i]!=-1) {
		        printf(" %d ",Return[i]);
		    }
		}
		printf("> and your cost is <%d> euros.\n", cost);
		rc = pthread_mutex_unlock(&screen_mutex);
		
		tele++;
		rc = pthread_mutex_unlock(&tel_mutex);
		rc = pthread_cond_signal(&cond);
		
		rc = pthread_mutex_lock(&bank_mutex);
		if (rc != 0) {	
			printf("ERROR in %d: Return code from pthread_mutex_lock(&bank_mutex) is %d\n",*tid, rc);
			pthread_exit(&rc);
		}
		balance = balance + cost;
		rc = pthread_mutex_unlock(&bank_mutex);
	}
	//error during transaction
	else {
		//wait till tel is available
		while (tele == 0) {
			rc = pthread_cond_wait(&cond, &tel_mutex);
		}
		
		rc = pthread_mutex_lock(&tel_mutex);
		if (rc != 0) {	
			printf("ERROR in %d: Return code from pthread_mutex_lock(&tel_mutex) is %d\n",*tid, rc);
			pthread_exit(&rc);
		}
		tele--;
		
		rc = pthread_mutex_lock(&screen_mutex);
		if (rc != 0) {	
			printf("ERROR in %d: Return code from pthread_mutex_lock(&screen_mutex) is %d\n",*tid, rc);
			pthread_exit(&rc);
		}
		printf("%d: Reservation canceled. Error during transaction.\n", *tid);		
		rc = pthread_mutex_unlock(&screen_mutex);
		
		tele++;
		rc = pthread_mutex_unlock(&tel_mutex);
		rc = pthread_cond_signal(&cond);
		
		rc = pthread_mutex_lock(&seats_mutex);
		if (rc != 0) {	
			printf("ERROR in %d: Return code from pthread_mutex_lock(&seats_mutex) is %d\n",*tid, rc);
			pthread_exit(&rc);
		}
		for(int i=0;i<j;i++){
			Seats[Return[i]]= 0;
			seats++;
		}
		rc = pthread_mutex_unlock(&seats_mutex);
		
        clock_gettime(CLOCK_REALTIME, &stop);
		
		rc = pthread_mutex_lock(&time_mutex);
		if (rc != 0) {	
			printf("ERROR in %d: Return code from pthread_mutex_lock(&time_mutex) is %d\n",*tid, rc);
			pthread_exit(&rc);
		}
        total_service_time = total_service_time + (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec);
		rc = pthread_mutex_unlock(&time_mutex);
		pthread_exit(NULL);
	}

	tele++;
	rc = pthread_cond_signal(&cond);
	rc = pthread_mutex_unlock(&tel_mutex);
	
	clock_gettime(CLOCK_REALTIME, &stop);
	
	rc = pthread_mutex_lock(&time_mutex);
	if (rc != 0) {	
		printf("ERROR in %d: Return code from pthread_mutex_lock(&time_mutex) is %d\n",*tid, rc);
		pthread_exit(&rc);
	}
	total_service_time = total_service_time + (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec);
	rc = pthread_mutex_unlock(&time_mutex);

	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

	if (argc != 3) {
		printf("Wrong input.\n");
		exit(-1);
	}
	
	int Ncust = atoi(argv[1]);
	
	seedp = atoi(argv[2]);
	
	pthread_t *threads;
	
	threads = (pthread_t*)malloc(Ncust*sizeof(pthread_t));
	if(threads==NULL) {
		printf("Not enough memory.\n");
		return -1;
	}
	
	int rc;
	int countNcust[Ncust];
	int customerCount;
	
	//initialize mutexes
	rc = pthread_mutex_init(&tel_mutex, NULL);
	if (rc != 0) {	
		printf("Error occurred while initializing tel_mutex.");
		exit(rc);
	}
	rc = pthread_mutex_init(&bank_mutex, NULL);
	if (rc != 0) {	
		printf("Error occurred while initializing bank_mutex.");
		exit(rc);
	}
	rc = pthread_mutex_init(&seats_mutex, NULL);
	if (rc != 0) {	
		printf("Error occurred while initializing seats_mutex.");
		exit(rc);
	}
	rc = pthread_mutex_init(&screen_mutex, NULL);
	if (rc != 0) {	
		printf("Error occurred while initializing screen_mutex.");
		exit(rc);
	}
	rc = pthread_mutex_init(&time_mutex, NULL);
	if (rc != 0) {	
		printf("Error occurred while initializing time_mutex.");
		exit(rc);
	}
	
   	for(customerCount = 0; customerCount < Ncust; customerCount++) {
		countNcust[customerCount] = customerCount + 1;
		int *tid = &countNcust[customerCount];
		
    	rc = pthread_create(&threads[customerCount], NULL, Client,tid);

    	if (rc != 0) {
    		printf("ERROR in %d: return code from pthread_create() is %d\n",*tid, rc);
       		exit(-1);
       	}
    }
		
	void *status;
	int i = 0;
	for (i = 0; i < Ncust; i++) {
		rc = pthread_join(threads[i], &status);
		
		if (rc != 0) {
			printf("ERROR in %d: return code from pthread_join() is %d\n",tid, rc);
			exit(-1);	
		}
	}
	for(int i=0;i<Nseat;i++) {
		if(Seats[i]!=0) printf("Seat %d/ client %d\n", i, Seats[i]);
	}
	
	printf("Total earnings are %d.\n", balance);
	
	printf("Average waiting time is %f seconds.\n",total_waiting_time/Ncust);
	
	printf("Average service time is %f seconds.\n",(total_service_time/BILLION)/Ncust);
	
	//destroy all mutexes
	pthread_mutex_destroy(&tel_mutex);
	if (rc != 0) {	
		printf("Error occurred while destroying tel_mutex.");
		exit(rc);
	}
	pthread_mutex_destroy(&bank_mutex);
	if (rc != 0) {	
		printf("Error occurred while destroying bank_mutex.");
		exit(rc);
	}
	pthread_mutex_destroy(&seats_mutex);
	if (rc != 0) {	
		printf("Error occurred while destroying seats_mutex.");
		exit(rc);
	}
	pthread_mutex_destroy(&screen_mutex);
	if (rc != 0) {	
		printf("Error occurred while destroying screen_mutex.");
		exit(rc);
	}
	pthread_mutex_destroy(&time_mutex);
	if (rc != 0) {	
		printf("Error occurred while destroying time_mutex.");
		exit(rc);
	}
	
	pthread_cond_destroy(&cond);
	if (rc != 0) {	
		printf("Error occurred while destroying condition.");
		exit(rc);
	}
	
	free(threads);

	return 1;
}