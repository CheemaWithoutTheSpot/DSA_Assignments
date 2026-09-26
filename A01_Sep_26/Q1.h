#pragma once
#include<iostream>
using namespace std;
#ifndef ROLL_N
#define ROLL_N 3125
#endif

/* your own N goes here */

#define P1 (((ROLL_N) % 4) + 3)
#define P2 (((ROLL_N) % 3) + 2)
#define P3 (((ROLL_N) % 5) + 1)
#define SEED ((ROLL_N) % 100)

struct Reading {
	char sensor[9]; 	// 8 chars + '\0', stored IN PLACE
	float value;	
	char status;		// 'N' nominal, 'W' warning, 'C' critical
};

struct Probe {


	int probeId;
	char* callSign; // heap block, exactly length + 1
	Reading* readings; // heap ARRAY OF STRUCT VALUES
	int readingCount;
	int readingCapacity;


};
struct Fleet {
	Probe** probes;	// heap array of POINTERS to Probe
	int count;
	int capacity;
};



#define SIGN_LIMIT 20
#define SENSOR_LIMIT 8
#define MAX_PROBES 4096
#define MAX_READINGS 64
#define ID_MIN 1
#define ID_MAX 9999

//QUESTION 1 A; HELPERS (mostly (i think))
int myStrLen(const char* s)
{
	int count = 0;
	while (*(s + count) != '\0')
	{
		count++;	//simple jeya hai si idi ki explaination chandy o
	}
	return count;
}

void myStrCopy(char* dest, const char* src)
{
	int i = 0;
	while (*(src + i) != '\0')
	{
		*(dest + i) = *(src + i);	//oh shah g kya baat, e stringan nu copy kardeya oho!
		i++;
	}
	*(dest + i) = '\0';
}

int myStrCompare(const char* a, const char* b)
{
	int i = 0;
	while (*(a + i) != '\0' && *(a + i) == *(b + i)) {
		i++; 
	}  //nvm fixed the warning
	return (int)(unsigned char)*(a + i) - (int)(unsigned char)*(b + i);

}

char* cloneCString(const char* src) {
	if (src == nullptr) {
		return nullptr;
	}

	int len = myStrLen(src);
	char* newBlock = new char[len + 1];
	myStrCopy(newBlock, src);
	return newBlock;
}
void reportSizes() {
	cout << "SIZEOF char=" << (int)sizeof(char) << " int=" << (int)sizeof(int) << " float=" << (int)sizeof(float) << " ptr=" << (int)sizeof(char*) << "\n";

	int readingPayload = (int)(sizeof(char) * (SENSOR_LIMIT + 1)) + (int)sizeof(float) + (int)sizeof(char);
	int readingPadding = (int)sizeof(Reading) - readingPayload;
	cout << "SIZEOF Reading=" << (int)sizeof(Reading) << " payload=" << readingPayload << " padding=" << readingPadding << "\n";

	int probePayload = (int)sizeof(int) + (int)sizeof(char*) + (int)sizeof(Reading*) + (int)sizeof(int) + (int)sizeof(int);
	int probePadding = (int)sizeof(Probe) - probePayload;
	cout << "SIZEOF Probe=" << (int)sizeof(Probe) << " payload=" << probePayload << " padding=" << probePadding << "\n";

	int fleetPayload = (int)sizeof(Probe**) + (int)sizeof(int) + (int)sizeof(int);
	int fleetPadding = (int)sizeof(Fleet) - fleetPayload;
	cout << "SIZEOF Fleet=" << (int)sizeof(Fleet) << " payload=" << fleetPayload << " padding=" << fleetPadding << "\n";
}
//Q1 MEUIYM

void initFleet(Fleet& f, int initialCapacity) 
{		//basic initilization of fleet nothing fance
	if (initialCapacity < 1) initialCapacity = 1;
	f.probes = new Probe * [initialCapacity]; 

	for (int i = 0; i < initialCapacity; i++) *(f.probes+i) = nullptr;

	f.count = 0;
	f.capacity = initialCapacity;
}
bool growFleet(Fleet& f)
{
	int cap = (f.capacity == 0) ? P1 : f.capacity * 2;	//double capacity

	if (cap > MAX_PROBES)	
	{
		cout << "ERR FLEET_FULL" << endl;
		return false;
	}

	Probe** p = new Probe * [cap];	//new cap numbers probe pointers 

	for (int i = 0; i < cap; i++)
	{
		if (i < f.capacity) *(p + i) = *(f.probes + i);	//copy them, initilize rest to null ptrs son
		else *(p + i) = nullptr;                         
	}

	delete[] f.probes;   //delete the previous ones and replaceeeuhh

	f.probes = p;
	f.capacity = cap; //new cap never forget
	return true;
}

Probe** findSlot(const Fleet& f, int probeId)
{
	if (f.probes == nullptr || f.count == 0) return nullptr;

	for (int i = 0; i < f.count; i++)
	{
		if ((*(f.probes + i))->probeId == probeId)	//searching and returning if probe id match
			return (f.probes + i);
	}
	return nullptr;
}

Probe* findProbe(const Fleet& f, int probeId)
{
	Probe** slt = findSlot(f, probeId);
	if (slt == nullptr) return nullptr;
	return *slt;
}

bool addProbe(Fleet& f, int probeId, const char* callSign)
{
	if (probeId < ID_MIN || probeId > ID_MAX)
	{
		cout << "ERR BAD_ID" << endl;
		return false;
	}

	int l = myStrLen(callSign);
	if (l < 1 || l > SIGN_LIMIT)
	{
		cout << "ERR BAD_SIGN" << endl;
		return false;
	}


	if (findProbe(f, probeId) != nullptr)
	{
		cout << "ERR DUP_ID" << endl;
		return false;
	}

	if (f.count == f.capacity)
	{
		if (!growFleet(f)) return false;  //if grow fleet returned false then nigga return false
	}

	Probe* p = new Probe;
	p->probeId = probeId;
	p->callSign = cloneCString(callSign);
	p->readings = nullptr;
	p->readingCount = 0;
	p->readingCapacity = 0;

	*(f.probes + f.count) = p;
	f.count++;

	return true;
}

bool growReadings(Probe* p)
{
	int nc = (p->readingCapacity != 0) ? p->readingCapacity + P3 : P2;//setting a new capapcity if ==0 ayusy

	if (nc > MAX_READINGS)
	{
		cout << "ERR LOG_FULL" << "\n";
		return false;
	}

	Reading* nA = new Reading[nc];	//make a new reaeding heap arr

	for (int i = 0; i < p->readingCount; i++)
	{
		for (int j = 0; j < SENSOR_LIMIT + 1; j++)	//holy unreadable 
		*((nA + i)->sensor+ j) = *(((p->readings + i)->sensor) + j);	//copying sonsor readings from p->readings in loop
		(nA + i )->value = (p->readings + i)->value;
		(nA + i)->status = (p->readings+i)->status;	//yaada yaada
	}

	delete[] p->readings;
	p->readings = nA;//same shyts bro i aint gotta explain everything i swear nih the fuh sorta assignmeent is ts
	p->readingCapacity = nc;//atleast give leetcode ahh queestions bro ts finna make me end myself

	return true;
}
bool addReading(Probe* p, const char* sensor, float value, char status)	//same stuff as beforeeeee UGH
{
	if (p == nullptr)
	{
		cout << "ERR NO_PROBE" << "\n";
		return false;
	}

	int senLen = myStrLen(sensor);
	if (senLen < 1 || senLen > SENSOR_LIMIT)
	{
		cout << "ERR BAD_SENSOR" << "\n";
		return false;
	}

	if (status != 'N' && status != 'W' && status != 'C')
	{
		cout << "ERR BAD_STATUS" << "\n";
		return false;
	}

	for (int i = 0; i < p->readingCount; i++)
	{
		if (myStrCompare((p->readings + i)->sensor, sensor) == 0)
		{
			cout << "ERR DUP_SENSOR" << "\n";
			return false;
		}
	}

	if (p->readingCount == p->readingCapacity)
	{
		if (!growReadings(p)) return false;  
	}

	myStrCopy((p->readings + p->readingCount)->sensor, sensor);
	(p->readings + p->readingCount)->value = value;
	(p->readings + p->readingCount)->status = status;
	p->readingCount++;

	return true;
}


float probeHealth(const Probe* p)//FINALLY a non reptitive question
{
	if (p == nullptr || p->readingCount == 0) return 0.0f;

	float sum = 0.0f;
	int c = p->readingCount;
	for (int i = 0; i < c; i++)
	{
		if ((p->readings + i )->status == 'N') sum += 1.0f;
		else if ((p->readings + i)->status == 'W') sum += 0.5f;	//so bsically you jus checking za status
		//and adding sumn to da sum
		
	}
	return sum / c;// and then normalizing it 
}

void printProbe(const Probe* p)
{		
	if (p == nullptr)
	{
		cout << "ERR NO_PROBE\n";
		return;
	}

	printf("PROBE %04d | %-20s | LOGS=%d | HEALTH=%.2f\n",
		p->probeId, p->callSign, p->readingCount, probeHealth(p));

	if (p->readingCount == 0)
	{
		printf(" (no telemetry)\n");
	}
	else
	{
		for (int i = 0; i < p->readingCount; i++)
		{
			printf(" <%02d> %-8s VAL=%8.2f ST=%c\n",
				i, (p->readings + i)->sensor, (p->readings + i)->value, (p->readings + i)->status);
		}
	}
}

void printFleet(const Fleet& f)
{
	cout << "FLEET count="<< f.count << " capacity =" << f.capacity << "\n";

	if (f.count == 0)
	{
		cout<< " (empty)\n";
	}
	else
	{
		for (int i = 0; i < f.count; i++)
		{
			printProbe( * (f.probes + i));
		}
	}
	cout << "END FLEET\n";
}


//Q1 HARD ooo scarryyyy 
void destroyProbe(Probe*& p)
{
	if (p == nullptr) return;
	delete[] p->callSign;
	delete[] p->readings;
	delete p;
	p = nullptr;
}	//that wan't that harddd.

void compactFleet(Fleet& f, int removedIndex)
{
	if (removedIndex < 0 || removedIndex >= f.count) return; 
	int c = f.count - 1;
	for (int i = removedIndex; i <c ; i++)
	{
		*(f.probes + i) = *(f.probes + i + 1);   //basically just moving everything back by 1 including and after removedIndex.

	}

	*(f.probes + c) = nullptr;  //and the last pace finna b a null ptr
	f.count--;	//and u finna count--;
}

bool removeProbe(Fleet& f, int probeId)
{
	Probe** b = findSlot(f, probeId);	//find the probe pointer
	if (b == nullptr) {
		cout << "ERR NOT_FOUND" << "\n";
		return false;
	}

	int idx = b - f.probes;		//find its index py pointer arithmatic
	destroyProbe(*b);	//DESSTROOYYYYY THAT NIGGA
	compactFleet(f, idx);// bring allat back
	return true;

}

void deepCopyProbe(const Probe* src, Probe*& dest)
{
	if (src == nullptr)
	{
		dest = nullptr;
		return;
	}

	Probe* n = new Probe;
	n->probeId = src->probeId;
	n->callSign = cloneCString(src->callSign);

	if (src->readingCapacity > 0)
	{
		n->readings = new Reading[src->readingCapacity];
		int c = src->readingCount;
		for (int i = 0; i <c ; i++)	// copying the readings
		{
			Reading* srcR = src->readings + i;         
			Reading* dstR = n->readings + i;	//need two one to take sensor from src and one for dest

			for (int j = 0; j < SENSOR_LIMIT + 1; j++)	//then copying the sensor readings of the readings
			{
				*(dstR->sensor + j) = *(srcR->sensor + j);  
			}
			dstR->value = srcR->value;	
			dstR->status = srcR->status;
		}
	}
	else
	{
		n->readings = nullptr;
	}

	n->readingCount = src->readingCount;
	n->readingCapacity = src->readingCapacity;

	dest = n;

}

void aliasCopyProbe(Probe* src, Probe* dest)//just normal copying
{
	if (src == nullptr || dest == nullptr) return;

	dest->probeId = src->probeId;
	dest->callSign = src->callSign;
	dest->readings = src->readings;
	dest->readingCount = src->readingCount;
	dest->readingCapacity = src->readingCapacity;
}


bool mergeFleets(Fleet& target, const Fleet& source)
{
	int srcCount = source.count;   

	for (int i = 0; i < srcCount; i++)//till num of probes
	{
		Probe* srcProbe = *(source.probes + i);   

		if (findProbe(target, srcProbe->probeId) != nullptr) // nun duplicate
		{
			continue;   
		}

		if (!addProbe(target, srcProbe->probeId, srcProbe->callSign))	//add probe, if it can't return false
		{
			return false;  
		}

		Probe* newProbe = *(target.probes + (target.count - 1)); //for reading
		int rc = srcProbe->readingCount;
		for (int j = 0; j < rc ;j++)
		{
			Reading* r = srcProbe->readings + j;   
			if (!addReading(newProbe, r->sensor, r->value, r->status))
			{
				return false;   
			}
		}
	}
	return true;
}

void destroyFleet(Fleet& f)	//destorying the ENITREEEUHH fleet
{
	for (int i = 0; i < f.count; i++)
	{
		destroyProbe(*(f.probes + i));   
	}

	delete[] f.probes;

	f.probes = nullptr;
	f.count = 0;
	f.capacity = 0;
}


bool addProbeByValue(Fleet f, int probeId, const char* callSign)
{
	if (probeId < ID_MIN || probeId > ID_MAX)
	{
		cout << "ERR BAD_ID" << endl;
		return false;
	}

	int l = myStrLen(callSign);
	if (l < 1 || l > SIGN_LIMIT)
	{
		cout << "ERR BAD_SIGN" << endl;
		return false;
	}


	if (findProbe(f, probeId) != nullptr)
	{
		cout << "ERR DUP_ID" << endl;
		return false;
	}

	if (f.count == f.capacity)
	{
		if (!growFleet(f)) return false;  //if grow fleet returned false then nigga return false
	}

	Probe* p = new Probe;
	p->probeId = probeId;
	p->callSign = cloneCString(callSign);
	p->readings = nullptr;
	p->readingCount = 0;
	p->readingCapacity = 0;

	*(f.probes + f.count) = p;
	f.count++;

	return true;
}

void loadFleetA(Fleet& f)
{
	const char* SIGNS[8] = { "Voyager","Pathfinder","Odyssey","Horizon",
							  "Sentinel","Aurora","Vanguard","Meridian" };
	const char* SENSORS[6] = { "TEMP-A","PWR-BUS","RAD-CNT","GYRO-X","COMMS-1","FUEL-P" };
	char STATUS[3] = { 'N', 'W', 'C' };

	destroyFleet(f);
	initFleet(f, P1);

	int total = (SEED % 3) + 3;

	for (int i = 0; i < total; i++)
	{
		int id = 1000 + SEED + 11 * i;
		const char* sign = SIGNS[(SEED + i) % 8];

		addProbe(f, id, sign);
		Probe* p = findProbe(f, id);

		int logs = (SEED + i) % 4;
		for (int j = 0; j < logs; j++)
		{
			const char* sensor = SENSORS[(SEED + i + j) % 6];
			float value = (float)(SEED + 10 * i + 3 * j) + 0.5f;
			char status = STATUS[(SEED + 2 * i + j) % 3];	//ajeeb why you warninig

			addReading(p, sensor, value, status);
		}
	}
}

void loadFleetB(Fleet& f)
{
	const char* SENSORS[6] = { "TEMP-A","PWR-BUS","RAD-CNT","GYRO-X","COMMS-1","FUEL-P" };
	char STATUS[3] = { 'N', 'W', 'C' };

	destroyFleet(f);
	initFleet(f, 2);

	// probe 1: Relay-Alpha, 1 reading
	int id1 = 1000 + SEED;
	addProbe(f, id1, "Relay-Alpha");
	Probe* p1 = findProbe(f, id1);
	addReading(p1, SENSORS[SEED % 6], (float)SEED + 0.5f, STATUS[SEED % 3]);

	// probe 2: Relay-Beta, 2 readings
	int id2 = 8000 + SEED;
	addProbe(f, id2, "Relay-Beta");
	Probe* p2 = findProbe(f, id2);
	addReading(p2, SENSORS[(SEED + 1) % 6], (float)SEED + 20.5f, STATUS[(SEED + 1) % 3]);
	addReading(p2, SENSORS[(SEED + 2) % 6], (float)SEED + 40.5f, STATUS[(SEED + 2) % 3]);
}

