#ifndef WEB_H
#define WEB_H

#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <string.h>
#include <sstream>
#include <math.h>
#include <unistd.h>
#include "dspic.hpp"

#define DEBUG_PID	0

#define COEF_SCALE_PID	1000000

struct floatPoint{
  float x;
  float y;
};

void* thread_HandleConnnection(void *threadid);
std::string simulateResponse(double i);
class Web
{
    public:
        Web(DsPIC *ds);
        virtual ~Web();
        bool acceptClient();
		void closeClient();
        bool sendMsg(std::string message);
		std::string receiveMsg();
        bool startThread();
		
        std::string s;
		DsPIC *dspic;
		bool waitingResponsePID = false;
		void addLidarPoints(float x, float y);
		void addLidarPoints(floatPoint fp);
		void addLidarPoints(std::vector<floatPoint> vect_fp);
		void clearLidarPoints();
		bool m_clearLidarPoints = false;
		bool m_radarScan = true;
		std::queue<floatPoint> lidarPoints;
    protected:
        int socket_listen;
        int socket_client;
        pthread_t threads;
		
    private:
};

std::string realResponse(Web *w);

#endif // WEB_H
