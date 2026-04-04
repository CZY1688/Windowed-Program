#include "resource.h"
#include "BForm.h"
#include "RedPacket.h"
#include <strsafe.h>
#include <tchar.h>
#include <vector>

class RedPacketApp
{
public:
RedPacketApp()
: m_form(ID_form1),
  m_packetA(30.0, 5, "Owner A"),
  m_packetB(50.0, 8, "Owner B"),
  m_packetC(0.0, 1, "Owner C"),
  m_packetCReady(false),
  m_robotIndex(1)
{
}

CBForm& Form()
{
return m_form;
}

// 中文界面常量（使用 \x 转义，兼容旧工具链编码）
LPCTSTR TCN_WindowTitle() const { return TEXT("\x6A21\x62DF\x5FAE\x4FE1\x62A2\x7EA2\x5305"); } // 模拟微信抢红包
LPCTSTR TCN_GroupA() const { return TEXT("\x7EA2\x5305\x41\xFF08\x94B1\x5DF2\x585E\x597D\xFF0C\x76F4\x63A5\x5F00\x62A2\xFF09"); } // 红包A（钱已塞好，直接开抢）
LPCTSTR TCN_GroupB() const { return TEXT("\x7EA2\x5305\x42\xFF08\x94B1\x5DF2\x585E\x597D\xFF0C\x76F4\x63A5\x5F00\x62A2\xFF09"); } // 红包B（钱已塞好，直接开抢）
LPCTSTR TCN_GroupC() const { return TEXT("\x7EA2\x5305\x43\xFF08\x5148\x585E\x94B1\xFF0C\x624D\x80FD\x62A2\xFF09"); } // 红包C（先塞钱，才能抢）
LPCTSTR TCN_Grab() const { return TEXT("\x62A2\x7EA2\x5305"); } // 抢红包
LPCTSTR TCN_View() const { return TEXT("\x67E5\x770B"); } // 查看
LPCTSTR TCN_RobotGrab() const { return TEXT("\x673A\x5668\x4EBA\x62A2\x7EA2\x5305"); } // 机器人抢红包
LPCTSTR TCN_CMoney() const { return TEXT("\x94B1\x6570(\x5143)\xFF1A"); } // 钱数(元)：
LPCTSTR TCN_CNum() const { return TEXT("\x5206\x51E0\x4E2A\x7EA2\x5305\xFF1A"); } // 分几个红包：
LPCTSTR TCN_CFill() const { return TEXT("\x585E\x94B1\x8FDB\x7EA2\x5305"); } // 塞钱进红包
LPCTSTR TCN_ResultDefault() const { return TEXT(""); }
LPCTSTR TCN_TitleInfo() const { return TEXT("\x63D0\x793A"); }
LPCTSTR TCN_TitleWarn() const { return TEXT("\x8B66\x544A"); }
LPCTSTR TCN_AnonymousUser() const { return TEXT("\x533F\x540D\x7528\x6237"); } // 匿名用户

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

if (!loadedCoverFromLocalBmp)
{
AppendLog(TEXT("\x672A\x627E\x5230\x672C\x5730\x5C01\x9762\x56FE\xFF0C\x5DF2\x4F7F\x7528\x5185\x7F6E\x56FE\x7247\x3002"));
}
AppendLog(TEXT("\x7EA2\x5305\x6A21\x62DF\x7A0B\x5E8F\x5DF2\x542F\x52A8\x3002"));
UpdateLeftFooter(m_packetA, PacketLabelA());
}

void OnAGrab() { DoGrab(m_packetA, ID_editAName, PacketLabelA(), false, false); }
void OnBGrab() { DoGrab(m_packetB, ID_editBName, PacketLabelB(), false, true); }
void OnCGrab() { DoGrab(m_packetC, ID_editCName, PacketLabelC(), true, false); }

void OnAShow() { LPCTSTR label = PacketLabelA(); ShowPacketLog(m_packetA, label); }
void OnBShow() { LPCTSTR label = PacketLabelB(); ShowPacketLog(m_packetB, label); }
void OnCShow() { LPCTSTR label = PacketLabelC(); ShowPacketLog(m_packetC, label); }

void OnCFill()
{
// 中文注释：红包 C 必须先塞钱成功，才能允许用户点击“抢红包”。
double money = m_form.Control(ID_editCMoney, false).TextVal();
int count = static_cast<int>(m_form.Control(ID_editCNum, false).TextVal());
if (money <= 0.0 || count <= 0)
{
MsgBox(TEXT("\x585E\x94B1\x5931\x8D25\xFF1A\x91D1\x989D\x548C\x4E2A\x6570\x90FD\x5FC5\x987B\x5927\x4E8E\x0030\x3002"), TCN_TitleWarn(), mb_OK, mb_IconExclamation);
return;
}
if (!m_packetC.canSetMoney())
{
MsgBox(TEXT("\x7EA2\x5305\x43\x5DF2\x6709\x7528\x6237\x62A2\x8FC7\xFF0C\x4E0D\x80FD\x91CD\x65B0\x585E\x94B1\x3002"), TCN_TitleWarn(), mb_OK, mb_IconExclamation);
return;
}

m_packetC.setMoney(money, count);
m_packetCReady = true;
m_form.Control(ID_btnCGrab, false).EnabledSet(true);

TCHAR msg[128] = { 0 };
_stprintf_s(msg, _countof(msg), TEXT("\x585E\x94B1\x6210\x529F\xFF01\x5DF2\x8BBE\x7F6E %.2f \x5143\xFF0C\x5171 %d \x4E2A\x7EA2\x5305\x3002"), money, count);
MsgBox(msg, TCN_TitleInfo(), mb_OK, mb_IconInformation);
ShowPacketLog(m_packetC, PacketLabelC());
}

void OnRobotGrab()
{
// 中文注释：机器人按 A->B->C 优先级抢可用红包，便于老师演示自动抢包功能。
if (TryRobotGrabPacket(m_packetA, PacketLabelA(), false, false)) return;
if (TryRobotGrabPacket(m_packetB, PacketLabelB(), false, true)) return;
if (TryRobotGrabPacket(m_packetC, PacketLabelC(), true, false)) return;
MsgBox(TEXT("\x76EE\x524D\x6CA1\x6709\x53EF\x62A2\x7684\x7EA2\x5305\x3002"), TCN_TitleWarn(), mb_OK, mb_IconExclamation);
}

private:
CBForm m_form;
RedPacket m_packetA;
RedPacket m_packetB;
RedPacket m_packetC;
bool m_packetCReady;
int m_robotIndex;

LPCTSTR PacketLabelA() const { return TEXT("\x7EA2\x5305A"); }
LPCTSTR PacketLabelB() const { return TEXT("\x7EA2\x5305B"); }
LPCTSTR PacketLabelC() const { return TEXT("\x7EA2\x5305C"); }

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

void ShowViewMsg(const RedPacket& packet, LPCTSTR packetLabel)
{
TCHAR msg[256] = { 0 };
_stprintf_s(msg, _countof(msg), TEXT("%s\x67E5\x770B\x5B8C\x6210\xFF1A\x5DF2\x62A2 %d / %d \x4E2A\x3002"),
	packetLabel, packet.grabbedCount(), packet.totalCount());
MsgBox(msg, TCN_TitleInfo(), mb_OK, mb_IconInformation);
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
MsgBox(msg, TCN_TitleInfo(), mb_OK, mb_IconInformation);
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
_stprintf_s(line, _countof(line), TEXT("%s\xFF1A\x5DF2\x62A2 %d / %d \x4E2A"), packetLabel, packet.grabbedCount(), packet.totalCount());
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
// 中文注释：查看按钮改为“切换日志内容”，每次只展示当前红包的抢包明细。
m_form.Control(ID_editLog, false).TextSet(TEXT(""));
AppendLog(TEXT("=================================================="));
TCHAR title[128] = { 0 };
_stprintf_s(title, _countof(title), TEXT("%s\x62A2\x5305\x8BB0\x5F55"), packetLabel);
AppendLog(title);
AppendLog(TEXT("--------------------------------------------------"));

std::vector<std::string> list = packet.records();
std::string best = packet.bestLuckRecord();
bool hasBest = !best.empty();
if (list.empty())
{
AppendLog(TEXT("\x6682\x65E0\x62A2\x5305\x8BB0\x5F55\x3002"));
}
else
{
AppendLog(TEXT("\x7528\x6237\x5217\x8868\xFF1A"));
for (size_t i = 0; i < list.size(); ++i)
{
std::string who;
std::string money;
if (!ParseNameMoney(list[i], who, money)) continue;
TCHAR line[256] = { 0 };
bool isBest = hasBest && list[i] == best;
_stprintf_s(line, _countof(line), isBest
	? TEXT("%d. \x7528\x6237\xFF1A%s\xFF0C\x91D1\x989D\xFF1A%s \x5143\x3000\x3010\x624B\x6C14\x6700\x4F73\x3011")
	: TEXT("%d. \x7528\x6237\xFF1A%s\xFF0C\x91D1\x989D\xFF1A%s \x5143"),
	static_cast<int>(i + 1), ToTString(who).c_str(), ToTString(money).c_str());
AppendLog(line);
}
}

	if (best.empty())
	{
	AppendLog(TEXT("\x624B\x6C14\x6700\x4F73\xFF1A\x6682\x65E0"));
}
else
{
std::string who;
std::string money;
if (ParseNameMoney(best, who, money))
{
TCHAR line[256] = { 0 };
_stprintf_s(line, _countof(line), TEXT("\x624B\x6C14\x6700\x4F73\xFF1A%s\xFF0C%s \x5143"), ToTString(who).c_str(), ToTString(money).c_str());
AppendLog(line);
}
}

TCHAR progress[128] = { 0 };
_stprintf_s(progress, _countof(progress), TEXT("\x8FDB\x5EA6\xFF1A\x5DF2\x62A2 %d / %d \x4E2A\x3002"), packet.grabbedCount(), packet.totalCount());
AppendLog(progress);
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

void DoGrab(RedPacket& packet, unsigned short idNameEdit, LPCTSTR packetLabel, bool checkReady, bool showResultText)
{
if (checkReady && !m_packetCReady)
{
MsgBox(TEXT("\x7EA2\x5305\x43\x8FD8\x6CA1\x585E\x94B1\xFF0C\x8BF7\x5148\x70B9\x51FB\x201C\x585E\x94B1\x8FDB\x7EA2\x5305\x201D\x3002"), TCN_TitleWarn(), mb_OK, mb_IconExclamation);
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
MsgBox(msg, TCN_TitleWarn(), mb_OK, mb_IconExclamation);
return;
}
if (got <= 0.0)
{
TCHAR msg[256] = { 0 };
_stprintf_s(msg, _countof(msg), TEXT("%s\x5DF2\x62A2\x5B8C\xFF0C\x624B\x6162\x4E86\xFF01"), packetLabel);
MsgBox(msg, TCN_TitleWarn(), mb_OK, mb_IconExclamation);
if (showResultText) m_form.Control(ID_txtResult, false).TextSet(TEXT("\x624B\x6162\x4E86\xFF0C\x7EA2\x5305\x5DF2\x62A2\x5B8C\x3002"));
ShowBestLuckMsg(packet, packetLabel);
return;
}

TCHAR msg[256] = { 0 };
_stprintf_s(msg, _countof(msg), TEXT("\x606D\x559C\xFF01%s \x5728%s\x62A2\x5230 %.2f \x5143\x3002"), ToTString(who).c_str(), packetLabel, got);
MsgBox(msg, TCN_TitleInfo(), mb_OK, mb_IconInformation);
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

int main()
{
g_app.Form().EventAdd(0, eForm_Load, AppOnFormLoad);
g_app.Form().EventAdd(ID_picCover, eStatic_Click, AppOnAGrab);
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
