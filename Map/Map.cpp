/*
 * Map.cpp
 *
 *  Created on: Feb 8, 2022
 *      Author: Amin-PC
 */

#include <Buffer/Map.h>

namespace Memory {


std::vector<__RequestPackage> Map::pop(uint8_t tag, __TagTypedef type){

	std::vector<__RequestPackage> vect;

	setStatus(__Status::BUSY);

	switch(type){
	case __TagTypedef::RECEIVER:
		vect= popReceiverTag(tag);
		break;
	case __TagTypedef::TRANMITTER:
		vect= popTransmitterTag(tag);
		break;
	case __TagTypedef::TARGET:
		vect= popTargetTag(tag);
		break;
	}
	setStatus(__Status::READY);
	return vect;
}


__Status Map::insert(std::vector<__RequestPackage> request){

	if (status!=__Status::READY)
		return __Status::BUSY;



	setStatus(__Status::BUSY);
	for(std::size_t idx=0;idx<request.size();++idx)
		buffer.push_back(request[idx]);

	setStatus(__Status::READY);

	return __Status::OK;
}



bool Map::isEmpty(){
	return buffer.empty();
}


std::size_t Map::getSize(){
	return buffer.size();
}


__Status Map::getStatus(){
	return status;
}


void Map::setStatus(__Status status){
	this->status=status;
}


std::vector<__RequestPackage> Map::popTransmitterTag(uint8_t tag){

	std::vector<__RequestPackage> vect;
	for(std::size_t idx=0;idx<buffer.size();idx++){
			if(buffer[idx].tag.tranmitter==tag){
				vect.push_back(buffer[idx]);
				buffer.erase(buffer.begin()+idx);
			}
		}
	return vect;
}

std::vector<__RequestPackage> Map::popReceiverTag(uint8_t tag){

	std::vector<__RequestPackage> vect;
	for(std::size_t idx=0;idx<buffer.size();idx++){

				if(buffer[idx].tag.receiver==tag){
					vect.push_back(buffer[idx]);
					buffer.erase(buffer.begin()+idx);
				}
			}
	return vect;
}
std::vector<__RequestPackage> Map::popTargetTag(uint8_t tag){

	std::vector<__RequestPackage> vect;
	for(std::size_t idx=0;idx<buffer.size();idx++){
		if(buffer[idx].tag.target==tag){
			vect.push_back(buffer[idx]);
			buffer.erase(buffer.begin()+idx);
		}
	}
	return vect;
}
} /* namespace Memory */
