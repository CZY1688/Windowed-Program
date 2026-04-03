#include "RedPacket.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

double RedPacket::Round2(double value)
{
	if (value >= 0.0) return static_cast<double>(static_cast<int>(value * 100.0 + 0.5)) / 100.0;
	return static_cast<double>(static_cast<int>(value * 100.0 - 0.5)) / 100.0;
}

RedPacket::RedPacket(double money, int packetNum, std::string owner)
	: total_money(money), num(packetNum > 0 ? packetNum : 1), grabbed(0), name(owner), arr(0)
{
	if (name.empty()) name = "未知";
	if (total_money < 0.0) total_money = 0.0;
	arr = new std::string[num];
	static bool seeded = false;
	if (!seeded)
	{
		std::srand(static_cast<unsigned int>(std::time(0)));
		seeded = true;
	}
}

RedPacket::~RedPacket()
{
	delete[] arr;
	arr = 0;
	std::cout << "RedPacket destroyed: " << name << std::endl;
}

void RedPacket::setMoney(double money, int packetNum)
{
	if (grabbed > 0)
	{
		std::cout << "红包已被抢过，无法修改" << std::endl;
		return;
	}
	if (packetNum <= 0 || money <= 0.0)
	{
		std::cout << "金额和红包个数必须为正数" << std::endl;
		return;
	}
	total_money = Round2(money);
	num = packetNum;
	delete[] arr;
	arr = new std::string[num];
	for (int i = 0; i < num; ++i) arr[i].clear();
}

double RedPacket::grab(std::string grabberName)
{
	if (grabberName.empty()) grabberName = "匿名";
	if (grabbed >= num) return 0.0;

	double got = 0.0;
	int remainCount = num - grabbed;

	if (remainCount == 1)
	{
		got = Round2(total_money);
	}
	else
	{
		double avg = total_money / remainCount;
		double maxMoney = avg * 2.0;
		if (maxMoney < 0.01) maxMoney = 0.01;

		double unit = static_cast<double>((std::rand() % 10000) + 1) / 10000.0;
		got = Round2(unit * maxMoney);
		if (got < 0.01) got = 0.01;

		double minRemain = static_cast<double>(remainCount - 1) * 0.01;
		if (total_money - got < minRemain)
		{
			got = Round2(total_money - minRemain);
			if (got < 0.01) got = 0.01;
		}
	}

	total_money = Round2(total_money - got);
	if (total_money < 0.0) total_money = 0.0;

	std::ostringstream oss;
	oss << std::fixed << std::setprecision(2) << got;
	arr[grabbed] = grabberName + ":" + oss.str();
	++grabbed;
	return got;
}

void RedPacket::show() const
{
	std::cout << summary();
}

std::string RedPacket::summary() const
{
	std::ostringstream oss;
	oss << "发红包者: " << name << "\n";
	oss << "剩余金额: " << std::fixed << std::setprecision(2) << total_money << "\n";
	oss << "总份数: " << num << "\n";
	oss << "已抢份数: " << grabbed << "\n";
	for (int i = 0; i < grabbed; ++i)
	{
		oss << "  " << i + 1 << ". " << arr[i] << "\n";
	}
	return oss.str();
}

std::vector<std::string> RedPacket::records() const
{
	std::vector<std::string> out;
	for (int i = 0; i < grabbed; ++i) out.push_back(arr[i]);
	return out;
}

bool RedPacket::canSetMoney() const
{
	return grabbed == 0;
}

int RedPacket::grabbedCount() const
{
	return grabbed;
}

int RedPacket::totalCount() const
{
	return num;
}
