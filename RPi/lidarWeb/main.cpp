#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <vector>
#include <queue>
#include "lidar.hpp"


#include "web.hpp"
#include "dspic.hpp"


//void *print(void *ptr);

int main()
{
	DsPIC dspic;
    //pthread_t thread_print;

    dspic.async_read(); //flush rx buffer

	Web web(&dspic);
    web.startThread();
	
	Lidar lidar;
	lidar.flush();
	dspic.x = 1000;
	dspic.y = 1000;
	dspic.t = 0;
	char c = 0;
	std::cout << "-Press 1 to start the lidar" << std::endl;
	std::cout << "-Press 2 to stop the lidar" << std::endl;
	std::cout << "-Press 3 to check received datas" << std::endl;
	std::cout << "-Press 4 to check messages" << std::endl;
	std::cout << "-Press 5 to get available data" << std::endl;
	std::cout << "-Press 6 to get raw data" << std::endl;
	std::cout << "-Press 7 continuous test" << std::endl;
	std::cout << "-Press 8 to set speed" << std::endl;
	std::cout << "-Press 9 to get speed" << std::endl;
	std::cout << "-Press d to get detected points" << std::endl;
	std::cout << "-Press c to get detected points continuously" << std::endl;
	std::cout << "-Press p to print data on web interface" << std::endl;
	std::cout << "-Press w to clear data on web interface" << std::endl;
	std::cout << "-Press z to change scannning mode (radar/standard)" << std::endl;
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
				for(uint32_t i = 0; i < lidar.nbBytesReceivedTotal;i++){
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
				for(int i = 0; i < 20000;i++){
					lidar.getRawPoint();
					//lidar.flush();
					uint8_t buffer[1];
					for(int i = 0; i < 15; i++){
						buffer[0] = 0;
						lidar.sendReceiveSPI(buffer[0]);
						delayMicroseconds(SPI_DELAY);
					}
					lidar.checkMessages();
					delayMicroseconds(SPI_DELAY);
				}
				//lidar.getRawPoint();
				break;
			case '8':{
				int speed = -1;
				std::cin >> speed;
				if(speed < 0 || speed > 255){
					std::cout << "speed must be within [0;255] range" << std::endl;
				}
				else{
					lidar.setSpeed(speed);
					std::cout << "speed set to " << speed << std::endl;
				}
				break;}
			case '9':
				lidar.getSpeed();
				break;
			case 'd':
				lidar.getDetectedPoints();
				uint8_t buffer[1];
				for(int i = 0; i < 255; i++){
						buffer[0] = 0;
						lidar.sendReceiveSPI(buffer[0]);
						delayMicroseconds(SPI_DELAY);
					}
				//delay(100);
				lidar.checkMessages();
				break;
			case 'c':
				for(int i = 0; i < 50; i++){
					lidar.getDetectedPoints();
					uint8_t buffer[1];
					for(int i = 0; i < 255; i++){
							buffer[0] = 0;
							lidar.sendReceiveSPI(buffer[0]);
							delayMicroseconds(SPI_DELAY);
						}
					//delay(100);
					lidar.checkMessages();
					delay(10);
				}
				break;
			case 'p':
				for(int i = 0; i < 50; i++){
					lidar.getDetectedPoints();
					uint8_t buffer[1];
					for(int i = 0; i < 255; i++){
							buffer[0] = 0;
							lidar.sendReceiveSPI(buffer[0]);
							delayMicroseconds(SPI_DELAY);
						}
					//delay(100);
					std::vector<floatPoint> vect_fp = lidar.checkDetectedPoints();
					web.addLidarPoints(vect_fp);
					//lidar.checkMessages();
					delay(10);
				}
				//web.clearLidarPoints();
				break;
			case 'o':
				for(int i = 0; i < 1000; i++){
					lidar.getDetectedPoints();
					uint8_t buffer[1];
					for(int i = 0; i < 255; i++){
							buffer[0] = 0;
							lidar.sendReceiveSPI(buffer[0]);
							delayMicroseconds(SPI_DELAY);
						}
					std::vector<floatPoint> vect_fp = lidar.checkDetectedPoints();
					web.addLidarPoints(vect_fp);
					delay(5);
				}
				break;
			case 'w':
				web.clearLidarPoints();
				break;
			case 'z':
				web.m_radarScan = !web.m_radarScan;
				break;
			default:
				break;
		}
	}
	
    return 0;
}