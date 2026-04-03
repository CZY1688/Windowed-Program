#ifndef REDPACKET_H
#define REDPACKET_H

#include <string>
#include <vector>

class RedPacket
{
public:
	RedPacket(double money = 0.0, int packetNum = 1, std::string owner = "未知");
	~RedPacket();

	void setMoney(double money, int packetNum);
	double grab(std::string grabberName);
	void show() const;

	std::string summary() const;
	std::vector<std::string> records() const;
	bool canSetMoney() const;
	int grabbedCount() const;
	int totalCount() const;

private:
	double total_money;
	int num;
	int grabbed;
	std::string name;
	std::string* arr;

	static double Round2(double value);
};

#endif
