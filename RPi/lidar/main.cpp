#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <vector>
#include <queue>
#include "lidar.hpp"


//void *print(void *ptr);

int main()
{
	Lidar lidar;
	lidar.flush();
	char c = 0;
	std::cout << "-Press 1 to start the lidar" << std::endl;
	std::cout << "-Press 2 to stop the lidar" << std::endl;
	std::cout << "-Press 3 to check received datas" << std::endl;
	std::cout << "-Press 4 to check messages" << std::endl;
	std::cout << "-Press 5 to get available data" << std::endl;
	std::cout << "-Press 6 to get raw data" << std::endl;
	std::cout << "-Press 7 continuous test" << std::endl;
	std::cout << "-Press q to quit" << std::endl;
	while(c != 'q'){
		c = getchar();
		switch(c){
			case '1':
				lidar.start();
				break;
			case '2':
				lidar.stop();
				break;
			case '3':
				std::cout << "Received datas :" << std::endl;
				std::cout << ">Total of received bytes : " << (int)lidar.nbBytesReceivedTotal << std::endl;
				std::cout << ">Number of received messages : " << (int)lidar.nbMsgReceived << std::endl;
				std::cout << ">received bytes for last message : " << (int)lidar.nbBytesReceived << " / " << (int)lidar.currentMsgSize << std::endl;
				std::cout << "bufferRx = ";
				for(uint8_t i = 0; i < lidar.nbBytesReceivedTotal;i++){
					std::cout << (int)lidar.bufferRx[i] << "/";
				}
				std::cout << std::endl;
				break;
			case '4':
				lidar.checkMessages();
				break;
			case '5':
				lidar.getAvailableData();
				break;
			case '6':
				lidar.getRawPoint();
				break;
			case '7':
				for(int i = 0; i < 10000;i++){
					lidar.getRawPoint();
					lidar.flush();
					lidar.checkMessages();
					delay(1);
				}
				//lidar.getRawPoint();
				break;
			default:
				break;
		}
	}
	
    return 0;
}