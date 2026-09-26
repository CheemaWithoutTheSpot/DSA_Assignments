
#pragma once
#include<iostream>
#include<limits>
using namespace std;
#ifndef ROLL_N
#define ROLL_N 3125
#endif

/* your own N goes here */

#define P1 (((ROLL_N) % 4) + 3)
#define P2 (((ROLL_N) % 3) + 2)
#define P3 (((ROLL_N) % 5) + 1)
#define SEED ((ROLL_N) % 100)

struct Appointment {
	int clientId;
	char* clientName;	// heap block, exactly length + 1
	// 8 chars + '\0', stored IN PLACE
	char service[9];
	float price;
};
struct DaySchedule {
	Appointment* slots;


	// heap ARRAY OF STRUCT VALUES

	int capacity;
	int count;
	

};
struct Week {
	DaySchedule* days; // heap array of 7 DaySchedule VALUES,
	int dayCount; // each owning a slot array of its own length
};
#define DAYS_IN_WEEK 7
#define NAME_LIMIT 14
#define SERVICE_LIMIT 8
#define MAX_SLOTS 40
#define CLIENT_MIN 1
#define CLIENT_MAX 9999
//QUESNTION 2 EASY


void fillPrices(float* begin, float* end, int n) {
	int i = 0;
	for (float* p = begin; p != end; p++) {
		*p = (float)((n + 613 * i) % 4000) / 2.0f + 250.0f;
		i++;
	}
}

float sumRange(const float* begin, const float* end)
{
	float sum = 0.0;
	for (int i=0; begin+i != end; i++) {
		sum += *(begin + i);
	}
	return sum;
}
//weird azz requirements bro i swear
float* maxElementPtr(float* begin, float* end)
{
	if (begin == end) {
		return nullptr;
	}

	float max = *begin;
	float* best = begin;
	for (int i = 0; begin + i != end; i++) {
		if (max < *(begin + i))
		{
			max = *(begin + i);
			best = begin + i;
		}
	}
	return best;

}

int countAbove(const float* begin, const float* end, float threshold)
{
	int count =0;
	for (int i = 0; begin + i != end; i++) {
		if(*(begin+i) > threshold)count++;
	}
	return count;
}

void reverseInPlace(float* begin, float* end) {
	float* l = begin;
	float* r = end - 1;

	while (l < r) {
		float temp = *l;
		*l = *r;
		*r = temp;

		l++;
		r--;
	}

}

void reportSizes() {

	int apptPayload = (int)sizeof(int) + (int)sizeof(char*) + (int)(sizeof(char) * (SERVICE_LIMIT + 1)) + (int)sizeof(float);
	int apptPadding = (int)sizeof(Appointment) - apptPayload;

	cout << "SIZEOF Appointment=" << (int)sizeof(Appointment) << " payload=" << apptPayload << " padding=" << apptPadding << "\n";


	int dayPayload = (int)sizeof(Appointment*) + (int)sizeof(int) + (int)sizeof(int);
	int dayPadding = (int)sizeof(DaySchedule) - dayPayload;
	cout << "SIZEOF DaySchedule=" << (int)sizeof(DaySchedule) << " payload=" << dayPayload << " padding=" << dayPadding << "\n";


	int weekPayload = (int)sizeof(DaySchedule*) + (int)sizeof(int);
	int weekPadding = (int)sizeof(Week) - weekPayload;
	cout << "SIZEOF Week=" << (int)sizeof(Week) << " payload=" << weekPayload << " padding=" << weekPadding << "\n";
}
//QUESTION 2 MEDIUM

void initWeek(Week& w)
{
	w.days = new DaySchedule[DAYS_IN_WEEK];
	w.dayCount = DAYS_IN_WEEK;

	for (int i = 0; i < DAYS_IN_WEEK; i++)
	{
		(w.days + i)->slots = nullptr;
		(w.days + i)->count = 0;
		(w.days + i)->capacity = 0;
	}
}

bool growDay(DaySchedule& d)	//ts the same as question 1
{
	int cap = (d.capacity == 0) ? P2 : d.capacity + P3;

	if (cap > MAX_SLOTS)
	{
		cout << "ERR DAY_FULL\n";
		return false;
	}

	Appointment* newSlots = new Appointment[cap];

	for (int i = 0; i < d.count; i++)
	{
		(newSlots+ i) ->clientId = (d.slots+i)->clientId;
		(newSlots + i)->clientName = (d.slots + i)->clientName;
		for (int j = 0; j < SERVICE_LIMIT + 1; j++)
		{
			*((newSlots + i)->service + j) = *((d.slots + i)->service + j);
		}
		(newSlots + i)->price = (d.slots + i)->price;
	}
	delete[] d.slots;
	d.capacity = cap;
	d.slots = newSlots;
	return true;
}
//Helper funcs
int strln(const char* s)
{
	int count = 0;
	while (*(s + count) != '\0')
	{
		count++;
	}
	return count;
}

void strcp(char* dest, const char* src)
{
	int i = 0;
	while (*(src + i) != '\0')
	{
		*(dest + i) = *(src + i);
		i++;
	}
	*(dest + i) = '\0';
}

char* ccsstr(const char* src) {
	if (src == nullptr) {
		return nullptr;
	}

	int len = strln(src);
	char* newBlock = new char[len + 1];
	strcp(newBlock, src);
	return newBlock;
}
//Helper funcs end;

bool bookAppointment(Week& w, int day, int clientId, const char* name, const char* service, float price)
{
	if (w.days == nullptr)
	{
		cout << "ERR WEEK_DESTROYED" << "\n";
		return false;
	}
	if (day < 0 || day >= DAYS_IN_WEEK)
	{
		cout << "ERR BAD_DAY" << "\n";
		return false;
	}
	if (clientId < CLIENT_MIN || clientId > CLIENT_MAX)
	{
		cout << "ERR BAD_CLIENT" << "\n";
		return false;
	}

	int nameLen = strln(name);
	if (nameLen < 1 || nameLen > NAME_LIMIT)
	{
		cout << "ERR BAD_NAME" << "\n";
		return false;
	}

	int serviceLen = strln(service);
	if (serviceLen < 1 || serviceLen > SERVICE_LIMIT)
	{
		cout << "ERR BAD_SERVICE" << "\n";
		return false;
	}

	if (price < 100.0f || price > 20000.0f)
	{
		cout << "ERR BAD_PRICE" << "\n";
		return false;
	}
	//HOLY ERROR SLOP BRO WHAT IS TS
	DaySchedule* d = w.days + day;

	for (int i = 0; i < d->count; i++)
	{
		Appointment* a = d->slots + i;
		if (a->clientId == clientId)
		{
			cout << "ERR DUP_BOOKING" << "\n";	//we still aint donee bro 💔✌
			return false;
		}
	}

	if (d->count == d->capacity)
	{
		if (!growDay(*d)) return false;   
	}

	Appointment* newSlot = d->slots + d->count;
	newSlot->clientId = clientId;
	newSlot->clientName = ccsstr(name);
	strcp(newSlot->service, service);
	newSlot->price = price;

	d->count = d->count + 1;

	return true;
}

Appointment* findAppointment(const Week& w, int clientId, int& outDay, int& outSlot)
{
	if (w.days == nullptr)
	{
		outDay = -1;
		outSlot = -1;
		return nullptr;
	}

	for (int day = 0; day < w.dayCount; day++)
	{
		DaySchedule* d = w.days + day;
		for (int slot = 0; slot < d->count; slot++)
		{
			Appointment* a = d->slots + slot;
			if (a->clientId == clientId)
			{
				outDay = day;
				outSlot = slot;
				return a;
			}
		}
	}

	outDay = -1;
	outSlot = -1;
	return nullptr;
}
//bro ts just a chaapa 1-1 of question 1 
float dayRevenue(const DaySchedule& d)
{
	float total = 0.0f;
	for (int i = 0; i < d.count; i++)
	{
		Appointment* a = d.slots + i;
		total = total + a->price;
	}
	return total;//naaa karrooo genuineee??????
}

bool cancelAppointment(Week& w, int day, int slot)
{
	if (w.days == nullptr)
	{
		cout << "ERR WEEK_DESTROYED" << "\n";
		return false;
	}
	if (day < 0 || day >= DAYS_IN_WEEK)
	{
		cout << "ERR BAD_DAY" << "\n";
		return false;
	}

	DaySchedule* d = w.days + day;

	if (slot < 0 || slot >= d->count)
	{
		cout << "ERR BAD_SLOT" << "\n";
		return false;
	}

	Appointment* target = d->slots + slot;
	delete[] target->clientName;

	for (int i = slot; i < d->count - 1; i++)
	{
		*(d->slots + i) = *(d->slots + i + 1);   
	}

	d->count = d->count - 1;

	return true;
}


void printDay(const DaySchedule& d, int dayIndex)
{
	const char* names[7] = { "MON","TUE","WED","THU","FRI","SAT","SUN" };
	const char* dayName = *(names + dayIndex);

	printf("DAY %d %s | BOOKED=%d/%d | REVENUE=%.2f\n",
		dayIndex, dayName, d.count, d.capacity, dayRevenue(d));

	if (d.count == 0)
	{
		printf("  (free)\n");
	}
	else
	{
		for (int i = 0; i < d.count; i++)
		{
			Appointment* a = d.slots + i;
			printf(" <%02d> %04d %-14s %-8s PKR %8.2f\n",
				i, a->clientId, a->clientName, a->service, a->price);
		}
	}
}

void printWeek(const Week& w)
{
	if (w.days == nullptr)
	{
		printf("WEEK bookings=0 revenue=0.00\n (destroyed)\nEND WEEK\n");
		return;
	}

	int totalBookings = 0;
	float totalRevenue = 0.0f;

	for (int day = 0; day < w.dayCount; day++)
	{
		DaySchedule* d = w.days + day;
		totalBookings = totalBookings + d->count;
		totalRevenue = totalRevenue + dayRevenue(*d);
	}

	printf("WEEK bookings=%d revenue=%.2f\n", totalBookings, totalRevenue);

	for (int day = 0; day < w.dayCount; day++)
	{
		printDay(*(w.days + day), day);
	}

	printf("END WEEK\n");
}

//WURSTION 2 HARDDDUHHHH man cmon

bool moveAppointment(Week& w, int fromDay, int fromSlot, int toDay)
{
	if (w.days == nullptr)
	{
		cout << "ERR WEEK_DESTROYED" << "\n";
		return false;
	}
	if (fromDay < 0 || fromDay >= DAYS_IN_WEEK || toDay < 0 || toDay >= DAYS_IN_WEEK)
	{
		cout << "ERR BAD_DAY" << "\n";
		return false;
	}
	if (fromDay == toDay)
	{
		cout << "ERR SAME_DAY" << "\n";
		return false;
	}

	DaySchedule* src = w.days + fromDay;

	if (fromSlot < 0 || fromSlot >= src->count)
	{
		cout << "ERR BAD_SLOT" << "\n";
		return false;
	}

	Appointment* moving = src->slots + fromSlot;
	DaySchedule* dst = w.days + toDay;

	for (int i = 0; i < dst->count; i++)
	{
		Appointment* a = dst->slots + i;
		if (a->clientId == moving->clientId)
		{
			cout << "ERR DUP_BOOKING" << "\n";
			return false;
		}
	}//OKAY FINALLY OUT OF THE ERROR SLOP

	if (dst->count == dst->capacity)
	{
		if (!growDay(*dst)) return false;
	}//you just grow the dest

	Appointment* dstSlot = dst->slots + dst->count;	//make a new slot for it on dst-slot offsetted by the count
	dstSlot->clientId = moving->clientId;
	dstSlot->clientName = moving->clientName;
	for (int j = 0; j < SERVICE_LIMIT + 1; j++)
	{
		*(dstSlot->service + j) = *(moving->service + j);
	}
	dstSlot->price = moving->price;
	dst->count = dst->count + 1;	//copycopycopy

	for (int i = fromSlot; i < src->count - 1; i++)
	{
		*(src->slots + i) = *(src->slots + i + 1);	//bring allat back 
	}
	src->count = src->count - 1;	//obviousy

	return true;
}

Appointment** buildIndex(const Week& w, int& outCount)
{
	if (w.days == nullptr)
	{
		outCount = 0;
		return nullptr;
	}

	int total = 0;
	for (int day = 0; day < w.dayCount; day++)
	{
		DaySchedule* d = w.days + day;
		total = total + d->count;
	}

	if (total == 0)
	{
		outCount = 0;
		return nullptr;
	}

	Appointment** index = new Appointment* [total];
	int pos = 0;

	for (int day = 0; day < w.dayCount; day++)
	{
		DaySchedule* d = w.days + day;
		for (int slot = 0; slot < d->count; slot++)
		{
			*(index + pos) = d->slots + slot;
			pos = pos + 1;	
		}		//jus making a linear dayschedule array of all the slots
	}

	outCount = total;
	return index;
}

void sortIndexByPrice(Appointment** index, int n)
{
	if (index == nullptr) return;

	for (int i = 0; i < n - 1; i++)
	{
		for (int j = 0; j < n - 1 - i; j++)
		{
			Appointment* a = *(index + j);
			Appointment* b = *(index + j + 1);

			if (b->price > a->price)  
			{
				*(index + j) = b;
				*(index + j + 1) = a;
			}//basic sorting, twt i aint finna use merge sort
		}
	}
}

void printIndex(Appointment** index, int n)
{
	printf("INDEX size=%d\n", n);

	if (index == nullptr || n == 0)
	{
		printf(" (none)\n");
	}
	else
	{
		for (int i = 0; i < n; i++)
		{
			Appointment* a = *(index + i);
			printf(" [%02d] %04d %-14s %-8s PKR %8.2f\n",
				i, a->clientId, a->clientName, a->service, a->price);
		}
	}

	printf("END INDEX\n");
}

void destroyIndex(Appointment**& index, int& n)
{
	delete[] index;
	index = nullptr;
	n = 0;
}

void destroyWeek(Week& w)
{
	if (w.days == nullptr)
	{
		w.dayCount = 0;
		return;
	}

	for (int day = 0; day < w.dayCount; day++)
	{
		DaySchedule* d = w.days + day;

		for (int i = 0; i < d->count; i++)
		{
			Appointment* a = d->slots + i;
			delete[] a->clientName;
		}

		delete[] d->slots;
	}

	delete[] w.days;

	w.days = nullptr;
	w.dayCount = 0;
}

bool bookByValue(DaySchedule day, int clientId, const char* name, const char* service, float price)
{
	if (clientId < CLIENT_MIN || clientId > CLIENT_MAX)
	{
		cout << "ERR BAD_CLIENT" << "\n";
		return false;
	}

	int nameLen = strln(name);
	if (nameLen < 1 || nameLen > NAME_LIMIT)
	{
		cout << "ERR BAD_NAME" << "\n";
		return false;
	}

	int serviceLen = strln(service);
	if (serviceLen < 1 || serviceLen > SERVICE_LIMIT)
	{
		cout << "ERR BAD_SERVICE" << "\n";
		return false;
	}

	if (price < 100.0f || price > 20000.0f)
	{
		cout << "ERR BAD_PRICE" << "\n";
		return false;
	}

	for (int i = 0; i < day.count; i++)
	{
		Appointment* a = day.slots + i;
		if (a->clientId == clientId)
		{
			cout << "ERR DUP_BOOKING" << "\n";
			return false;
		}
	}

	if (day.count == day.capacity)
	{
		if (!growDay(day)) return false;  
	}

	Appointment* newSlot = day.slots + day.count;
	newSlot->clientId = clientId;
	newSlot->clientName = ccsstr(name);
	strcp(newSlot->service, service);
	newSlot->price = price;

	day.count = day.count + 1;

	return true;
}


void loadSeedWeek(Week& w)
{
	const char* NAMES[8] = { "Ayesha","Hina","Sana","Mahnoor","Zara","Iqra","Nimra","Rabia" };	//ye hina ajeeb har jagga aajati
	const char* SERVICES[6] = { "HAIRCUT","FACIAL","MANI","PEDI","COLOR","MAKEUP" };	//haan haan karlawo makeup 
	float PRICES[6] = { 1500.0f, 2500.0f, 800.0f, 4200.0f, 1200.0f, 3000.0f };	//phir rooti jab poochta gpa kitna, you really can't blame me gng 😹✌

	destroyWeek(w);
	initWeek(w);

	int total = (SEED % 3) + 9;

	for (int k = 0; k < total; k++)
	{
		int day = 5 + (k % 2); // 5=styrday, 6=sujday
		int clientId = 500 + SEED + 7 * k;
		const char* name = *(NAMES + (SEED + k) % 8);
		int which = (SEED + k) % 6;
		const char* service = *(SERVICES + which);
		float price = *(PRICES + which);

		bookAppointment(w, day, clientId, name, service, price);
	}

	// midweek booking
	int day = SEED % 5;
	int clientId = 500 + SEED + 7 * total;
	const char* name = *(NAMES + (SEED + total) % 8);
	int which = (SEED + total) % 6;
	const char* service = *(SERVICES + which);
	float price = *(PRICES + which);

	bookAppointment(w, day, clientId, name, service, price);
}



