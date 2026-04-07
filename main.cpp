#include "resource.h"   // 资源文件头，定义了控件ID（如ID_form1, ID_btnAGrab等）和资源ID（如IDI_ICON1）
#include "BForm.h"      // 自定义UI框架头文件，提供CBForm类用于窗口和控件管理
#include "RedPacket.h"   // 红包逻辑类头文件，提供RedPacket类处理金额、拆分、记录等核心逻辑
#include <strsafe.h>    // 安全字符串操作库（虽然保留了头文件，但下面未使用其函数）
#include <tchar.h>      // TCHAR宏定义，支持Unicode和多字节字符集切换

using namespace std;  // 简化标准库调用（如vector不用写std::vector）

// 定义红包UI标签的前缀
#define PACKET_LABEL_PREFIX TEXT("红包")

// 应用程序主类，负责整合UI逻辑与红包业务逻辑
class RedPacketApp
{
public:
    // 枚举：当前选中/查看的红包（A, B, 或 C）
    enum ECurrentPacket
    {
        CurrentPacketA = 0,
        CurrentPacketB = 1,
        CurrentPacketC = 2
    };

    // 构造函数：初始化成员变量（使用初始化列表）
    RedPacketApp()
        : m_form(ID_form1),                          // 绑定窗口资源ID
        m_packetA(30.0, 5, "Owner A"),              // 初始化红包A：30元，5个，所有者A
        m_packetB(50.0, 8, "Owner B"),              // 初始化红包B：50元，8个，所有者B
        m_packetC(0.0, 1, "Owner C"),               // 初始化红包C：初始金额0，需塞钱
        m_packetCReady(false),                       // 红包C初始状态：未塞钱
        m_currentPacket(CurrentPacketA),             // 默认当前查看红包A
        m_robotIndex(1)                              // 机器人序号初始化为1
    {
    }

    // 获取窗体对象的引用，用于外部（如main函数）访问
    CBForm& Form()
    {
        return m_form;
    }

    // --- 界面文本常量定义区 ---
    LPCTSTR TCN_TitleInfo() const { return TEXT("提示"); }
    LPCTSTR TCN_TitleWarn() const { return TEXT("警告"); }
    LPCTSTR TCN_AnonymousUser() const { return TEXT("匿名用户"); }

    // --- 窗体事件处理逻辑 ---

    // 窗体加载事件：初始化界面外观、加载图片、设置初始状态
    void OnFormLoad()
    {
        m_form.IconSet(IDI_ICON1);                               // 设置窗口图标
        m_form.MoveToScreenCenter();                              // 窗口居中显示
        m_form.BackColorSet(RGB(236, 236, 236));                // 设置背景色（浅灰色）
        m_form.KeyPreview = true;                                 // 允许窗体预览键盘事件

        // 1. 尝试从EXE所在目录的相对路径加载封面图片
        bool loadedCoverFromLocalBmp = false;
        tstring coverPath;
        if (TryBuildExeRelativeCoverPath(coverPath))
        {
            loadedCoverFromLocalBmp = TryLoadCoverFromPath(coverPath.c_str());
        }

        // 2. 如果失败，尝试直接从硬编码的相对路径加载
        if (!loadedCoverFromLocalBmp)
        {
            loadedCoverFromLocalBmp = TryLoadCoverFromPath(TEXT("assets\\redpacket_cover.bmp"));
        }

        // 3. 如果还是失败，使用资源文件中的默认图片
        if (!loadedCoverFromLocalBmp)
        {
            m_form.Control(ID_picCover, false).PictureSet(IDB_PACKET_COVER);
        }

        ResetUIState();       // 重置UI控件状态

        // 初始更新左下角状态为红包A的信息
        UpdateLeftFooter(m_packetA, PacketLabelA());
    }

    // 红包A的"抢红包"按钮点击事件
    void OnAGrab() { DoGrab(m_packetA, ID_editAName, PacketLabelA(), false, false); }

    // 红包B的"抢红包"按钮点击事件
    void OnBGrab() { DoGrab(m_packetB, ID_editBName, PacketLabelB(), false, true); }

    // 红包C的"抢红包"按钮点击事件
    void OnCGrab() { DoGrab(m_packetC, ID_editCName, PacketLabelC(), true, false); }

    // 查看红包A记录
    void OnAShow() { m_currentPacket = CurrentPacketA; LPCTSTR label = PacketLabelA(); ShowPacketLog(m_packetA, label); }

    // 查看红包B记录
    void OnBShow() { m_currentPacket = CurrentPacketB; LPCTSTR label = PacketLabelB(); ShowPacketLog(m_packetB, label); }

    // 查看红包C记录
    void OnCShow() { m_currentPacket = CurrentPacketC; LPCTSTR label = PacketLabelC(); ShowPacketLog(m_packetC, label); }

    // 红包C的"塞钱"按钮点击事件
    void OnCFill()
    {
        // 1. 获取输入的金额和数量
        double money = m_form.Control(ID_editCMoney, false).TextVal();
        int count = static_cast<int>(m_form.Control(ID_editCNum, false).TextVal());

        // 2. 校验输入合法性
        if (money <= 0.0 || count <= 0)
        {
            UpdateLeftFooter(m_packetC, PacketLabelC());
            ShowWarnBox(TEXT("塞钱失败：金额和个数都必须大于0。"));
            return;
        }

        // 3. 校验红包状态（如果有人抢过了，就不能再塞钱了）
        if (!m_packetC.canSetMoney())
        {
            UpdateLeftFooter(m_packetC, PacketLabelC());
            ShowWarnBox(TEXT("红包C已有用户抢过，不能重新塞钱。"));
            return;
        }

        // 4. 执行塞钱逻辑
        m_packetC.setMoney(money, count);
        m_packetCReady = true;
        m_form.Control(ID_btnCGrab, false).EnabledSet(true); // 启用"抢红包"按钮

        // 5. 提示成功并刷新日志
        TCHAR msg[128] = { 0 };
        _stprintf(msg, TEXT("塞钱成功！已设置 %.2f 元，共 %d 个红包。"), money, count);
        ShowInfoBox(msg);
        ShowPacketLog(m_packetC, PacketLabelC());
    }

    // "机器人抢红包"按钮点击事件
    void OnRobotGrab()
    {
        // 机器人仅抢当前正在查看的那个红包
        DoGrabCurrentWithName(NextRobotName());
    }

    // 点击封面图片时触发抢当前红包
    void OnCoverClickGrabCurrent()
    {
        DoGrabCurrentFromInput();
    }

private:
    // --- 成员变量 ---
    CBForm m_form;              // 主窗体对象
    RedPacket m_packetA;        // 红包A对象
    RedPacket m_packetB;        // 红包B对象
    RedPacket m_packetC;        // 红包C对象
    bool m_packetCReady;        // 标记红包C是否已塞钱
    ECurrentPacket m_currentPacket; // 当前选中的红包枚举
    int m_robotIndex;           // 机器人名字的序号计数器

    // --- 辅助工具函数 ---

    // 获取红包标签（如 "红包A"）
    LPCTSTR PacketLabelA() const { return PACKET_LABEL_PREFIX TEXT("A"); }
    LPCTSTR PacketLabelB() const { return PACKET_LABEL_PREFIX TEXT("B"); }
    LPCTSTR PacketLabelC() const { return PACKET_LABEL_PREFIX TEXT("C"); }

    // 生成下一个机器人的名字（如 "机器人1", "机器人2"...）
    string NextRobotName()
    {
        TCHAR robotName[64] = { 0 };
        _stprintf(robotName, TEXT("机器人%d"), m_robotIndex++);
        return ToString(tstring(robotName));
    }

    // 弹出信息提示框
    int ShowInfoBox(LPCTSTR msg)
    {
        return MessageBox(m_form.hWnd(), msg, TCN_TitleInfo(), MB_OK | MB_ICONINFORMATION);
    }

    // 弹出警告提示框
    int ShowWarnBox(LPCTSTR msg)
    {
        return MessageBox(m_form.hWnd(), msg, TCN_TitleWarn(), MB_OK | MB_ICONWARNING);
    }

    // 重置所有UI控件到初始状态（清空文本、显隐控制、ZOrder顺序）
    void ResetUIState()
    {
        m_form.Control(ID_editLog, false).TextSet(TEXT(""));
        m_form.Control(ID_txtResult, false).TextSet(TEXT(""));
        m_form.Control(ID_txtSep, false).VisibleSet(false);

        // 重置红包A区域
        m_form.Control(ID_editAName, false).TextSet(TEXT(""));
        m_form.Control(ID_btnAGrab, false).VisibleSet(true);
        m_form.Control(ID_btnAShow, false).VisibleSet(true);
        m_form.Control(ID_editAName, false).VisibleSet(true);

        // 重置红包B区域
        m_form.Control(ID_editBName, false).TextSet(TEXT(""));
        m_form.Control(ID_editBName, false).VisibleSet(true);
        m_form.Control(ID_btnBGrab, false).VisibleSet(true);
        m_form.Control(ID_btnBShow, false).VisibleSet(true);
        m_form.Control(ID_txtResult, false).VisibleSet(true);

        // 重置红包C区域
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
        m_form.Control(ID_btnCGrab, false).EnabledSet(false); // 默认禁用
        m_form.Control(ID_btnRobotGrab, false).VisibleSet(true);

    }

    // 尝试构建EXE同级目录下的assets图片路径
    bool TryBuildExeRelativeCoverPath(tstring& outPath)
    {
        TCHAR modulePath[MAX_PATH] = { 0 };
        // 获取当前EXE的完整路径
        DWORD n = GetModuleFileName(0, modulePath, MAX_PATH);
        if (n == 0 || n == MAX_PATH) return false;

        // 查找最后一个反斜杠（或正斜杠），截断路径得到目录
        TCHAR* pSlashBack = _tcsrchr(modulePath, TEXT('\\'));
        TCHAR* pSlashFwd = _tcsrchr(modulePath, TEXT('/'));
        TCHAR* pSlash = pSlashBack;
        if (!pSlash || (pSlashFwd && pSlashFwd > pSlash)) pSlash = pSlashFwd;
        if (!pSlash) return false;

        // 拼接路径
        *pSlash = 0;
        outPath = modulePath;
        outPath += TEXT("\\assets\\redpacket_cover.bmp");
        return true;
    }

    // 尝试从指定路径加载图片
    bool TryLoadCoverFromPath(LPCTSTR bmpPath)
    {
        // 检查文件是否存在且不是文件夹
        DWORD attr = GetFileAttributes(bmpPath);
        if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
            return false;
        }
        m_form.Control(ID_picCover, false).PictureSet(bmpPath);
        return true;
    }

    // string 转 tstring (适应Unicode/ANSI编译环境)
    tstring ToTString(const string& s)
    {
#ifdef UNICODE
        int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, 0, 0);
        if (len <= 0) return TEXT("");
        wchar_t* buf = new wchar_t[len];
        MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buf, len);
        tstring out = buf;
        delete[] buf;
        return out;
#else
        return s;
#endif
    }

    // tstring 转 string
    string ToString(const tstring& s)
    {
#ifdef UNICODE
        int len = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, 0, 0, 0, 0);
        if (len <= 0) return string();
        char* buf = new char[len];
        WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, buf, len, 0, 0);
        string out = buf;
        delete[] buf;
        return out;
#else
        return s;
#endif
    }

    // 读取编辑框文本
    tstring ReadText(unsigned short idEdit)
    {
        return m_form.Control(idEdit, false).Text();
    }

    // 读取用户名，如果为空则返回"匿名用户"
    string ReadNameOrDefault(unsigned short idEdit)
    {
        tstring s = ReadText(idEdit);
        if (s.empty()) return ToString(TCN_AnonymousUser());
        return ToString(s);
    }

    // 显示"手气最佳"消息框
    void ShowBestLuckMsg(const RedPacket& packet, LPCTSTR packetLabel)
    {
        string best = packet.bestLuckRecord();
        TCHAR msg[256] = { 0 };
        if (best.empty())
        {
            _stprintf(msg, TEXT("%s手气最佳：暂无。"), packetLabel);
        }
        else
        {
            string who;
            string money;
            if (ParseNameMoney(best, who, money))
            {
                _stprintf(msg, TEXT("%s手气最佳：%s，%s 元。"),
                    packetLabel, ToTString(who).c_str(), ToTString(money).c_str());
            }
            else
            {
                _stprintf(msg, TEXT("%s手气最佳：%s。"),
                    packetLabel, ToTString(best).c_str());
            }
        }
        ShowInfoBox(msg);
    }

    // 向日志编辑框追加一行
    void AppendLog(LPCTSTR s)
    {
        m_form.Control(ID_editLog, false).TextAdd(s);
        m_form.Control(ID_editLog, false).TextAdd(TEXT("\r\n")); // Windows换行符
    }

    // 设置日志编辑框为单行文本
    void SetLogLine(LPCTSTR s)
    {
        m_form.Control(ID_editLog, false).TextSet(TEXT(""));
        AppendLog(s);
    }

    // 更新顶部的结果提示文本
    void UpdateResultText(const string& grabber, double money)
    {
        TCHAR line[256] = { 0 };
        _stprintf(line, TEXT("恭喜！%s 抢到 %.2f 元红包！"), ToTString(grabber).c_str(), money);
        m_form.Control(ID_txtResult, false).TextSet(line);
    }

    // 更新左下角的红包剩余状态
    void UpdateLeftFooter(const RedPacket& packet, LPCTSTR packetLabel)
    {
        TCHAR line[256] = { 0 };
        int grabbed = packet.grabbedCount();
        int total = packet.totalCount();
        int left = total - grabbed;
        if (left < 0) left = 0;
        _stprintf(line, TEXT("%s：已抢 %d 个，剩余 %d 个"), packetLabel, grabbed, left);
        m_form.Control(ID_txtLeftStatic, false).TextSet(line);
    }

    // 解析记录字符串（格式："Name:Money"）
    bool ParseNameMoney(const string& src, string& outName, string& outMoney)
    {
        size_t pos = src.rfind(':');
        if (pos == string::npos) return false;
        outName = src.substr(0, pos);
        outMoney = src.substr(pos + 1);
        return true;
    }

    // 刷新日志编辑框，显示某个红包的所有记录
    void ShowPacketLog(const RedPacket& packet, LPCTSTR packetLabel)
    {
        m_form.Control(ID_editLog, false).TextSet(TEXT(""));
        const string* recs = packet.records();
        int recCount = packet.grabbedCount();
        string best = packet.bestLuckRecord();

        AppendLog(TEXT("抢红包记录："));

        for (int i = 0; i < recCount; ++i)
        {
            string who;
            string money;
            TCHAR line[256] = { 0 };
            if (ParseNameMoney(recs[i], who, money))
            {
                // 如果是最佳手气，加上特殊标记
                if (!best.empty() && recs[i] == best)
                {
                    _stprintf(line, TEXT("%s 抢到了 %s 元，手气最佳！"), ToTString(who).c_str(), ToTString(money).c_str());
                }
                else
                {
                    _stprintf(line, TEXT("%s 抢到了 %s 元"), ToTString(who).c_str(), ToTString(money).c_str());
                }
            }
            else
            {
                _stprintf(line, TEXT("%s"), ToTString(recs[i]).c_str());
            }
            AppendLog(line);
        }

        if (recCount == 0) AppendLog(TEXT("暂无抢包记录"));

        UpdateLeftFooter(packet, packetLabel);
    }

    // 根据当前选中的红包，执行抢红包动作（从输入框读取名字）
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

    // 根据当前选中的红包，执行抢红包动作（指定名字）
    void DoGrabCurrentWithName(const string& who)
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

        // 红包C需要特殊检查是否已塞钱
        if (!m_packetCReady)
        {
            UpdateLeftFooter(m_packetC, PacketLabelC());
            ShowWarnBox(TEXT("红包C还没塞钱，请先点击“塞钱进红包”。"));
            return;
        }
        DoGrabWithName(m_packetC, who, PacketLabelC(), false);
    }

    // 执行抢红包的入口函数（从界面读取名字）
    void DoGrab(RedPacket& packet, unsigned short idNameEdit, LPCTSTR packetLabel, bool checkReady, bool showResultText)
    {
        // 检查红包C是否就绪
        if (checkReady && !m_packetCReady)
        {
            UpdateLeftFooter(packet, packetLabel);
            ShowWarnBox(TEXT("红包C还没塞钱，请先点击“塞钱进红包”。"));
            return;
        }

        // 获取名字
        string who = ReadNameOrDefault(idNameEdit);
        DoGrabWithName(packet, who, packetLabel, showResultText);
    }

    // --- 核心抢红包逻辑 ---
    void DoGrabWithName(RedPacket& packet, const string& who, LPCTSTR packetLabel, bool showResultText)
    {
        int status = RedPacket::GrabEmpty;
        // 调用RedPacket类的核心方法grab
        double got = packet.grab(who, &status);

        // 1. 检查是否重复抢
        if (status == RedPacket::GrabDuplicate)
        {
            TCHAR msg[256] = { 0 };
            _stprintf(msg, TEXT("%s 已经抢过%s，每个用户只能抢一次。"), ToTString(who).c_str(), packetLabel);
            UpdateLeftFooter(packet, packetLabel);
            ShowWarnBox(msg);
            return;
        }

        // 2. 检查是否抢完了
        if (got <= 0.0)
        {
            TCHAR msg[256] = { 0 };
            _stprintf(msg, TEXT("%s已抢完，手慢了！"), packetLabel);
            UpdateLeftFooter(packet, packetLabel);
            ShowWarnBox(msg);
            if (showResultText) m_form.Control(ID_txtResult, false).TextSet(TEXT("手慢了，红包已抢完。"));

            // 抢完时顺便公布手气最佳
            ShowBestLuckMsg(packet, packetLabel);
            return;
        }

        // 3. 抢到了
        TCHAR msg[256] = { 0 };
        _stprintf(msg, TEXT("恭喜！%s 在%s抢到 %.2f 元。"), ToTString(who).c_str(), packetLabel, got);
        ShowInfoBox(msg);

        if (showResultText) UpdateResultText(who, got);

        // 刷新日志列表
        ShowPacketLog(packet, packetLabel);
    }
};

// --- 全局变量与全局事件回调 ---

// 全局应用程序实例（因为C风格的回调函数需要访问它）
RedPacketApp g_app;

// 这一组全局函数是给CBForm的事件机制用的回调
// 它们只是简单地转发给g_app的对应方法
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
    // 1. 绑定事件：将UI事件与全局回调函数关联
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

    // 2. 显示窗体，进入消息循环
    g_app.Form().Show();

    return 0;
}
