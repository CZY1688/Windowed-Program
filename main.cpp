#include "resource.h"
#include "BForm.h"
#include "RedPacket.h"
#include <strsafe.h>
#include <vector>

static CBForm g_form(ID_form1);

static RedPacket g_packetA(30.0, 5, "Owner A");
static RedPacket g_packetB(50.0, 8, "Owner B");
static RedPacket g_packetC(0.0, 1, "Owner C");
static bool g_packetCReady = false;

static tstring ToTString(const std::string& s)
{
#ifdef UNICODE
	int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, 0, 0);
	if (len <= 0) return TEXT("");
	std::vector<wchar_t> buf(len);
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &buf[0], len);
	return tstring(&buf[0]);
#else
	return s;
#endif
}

static std::string ToString(const tstring& s)
{
#ifdef UNICODE
	int len = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, 0, 0, 0, 0);
	if (len <= 0) return std::string();
	std::vector<char> buf(len);
	WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, &buf[0], len, 0, 0);
	return std::string(&buf[0]);
#else
	return s;
#endif
}

static tstring ReadText(unsigned short idEdit)
{
	return g_form.Control(idEdit, false).Text();
}

static void AppendLog(LPCTSTR s)
{
	g_form.Control(ID_editLog, false).TextAdd(s);
	g_form.Control(ID_editLog, false).TextAdd(TEXT("\r\n"));
}

static std::string ReadNameOrDefault(unsigned short idEdit)
{
	tstring s = ReadText(idEdit);
	if (s.empty()) return "Anonymous";
	return ToString(s);
}

static void AppendGrabResult(LPCTSTR packetName, const std::string& grabber, double money)
{
	TCHAR line[256] = { 0 };
	StringCchPrintf(line, 256, TEXT("%s - %s grabbed %.2f"),
		packetName, ToTString(grabber).c_str(), money);
	AppendLog(line);
}

static void AppendSummary(const RedPacket& packet, LPCTSTR title)
{
	AppendLog(TEXT("--------------------------------------------------"));
	AppendLog(title);
	tstring s = ToTString(packet.summary());
	AppendLog(s.c_str());
}

static void DoGrab(RedPacket& packet, unsigned short idNameEdit, LPCTSTR packetName, bool checkReady = false)
{
	if (checkReady && !g_packetCReady)
	{
		AppendLog(TEXT("Packet C is not funded yet, cannot grab."));
		return;
	}

	std::string who = ReadNameOrDefault(idNameEdit);
	double got = packet.grab(who);
	if (got <= 0.0)
	{
		TCHAR line[128] = { 0 };
		StringCchPrintf(line, 128, TEXT("%s is empty"), packetName);
		AppendLog(line);
		return;
	}
	AppendGrabResult(packetName, who, got);
}

static void OnFormLoad()
{
	g_form.IconSet(IDI_ICON1);
	g_form.TextSet(TEXT("Red Packet Simulator"));
	g_form.MoveToScreenCenter(920, 650);
	g_form.BackColorSet(RGB(236, 236, 236));
	g_form.KeyPreview = true;

	g_form.Control(ID_picCover, false).PictureSet(IDB_PACKET_COVER);
	g_form.Control(ID_editLog, false).TextSet(TEXT(""));

	g_form.Control(ID_editAName, false).TextSet(TEXT(""));
	g_form.Control(ID_btnAGrab, false).VisibleSet(false);
	g_form.Control(ID_btnAShow, false).VisibleSet(false);
	g_form.Control(ID_editAName, false).VisibleSet(false);

	g_form.Control(ID_editBName, false).TextSet(TEXT(""));

	g_form.Control(ID_editCMoney, false).TextSet(TEXT(""));
	g_form.Control(ID_editCNum, false).TextSet(TEXT(""));
	g_form.Control(ID_editCName, false).TextSet(TEXT(""));
	g_form.Control(ID_btnCGrab, false).EnabledSet(false);

	AppendLog(TEXT("Red packet simulator started."));
}

static void OnAGrab()
{
	DoGrab(g_packetA, ID_editAName, TEXT("Packet A"));
}

static void OnAShow()
{
	AppendSummary(g_packetA, TEXT("Packet A summary"));
}

static void OnBGrab()
{
	DoGrab(g_packetB, ID_editBName, TEXT("Packet B"));
}

static void OnBShow()
{
	AppendSummary(g_packetB, TEXT("Packet B summary"));
}

static void OnCFill()
{
	double money = g_form.Control(ID_editCMoney, false).TextVal();
	int count = static_cast<int>(g_form.Control(ID_editCNum, false).TextVal());
	if (money <= 0.0 || count <= 0)
	{
		AppendLog(TEXT("Funding failed: money and count must both be positive."));
		return;
	}
	if (!g_packetC.canSetMoney())
	{
		AppendLog(TEXT("Packet C has already been grabbed and cannot be funded again."));
		return;
	}

	g_packetC.setMoney(money, count);
	g_packetCReady = true;
	g_form.Control(ID_btnCGrab, false).EnabledSet(true);
	AppendLog(TEXT("Packet C funded successfully. You can now start grabbing."));
}

static void OnCGrab()
{
	DoGrab(g_packetC, ID_editCName, TEXT("Packet C"), true);
}

static void OnCShow()
{
	AppendSummary(g_packetC, TEXT("Packet C summary"));
}

int main()
{
	g_form.EventAdd(0, eForm_Load, OnFormLoad);
	g_form.EventAdd(ID_btnAGrab, eCommandButton_Click, OnAGrab);
	g_form.EventAdd(ID_btnAShow, eCommandButton_Click, OnAShow);
	g_form.EventAdd(ID_btnBGrab, eCommandButton_Click, OnBGrab);
	g_form.EventAdd(ID_btnBShow, eCommandButton_Click, OnBShow);
	g_form.EventAdd(ID_btnCFill, eCommandButton_Click, OnCFill);
	g_form.EventAdd(ID_btnCGrab, eCommandButton_Click, OnCGrab);
	g_form.EventAdd(ID_btnCShow, eCommandButton_Click, OnCShow);

	g_form.Show();
	return 0;
}
