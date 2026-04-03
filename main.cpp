#include "resource.h"
#include "BForm.h"
#include "RedPacket.h"
#include <strsafe.h>
#include <tchar.h>
#include <vector>

static CBForm g_form(ID_form1);

static RedPacket g_packetA(30.0, 5, "Owner A");
static RedPacket g_packetB(50.0, 8, "Owner B");
static RedPacket g_packetC(0.0, 1, "Owner C");
static bool g_packetCReady = false;
static LPCTSTR kCoverBmpPath = TEXT("assets\\redpacket_cover.bmp");
static LPCTSTR kCoverBmpRelativeToExe = TEXT("\\assets\\redpacket_cover.bmp");

// Runtime Chinese text is provided via Unicode escapes to keep source/resource ASCII-safe
// for legacy VC2010/CP936 toolchains while still showing Chinese UI at runtime.
static LPCTSTR TCN_WindowTitle()
{
	return TEXT("\x6A21\x62DF\x5FAE\x4FE1\x62A2\x7EA2\x5305");
}

static LPCTSTR TCN_GroupA()
{
	return TEXT("\x7EA2\x5305\x41\xFF08\x94B1\x5DF2\x585E\x597D\xFF0C\x76F4\x63A5\x5F00\x62A2\xFF09");
}

static LPCTSTR TCN_GroupB()
{
	return TEXT("\x7EA2\x5305\x42\xFF08\x94B1\x5DF2\x585E\x597D\xFF0C\x76F4\x63A5\x5F00\x62A2\xFF09");
}

static LPCTSTR TCN_GroupC()
{
	return TEXT("\x7EA2\x5305\x43\xFF08\x5148\x585E\x94B1\xFF0C\x624D\x80FD\x62A2\xFF09");
}

static LPCTSTR TCN_Grab()
{
	return TEXT("\x62A2\x7EA2\x5305");
}

static LPCTSTR TCN_View()
{
	return TEXT("\x67E5\x770B");
}

static LPCTSTR TCN_CMoney()
{
	return TEXT("\x94B1\x6570(\x5143)\xFF1A");
}

static LPCTSTR TCN_CNum()
{
	return TEXT("\x5206\x51E0\x4E2A\x7EA2\x5305\xFF1A");
}

static LPCTSTR TCN_CFill()
{
	return TEXT("\x585E\x94B1\x8FDB\x7EA2\x5305");
}

// Build an exe-relative path so local bmp works even when cwd is different.
static bool TryBuildExeRelativeCoverPath(tstring& outPath)
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
	outPath += kCoverBmpRelativeToExe;
	return true;
}

// Try loading cover bmp from a file path; returns true on success.
static bool TryLoadCoverFromPath(LPCTSTR bmpPath)
{
	DWORD attr = GetFileAttributes(bmpPath);
	if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY) != 0)
	{
		return false;
	}
	g_form.Control(ID_picCover, false).PictureSet(bmpPath);
	return true;
}

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

// Unified log output: append one line and newline into the left log box.
static void AppendLog(LPCTSTR s)
{
	g_form.Control(ID_editLog, false).TextAdd(s);
	g_form.Control(ID_editLog, false).TextAdd(TEXT("\r\n"));
}

// Empty name falls back to Anonymous to keep record format stable.
static std::string ReadNameOrDefault(unsigned short idEdit)
{
	tstring s = ReadText(idEdit);
	if (s.empty()) return "Anonymous";
	return ToString(s);
}

static void AppendGrabResult(LPCTSTR packetName, const std::string& grabber, double money)
{
	TCHAR line[256] = { 0 };
	_stprintf_s(line, _countof(line), TEXT("%s - %s grabbed %.2f"),
		packetName, ToTString(grabber).c_str(), money);
	AppendLog(line);
}

// Print packet detail block into the common log area.
static void AppendSummary(const RedPacket& packet, LPCTSTR title)
{
	AppendLog(TEXT("--------------------------------------------------"));
	AppendLog(title);
	tstring s = ToTString(packet.summary());
	AppendLog(s.c_str());
}

static void DoGrab(RedPacket& packet, unsigned short idNameEdit, LPCTSTR packetName, bool checkReady = false)
{
	// Packet C requires a successful funding step before grab is allowed.
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
		_stprintf_s(line, _countof(line), TEXT("%s is empty"), packetName);
		AppendLog(line);
		return;
	}
	AppendGrabResult(packetName, who, got);
}

static void OnFormLoad()
{
	// Keep window icon setup: required by assignment and runtime UX.
	g_form.IconSet(IDI_ICON1);
	g_form.TextSet(TCN_WindowTitle());
	g_form.MoveToScreenCenter(920, 650);
	g_form.BackColorSet(RGB(236, 236, 236));
	g_form.KeyPreview = true;

	bool loadedCoverFromLocalBmp = false;
	tstring coverPath;
	if (TryBuildExeRelativeCoverPath(coverPath))
	{
		loadedCoverFromLocalBmp = TryLoadCoverFromPath(coverPath.c_str());
	}
	if (!loadedCoverFromLocalBmp)
	{
		loadedCoverFromLocalBmp = TryLoadCoverFromPath(kCoverBmpPath);
	}
	if (!loadedCoverFromLocalBmp)
		g_form.Control(ID_picCover, false).PictureSet(IDB_PACKET_COVER);

	// Runtime captions are applied here (resource text stays ASCII-safe).
	LPCTSTR textGrab = TCN_Grab();
	LPCTSTR textView = TCN_View();
	g_form.Control(ID_grpA, false).TextSet(TCN_GroupA());
	g_form.Control(ID_grpB, false).TextSet(TCN_GroupB());
	g_form.Control(ID_grpC, false).TextSet(TCN_GroupC());
	g_form.Control(ID_btnAGrab, false).TextSet(textGrab);
	g_form.Control(ID_btnAShow, false).TextSet(textView);
	g_form.Control(ID_btnBGrab, false).TextSet(textGrab);
	g_form.Control(ID_btnBShow, false).TextSet(textView);
	g_form.Control(ID_txtCMoney, false).TextSet(TCN_CMoney());
	g_form.Control(ID_txtCNum, false).TextSet(TCN_CNum());
	g_form.Control(ID_btnCFill, false).TextSet(TCN_CFill());
	g_form.Control(ID_btnCGrab, false).TextSet(textGrab);
	g_form.Control(ID_btnCShow, false).TextSet(textView);
	g_form.Control(ID_editLog, false).TextSet(TEXT(""));

	g_form.Control(ID_editAName, false).TextSet(TEXT(""));
	// Packet A UI controls are hidden in runtime; cover click is the trigger.
	g_form.Control(ID_btnAGrab, false).VisibleSet(false);
	g_form.Control(ID_btnAShow, false).VisibleSet(false);
	g_form.Control(ID_editAName, false).VisibleSet(false);

	g_form.Control(ID_editBName, false).TextSet(TEXT(""));

	g_form.Control(ID_editCMoney, false).TextSet(TEXT(""));
	g_form.Control(ID_editCNum, false).TextSet(TEXT(""));
	g_form.Control(ID_editCName, false).TextSet(TEXT(""));
	g_form.Control(ID_btnCGrab, false).EnabledSet(false);

	if (!loadedCoverFromLocalBmp)
	{
		AppendLog(TEXT("Local BMP not found, using embedded cover image."));
	}
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
	// Read custom amount + count from C group input boxes.
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
	// After funding succeeds, enable C-Grab button.
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
	// Packet A edit/button controls are hidden at runtime, so cover click triggers Packet A grab.
	g_form.EventAdd(ID_picCover, eStatic_Click, OnAGrab);
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
