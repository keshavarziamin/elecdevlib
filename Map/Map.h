/*
 * Map.h
 *
 *  Created on: Feb 8, 2022
 *      Author: Amin-PC
 */

#ifndef BUFFER_MAP_H_
#define BUFFER_MAP_H_

#include "status.h"
#include <map>
#include <vector>
#include <string>
#include "request/request.h"


namespace Memory {


class Map {
public:

	Map(){};
	virtual ~Map(){};

	__Status insert(std::vector<__RequestPackage>);
	std::vector<__RequestPackage> pop(uint8_t , __TagTypedef );
	__Status getStatus();
	void setStatus(__Status);
	std::size_t getSize();
	bool isEmpty();
	__Status clear();
private:
	std::vector<__RequestPackage> buffer;

	std::vector<__RequestPackage> popTransmitterTag(uint8_t );
	std::vector<__RequestPackage> popReceiverTag(uint8_t );
	std::vector<__RequestPackage> popTargetTag(uint8_t );

	__Status status=__Status::READY;
};

} /* namespace Memory */

#endif /* BUFFER_MAP_H_ */
