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

struct physicsData myData;

PathMap pm;

bool takePx;
//bool isMoving; //1 si la voiture roule et 0 sinon
bool inCharge;

int main() {

	init();

	return 0;
}

void * cameraControl_worker(void * data) {
    int delta, x, y;
        while(1)        {
                pthread_mutex_lock(&mutDataCurrPos);
                x = myData.currPos.x;
                y = myData.currPos.y;
                pthread_mutex_unlock(&mutDataCurrPos);
                while (delta < 10) {
                        pthread_mutex_lock(&mutDataCurrPos);
                        delta = sqrt(pow((myData.currPos.x - x), 2) + pow((myData.currPos.y - y), 2));
                        pthread_mutex_unlock(&mutDataCurrPos);
                }
                takePx = true;
                delta = 0;
                }
        return NULL;
}
void * camera_worker(void * data) {
        rgb_t image;
        while(1)        {
                if (takePx) {
                        image = pm.takePhoto(myData.currPos);
                		pthread_mutex_unlock(&mutDataCurrPos);
                        takePx = false;        }
        }
        return NULL;
}

void * battery_worker(void * data) {

	float speed_local;

	cout << "battery" << endl;

	while(1)	{
		if (inCharge)	{
			pthread_mutex_lock(&mutDataBattLevel);
			myData.battLevel += COEFF_CHARGE;
			pthread_mutex_unlock(&mutDataBattLevel);
		}
		pthread_mutex_lock(&mutDataBattLevel);
		pthread_mutex_lock(&mutDataSpeed);
		myData.battLevel -= (COEFF_DECHARGE*myData.speed + CONST_DECHARGE);
		pthread_mutex_unlock(&mutDataSpeed);
		pthread_mutex_unlock(&mutDataBattLevel);
		cout << myData.battLevel << endl;

		sleep(1);
	}
	return NULL; 
}	


/******************************************************
* currPos_worker : met à jour la position en fonction *
* de la vitesse et de la l'angle					  *
******************************************************/
void * currPos_worker(void * data) {

	float deltaX, deltaY, dist;

	while (1) { 

		pthread_mutex_lock(&mutDataSpeed);
		dist = 1000 * (myData.speed * (PERIOD/3600));
		pthread_mutex_unlock(&mutDataSpeed);
		pthread_mutex_lock(&mutDataAngle);
		deltaX = dist * cos(myData.angle * PI / 180);
		deltaY = dist * sin(myData.angle * PI / 180);
		pthread_mutex_unlock(&mutDataAngle);
		pthread_mutex_lock(&mutDataCurrPos);
		myData.currPos.x += deltaX;	
		myData.currPos.y += deltaY;	
		pthread_mutex_unlock(&mutDataCurrPos);	

		// cout << "x :" << myData.currPos.x << " y : " << myData.currPos.y << endl;
	
		sleep(PERIOD); // period
	}
	/* return NULL; */ 
}	

void * angle_worker(void * data) {
	return NULL; 
}	

void * navControl_worker(void * data) {

	enum carState state = GOTO_DEST; // état initial à GOTO_DEST
	float batt;

	while(1) {
		switch(state) {
			case GOTO_DEST:
				pthread_mutex_lock(&mutDataBattLevel);
				batt = myData.battLevel;
				pthread_mutex_unlock(&mutDataBattLevel);
				if (batt <= 10) {
					state = BATT_LOW;
				} else {
					// 
				}
				break;
			case BATT_LOW:

				break;
			case CHARGING:

				break;
		}
	}
	

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
	myData.speed = 50;
	myData.angle = 90;
	myData.battLevel = 100;
	myData.currPos.x = 0;
	myData.currPos.y = 0;

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
	if ( pthread_create(&tid[0], NULL, cameraControl_worker, NULL ) < 0)
		printf("taskSpawn cameraControl_worker failed!\n");

	mySchedParam.sched_priority = 20;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[1], NULL, camera_worker, NULL ) < 0)
		printf("taskSpawn camera_worker failed!\n");

	mySchedParam.sched_priority = 20;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[2], NULL, battery_worker, NULL ) < 0)
		printf("taskSpawn battery_worker failed!\n");

	mySchedParam.sched_priority = 1;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[3], NULL, currPos_worker, NULL ) < 0)
		cout << "taskSpawn currPos_worker failed!" << endl;

	mySchedParam.sched_priority = 20;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[4], NULL, angle_worker, NULL ) < 0)
		printf("taskSpawn angle_worker failed!\n");
	
	mySchedParam.sched_priority = 20;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[5], NULL, navControl_worker, NULL ) < 0)
		printf("taskSpawn navControl_worker failed!\n");

	mySchedParam.sched_priority = 20;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[6], NULL, destControl_worker, NULL ) < 0)
		printf("taskSpawn destControl_worker failed!\n");

	mySchedParam.sched_priority = 1;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[7], NULL, battLow_worker, NULL ) < 0)
		printf("taskSpawn battLow_worker failed!\n");

	mySchedParam.sched_priority = 1;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[8], NULL, battHigh_worker, NULL ) < 0)
		printf("taskSpawn battHigh_worker failed!\n");

	mySchedParam.sched_priority = 20;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if ( pthread_create(&tid[9], NULL, display_worker, NULL ) < 0)
		printf("taskSpawn display_worker failed!\n");

	// join des threads
	for (i = 0; i < 10; ++i) {
		if (pthread_join(tid[i], NULL) < 0)
			printf("Join failed for thread %d\n", i);
	}
}

/* vim: set ts=4 sw=4 tw=90 noet :*/
