#include "lidar.hpp"

Lidar::Lidar(){
	fd = wiringPiSPISetup(CHANNEL, SPI_FREQUENCY);
}
Lidar::~Lidar(){
	
}
void Lidar::start(){
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_START;
	sendSPI(buffer,1);
}
void Lidar::stop(){
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_STOP;
	sendSPI(buffer,1);
}
void Lidar::getAvailableData(){
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_GET_DATA_AVAILABLE;
	sendSPI(buffer,1);
}
void Lidar::getRawPoint(){
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_GET_RAW_POINT;
	sendSPI(buffer,1);
}
void Lidar::getDetectedPoints(){
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_GET_DETECTED_POINTS;
	sendSPI(buffer,1);
}
void Lidar::setSpeed(uint8_t speed){	//change rotation speed
	uint8_t buffer[2];
	buffer[0] = LIDAR_CMD_SET_SPEED;
	buffer[1] = speed;
	sendSPI(buffer,2);
}
void Lidar::getSpeed(){	//change rotation speed
	uint8_t buffer[1];
	buffer[0] = LIDAR_CMD_GET_SPEED;
	sendSPI(buffer,1);
}

void Lidar::sendSPI(uint8_t *buf, uint8_t bufSize){	//add size & checksum
	uint8_t b[1];
	b[0] = bufSize + 1;
	uint8_t checksum = b[0];
	//wiringPiSPIDataRW(CHANNEL, b, 1);
	sendReceiveSPI(b[0]);
	delayMicroseconds(SPI_DELAY);
	for(int i = 0; i < bufSize; i++){
		b[0] = buf[i];
		checksum += buf[i];
		//wiringPiSPIDataRW(CHANNEL, b, 1);
		sendReceiveSPI(b[0]);
		delayMicroseconds(SPI_DELAY);
	}
	b[0] = checksum;
	//wiringPiSPIDataRW(CHANNEL, b, 1);
	sendReceiveSPI(b[0]);
	delayMicroseconds(SPI_DELAY);
}
void Lidar::sendReceiveSPI(uint8_t data){	//send & handle response
	//std::cout << "sent : " << (int)data << std::endl;
	uint8_t buffer[1];
	buffer[0] = data;
	wiringPiSPIDataRW(CHANNEL, buffer, 1);
	if(receivingMsg){
		bufferRx[iRxIn] = buffer[0];
		iRxIn++;
		if(iRxIn == SIZE_BUFFER_RX){
			iRxIn = 0;
		}
		nbBytesReceived++;
		nbBytesReceivedTotal++;
	}
	else{
		if(buffer[0] != 0){
			currentMsgSize = buffer[0];
			bufferRx[iRxIn] = currentMsgSize;
			iRxIn++;
			if(iRxIn == SIZE_BUFFER_RX){
				iRxIn = 0;
			}
			receivingMsg = true;
			nbBytesReceived++;
			nbBytesReceivedTotal++;
		}
	}
	if(nbBytesReceived == currentMsgSize && receivingMsg){	//message received
		receivingMsg = false;
		nbMsgReceived++;
		nbBytesReceived = 0;
	}
}
void Lidar::flush(){
	uint8_t buffer[1];
	for(int i = 0; i < 100; i++){
		buffer[0] = 0;
		sendReceiveSPI(buffer[0]);
		delay(1);
	}
}
void Lidar::checkMessages(){
	while(nbMsgReceived > 0){
		nbMsgReceived--;
		uint8_t msgSize = bufferRx[iRxOut];
		iRxOut++;
		if(iRxOut == SIZE_BUFFER_RX){
			iRxOut = 0;
		}
		uint8_t buf[msgSize];
		buf[0] = msgSize;
		for(int i = 1; i < msgSize; i++){
			buf[i] = bufferRx[iRxOut];
			iRxOut++;
			if(iRxOut == SIZE_BUFFER_RX){
				iRxOut = 0;
			}
		}
		uint8_t checksum = 0;
		for(int i = 0; i < msgSize-1; i++){
			checksum += buf[i];
		}
			
		if(checksum != buf[msgSize-1]){
			std::cout << "CHECKSUM ERROR ! (msgSize = " << (int)msgSize << " & iRxOut = " << (int)iRxOut << ")" << std::endl;
		}
		else{	//Checksum ok
			switch(buf[1]){	//type of msg
				case LIDAR_RET_DEBUG_DEBUG:
					std::cout << "Debug : debug received" << std::endl;
					break;
				case LIDAR_RET_DEBUG_START:
					std::cout << "Debug : Start received" << std::endl;
					break;
				case LIDAR_RET_DEBUG_STOP:
					std::cout << "Debug : Stop received" << std::endl;
					break;
				case LIDAR_RET_DATA_AVAILABLE:
					std::cout << "Data available = " << (int)buf[2]  << std::endl;
					break;
				case LIDAR_RET_RAW_POINT:{
					float distance;
					float *dPtr = &distance;
					uint8_t *ptr = (uint8_t*)dPtr;
					ptr[0] = buf[2];
					ptr[1] = buf[3];
					ptr[2] = buf[4];
					ptr[3] = buf[5];
					float angle;
					dPtr = &angle;
					ptr = (uint8_t*)dPtr;
					ptr[0] = buf[6];
					ptr[1] = buf[7];
					ptr[2] = buf[8];
					ptr[3] = buf[9];
					
					uint8_t quality = buf[10];
					/*for(int i = 2; i < 10; i++){
						std::cout << "buf["<<i<<"] : " << (int)buf[i] << " / ";
					}*/
					std::cout << "Distance : " << distance << " & angle : " << angle << "& quality : " << (int)quality << std::endl;
					break;}
				case LIDAR_RET_DETECTED_POINTS:{
					uint8_t s = buf[0];
					uint8_t nbPoints = s/8;
					//std::cout << "s : " << (int)s << " & nbPoints : " << (int)nbPoints << std::endl;
					for(int i =0; i < nbPoints; i++){
						float x,y;
						float *dPtr = &x;
						uint8_t *ptr = (uint8_t*)dPtr;
						ptr[0] = buf[i*8+2];
						ptr[1] = buf[i*8+3];
						ptr[2] = buf[i*8+4];
						ptr[3] = buf[i*8+5];
						dPtr = &y;
						ptr = (uint8_t*)dPtr;
						ptr[0] = buf[i*8+6];
						ptr[1] = buf[i*8+7];
						ptr[2] = buf[i*8+8];
						ptr[3] = buf[i*8+9];
						/*for(int i = 2; i < 10; i++){
							std::cout << "buf["<<i<<"] : " << (int)buf[i] << " / ";
						}*/
						//std::cout << "x : " << x << " & y : " << y << std::endl;
						std::cout << x << "," << y << std::endl;
					}
					
					break;}
				case LIDAR_RET_SPEED:{
					float speed;
					float *dPtr = &speed;
					uint8_t *ptr = (uint8_t*)dPtr;
					ptr[0] = buf[2];
					ptr[1] = buf[3];
					ptr[2] = buf[4];
					ptr[3] = buf[5];
					for(int i = 2; i < 6; i++){
						std::cout << "buf["<<i<<"] : " << (int)buf[i] << " / ";
					}
					std::cout << "speed : " << speed << std::endl;
					break;}
			}
		}
	}
}
/*void Lidar::initPos(double x, double y, double t){
    setVarDouble64b(CODE_VAR_X_LD,x);
    setVarDouble64b(CODE_VAR_Y_LD,y);
    setVarDouble64b(CODE_VAR_T_LD,t);
}
void Lidar::setVarDouble64b(uint8_t varCode, double Var){
    double *ptrVar = &Var;
    uint8_t *ptr = (uint8_t*)ptrVar;
    uint8_t buffer[RX_SIZE_SET_64b + 1];
    buffer[0] = RX_SIZE_SET_64b;
    buffer[1] = RX_CODE_SET;
    buffer[2] = varCode;
    buffer[3] = VAR_LD_64b;
    buffer[4] = ptr[0];
    buffer[5] = ptr[1];
    buffer[6] = ptr[2];
    buffer[7] = ptr[3];
    buffer[8] = ptr[4];
    buffer[9] = ptr[5];
    buffer[10] = ptr[6];
    buffer[11] = ptr[7];
    buffer[12] = 0;
    for(int i = 0; i < RX_SIZE_SET_64b; i++){
        buffer[12] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_SET_64b + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
void Lidar::setVar32(uint8_t varCode, uint32_t var){

    uint8_t buffer[RX_SIZE_SET_32b + 1];
    buffer[0] = RX_SIZE_SET_32b;
    buffer[1] = RX_CODE_SET;
    buffer[2] = varCode;
    buffer[3] = VAR_32b;
    buffer[4] = (uint8_t)(var >> 24);
    buffer[5] = (uint8_t)(var >> 16);
    buffer[6] = (uint8_t)(var >> 8);
    buffer[7] = (uint8_t)(var & 0xFF);
    buffer[8] = 0;
    for(int i = 0; i < RX_SIZE_SET_32b; i++){
        buffer[8] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_SET_32b + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
void Lidar::setVar8(uint8_t varCode, uint8_t var){

    uint8_t buffer[RX_SIZE_SET_8b + 1];
    buffer[0] = RX_SIZE_SET_8b;
    buffer[1] = RX_CODE_SET;
    buffer[2] = varCode;
    buffer[3] = VAR_8b;
    buffer[4] = var;
    buffer[5] = 0;
    for(int i = 0; i < RX_SIZE_SET_8b; i++){
        buffer[5] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_SET_8b + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
void Lidar::getVar(uint8_t varCode){
    uint8_t buffer[RX_SIZE_GET + 1];
    buffer[0] = RX_SIZE_GET;
    buffer[1] = RX_CODE_GET;
    buffer[2] = varCode;
    buffer[3] = 0;
    for(int i = 0; i < RX_SIZE_GET; i++){
        buffer[3] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_GET + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
std::string Lidar::async_read(){
    std::string s("");
    while (serialDataAvail(fd)){
      s += serialGetchar(fd);
    }
    return s;
}*/