#include "resource.h"
#include "BForm.h"
#include <strsafe.h>
#include <vector>

static CBForm g_form(ID_form1);
static const UINT kTimerPomodoro = 1001;
static const int kTickMs = 200;
static const int kFrameCount = 12;
static const TCHAR* kFramePattern = TEXT("%s\\assets\\pomodoro_frames\\tomato_%02d.bmp");

static bool g_running = false;
static bool g_isWorkMode = true;
static int g_roundCount = 0;
static int g_workMinutes = 25;
static int g_breakMinutes = 5;
static int g_remainSeconds = 25 * 60;
static int g_accMs = 0;

static std::vector<tstring> g_frames;
static int g_frameIndex = 0;

static tstring BuildFramePath(int idx)
{
	TCHAR exeDir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, exeDir, MAX_PATH);
	for (int i = lstrlen(exeDir) - 1; i >= 0; --i)
	{
		if (exeDir[i] == TEXT('\\') || exeDir[i] == TEXT('/'))
		{
			exeDir[i] = 0;
			break;
		}
	}
	TCHAR path[MAX_PATH] = { 0 };
	StringCchPrintf(path, MAX_PATH, kFramePattern, exeDir, idx);
	return tstring(path);
}

static int ParseEditMinutes(unsigned short idEdit, int fallbackValue)
{
	int value = g_form.Control(idEdit, false).TextInt();
	if (value <= 0) return fallbackValue;
	if (value > 240) return 240;
	return value;
}

static void UpdateLabels()
{
	TCHAR sRemain[64] = { 0 };
	int mm = g_remainSeconds / 60;
	int ss = g_remainSeconds % 60;
	StringCchPrintf(sRemain, 64, TEXT("Remain: %02d:%02d"), mm, ss);
	g_form.Control(ID_txtRemain, false).TextSet(sRemain);

	g_form.Control(ID_txtMode, false).TextSet(g_isWorkMode ? TEXT("Mode: Work") : TEXT("Mode: Break"));

	TCHAR sRound[64] = { 0 };
	StringCchPrintf(sRound, 64, TEXT("Round: %d"), g_roundCount);
	g_form.Control(ID_txtRound, false).TextSet(sRound);

	g_form.Control(ID_btnStartPause, false).TextSet(g_running ? TEXT("Pause") : TEXT("Start"));
}

static void UpdateTomatoFrame(bool force = false)
{
	if (g_frames.empty()) return;
	if (!force) g_frameIndex = (g_frameIndex + 1) % (int)g_frames.size();
	g_form.Control(ID_picTomato, false).PictureSet(g_frames[g_frameIndex].c_str());
}

static void EnterMode(bool workMode)
{
	g_isWorkMode = workMode;
	g_remainSeconds = (workMode ? g_workMinutes : g_breakMinutes) * 60;
	g_frameIndex = 0;
	UpdateTomatoFrame(true);
	UpdateLabels();
}

static void StartTimer()
{
	if (g_running) return;
	SetTimer(g_form.hWnd(), kTimerPomodoro, kTickMs, NULL);
	g_running = true;
	UpdateLabels();
}

static void StopTimer()
{
	KillTimer(g_form.hWnd(), kTimerPomodoro);
	g_running = false;
	g_accMs = 0;
	UpdateLabels();
}

static void ToggleStartPause()
{
	if (g_running) StopTimer();
	else StartTimer();
}

static void ApplyMinutesFromInputs()
{
	g_workMinutes = ParseEditMinutes(ID_editWorkMin, g_workMinutes);
	g_breakMinutes = ParseEditMinutes(ID_editBreakMin, g_breakMinutes);
	g_form.Control(ID_editWorkMin, false).TextSet(g_workMinutes);
	g_form.Control(ID_editBreakMin, false).TextSet(g_breakMinutes);
}

static void ResetPomodoro()
{
	StopTimer();
	ApplyMinutesFromInputs();
	g_roundCount = 0;
	EnterMode(true);
	g_form.Control(ID_txtStatus, false).TextSet(TEXT("Pomodoro reset"));
}

static void SkipCurrentMode()
{
	bool wasWork = g_isWorkMode;
	if (wasWork) ++g_roundCount;
	EnterMode(!g_isWorkMode);
	g_form.Control(ID_txtStatus, false).TextSet(wasWork ? TEXT("Switched to break") : TEXT("Switched to work"));
}

static void OnFormLoad()
{
	g_form.IconSet(IDI_ICON1);
	g_form.TextSet(TEXT("Pomodoro Timer"));
	g_form.MoveToScreenCenter(520, 360);
	g_form.KeyPreview = true;

	g_form.Control(ID_editWorkMin, false).TextSet(g_workMinutes);
	g_form.Control(ID_editBreakMin, false).TextSet(g_breakMinutes);

	g_frames.clear();
	for (int i = 0; i < kFrameCount; ++i) g_frames.push_back(BuildFramePath(i));

	EnterMode(true);
	g_form.Control(ID_txtStatus, false).TextSet(TEXT("Ready to focus"));
}

static void OnStartPauseClick()
{
	ToggleStartPause();
	g_form.Control(ID_txtStatus, false).TextSet(g_running ? TEXT("Running") : TEXT("Paused"));
}

static void OnResetClick()
{
	ResetPomodoro();
}

static void OnSkipClick()
{
	SkipCurrentMode();
}

static void OnKeyDown(int keyCode, int, int)
{
	if (keyCode == VK_SPACE) OnStartPauseClick();
	else if (keyCode == 'R') OnResetClick();
	else if (keyCode == 'N') OnSkipClick();
}

static void OnTimer(int, int, int)
{
	if (!g_running) return;
	g_accMs += kTickMs;
	UpdateTomatoFrame();

	if (g_accMs < 1000) return;
	g_accMs = 0;

	if (g_remainSeconds > 0) --g_remainSeconds;

	if (g_remainSeconds <= 0)
	{
		bool finishedWork = g_isWorkMode;
		if (finishedWork)
		{
			++g_roundCount;
			g_form.Control(ID_txtStatus, false).TextSet(TEXT("Work finished, break started"));
		}
		else
		{
			g_form.Control(ID_txtStatus, false).TextSet(TEXT("Break finished, work started"));
		}
		EnterMode(!g_isWorkMode);
	}
	else
	{
		UpdateLabels();
	}
}

int main()
{
	g_form.EventAdd(0, eForm_Load, OnFormLoad);
	g_form.EventAdd(ID_btnStartPause, eCommandButton_Click, OnStartPauseClick);
	g_form.EventAdd(ID_btnReset, eCommandButton_Click, OnResetClick);
	g_form.EventAdd(ID_btnSkip, eCommandButton_Click, OnSkipClick);
	g_form.EventAdd(0, eKeyDown, OnKeyDown);
	g_form.EventAdd(0, (ECBFormCtrlEventsIII)WM_TIMER, OnTimer);

	g_form.Show();
	return 0;
}
