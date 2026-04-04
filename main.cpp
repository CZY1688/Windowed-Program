#include "resource.h"
#include "BForm.h"
#include "RedPacket.h"
#include <strsafe.h>
#include <tchar.h>
#include <vector>

#define PACKET_LABEL_PREFIX TEXT("\x7EA2\x5305")

class RedPacketApp
{
public:
enum ECurrentPacket
{
CurrentPacketA = 0,
CurrentPacketB = 1,
CurrentPacketC = 2
};

RedPacketApp()
: m_form(ID_form1),
  m_packetA(30.0, 5, "Owner A"),
  m_packetB(50.0, 8, "Owner B"),
  m_packetC(0.0, 1, "Owner C"),
  m_packetCReady(false),
  m_currentPacket(CurrentPacketA),
  m_robotIndex(1)
{
}

CBForm& Form()
{
return m_form;
}

// All TCN_* methods below are UI text constants via \x escapes for legacy toolchain compatibility.
LPCTSTR TCN_WindowTitle() const { return TEXT("\x6A21\x62DF\x5FAE\x4FE1\x62A2\x7EA2\x5305"); }
LPCTSTR TCN_GroupA() const { return TEXT("\x7EA2\x5305\x41\xFF08\x94B1\x5DF2\x585E\x597D\xFF0C\x76F4\x63A5\x5F00\x62A2\xFF09"); }
LPCTSTR TCN_GroupB() const { return TEXT("\x7EA2\x5305\x42\xFF08\x94B1\x5DF2\x585E\x597D\xFF0C\x76F4\x63A5\x5F00\x62A2\xFF09"); }
LPCTSTR TCN_GroupC() const { return TEXT("\x7EA2\x5305\x43\xFF08\x5148\x585E\x94B1\xFF0C\x624D\x80FD\x62A2\xFF09"); }
LPCTSTR TCN_Grab() const { return TEXT("\x62A2\x7EA2\x5305"); }
LPCTSTR TCN_View() const { return TEXT("\x67E5\x770B"); }
LPCTSTR TCN_RobotGrab() const { return TEXT("\x673A\x5668\x4EBA\x62A2\x7EA2\x5305"); }
LPCTSTR TCN_CMoney() const { return TEXT("\x94B1\x6570(\x5143)\xFF1A"); }
LPCTSTR TCN_CNum() const { return TEXT("\x5206\x51E0\x4E2A\x7EA2\x5305\xFF1A"); }
LPCTSTR TCN_CFill() const { return TEXT("\x585E\x94B1\x8FDB\x7EA2\x5305"); }
LPCTSTR TCN_ResultDefault() const { return TEXT(""); }
LPCTSTR TCN_TitleInfo() const { return TEXT("\x63D0\x793A"); }
LPCTSTR TCN_TitleWarn() const { return TEXT("\x8B66\x544A"); }
LPCTSTR TCN_AnonymousUser() const { return TEXT("\x533F\x540D\x7528\x6237"); }

void OnFormLoad()
{
m_form.IconSet(IDI_ICON1);
m_form.TextSet(TCN_WindowTitle());
m_form.MoveToScreenCenter();
m_form.BackColorSet(RGB(236, 236, 236));
m_form.KeyPreview = true;

bool loadedCoverFromLocalBmp = false;
tstring coverPath;
if (TryBuildExeRelativeCoverPath(coverPath))
{
loadedCoverFromLocalBmp = TryLoadCoverFromPath(coverPath.c_str());
}
if (!loadedCoverFromLocalBmp)
{
loadedCoverFromLocalBmp = TryLoadCoverFromPath(TEXT("assets\\redpacket_cover.bmp"));
}
if (!loadedCoverFromLocalBmp)
{
m_form.Control(ID_picCover, false).PictureSet(IDB_PACKET_COVER);
}

ApplyRuntimeTexts();
ResetUIState();

	UpdateLeftFooter(m_packetA, PacketLabelA());
}

void OnAGrab() { DoGrab(m_packetA, ID_editAName, PacketLabelA(), false, false); }
void OnBGrab() { DoGrab(m_packetB, ID_editBName, PacketLabelB(), false, true); }
void OnCGrab() { DoGrab(m_packetC, ID_editCName, PacketLabelC(), true, false); }

void OnAShow() { m_currentPacket = CurrentPacketA; LPCTSTR label = PacketLabelA(); ShowPacketLog(m_packetA, label); }
void OnBShow() { m_currentPacket = CurrentPacketB; LPCTSTR label = PacketLabelB(); ShowPacketLog(m_packetB, label); }
void OnCShow() { m_currentPacket = CurrentPacketC; LPCTSTR label = PacketLabelC(); ShowPacketLog(m_packetC, label); }

void OnCFill()
{
// Packet C must be filled before grab is enabled.
double money = m_form.Control(ID_editCMoney, false).TextVal();
int count = static_cast<int>(m_form.Control(ID_editCNum, false).TextVal());
	if (money <= 0.0 || count <= 0)
	{
		UpdateLeftFooter(m_packetC, PacketLabelC());
		ShowWarnBox(TEXT("\x585E\x94B1\x5931\x8D25\xFF1A\x91D1\x989D\x548C\x4E2A\x6570\x90FD\x5FC5\x987B\x5927\x4E8E\x0030\x3002"));
		return;
	}
	if (!m_packetC.canSetMoney())
	{
		UpdateLeftFooter(m_packetC, PacketLabelC());
		ShowWarnBox(TEXT("\x7EA2\x5305\x43\x5DF2\x6709\x7528\x6237\x62A2\x8FC7\xFF0C\x4E0D\x80FD\x91CD\x65B0\x585E\x94B1\x3002"));
		return;
	}

m_packetC.setMoney(money, count);
m_packetCReady = true;
m_form.Control(ID_btnCGrab, false).EnabledSet(true);

TCHAR msg[128] = { 0 };
_stprintf_s(msg, _countof(msg), TEXT("\x585E\x94B1\x6210\x529F\xFF01\x5DF2\x8BBE\x7F6E %.2f \x5143\xFF0C\x5171 %d \x4E2A\x7EA2\x5305\x3002"), money, count);
ShowInfoBox(msg);
ShowPacketLog(m_packetC, PacketLabelC());
}

void OnRobotGrab()
{
// Robot only grabs the packet currently being viewed.
TCHAR robotName[64] = { 0 };
_stprintf_s(robotName, _countof(robotName), TEXT("\x673A\x5668\x4EBA%d"), m_robotIndex++);
DoGrabCurrentWithName(ToString(robotName));
}

void OnCoverClickGrabCurrent()
{
DoGrabCurrentFromInput();
}

private:
CBForm m_form;
RedPacket m_packetA;
RedPacket m_packetB;
RedPacket m_packetC;
bool m_packetCReady;
ECurrentPacket m_currentPacket;
int m_robotIndex;

// Split literals to avoid \x... consuming following hex-like chars in old compilers.
LPCTSTR PacketLabelA() const { return PACKET_LABEL_PREFIX TEXT("A"); } // Red Packet A
LPCTSTR PacketLabelB() const { return PACKET_LABEL_PREFIX TEXT("B"); } // Red Packet B
LPCTSTR PacketLabelC() const { return PACKET_LABEL_PREFIX TEXT("C"); } // Red Packet C

int ShowInfoBox(LPCTSTR msg)
{
	return MessageBox(m_form.hWnd(), msg, TCN_TitleInfo(), MB_OK | MB_ICONINFORMATION);
}

int ShowWarnBox(LPCTSTR msg)
{
	return MessageBox(m_form.hWnd(), msg, TCN_TitleWarn(), MB_OK | MB_ICONWARNING);
}

void ApplyRuntimeTexts()
{
LPCTSTR textGrab = TCN_Grab();
LPCTSTR textView = TCN_View();
m_form.Control(ID_grpA, false).TextSet(TCN_GroupA());
m_form.Control(ID_grpB, false).TextSet(TCN_GroupB());
m_form.Control(ID_grpC, false).TextSet(TCN_GroupC());
m_form.Control(ID_btnAGrab, false).TextSet(textGrab);
m_form.Control(ID_btnAShow, false).TextSet(textView);
m_form.Control(ID_btnBGrab, false).TextSet(textGrab);
m_form.Control(ID_btnBShow, false).TextSet(textView);
m_form.Control(ID_txtCMoney, false).TextSet(TCN_CMoney());
m_form.Control(ID_txtCNum, false).TextSet(TCN_CNum());
m_form.Control(ID_btnCFill, false).TextSet(TCN_CFill());
m_form.Control(ID_btnCGrab, false).TextSet(textGrab);
m_form.Control(ID_btnCShow, false).TextSet(textView);
m_form.Control(ID_btnRobotGrab, false).TextSet(TCN_RobotGrab());
m_form.Control(ID_txtResult, false).TextSet(TCN_ResultDefault());
m_form.Control(ID_txtSep, false).VisibleSet(false);
}

void ResetUIState()
{
m_form.Control(ID_editLog, false).TextSet(TEXT(""));

m_form.Control(ID_editAName, false).TextSet(TEXT(""));
m_form.Control(ID_btnAGrab, false).VisibleSet(true);
m_form.Control(ID_btnAShow, false).VisibleSet(true);
m_form.Control(ID_editAName, false).VisibleSet(true);

m_form.Control(ID_editBName, false).TextSet(TEXT(""));
m_form.Control(ID_editBName, false).VisibleSet(true);
m_form.Control(ID_btnBGrab, false).VisibleSet(true);
m_form.Control(ID_btnBShow, false).VisibleSet(true);
m_form.Control(ID_txtResult, false).VisibleSet(true);

m_form.Control(ID_editCMoney, false).TextSet(TEXT(""));
m_form.Control(ID_editCNum, false).TextSet(TEXT(""));
m_form.Control(ID_editCName, false).TextSet(TEXT(""));
m_form.Control(ID_txtCMoney, false).VisibleSet(true);
m_form.Control(ID_editCMoney, false).VisibleSet(true);
m_form.Control(ID_txtCNum, false).VisibleSet(true);
m_form.Control(ID_editCNum, false).VisibleSet(true);
m_form.Control(ID_btnCFill, false).VisibleSet(true);
m_form.Control(ID_editCName, false).VisibleSet(true);
m_form.Control(ID_btnCGrab, false).VisibleSet(true);
m_form.Control(ID_btnCShow, false).VisibleSet(true);
m_form.Control(ID_btnCGrab, false).EnabledSet(false);
m_form.Control(ID_btnRobotGrab, false).VisibleSet(true);

m_form.Control(ID_grpA, false).ZOrder(1);
m_form.Control(ID_grpB, false).ZOrder(1);
m_form.Control(ID_grpC, false).ZOrder(1);
m_form.Control(ID_editAName, false).ZOrder(0);
m_form.Control(ID_btnAGrab, false).ZOrder(0);
m_form.Control(ID_btnAShow, false).ZOrder(0);
m_form.Control(ID_editBName, false).ZOrder(0);
m_form.Control(ID_btnBGrab, false).ZOrder(0);
m_form.Control(ID_btnBShow, false).ZOrder(0);
m_form.Control(ID_txtResult, false).ZOrder(0);
m_form.Control(ID_txtCMoney, false).ZOrder(0);
m_form.Control(ID_editCMoney, false).ZOrder(0);
m_form.Control(ID_txtCNum, false).ZOrder(0);
m_form.Control(ID_editCNum, false).ZOrder(0);
m_form.Control(ID_btnCFill, false).ZOrder(0);
m_form.Control(ID_editCName, false).ZOrder(0);
m_form.Control(ID_btnCGrab, false).ZOrder(0);
m_form.Control(ID_btnCShow, false).ZOrder(0);
m_form.Control(ID_btnRobotGrab, false).ZOrder(0);
}

bool TryBuildExeRelativeCoverPath(tstring& outPath)
{
TCHAR modulePath[MAX_PATH] = { 0 };
DWORD n = GetModuleFileName(0, modulePath, MAX_PATH);
if (n == 0 || n == MAX_PATH) return false;

TCHAR* pSlashBack = _tcsrchr(modulePath, TEXT('\\'));
TCHAR* pSlashFwd = _tcsrchr(modulePath, TEXT('/'));
TCHAR* pSlash = pSlashBack;
if (!pSlash || (pSlashFwd && pSlashFwd > pSlash)) pSlash = pSlashFwd;
if (!pSlash) return false;

*pSlash = 0;
outPath = modulePath;
outPath += TEXT("\\assets\\redpacket_cover.bmp");
return true;
}

bool TryLoadCoverFromPath(LPCTSTR bmpPath)
{
DWORD attr = GetFileAttributes(bmpPath);
if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY) != 0)
{
return false;
}
m_form.Control(ID_picCover, false).PictureSet(bmpPath);
return true;
}

tstring ToTString(const std::string& s)
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

std::string ToString(const tstring& s)
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

tstring ReadText(unsigned short idEdit)
{
return m_form.Control(idEdit, false).Text();
}

std::string ReadNameOrDefault(unsigned short idEdit)
{
tstring s = ReadText(idEdit);
if (s.empty()) return ToString(TCN_AnonymousUser());
return ToString(s);
}

void ShowBestLuckMsg(const RedPacket& packet, LPCTSTR packetLabel)
{
std::string best = packet.bestLuckRecord();
TCHAR msg[256] = { 0 };
if (best.empty())
{
	_stprintf_s(msg, _countof(msg), TEXT("%s\x624B\x6C14\x6700\x4F73\xFF1A\x6682\x65E0\x3002"), packetLabel);
}
else
{
	std::string who;
	std::string money;
	if (ParseNameMoney(best, who, money))
	{
		_stprintf_s(msg, _countof(msg), TEXT("%s\x624B\x6C14\x6700\x4F73\xFF1A%s\xFF0C%s \x5143\x3002"),
			packetLabel, ToTString(who).c_str(), ToTString(money).c_str());
	}
	else
	{
		_stprintf_s(msg, _countof(msg), TEXT("%s\x624B\x6C14\x6700\x4F73\xFF1A%s\x3002"),
			packetLabel, ToTString(best).c_str());
	}
}
ShowInfoBox(msg);
}

void AppendLog(LPCTSTR s)
{
m_form.Control(ID_editLog, false).TextAdd(s);
m_form.Control(ID_editLog, false).TextAdd(TEXT("\r\n"));
}

void SetLogLine(LPCTSTR s)
{
m_form.Control(ID_editLog, false).TextSet(TEXT(""));
AppendLog(s);
}

void UpdateResultText(const std::string& grabber, double money)
{
TCHAR line[256] = { 0 };
_stprintf_s(line, _countof(line), TEXT("\x606D\x559C\xFF01%s \x62A2\x5230 %.2f \x5143\x7EA2\x5305\xFF01"), ToTString(grabber).c_str(), money);
m_form.Control(ID_txtResult, false).TextSet(line);
}

void UpdateLeftFooter(const RedPacket& packet, LPCTSTR packetLabel)
{
	TCHAR line[256] = { 0 };
	int grabbed = packet.grabbedCount();
	int total = packet.totalCount();
	int left = total - grabbed;
	if (left < 0) left = 0;
	_stprintf_s(line, _countof(line), TEXT("%s\xFF1A\x5DF2\x62A2 %d \x4E2A\xFF0C\x5269\x4F59 %d \x4E2A"), packetLabel, grabbed, left);
	m_form.Control(ID_txtLeftStatic, false).TextSet(line);
}

bool ParseNameMoney(const std::string& src, std::string& outName, std::string& outMoney)
{
size_t pos = src.rfind(':');
if (pos == std::string::npos) return false;
outName = src.substr(0, pos);
outMoney = src.substr(pos + 1);
return true;
}

void ShowPacketLog(const RedPacket& packet, LPCTSTR packetLabel)
{
// Refresh log with current packet details after view/grab.
m_form.Control(ID_editLog, false).TextSet(TEXT(""));
	std::vector<std::string> recs = packet.records();
	std::string best = packet.bestLuckRecord();
	AppendLog(TEXT("\x62A2\x7EA2\x5305\x8BB0\x5F55\xFF1A"));
	for (size_t i = 0; i < recs.size(); ++i)
	{
		std::string who;
		std::string money;
		TCHAR line[256] = { 0 };
		if (ParseNameMoney(recs[i], who, money))
		{
			if (!best.empty() && recs[i] == best)
			{
				_stprintf_s(line, _countof(line), TEXT("%s \x62A2\x5230\x4E86 %s \x5143\xFF0C\x624B\x6C14\x6700\x4F73\xFF01"), ToTString(who).c_str(), ToTString(money).c_str());
			}
			else
			{
				_stprintf_s(line, _countof(line), TEXT("%s \x62A2\x5230\x4E86 %s \x5143"), ToTString(who).c_str(), ToTString(money).c_str());
			}
		}
		else
		{
			_stprintf_s(line, _countof(line), TEXT("%s"), ToTString(recs[i]).c_str());
		}
		AppendLog(line);
	}
	if (recs.empty()) AppendLog(TEXT("\x6682\x65E0\x62A2\x5305\x8BB0\x5F55"));
	UpdateLeftFooter(packet, packetLabel);
}

bool TryRobotGrabPacket(RedPacket& packet, LPCTSTR packetLabel, bool checkReady, bool showResultText)
{
if (checkReady && !m_packetCReady) return false;
if (packet.grabbedCount() >= packet.totalCount()) return false;

TCHAR robotName[64] = { 0 };
_stprintf_s(robotName, _countof(robotName), TEXT("\x673A\x5668\x4EBA%d"), m_robotIndex++);
DoGrabWithName(packet, ToString(robotName), packetLabel, showResultText);
return true;
}

void DoGrabCurrentFromInput()
{
if (m_currentPacket == CurrentPacketA)
{
	DoGrab(m_packetA, ID_editAName, PacketLabelA(), false, false);
	return;
}
if (m_currentPacket == CurrentPacketB)
{
	DoGrab(m_packetB, ID_editBName, PacketLabelB(), false, true);
	return;
}
DoGrab(m_packetC, ID_editCName, PacketLabelC(), true, false);
}

void DoGrabCurrentWithName(const std::string& who)
{
if (m_currentPacket == CurrentPacketA)
{
	DoGrabWithName(m_packetA, who, PacketLabelA(), false);
	return;
}
if (m_currentPacket == CurrentPacketB)
{
	DoGrabWithName(m_packetB, who, PacketLabelB(), true);
	return;
}
if (!m_packetCReady)
{
	UpdateLeftFooter(m_packetC, PacketLabelC());
	ShowWarnBox(TEXT("\x7EA2\x5305\x43\x8FD8\x6CA1\x585E\x94B1\xFF0C\x8BF7\x5148\x70B9\x51FB\x201C\x585E\x94B1\x8FDB\x7EA2\x5305\x201D\x3002"));
	return;
}
DoGrabWithName(m_packetC, who, PacketLabelC(), false);
}

void DoGrab(RedPacket& packet, unsigned short idNameEdit, LPCTSTR packetLabel, bool checkReady, bool showResultText)
{
	if (checkReady && !m_packetCReady)
	{
		UpdateLeftFooter(packet, packetLabel);
		ShowWarnBox(TEXT("\x7EA2\x5305\x43\x8FD8\x6CA1\x585E\x94B1\xFF0C\x8BF7\x5148\x70B9\x51FB\x201C\x585E\x94B1\x8FDB\x7EA2\x5305\x201D\x3002"));
		return;
	}
std::string who = ReadNameOrDefault(idNameEdit);
DoGrabWithName(packet, who, packetLabel, showResultText);
}

void DoGrabWithName(RedPacket& packet, const std::string& who, LPCTSTR packetLabel, bool showResultText)
{
int status = RedPacket::GrabEmpty;
double got = packet.grab(who, &status);
	if (status == RedPacket::GrabDuplicate)
	{
		TCHAR msg[256] = { 0 };
		_stprintf_s(msg, _countof(msg), TEXT("%s \x5DF2\x7ECF\x62A2\x8FC7%s\xFF0C\x6BCF\x4E2A\x7528\x6237\x53EA\x80FD\x62A2\x4E00\x6B21\x3002"), ToTString(who).c_str(), packetLabel);
		UpdateLeftFooter(packet, packetLabel);
		ShowWarnBox(msg);
		return;
	}
	if (got <= 0.0)
	{
		TCHAR msg[256] = { 0 };
		_stprintf_s(msg, _countof(msg), TEXT("%s\x5DF2\x62A2\x5B8C\xFF0C\x624B\x6162\x4E86\xFF01"), packetLabel);
		UpdateLeftFooter(packet, packetLabel);
		ShowWarnBox(msg);
		if (showResultText) m_form.Control(ID_txtResult, false).TextSet(TEXT("\x624B\x6162\x4E86\xFF0C\x7EA2\x5305\x5DF2\x62A2\x5B8C\x3002"));
		ShowBestLuckMsg(packet, packetLabel);
		return;
}

TCHAR msg[256] = { 0 };
_stprintf_s(msg, _countof(msg), TEXT("\x606D\x559C\xFF01%s \x5728%s\x62A2\x5230 %.2f \x5143\x3002"), ToTString(who).c_str(), packetLabel, got);
ShowInfoBox(msg);
if (showResultText) UpdateResultText(who, got);
ShowPacketLog(packet, packetLabel);
}
};

RedPacketApp g_app;

void AppOnFormLoad() { g_app.OnFormLoad(); }
void AppOnAGrab() { g_app.OnAGrab(); }
void AppOnAShow() { g_app.OnAShow(); }
void AppOnBGrab() { g_app.OnBGrab(); }
void AppOnBShow() { g_app.OnBShow(); }
void AppOnCFill() { g_app.OnCFill(); }
void AppOnCGrab() { g_app.OnCGrab(); }
void AppOnCShow() { g_app.OnCShow(); }
void AppOnRobotGrab() { g_app.OnRobotGrab(); }
void AppOnCoverClickGrabCurrent() { g_app.OnCoverClickGrabCurrent(); }

int main()
{
g_app.Form().EventAdd(0, eForm_Load, AppOnFormLoad);
g_app.Form().EventAdd(ID_picCover, eStatic_Click, AppOnCoverClickGrabCurrent);
g_app.Form().EventAdd(ID_btnAGrab, eCommandButton_Click, AppOnAGrab);
g_app.Form().EventAdd(ID_btnAShow, eCommandButton_Click, AppOnAShow);
g_app.Form().EventAdd(ID_btnBGrab, eCommandButton_Click, AppOnBGrab);
g_app.Form().EventAdd(ID_btnBShow, eCommandButton_Click, AppOnBShow);
g_app.Form().EventAdd(ID_btnCFill, eCommandButton_Click, AppOnCFill);
g_app.Form().EventAdd(ID_btnCGrab, eCommandButton_Click, AppOnCGrab);
g_app.Form().EventAdd(ID_btnCShow, eCommandButton_Click, AppOnCShow);
g_app.Form().EventAdd(ID_btnRobotGrab, eCommandButton_Click, AppOnRobotGrab);

g_app.Form().Show();
return 0;
}
