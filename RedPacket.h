#ifndef REDPACKET_H
#define REDPACKET_H

#include <string>
#include <vector>

class RedPacket
{
public:
	enum EGrabStatus
	{
		GrabSuccess = 1,
		GrabEmpty = 0,
		GrabDuplicate = -1
	};

	RedPacket(double money = 0.0, int packetNum = 1, std::string owner = "Unknown");
	~RedPacket();

	void setMoney(double money, int packetNum);
	double grab(std::string grabberName);
	double grab(std::string grabberName, int* outStatus);
	void show() const;

	std::string summary() const;
	std::vector<std::string> records() const;
	bool canSetMoney() const;
	int grabbedCount() const;
	int totalCount() const;
	std::string bestLuckRecord() const;

private:
	double total_money;
	int num;
	int grabbed;
	std::string name;
	std::string* arr;
	std::vector<std::string> grabbed_names;

	double Round2(double value) const;
	bool HasGrabbed(const std::string& grabberName) const;
};

#endif
