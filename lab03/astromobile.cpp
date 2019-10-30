#include <iostream>
#include <sched.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <mqueue.h> 
#include <unistd.h> 
#include <pthread.h> 
#include <semaphore.h> 
#include <sys/types.h> 
#include <time.h>
#include <signal.h>
/* #include <sys/syspage.h> */ 
/* #include <sys/neutrino.h> */ 
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

#include "genMap.h"
#include "astromobile.h"

using namespace std;

struct physicsData my_data;

int main() {

	init();

	return 0;
}

void * cameraControl_worker(void * data) {
	return NULL; 
}	
void * camera_worker(void * data) {
	return NULL; 
}	
void * battery_worker(void * data) {
	return NULL; 
}	


/******************************************************
* currPos_worker : met à jour la position en fonction *
* de la vitesse et de la l'angle					  *
******************************************************/
void * currPos_worker(void * data) {

	float deltaX, deltaY, dist;
	cout << "test" << endl;

	while (1) { 

		pthread_mutex_lock(&mutDataSpeed);
		dist = 1000 * (my_data.speed * (PERIOD/3600));
		pthread_mutex_unlock(&mutDataSpeed);
		pthread_mutex_lock(&mutDataAngle);
		deltaX = dist * cos(my_data.angle * PI / 180);
		deltaY = dist * sin(my_data.angle * PI / 180);
		pthread_mutex_unlock(&mutDataAngle);
		pthread_mutex_lock(&mutDataCurrPos);
		my_data.currPos.x += deltaX;	
		my_data.currPos.y += deltaY;	
		pthread_mutex_unlock(&mutDataCurrPos);	

		cout << my_data.currPos.x << " " << my_data.currPos.y << endl;
	
		sleep(PERIOD); // period
	}
	/* return NULL; */ 
}	
void * angle_worker(void * data) {
	return NULL; 
}	
void * navControl_worker(void * data) {
	return NULL; 
}	
void * destControl_worker(void * data) {
	return NULL; 
}	
void * battLow_worker(void * data) {
	return NULL; 
}	
void * battHigh_worker(void * data) {
	return NULL; 
}	
void * display_worker(void * data) {
	return NULL; 
}	

void init()
{


	my_data.speed = 50;
	my_data.angle = 0;
	my_data.battLevel = 100;
	my_data.currPos.x = 0;
	my_data.currPos.y = 0;


	pthread_t tid[THREAD_NUM];
	pthread_attr_t attrib;
	struct sched_param mySchedParam;
//	struct mq_attr mqattr;
	int i; 

//    création des queues
//	memset(&mqattr, 0, sizeof(mqattr));
//	mqattr.mq_maxmsg = MAX_NUM_MSG;
//	mqattr.mq_msgsize = sizeof(message) ;
//
//	mq_unlink(MQ_CAM) ;
//	if ((msgQCam = mq_open(MQ_CAM, O_RDWR|O_CREAT, 0664,
//			&mqattr)) < 0){
//		printf("msgQCreate MQ_CAM échouée!\n");
//		cout << errno << " " << strerror(errno);
//		return;
//	}
//	mq_unlink(MQ_NAV) ;
//	if ((msgQNav = mq_open(MQ_NAV, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR,
//			&mqattr)) < 0){
//		printf("msgQCreate MQ_NAV échouée!\n");
//		return;
//	}
//
//	mq_unlink(MQ_BATT) ;
//	if ((msgQBatt = mq_open(MQ_BATT, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR,
//			&mqattr)) < 0){
//		printf("msgQCreate MQ_BATT échouée!\n");
//		return;
//	}

	// creation des threads
	//setprio(0,20);
	pthread_attr_init (&attrib);
	pthread_attr_setinheritsched (&attrib, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy (&attrib, SCHED_FIFO);

	mySchedParam.sched_priority = 20;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[0], &attrib, cameraControl_worker, NULL ) < 0)
		printf("taskSpawn cameraControl_worker failed!\n");

	mySchedParam.sched_priority = 20;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[1], &attrib, camera_worker, NULL ) < 0)
		printf("taskSpawn camera_worker failed!\n");

	mySchedParam.sched_priority = 20;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[2], &attrib, battery_worker, NULL ) < 0)
		printf("taskSpawn battery_worker failed!\n");

	mySchedParam.sched_priority = 1;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[3], &attrib, currPos_worker, NULL ) < 0)
		cout << "taskSpawn currPos_worker failed!" << endl;

	mySchedParam.sched_priority = 20;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[4], &attrib, angle_worker, NULL ) < 0)
		printf("taskSpawn angle_worker failed!\n");
	
	mySchedParam.sched_priority = 20;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[5], &attrib, navControl_worker, NULL ) < 0)
		printf("taskSpawn navControl_worker failed!\n");

	mySchedParam.sched_priority = 20;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[6], &attrib, destControl_worker, NULL ) < 0)
		printf("taskSpawn destControl_worker failed!\n");

	mySchedParam.sched_priority = 1;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[7], &attrib, battLow_worker, NULL ) < 0)
		printf("taskSpawn battLow_worker failed!\n");

	mySchedParam.sched_priority = 1;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[8], &attrib, battHigh_worker, NULL ) < 0)
		printf("taskSpawn battHigh_worker failed!\n");

	mySchedParam.sched_priority = 20;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[9], &attrib, display_worker, NULL ) < 0)
		printf("taskSpawn display_worker failed!\n");

	// join des threads
	for (i = 0; i < 10; ++i) {
		if (pthread_join(tid[i], NULL) < 0)
			printf("Join failed for thread %d\n", i);
	}
}

/* vim: set ts=4 sw=4 tw=90 noet :*/
