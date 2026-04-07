#ifndef REDPACKET_H
#define REDPACKET_H

#include <string>
using namespace std;

class RedPacket
{
public:
	enum EGrabStatus
	{
		GrabSuccess = 1,
		GrabEmpty = 0,
		GrabDuplicate = -1
	};

	RedPacket(double money = 0.0, int packetNum = 1, string owner = "Unknown");
	~RedPacket();

	void setMoney(double money, int packetNum);
	double grab(string grabberName);
	double grab(string grabberName, int* outStatus);
	void show() const;

	string summary() const;
	const string* records() const;
	bool canSetMoney() const;
	int grabbedCount() const;
	int totalCount() const;
	string bestLuckRecord() const;

private:
	double total_money;
	int num;
	int grabbed;
	string name;
	string* arr;
	string* grabbed_names;
	int grabbed_name_count;

	double Round2(double value) const;
	bool HasGrabbed(const string& grabberName) const;
};

#endif
