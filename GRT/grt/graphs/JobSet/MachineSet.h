/*
 * MachineSet.h
 *
 *  Created on: Jan 24, 2013
 *      Author: jheulot
 */

#ifndef MACHINESET_H_
#define MACHINESET_H_

class MachineSet {
private:
	unsigned int start;
	unsigned int count;

public:
	MachineSet(){start = count = 0;}
	virtual ~MachineSet(){}

	void setMachines(unsigned int s, unsigned int c){start = s; count = c;}
	int belong(unsigned int machineId);

	int getStart(){return start;}
	int getLast(){return start+count-1;}
	int getCount(){return count;}
};

inline int MachineSet::belong(unsigned int machineId){
	return machineId >= start && (machineId-start <= count);
}

#endif /* MACHINESET_H_ */
