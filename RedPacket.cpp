#include "RedPacket.h"
#include <windows.h>

#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <random>
#include <sstream>
#include <cstdlib>
#include <cerrno>

namespace
{
constexpr const char* DEFAULT_GRABBER_NAME = "AnonymousUser";
}

double RedPacket::Round2(double value) const
{
	if (value >= 0.0) return static_cast<double>(static_cast<int>(value * 100.0 + 0.5)) / 100.0;
	return static_cast<double>(static_cast<int>(value * 100.0 - 0.5)) / 100.0;
}

RedPacket::RedPacket(double money, int packetNum, std::string owner)
	: total_money(money), num(packetNum > 0 ? packetNum : 1), grabbed(0), name(owner), arr(0)
{
	if (name.empty()) name = "Unknown";
	if (total_money < 0.0) total_money = 0.0;
	arr = new std::string[num];
}

RedPacket::~RedPacket()
{
	delete[] arr;
	arr = 0;
}

void RedPacket::setMoney(double money, int packetNum)
{
	if (grabbed > 0)
	{
		return;
	}
	if (packetNum <= 0 || money <= 0.0)
	{
		return;
	}
	total_money = Round2(money);
	num = packetNum;
	delete[] arr;
	arr = new std::string[num];
	for (int i = 0; i < num; ++i) arr[i].clear();
	grabbed_names.clear();
}

double RedPacket::grab(std::string grabberName)
{
	return grab(grabberName, 0);
}

bool RedPacket::HasGrabbed(const std::string& grabberName) const
{
	for (size_t i = 0; i < grabbed_names.size(); ++i)
	{
		if (grabbed_names[i] == grabberName) return true;
	}
	return false;
}

double RedPacket::grab(std::string grabberName, int* outStatus)
{
	if (grabberName.empty()) grabberName = DEFAULT_GRABBER_NAME;
	if (HasGrabbed(grabberName))
	{
		if (outStatus) *outStatus = GrabDuplicate;
		return 0.0;
	}
	if (grabbed >= num)
	{
		if (outStatus) *outStatus = GrabEmpty;
		return 0.0;
	}

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

		static thread_local std::mt19937 rng(std::random_device{}());
		std::uniform_real_distribution<double> dist(0.0001, 1.0);
		double unit = dist(rng);
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
	grabbed_names.push_back(grabberName);
	++grabbed;
	if (outStatus) *outStatus = GrabSuccess;
	return got;
}

void RedPacket::show() const
{
	std::string s = summary();
	OutputDebugStringA(s.c_str());
}

std::string RedPacket::summary() const
{
	std::ostringstream oss;
	oss << "Owner: " << name << "\n";
	oss << "Money left: " << std::fixed << std::setprecision(2) << total_money << " yuan\n";
	oss << "Total packets: " << num << "\n";
	oss << "Grabbed count: " << grabbed << "\n";
	for (int i = 0; i < grabbed; ++i)
	{
		oss << "  " << i + 1 << ". " << arr[i] << " yuan\n";
	}
	std::string best = bestLuckRecord();
	if (!best.empty()) oss << "Best luck: " << best << " yuan\n";
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

std::string RedPacket::bestLuckRecord() const
{
	double bestMoney = -1.0;
	std::string bestName;
	for (int i = 0; i < grabbed; ++i)
	{
		size_t pos = arr[i].find(':');
		if (pos == std::string::npos) continue;
		std::string who = arr[i].substr(0, pos);
		std::string moneyText = arr[i].substr(pos + 1);
		char* pEnd = 0;
		errno = 0;
		double money = std::strtod(moneyText.c_str(), &pEnd);
		if (pEnd == moneyText.c_str() || errno == ERANGE) continue;
		if (money > bestMoney)
		{
			bestMoney = money;
			bestName = who;
		}
	}
	if (bestMoney < 0.0 || bestName.empty()) return std::string();
	std::ostringstream oss;
	oss << bestName << ":" << std::fixed << std::setprecision(2) << bestMoney;
	return oss.str();
}
