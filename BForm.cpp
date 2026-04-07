//////////////////////////////////////////////////////////////////////
// BForm.cpp: һЩȫ�ֺ������� DoEvents ������ �� WinMain �����Ķ��壬
//            CBForm ���ʵ��
//   
//////////////////////////////////////////////////////////////////////

#include <memory.h>
#include "BForm.h"
#include <commctrl.h>
#pragma comment(lib,"comctl32.lib")	// ʹ�������� comctl32.lib ��


//////////////////////////////////////////////////////////////////////////
// ȫ�ֺ��� �� WinMain ����
//////////////////////////////////////////////////////////////////////////

  
// ��װ WinMain �������û��Կɽ� main() ��Ϊ��������
int main();
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, char * lpCmdLine, int nShowCmd )
{
	// ��ʼ��ͨ�ÿؼ���
	InitCommonControls();  // ������⹫�к����У����� SHFileOperation �ƺ�Ҳ�ܴﵽͬ����Ч����ִ��SHFileOperation(0);���ɣ�

	// ��ʼ�� rich edit
	HMODULE hRichDll = LoadLibrary(TEXT("Msftedit.dll"));		// rich edit 5.0  ���޸���Դrc�ļ��Ķ�Ӧ�ؼ�����Ϊ RICHEDIT50W
	//HMODULE hRichDll = LoadLibrary(TEXT("riched20.dll"));		// rich edit 2.0, 3.0 

	// ���� CBApp ���󣬽��йس�����Ϣ�������С��ö�����Ϊ WinMain �ľֲ�����
	//   ����������ȫ�̶��Ǵ��ڵģ���Ϊ WinMain ��������������ͽ�����
	// ʹȫ��ָ����� pApp ָ�� app����ȫ�ֳ����п�ʹ�� (*pApp) ���ʴ˱���
	CBApp app(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
	pApp=&app;
	
	// ���� main ������main �������û����Լ���ģ�����Լ����塣���������
	//   ������� main ����
	int retMain=main();

	// ���뱾�̵߳���Ϣѭ�����������û� main() ����ִ�н��������
	// ע���û� main() ������������������δ����
	MessageLoop(0);		// ���� 0 ��ʾ����Ϣѭ����GetMessage �յ� 0 �Ž���

	// ж�� rich edit
	FreeLibrary(hRichDll); hRichDll=0;

	// WinMain ���أ�����ֵΪ�û� main() �����ķ���ֵ
	return retMain;
}


// ��Ϣѭ������
// bExitStyle=0ʱ��Ϊ����Ϣѭ��������Ϣ�����޴���ʱ�� return
// bExitStyle>1ʱ��ʵ���� +1 ��� ms_iModalLevel ��ֵ
//   ����ģ̬�Ի�����ʾ�����Ϣѭ����ÿ��ʾһ��ģ̬�Ի����½�һ��
//   �µ� MessageLoop���� ms_iModalLevel<iExitStyle�����޴���ʱ return
// bExitStyle=-1ʱ������ DoEvents()��
//   ��ǰ�߳�����Ϣ��PeekMessage ����0��ʱ�� return
// ���� bExitStyle Ϊ���٣��� GetMessage �յ� 0 ���� return 
//   ���� return ǰ�ٴ� PostQuitMessage(0); �Խ��˳���Ϣ����
//   ��ǰ������ MessageLoop ʹǰ������ MessageLoop �����˳�
static void MessageLoop(int iExitStyle/*=0*/)
{
	// ���뱾�̵߳���Ϣѭ��������ñ��߳��е�������Ϣ��Ȼ����Ϣ
	//   �ɷ����������ĸ��ԵĴ��ڹ�����
	// ��ʾģ̬����ʱ���ٴε��ñ������������� iExitStyle Ϊ ms_iModalLevel��>0��
	//   ��ģ̬�������ػ�ж��ʱ���˳��½�����õı������������ϲ���õı����� 
	//   ���һ���� WinMain �����õı�����

	MSG msg;
	int iret=-1;		// ���ܳ�ʼ��Ϊ0���� while ���������жϣ�iret==0 ��ʾ�յ� WM_QUIT ��Ϣ

	while (1) 
	{
		if (iExitStyle>0)		// ����ģ̬���壺ms_iModalLevel<iExitStyle�����޴���ʱ return
		{
			if ( CBForm::ms_iModalLevel<iExitStyle ||  
				CBForm::ms_hashWnd.Count()==0 ) break;
		}
		else if (iExitStyle<0)	// ���� DoEvents����ǰ�߳�����Ϣ�� return
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)==0) break;
		}
		else					// ����Ϣѭ��������Ϣ���޴���ʱ�� return
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)==0  &&  
				CBForm::ms_hashWnd.Count()==0 ) break;
		}
		
		iret=GetMessage(&msg, NULL, 0, 0);	// ��ñ��߳����д��ڵ���Ϣ

		// GetMessage�����᷵�� -1���� 
		// while (GetMessage( lpMsg, hWnd, 0, 0)) ... �ǲ���ȡ��
		// ���� 0 ��ʾ���߳��յ��˳��ź� WM_QUIT ��Ϣ���˳���Ϣѭ��
		if (iret==-1 || iret==0) break;	

		// �������ټ�
		// �� TranslateAccelerator ת���ʹ�����Ӧ��Ϣ������м��ټ������ټ����Ϊ iret��
		// ����ת����������Ҫ�� IsDialogMessage��TranslateMessage��DispatchMessage
		// ����ת��Ϊ���ټ�����Ϣ TranslateAccelerator ������������
		if (CBForm::ms_hAccelCurrUsed && CBForm::ms_hWndActiForm) 
		{
			if (TranslateAccelerator(CBForm::ms_hWndActiForm, CBForm::ms_hAccelCurrUsed, &msg)) continue;
		}
		
		// �����Ի�����Ϣ���簴 Tab ��ת�ؼ�����ȣ�
		// �� IsDialogMessage ת���ʹ�����Ӧ��Ϣ
		// �������Ϣ�ѱ� IsDialogMessage ��������Ҫ�� TranslateMessage��DispatchMessage
		// �ǶԻ������Ϣ IsDialogMessage ���������� TranslateMessage��DispatchMessage
		
		if (msg.message == WM_KEYDOWN && (msg.wParam==13 || msg.wParam==27))
		{
			// ���»س� �� ���� ESC ������ IsDialogMessage ����
			//   ���� ���»س� �� ���� ESC �ᱻ IsDialogMessage ת��Ϊ WM_COMMAND ��Ϣ
			//   ��wParam==1 �� 2����ʹ����������ؼ������������� �س���ESC
			// ��Ϣת�����ɷ�
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (!IsDialogMessage(CBForm::ms_hWndActiForm, &msg))	
			{
				// ��Ϣת�����ɷ�
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}	// end of while (1)

	// ����������յ������˳���Ϣ ���˳��� while ѭ����iret==0��
	//   �ٴ� PostQuitMessage(0); �Խ��˳���Ϣ����
	//   ��ǰ������ MessageLoop ʹǰ������ MessageLoop �����˳�
	if (iret==0) PostQuitMessage(0);
}


void DoEvents()
{
	MessageLoop(-1);
}


extern void End( int nExitCode/*=0*/ )
{
	PostQuitMessage(nExitCode);
}



//////////////////////////////////////////////////////////////////////////
// CBForm ���ʵ��
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// ���� CBForm ��� ��Ԫ����


// ���ô��ڹ��̣����б�����󣨴��壩���ô˺�����Ϊ���ڹ���
// CBForm �����Ԫ����
static BOOL WINAPI CBForm_DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// =======================================================================
	// �������в��� PostQuitMessage
	// ��Ϊ��һ���Ի����� PostQuitMessage ����������Ͷ��ر��ˣ�������ֻ��
	//   ��һ���Ի��򱻹ر�
	// =======================================================================
	switch(uMsg)
	{
	case WM_INITDIALOG:
		// ===================================================================
		// lparam ӦΪһ������ĵ�ַ�����˵�ַ�� hwnd �����Ĺ�ϵ�����ϣ��
		if (lParam)
		{
			// =================================================================
			// ============ ���´��崴����������Ϣ��¼�� ms_hashWnd ============
			// =================================================================

			// �ݴ�������ϣ�����Ѵ��ڼ�Ϊ hwnd ����Ŀ������ɾ�����������µ����ݸ���
			if (CBForm::ms_hashWnd.IsKeyExist((long)hWnd)) 
				CBForm::ms_hashWnd.Remove((long)hWnd,false); 
			
			// ���ϣ�����������Key=hwnd��Data=�����ַ��
			//   ItemLong=���ټ������ItemLong2=��ģ̬�Ի���Ӱ��� Enabled ״̬��
			CBForm::ms_hashWnd.Add(lParam, (long)hWnd, 0, 0);
			
			// ���ö����е� m_hWnd ��ԱΪ ���ھ��
			((CBForm *)lParam)->m_hWnd = hWnd;

			// ���ö����е� ������Ա
			memset( ((CBForm *)lParam)->m_ClassName, 0, sizeof( ((CBForm *)lParam)->m_ClassName));
			((CBForm *)lParam)->m_atom = GetClassLong(hWnd, GCW_ATOM);
			GetClassName(hWnd, ((CBForm *)lParam)->m_ClassName, 
				sizeof( ((CBForm *)lParam)->m_ClassName ) / sizeof(TCHAR)-1 );
			
			// �������Ӵ��ڿؼ����ദ�������� lParam ��Ϊ������ľ�� m_hWnd
			EnumChildWindows( hWnd, EnumChildProcSubClass, (LPARAM)hWnd );

			// ���� Form_Load �¼�
			((CBForm *)lParam)->EventsGenerator(WM_INITDIALOG, wParam, lParam);
		}

		return 1; // ���� True, Windows ���Զ������뽹��ŵ���һ���� WS_TABSTOP �Ŀؼ���

		break;

	default:
		
		// ===================================================================
		// ���ø��Զ���� EventsGenerator�����ߴ�����Щ��Ϣ����Ҫʱ�����¼�
		//   ���������� EventsGenerator �ķ���ֵ
		// ===================================================================

		CBForm *pForm;
		pForm=0;
		if (CBForm::ms_hashWnd.IsKeyExist((long)hWnd))
		{
			pForm=(CBForm *)CBForm::ms_hashWnd.Item((long)hWnd, false);
			if (pForm) 
			{ 
				return pForm->EventsGenerator(uMsg, wParam, lParam);
			}
		}
	}

	return 0;
}


// ���ô��ڹ��̣����б�����󣨴��壩�е��Ӵ��ڿؼ������ô˺�����Ϊ���ڹ��̣��������Ӵ��ڿؼ��������ദ��
// CBForm �����Ԫ����
static int CALLBACK CBForm_ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	CBForm *pForm = 0; 
	long r; 

	// ������Ϣ�͵� ��Ӧ�������.EventsGeneratorCtrl
	if (CBWndBase::ms_hashCtrls.IsKeyExist((long)hWnd))
	{
		// ItemLong = ��λ�ڴ���� hWnd�����Դ�Ϊ key �� ms_hashWnd �л�ô���
		//   CBForm ����ĵ�ַ
		long lHWndForm=CBWndBase::ms_hashCtrls.ItemLong((long)hWnd, false);
		pForm = (CBForm *)CBForm::ms_hashWnd.Item(lHWndForm, false);
		if (pForm) 
		{ 
			r = pForm->EventsGeneratorCtrl(hWnd, uMsg, wParam, lParam);
			if (r != gc_APICEventsGenDefautRet) return r;
		}
	}

	// �� CBForm::ms_hashCtrls �л�ñ����ڵ�Ĭ�ϴ��ڳ���ĵ�ַ��������Ĭ�ϴ��ڳ���
	return CallWindowProc ((WNDPROC)(CBWndBase::ms_hashCtrls.Item((long)hWnd,false)), 
		hWnd, uMsg, wParam, lParam);
}


// ö���Ӵ��ڿؼ������ദ�������Ӵ��ڿؼ��Ļص�������
// lParam Ϊ�ؼ���������ľ��ʱ����0������ʾ���ش���ʱ�����ദ����
// lParam Ϊ 0 ʱ����ʾж�ش���ʱ�Ļָ����ദ��
// CBForm �����Ԫ����
static BOOL CALLBACK EnumChildProcSubClass(HWND hWnd, LPARAM lParam)
{
	if (lParam)
	{
		// ======== �������ʱ��WM_INITDIALOG�����õ� ========
		// ���������ڵ��Ӵ���
		EnumChildWindows(hWnd, EnumChildProcSubClass, lParam);

		// ���������Ӵ��ھ��� WS_CLIPSIBLINGS ���
		// ����ؼ��˴˸���ʱ���󷽿ؼ��ػ�ʱ�����ǰ���ؼ�
		SetWindowLong( hWnd, GWL_STYLE,
			GetWindowLong(hWnd, GWL_STYLE) | WS_CLIPSIBLINGS ) ;

		// ��ͬ���͵Ŀؼ��������
		TCHAR strClassName[128];
		GetClassName(hWnd, strClassName, sizeof(strClassName)/sizeof(TCHAR)-1);
		
		if (_tcscmp(strClassName, TEXT("Static"))==0)
		{
			// ����� Static �ؼ��������� SS_NOTIFY ��񣬷��򲻵�ĳЩ�¼�������Ӧ
			//   �ؼ�Ҳ������Ӧ Mouse_Move��MouseDown ��
			SetWindowLong( hWnd, GWL_STYLE,
				GetWindowLong(hWnd, GWL_STYLE) | SS_NOTIFY ) ;
		}
		else if (_tcscmp(strClassName, TEXT("Button")) == 0)
		{
			long styleBtn = GetWindowLong(hWnd, GWL_STYLE);
			if (styleBtn & BS_GROUPBOX)
			{
				UINT flags = SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE;
				SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0, flags);
			}
		}
		else if (_tcscmp(strClassName, TEXT("ComboBox"))==0)
		{
			// ���� ComboBox ���߶�̫С�������������������
			// ��������ַ��� => strClassName 
			// (HWND)lParam �Ǹ�������
			CBControl ctrl(hWnd);		// �� CBControl �����޸� ComboBox �߶ȣ�lParam Ϊ��������
			int heightReq = SendMessage(hWnd, CB_GETITEMHEIGHT, 0, 0) *10; 
			if ( ctrl.Height() < heightReq ) ctrl.HeightSet(heightReq);
		}
		else if (_tcsstr(strClassName, TEXT("RICHEDIT")))
		{
			// ���� Rich Edit ��������ı����� &H7FFFFFFE����������֪ͨ��Ϣ
			SendMessage(hWnd, EM_SETLIMITTEXT, 0x7FFFFFFE, 0);		// �����������ı�����Ϊ &H7FFFFFFE
			SendMessage(hWnd, EM_SETEVENTMASK, 0, 0xFFFFFFFF);		// ��������֪ͨ��Ϣ��Ч
		}

		// �������Ӵ��ڿؼ����ദ����Control_Proc�����������ǵ�ԭ���ڳ����ַ
		//   ���� CBForm ��ľ�̬��Ա ��ϣ�� ms_hashCtrls
		// �� lParam ����������ľ���������ϣ��Ԫ�ص� ItemLong
		if (! CBWndBase::ms_hashCtrls.IsKeyExist((long)hWnd))
		{
			// ItemLong2 ��Ϊ0����ʾû�����ù��������ԣ�ֻ�����ø�������ʱ
			//   �ٶ�̬���� STWndProp �Ŀռ䣬ItemLong2 ��Ϊ�ռ��ַ����Ϊ0��
			CBWndBase::ms_hashCtrls.Add((long)GetWindowLong(hWnd,GWL_WNDPROC), (long)hWnd,
				(long)lParam, 0, 0, 0, 0, false);
			SetWindowLong(hWnd, GWL_WNDPROC, (long)CBForm_ControlProc);
		}

		
		// �������Ӵ��ڿؼ��� ID ������������ھ������������Ӧ��ϵ
		// �����ϣ�� ms_hashCtrlResIDs
		// Key = �ؼ���ԴID��Item = �ؼ������� hWnd
		// ItemLong = ֱ�����������ڵ� hWnd��ItemLong2 = ��λ�ڴ���� hWnd
		int idCtrl = GetDlgCtrlID(hWnd);
		if (idCtrl  &&  ! CBWndBase::ms_hashCtrlResIDs.IsKeyExist(idCtrl))
			CBWndBase::ms_hashCtrlResIDs.Add((long)hWnd, idCtrl, 
			    (long)GetParent(hWnd), (long)lParam, 0, 0, 0.0, false);
		

		// ���ط�0ֵ���Լ���ö��ͬ�������Ӵ��ڿؼ�
		return 1;
	}
	else  // if (lParam) else�� (lParam == 0)
	{
		// ======== ����ж��ʱ��WM_DESTROY�����õ� ========
		// ���������Ӵ��ڿؼ��������Ϣ
		//   �� CBForm ��ľ�̬��Ա ��ϣ�� CBWndBase::ms_hashCtrls �е������Ŀ

		if ( CBWndBase::ms_hashCtrls.IsKeyExist((long)hWnd) )
		{
			// ɾ���������ԵĿռ䣨������ù��������ԵĻ���
			STWndProp * pPro = CBWndBase::PropertyMemCtrl(hWnd, false); 
			if (pPro)
			{
				// �ͷŸ��������е������Դ
				if (pPro->hBrushBack) 
				{ DeleteObject(pPro->hBrushBack); pPro->hBrushBack = NULL; }
				
				// ��ԭ�ȹ�� ������Դ��꣬ɾ��ԭ�ȵĹ��
				// ����ɾ��������ϵͳ���Զ�ɾ��
				// pPro->hCursor
				// if (pPro->cursorIdx > 0 && pPro->cursorIdx < gc_IDStandCursorIDBase) 
				//	DestroyCursor(pPro->hCursor);
				pPro->hCursor = NULL; pPro->hCursor2 = NULL;
				pPro->cursorIdx = 0;  pPro->cursorIdx2 = 0;

				
				if (pPro->hFont) 
				{ DeleteObject(pPro->hFont); pPro->hFont = NULL; }
				
				if (pPro->hBmpDisp)
				{  DeleteObject(pPro->hBmpDisp); pPro->hBmpDisp = NULL; }

				// �� rcPicture �ĸ���Ա��Ϊ -1 ��ʾ����ȡͼƬ��һ������ʾ������Ҫ��ʾͼƬ��ȫ��
				SetRect(&pPro->rcPictureClip, -1, -1, -1, -1);

				if (pPro->tagString)	// pPro->tagString δ�� HM ����
				{ delete [](pPro->tagString); pPro->tagString = NULL; 	}

				// ɾ���������ԵĿռ�
				delete pPro;
			}	// end if (pPro)

			// �ָ����ദ����ԭ���ڳ����ַλ�� ms_hashCtrls.Item �У�
			SetWindowLong(hWnd, GWL_WNDPROC, 
				CBWndBase::ms_hashCtrls.Item((long)hWnd, false));
			
			// ɾ�� ms_hashCtrls �� ms_hashCtrlResIDs �еĶ�Ӧ��Ŀ
			CBWndBase::ms_hashCtrls.Remove((long)hWnd, false);
			if (CBWndBase::ms_hashCtrlResIDs.Item((long)GetDlgCtrlID(hWnd),false) == (long)hWnd)
				CBWndBase::ms_hashCtrlResIDs.Remove((long)GetDlgCtrlID(hWnd), false);
		}	// end if ( CBWndBase::ms_hashCtrls.IsKeyExist((long)hWnd) )

		
		// ���������ڵ��Ӵ���
		EnumChildWindows(hWnd, EnumChildProcSubClass, lParam);
		
		// ���ط�0ֵ���Լ���ö��ͬ�������Ӵ��ڿؼ�
		return 1;
	}   // end if (lParam)-else  // (lParam == 0)
}


//////////////////////////////////////////////////////////////////////////
// �������е� static ��Ա�� static ����

CBHashLK CBForm::ms_hashWnd;


// ģ̬�Ի��� �Ĳ��
int CBForm::ms_iModalLevel = 0;


// ���ټ�����ͼ��ټ�Ҫ���͵���Ŀ�괰��
HACCEL CBForm::ms_hAccelCurrUsed=NULL;  
HWND CBForm::ms_hWndActiForm=NULL;


int CBForm::FormsCount()
{
	return ms_hashWnd.Count();
}

CBForm * CBForm::FormsObj( int index )
{
	return (CBForm *)ms_hashWnd.ItemFromIndex(index, false);
}



//////////////////////////////////////////////////////////////////////////
// ���������

// ���캯��
CBForm::CBForm( unsigned short int idResDialog /*=0*/):CBWndBase(NULL, &m_WndProp)
{
	mResDlgID = idResDialog;

	// ��ǰû��ϵͳ����
	m_NIData.cbSize = 0;

	// ��ʼ����������
	m_WndProp.cursorIdx = 0;		// ���嵱ǰ������������ţ�0 ��ʾʹ��ϵͳĬ�ϣ�
	m_WndProp.cursorIdx2 = 0;		// .cursorIdx �Ǹ���
	m_WndProp.hCursor = NULL;		// ���嵱ǰ������������� cursorIdx �� 0 ʱ����Ч��
	m_WndProp.hCursor2 = NULL;		// .hCursor �ĸ���

	m_WndProp.backColor = -1;		// ���ڱ���ɫ��-1 ��ʾʹ��Ĭ��ɫ�����޸Ĵ����࣬����ͨ����Ӧ WM_ERASEBKGND ʵ�֣�
	m_WndProp.hBrushBack = NULL;	// ���ڱ������Ļ�ˢ

	m_WndProp.foreColor = -1;		// �����ؼ� ǰ��ɫ��-1 ��ʾʹ��Ĭ��ɫ
	m_WndProp.bBackStyleTransparent = false;		// �ؼ������Ƿ�͸��������Static��ĳЩ�ؼ���Ч��
	m_WndProp.hFont = NULL;			// �������壨Ϊ0��ʾ��ϵͳ���壻�����ʾ���Զ������壬����� DeleteObject ��
	SetRect(&m_WndProp.rcTextOutClip, 0, 0, 0, 0);	// PrintText ����ı�ʱ�����λ�ã�.left��.top���������Χ
	m_WndProp.iTextOutStartLeft = 0;// ����ı��ķ�Χ��ԭʼ��߽磬�� PrintText ����ʱȷ��������߽�
	m_WndProp.iTextOutFlags = 0;	// ����ı���ѡ��

	m_WndProp.stretch = false;		// ���屻����ͼƬʱʹ�ã��Ƿ��Զ�����ͼƬ��С����Ӧ�ؼ�
	m_WndProp.hBmpDisp=NULL;		// ������Ϊ����ʹ�ã�Ҫ��ʾ��λͼͼƬ��stretch=true ʱ����Ϊԭʼδ���ŵ�λͼ���
	SetRect(&m_WndProp.rcPictureClip, -1, -1, -1, -1);	// ��Ϊ-1 ��ʾ����ȡͼƬ��һ������ʾ
	m_WndProp.iTitleBarBehav = 0;	// �Ƿ���б�������Ϊ��=0�����У�=1 ����϶����ƶ����壩
	
	m_WndProp.fDisableContextMenu = false;	// �Ƿ�����Ҽ��˵������Ƿ�ȡ�� WM_CONTEXTMENU ��Ϣ��

	m_WndProp.tagString = NULL;	// �����ַ�������
	m_WndProp.iTag1 = 0;		// ������������1
	m_WndProp.iTag2 = 0;		// ������������2
	m_WndProp.dTag = 0.0;		// ���� double ������	



	m_ModalShown = false;	// �Ƿ�����ģ̬�Ի�����ʾ�ı�־

	m_EdgeWidth=0;			// >0 ʱ��ʾҪ�� WM_PAINT �¼����Զ����ƴ���߿򣬴�Ϊ�߿���ȣ�֧��Բ�Ǿ��α߿�
	m_EdgeColor=0;			// m_EdgeWidth==0ʱ��m_EdgeColor��Ч��m_EdgeWidth >0 ʱ��ʾҪ�� WM_PAINT �¼����Զ����ƴ���߿����ɫ��֧��Բ�Ǿ��α߿�

	m_iBorderAutoResizable=0;	// �����ޱ߿��壩�Ƿ����������϶��������ܸı䴰���С��������>0��ֵΪ�ɸı��С�ı߿�Χ��

	m_RoundRect = 0;		// �����Բ�Ǿ��δ���
	m_hRgnRoundRect = NULL;	

	// �๫�г�Ա����ֵ
	KeyPreview = false;		// �Ƿ������ɴ��崦�����пؼ��ļ��̰�����Ϣ
}

// ��������
CBForm::~CBForm()
{
	UnLoad();
	ClearResource();
}


//////////////////////////////////////////////////////////////////////////
// ���г�Ա����

HWND CBForm::hWnd()
{
	if (m_hWnd==NULL) Load(); // ������ڻ�δ���أ����ڼ���
	return m_hWnd;
}


HMENU CBForm::hMenu()
{
	if (m_hWnd==NULL) Load(); // ������ڻ�δ���أ����ڼ���
	return GetMenu(m_hWnd);
}


void CBForm::hMenuSet( HMENU hMenuNew )
{
	if (m_hWnd==NULL) Load(); // ������ڻ�δ���أ����ڼ���

	SetMenu(m_hWnd, hMenuNew); 
	if (hMenuNew)
	{	
		ConstructMenuIDItems(GetMenu(m_hWnd), true);
		ConstructMenuIDItems(GetSystemMenu(m_hWnd, 0), false);	// ϵͳ�˵�
	}
	else
	{
		ConstructMenuIDItems(NULL, true);
		ConstructMenuIDItems(GetSystemMenu(m_hWnd, 0), false);	// ϵͳ�˵�
	}
}


HACCEL CBForm::hAccel()
{
	if (m_hWnd==NULL) Load(); // ������ڻ�δ���أ����ڼ���
	return (HACCEL)ms_hashWnd.ItemLong((long)m_hWnd, false);
}


void CBForm::hAccelSet( HACCEL hAccelNew )
{
	if (m_hWnd==NULL) Load(); // ������ڻ�δ���أ����ڼ��أ�Ȼ��������ü��ټ�
	
	ms_hashWnd.ItemLongSet((long)m_hWnd, (long)hAccelNew, false);	// ��¼�� ms_hashWnd �� ItemLong
	// �������ǰ̨�������Ǳ�����Ĵ��壬������ CBForm::hWndAccel��CBForm::hAccel
	if (m_hWnd==GetActiveWindow())
	{
		CBForm::ms_hWndActiForm = m_hWnd; 
		CBForm::ms_hAccelCurrUsed = hAccelNew; 
	}
}


bool CBForm::Show( int modal/*=0*/, HWND hwndOwner/*=NULL*/ )
{
	// ����ģ̬��ʽ��ʾ���壬������д��Ϣѭ�������²��ܴ������ټ�
	//   �ڱ����У����д����Է�ģ̬��ʽ��ʾ
	// ���ڡ�ģ̬����ʵ�֣��Ǳ�����ͨ�������ж��㴰��ȫ�� 
	//   Enabled=False ʵ�ֵ�

	HWND hwnd;
	HWND hWndActiLast = GetActiveWindow(); 

	if (0==m_hWnd) 
	{	
		// ����δ���ضԻ������ڼ���
		// ���д����Է�ģ̬��ʽ CreateDialogParam ����
		hwnd=Load(hwndOwner);   
		if (hwnd==0) return false;
	}
	else 
	{	
		// �Ի����Ѿ����أ�����ʾʱ���ܻ�ı��� Owner
		// ͨ���������ı䴰��� Owner ������Parent��
		//   MSDN ��û�н��ܸı� Owner �ķ�����Ӧ������  
		//   SetWindowLong �� -8 ���� Owner
		if (hwndOwner !=0 && hwndOwner != GetWindow(m_hWnd, GW_OWNER)) 
			SetWindowLong(m_hWnd, -8, (long)hwndOwner); 
	}

	if (modal)
	{
		// ======== ��ʾģ̬�Ի��� ========

		// ����ʾһ��ģ̬�Ի���ʱ��
		//   ������ĳ����ĿǰΪ Disabled �� ms_hashWnd.ItemLong2 ֵΪ0����ά�ָô��ڵĴ�ֵ���䣻
		//   ��������ĳ����ĿǰΪ Enabled�����߸ô��ڴ�ֵ >0�����Ὣ�ô��ڵĴ�ֵ +1
		// ������һ��ģ̬�Ի���ʱ��
		//   ��ĳ���ڴ�ֵΪ0�������κβ��������򽫴�ֵ-1����-1��Ϊ0����ָ�Ϊ Enabled ״̬

		int i; 
		BOOL ena; 
		long level=0; 
		HWND hwndEach=0;
		for (i=1; i<=ms_hashWnd.Count(); i++)
		{
			hwndEach=(HWND)ms_hashWnd.IndexToKey(i);
			if (hwndEach==m_hWnd) 
			{
				// ��ģ̬���壬������������ ItemLong2 Ϊ 0
				ms_hashWnd.ItemLong2FromIndexSet(i, (long)0, false);
				continue;	
			}

			
			// ���ݴ��� i �ĵ�ǰ enabled ״̬�� ItemLong2��level�� �����ж�
			ena = IsWindowEnabled(hwndEach);
			level = ms_hashWnd.ItemLong2FromIndex(i, false); 
			if (ena || level>0)	
			{	
				// ��¼����Ҫ�ѱ����ڻָ���Enabled
				ms_hashWnd.ItemLong2FromIndexSet(i, level+1, false);	
			}
			// else ������ԭΪ Disabled����  ItemLong2��level�� Ϊ0
			//   ����ά�ִ�ֵΪ0�����䣩

			// ������ i ����Ϊ Diabled
			EnableWindow(hwndEach, 0);
		}


		// ���ñ��������ʾģ̬��־
		m_ModalShown = true;
	
		// ģ̬��� ++
		if (ms_iModalLevel<0) ms_iModalLevel=0;		// �ݴ�
		ms_iModalLevel++;

		// ��ʾ��ģ̬������
		ShowWindow(m_hWnd, SW_NORMAL);
		
		// ��������Ҫ��������
		// �ٴε��� MessageLoop�������ò���Ϊ 1
		// ֱ����ģ̬�����屻���ػ�رպ������ MessageLoop �����ŷ���
		MessageLoop(ms_iModalLevel); 
	}
	else
	{
		// ======== ��ʾ��ģ̬�Ի��� ========
		
		if ( ms_iModalLevel>0 )	
		{
			// �������ģ̬�Ի���������ʾ��ԭ���ϲ���������ʾ
			//   ��ģ̬�Ի��򡣵�����������ֻҪ���¶Ի�����Ϊ Disabled ����
			// �¶Ի���� ItemLong2 ֵӦΪ ms_iModalLevel
			ms_hashWnd.ItemLong2Set((long)m_hWnd, ms_iModalLevel, false);
			EnableWindow(m_hWnd, 0);

			// ��ʾ�´��ڣ�����Ҫ���´�����Ϊǰ̨��
			ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);	
			SetActiveWindow(hWndActiLast);	// �ָ�ԭ��ǰ̨������Ϊǰ̨
		}
		else
		{
			ShowWindow(m_hWnd, SW_NORMAL);	
		}
		
		// �Է�ģ̬��ʾ�����ñ�־
		m_ModalShown = false;
	}

	return true;
}


long CBForm::Hide()
{
	if (m_hWnd==NULL) Load(); // ������ڻ�δ���أ����ڼ���
	return ShowWindow(m_hWnd, SW_HIDE);
}


// ���ضԻ��򵫲�����ʾ������ֻ���Է�ģ̬��ʽ���أ�
// ����Ҫ�Ի��򱻼��غ���ܵ��õ�һЩ������ SetAccelerator �У����Զ����ñ�����
HWND CBForm::Load( HWND hwndOwner/*=NULL*/, bool fRaiseErrIfFail/*=true*/ )
{
	// һ���ԡ���ģ̬����ʽ���ش��壺���� CreateDialogParam
	// �� dwInitParam ������Ϊ�������ַ��this��
	//   �˽���Ϊ WM_INITDIALOG �� lParam �������� CBForm_DlgProc
	HWND hwnd;
	hwnd=CreateDialogParam(pApp->hInstance, MAKEINTRESOURCE(mResDlgID), hwndOwner, 
	  CBForm_DlgProc, (long)this);	// �������أ����ش��ھ�����ڴ��ڹ��̴��� WM_INITDIALOG ʱ������ m_hWnd

	// ���û�� WS_VISIBLE ��ʽ��ʱ���岻���Զ���ʾ
	
	if (!hwnd)
	{
		if (fRaiseErrIfFail) 
		{
			MsgBox(  StrAppend( TEXT("CBForm::Load() failed. Form ID="), Str((int)mResDlgID), 
				TEXT("\r\n"), TEXT("GetLastError="), Str(GetLastError())), 
				TEXT("Debug Error from CBForm::Load()"), mb_OK, mb_IconError);
			throw (unsigned char)5;	// ��Ч�Ĺ��̵��û����
		}
	}
	else
	{
		// ��ʼ���˵�
		ConstructMenuIDItems(GetMenu(hwnd),true);	// ���޲˵�������Ϊ0��������˵���ϣ����¼
		ConstructMenuIDItems(GetSystemMenu(hwnd, 0), false);	// ϵͳ�˵�
	}

	return hwnd;
}



// �����Ի���
// ���յ� WM_CLOSE ��Ϣʱ���Զ����ñ�������Ҳ��ֱ�����û�����
void CBForm::UnLoad()
{
	if (m_NIData.cbSize) SysTrayRemove();	// ����ϵͳ���̣�����ɾ��
	SendMessage(m_hWnd, WM_CLOSE, 0, 0);
}


// ���úͷ�����С��(=1)�����(=2)����ԭ״̬(=0)
int CBForm::WindowState()
{
	WINDOWPLACEMENT wp;
	wp.length = sizeof(wp);
	GetWindowPlacement(m_hWnd, &wp);
	
	switch(wp.showCmd)
	{
	case SW_SHOWNORMAL: case SW_RESTORE: case SW_SHOW: 
	case SW_SHOWMINNOACTIVE: case SW_SHOWNA: case SW_SHOWNOACTIVATE:
		return 0;
		break;
	case SW_SHOWMINIMIZED: case SW_MINIMIZE:
		return 1;
		break;
	case SW_SHOWMAXIMIZED:
		return 2;
		break;
	case SW_HIDE:
	default:
		return -1;
		break;
	}
}

void CBForm::WindowStateSet( int iState )
{
	WINDOWPLACEMENT wp;
	wp.length = sizeof(wp);
	GetWindowPlacement(m_hWnd, &wp);

	switch(iState)
	{
	case 0:
		wp.showCmd = SW_SHOWNORMAL;
		break;
	case 1:
		wp.showCmd = SW_SHOWMINIMIZED;
		break;
	case 2:
		wp.showCmd = SW_SHOWMAXIMIZED;
		break;
	}
	
	SetWindowPlacement(m_hWnd, &wp);
}



// ���ñ����ڽ��������ټ�����Դ ID Ϊ idResAcce
// ��ȡ�����ټ�����������Ϊ 0 ����
void CBForm::SetAccelerator( unsigned short int idResAcce )
{
	HACCEL hAcc=NULL;
	if (idResAcce) hAcc=LoadAccelerators(pApp->hInstance, MAKEINTRESOURCE(idResAcce)); 
	hAccelSet(hAcc);	// ���ù��з��� hAccelSet ����
}

// �������ñ����ڵĲ˵�
void CBForm::SetMenuMain( unsigned short idResMenu )
{
	HMENU hMenu=NULL;
	if (idResMenu)
	{
		hMenu=LoadMenu(pApp->hInstance, MAKEINTRESOURCE(idResMenu));
		hMenuSet(hMenu);	// ���ù��з��� hMenuSet ����
	}
	else
		hMenuSet(NULL);		// ���ù��з��� hMenuSet ����
}


// ����һ���ؼ���CBControl ���󣩣�֮��ɶԸÿؼ����в��������� CBControl ������Է�����
CBControl CBForm::Control( unsigned short int idResControl, bool fRaiseErrIfFail/*=true*/ )
{
	if (m_hWnd==NULL) Load(); // ������ڻ�δ���أ����ڼ���

	if (m_Control.SetResID(idResControl))
	{	
		return m_Control;
	}
	else
	{
		if (fRaiseErrIfFail) 
		{
			MsgBox(  StrAppend( TEXT("CBForm::Control() Failed. \r\nControl not found. ID="), Str((int)idResControl)), 
				TEXT("Debug Error from CBForm::Control()"), mb_OK, mb_IconError);
			throw (unsigned char)5;	// ��Ч�Ĺ��̵��û����
		}
		m_Control.SetResID( (unsigned short int)0 );
		return m_Control;
	}
}


// ����һ���˵��CBMenuItem ���󣩣�֮��ɶԸò˵�����в��������� CBMenuItem ������Է�����
CBMenuItem CBForm::Menu( UINT idResMenuItem )
{
	if (m_hWnd==NULL) Load(); // ������ڻ�δ���أ����ڼ���

	// ���� idMenuItem���ڹ�ϣ���в���������Ӧ�ĸ��˵��� ��� -> hMenu
	HMENU hMenu;
	hMenu = (HMENU)m_hashMenuIDs.Item(idResMenuItem, false);

	if (hMenu)
	{
		m_MenuItem.SetFromResID(hMenu, idResMenuItem, m_hWnd, &m_hashMenuIDs); 
		return m_MenuItem;
	}
	else
		return 0;
}

CBMenuItem CBForm::Menu( int pos1, int pos2, int pos3/*=0*/, int pos4/*=0*/, int pos5/*=0*/, int pos6/*=0*/, int pos7/*=0*/ )
{
	if (m_hWnd==NULL) Load(); // ������ڻ�δ���أ����ڼ���

	HMENU hMenu;

	if (pos1>0)
	{
		// ������˵�
		hMenu = GetMenu(m_hWnd);

		if (pos2)
		{
			hMenu = GetSubMenu(hMenu, pos1 - 1);		//-1��Ϊ��0��ʼ��ţ���ö���˵��ľ����������pos1==1��hMenuSubΪ�������ļ����˵��ľ��
		}
		else
		{
			m_MenuItem.SetFromPos(hMenu, pos1, m_hWnd, &m_hashMenuIDs);	// ���ض����˵���һ���˵��λ�ò�����1��ʼ����������pos1==1�����ء��ļ����˵���
			return m_MenuItem;
		}
	}
	else if (pos1<0)
	{
		// ���ϵͳ�˵�
		hMenu = GetSystemMenu(m_hWnd, 0);
		if (pos2==0)
		{
			// ����ϵͳ�˵��ġ����˵��
			//  �����������Ӳ˵����Ϊ GetSystemMenu���������Ӳ˵��ĵ�һ���ǡ���ԭ����
			m_MenuItem.SetFromPos(0, 1, m_hWnd, &m_hashMenuIDs);		// 1-1 == 0
			return m_MenuItem; 
		}
	}
	else	// pos1 == 0
	{
		// ��ö����˵�
		hMenu = GetMenu(m_hWnd);
		if (pos2==0)
		{
			// ���ض���˵��ġ����˵��
			//  �����������Ӳ˵����Ϊ GetMenu���������Ӳ˵��ĵ�һ���ǡ��ļ�����
			m_MenuItem.SetFromPos(0, 2, m_hWnd, &m_hashMenuIDs);
			return m_MenuItem; 
		}
	}
	

	if (pos3)
	{
		hMenu = GetSubMenu(hMenu, pos2 - 1);		//-1��Ϊ��0��ʼ���
	}
	else
	{
		m_MenuItem.SetFromPos(hMenu, pos2, m_hWnd, &m_hashMenuIDs);	// ����һ���˵���һ���˵��λ�ò�����1��ʼ����������pos1==1��pos2==2�����ء��ļ���-���򿪡��Ĳ˵���
		return m_MenuItem;	
	}

	if (pos4)
	{
		hMenu = GetSubMenu(hMenu, pos3 - 1);		//-1��Ϊ��0��ʼ���
	}
	else
	{
		m_MenuItem.SetFromPos(hMenu, pos3, m_hWnd, &m_hashMenuIDs);	// λ�ò�����1��ʼ
		return m_MenuItem;	
	}

	if (pos5)
	{
		hMenu = GetSubMenu(hMenu, pos4 - 1);		//-1��Ϊ��0��ʼ���
	}
	else
	{
		m_MenuItem.SetFromPos(hMenu, pos4, m_hWnd, &m_hashMenuIDs);	// λ�ò�����1��ʼ
		return m_MenuItem;	
	}

	if (pos6)
	{
		hMenu = GetSubMenu(hMenu, pos5 - 1);		//-1��Ϊ��0��ʼ���
	}
	else
	{
		m_MenuItem.SetFromPos(hMenu, pos5, m_hWnd, &m_hashMenuIDs);	// λ�ò�����1��ʼ
		return m_MenuItem;	
	}

	if (pos7)
	{
		hMenu = GetSubMenu(hMenu, pos6 - 1);		//-1��Ϊ��0��ʼ���
	}
	else
	{
		m_MenuItem.SetFromPos(hMenu, pos6, m_hWnd, &m_hashMenuIDs);	// λ�ò�����1��ʼ
		return m_MenuItem;	
	}

	m_MenuItem.SetFromPos(hMenu, pos7, m_hWnd, &m_hashMenuIDs);		// λ�ò�����1��ʼ
	return m_MenuItem;	

}


CBMenuItem CBForm::Menu( ESysMenu idSysMenu )
{
	if (m_hWnd==NULL) Load(); // ������ڻ�δ���أ����ڼ���
	
	return this->Menu((UINT)idSysMenu);
}

void CBForm::MenuSysRestore() const
{
	GetSystemMenu(m_hWnd, 1);
}


BOOL CBForm::PopupMenu( UINT idResMenu, int x, int y, bool bAllowRightClick/*=true*/ )
{
	HMENU hMenuPop;
	BOOL ret;
	UINT flags=0;
	POINT pt; 
	hMenuPop = LoadMenu(pApp->hInstance, MAKEINTRESOURCE(idResMenu)); 
	if (bAllowRightClick) flags = flags | TPM_RIGHTBUTTON;
	pt.x=x; pt.y=y; 
	ClientToScreen(m_hWnd, &pt);
	ret = TrackPopupMenu(GetSubMenu(hMenuPop, 0), flags, pt.x, pt.y, 0, m_hWnd, NULL);
		// TrackPopupMenu ֻ�ܵ��� popup ʽ�˵���LoadMenu �õ��Ĳ��� popup ʽ�˵���
		//   Ӧ�� GetSubMenu ȡ���Ӳ˵�������ֻȡ��0����Ӳ˵���
	DestroyMenu(hMenuPop);
	return ret;
}


void CBForm::IconSet(EStandardIcon icon)
{
	if (m_hWnd==NULL) Load();					// ������ڻ�δ���أ����ڼ���
	HICON hIco = LoadIcon(0, (LPCTSTR)icon);	// ��õ���shared icon����Ҫ DestroyIcon
	SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIco);
	SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIco);
}

void CBForm::IconSet(unsigned short iconRes)
{
	if (m_hWnd==NULL) Load();		// ������ڻ�δ���أ����ڼ���
	HICON hIco = LoadIcon(pApp->hInstance, (LPCTSTR)((DWORD)iconRes));	// ��õ���shared icon����Ҫ DestroyIcon
	SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIco);
	SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIco);
}

#ifndef WS_EX_NOACTIVATE
	#define WS_EX_NOACTIVATE        0x08000000L
#endif
bool CBForm::NoActivated()
{
	return ((StyleEx() & WS_EX_NOACTIVATE) != 0);
}

void CBForm::NoActivatedSet( bool newValue )
{
	StyleExSet(WS_EX_NOACTIVATE, newValue?1:-1);
}







// =================== �¼��������� ===================



// ����һ������Ļ��Ӵ��ڿؼ����¼���������ذ汾��
// ptrFunHandler Ϊһ���¼����������ĵ�ַ
// ����Ǵ����¼���idResControl ����Ӧ��Ϊ0
// ����ǿؼ��¼���idResControl ����Ӧ��Ϊ �ؼ�����ԴID
// ����ǲ˵�������ϵͳ�˵��������ټ��¼�����2������Ϊ eMenu_Click�������� idResControl ����
void CBForm::EventAdd( unsigned short int idResControl, 
					  ECBFormCtrlEventsVoid eventType, 
					  ONEventVoid ptrFunHandler )
{
	EventAddPrivate(idResControl, (long)eventType, (long)ptrFunHandler, 0);
}

void CBForm::EventAdd( unsigned short int idResControl, 
					   ECBFormCtrlEventsI eventType, 
					   ONEventI ptrFunHandler )
{
	EventAddPrivate(idResControl, (long)eventType, (long)ptrFunHandler, 1);
}

void CBForm::EventAdd( unsigned short int idResControl, 
					  ECBFormCtrlEventsII eventType, 
					  ONEventII ptrFunHandler )
{
	EventAddPrivate(idResControl, (long)eventType, (long)ptrFunHandler, 2);
}

void CBForm::EventAdd( unsigned short int idResControl, 
					  ECBFormCtrlEventsIII eventType, 
					  ONEventIII ptrFunHandler )
{

	if (eventType == eMenu_Click)
	{
		// ============ �˵�������ϵͳ�˵�������ټ����¼� ============
		// ���� idResControl
		// ���¼�����������ַ���� m_hashEventNotify.Item(c_CBMenuClickEventKey)�������� 3 �� int �͵Ĳ���
		if (m_hashEventNotify.IsKeyExist(c_CBMenuClickEventKey)) 
			m_hashEventNotify.Remove(c_CBMenuClickEventKey);
		m_hashEventNotify.Add( (long)ptrFunHandler, c_CBMenuClickEventKey, 3, 0, 0, 0, 0, false); 
	}
	else
	{
		// ============ �����ؼ����¼� ============
		EventAddPrivate(idResControl, (long)eventType, (long)ptrFunHandler, 3);
	}

}

void CBForm::EventAdd( unsigned short int idResControl, 
					  ECBFormCtrlEventsIIII eventType, 
					  ONEventIIII ptrFunHandler )
{
	EventAddPrivate(idResControl, (long)eventType, (long)ptrFunHandler, 4);
}

// ����һ������Ļ��Ӵ��ڿؼ����¼���������ذ汾�Ĺ��ú�����
void CBForm::EventAddPrivate(unsigned short int idResControl, long eventType, long ptrFunHandler, int nParamsIntCount )
{
	//////////////////////////////////////////////////////////////////////////
	if (eventType & c_CBNotifyEventBase )
	{   
		// ============ ��֪ͨ��Ϣ�����Ŀؼ��¼� ============
		// ���¼�������ַ���� m_hashEventNotify
		// key �ĸ� 2 λΪ WM_COMMAND ��Ϣ�� WM_NOTIFY ��Ϣ��֪ͨ��
		//  ��֪ͨ����� eventType ��ö��ֵ������2λΪ�ؼ�ID
		long key = (long)MAKELONG(idResControl, eventType); 
		if (m_hashEventNotify.IsKeyExist(key))
			m_hashEventNotify.Remove(key);
		m_hashEventNotify.Add(ptrFunHandler, key, nParamsIntCount,0,0,0,0,false);
		
		// ������Ҫ�ķ�񣬷��������Ǵ˷��������Ӧ���¼�
		if (m_hWnd)
		{ 
			// Ϊ�Ӵ��ڿؼ����� BS_NOTIFY��SS_NOTIFY �� LBS_NOTIFY ���
			HWND hwndCtrl = GetDlgItem(m_hWnd, (int)idResControl);
			long style=GetWindowLong(hwndCtrl, GWL_STYLE);
			CBControl ctrl;
			ctrl.SetResID(idResControl);
			if ( ctrl.IsClassName(TEXT("Button")) )
				SetWindowLong(hwndCtrl, GWL_STYLE, style | BS_NOTIFY);
			else if ( ctrl.IsClassName(TEXT("ListBox")) )
				SetWindowLong(hwndCtrl, GWL_STYLE, style | LBS_NOTIFY);
			// Static �ؼ��� SS_NOTIFY ���� EnumChildProcSubClass �����
		}

	}
	//////////////////////////////////////////////////////////////////////////
	else	// if (eventType & c_CBNotifyEventBase ) else
	{
		// ============ �Դ�����Ϣ�������¼� ============
		//  ����֪ͨ��Ϣ�� WM_XXX��������������Ӵ��ڿؼ����� 
		//   WM_XXX �����ģ����������¼��Ϳؼ��¼���
		// ���¼�������ַ���� m_hashEventMessage
		// key �ĵ� 2 λΪ��Ϣ WM_XXX��
		//       �� 2 λΪ 0 ���Դ����¼����� �ؼ���ԴID���Կؼ��¼���
		long key = (long)MAKELONG(eventType, idResControl);
		
		// key=WM_COMMAND��Ϣ��wParam����λΪ֪ͨ�룬��λΪ�ؼ�ID����ֵ=�û�������ַ
		if ( m_hashEventMessage.IsKeyExist(key) )
			m_hashEventMessage.Remove(key, false);
		m_hashEventMessage.Add(ptrFunHandler, key, nParamsIntCount,0,0,0,0,false);

		// ������Ҫ�ķ�񣬷��������Ǵ˷��������Ӧ���¼�
		if (m_hWnd)
		{ 
			// Static �ؼ��� SS_NOTIFY ���� EnumChildProcSubClass �����

			// WM_DROPFILES ��Ϣ
			if (eventType == eFilesDrop)	
			{
				if (idResControl==0)
				{
					// Ϊ�������� ���� WM_DROPFILES ��Ϣ
					DragAcceptFiles(m_hWnd, 1);
				}
				else
				{ 
					// Ϊ�Ӵ��ڿؼ����� ���� WM_DROPFILES ��Ϣ
					HWND hwndCtrl = GetDlgItem(m_hWnd, (int)idResControl);
					DragAcceptFiles(hwndCtrl, 1);
				}
			}
		}	// end if (m_hWnd)
	}	// end if (eventType & c_CBNotifyEventBase ) - else
	//////////////////////////////////////////////////////////////////////////
}


int CBForm::IDRaisingEvent()
{
	return m_idRaisingEvent;
}




void CBForm::RestoreModalWinEnabled()
{
	// ����ʾһ��ģ̬�Ի���ʱ��
	//   ������ĳ����ĿǰΪ Disabled �Ҵ�ֵΪ0����ά�ָô��ڵĴ�ֵ���䣻
	//   ��������ĳ����ĿǰΪ Enabled�����߸ô��ڴ�ֵ >0�����Ὣ�ô��ڵĴ�ֵ +1
	// ������һ��ģ̬�Ի���ʱ��
	//   ��ĳ���ڴ�ֵΪ0�������κβ��������򽫴�ֵ-1����-1��Ϊ0����ָ�Ϊ Enabled ״̬

	int i;  long level=0; 
	for (i=1; i<=ms_hashWnd.Count(); i++)
	{
		level = ms_hashWnd.ItemLong2FromIndex(i, false);
		if (level>0)
		{
			level--;
	
			if (level<=0)
			{
				// �ָ����� Enabled
				EnableWindow((HWND)ms_hashWnd.IndexToKey(i), 1);
				level = 0;		// �ݴ�����ʱ level ��ӦΪ0��
			}
			
			// ����μ�¼�� ItemLong2
			ms_hashWnd.ItemLong2FromIndexSet(i, level, false);
		}
		// else level==0 �����κβ���
	}

	// �ָ� Owner ����Ϊǰ̨
	SetActiveWindow (GetWindow(m_hWnd, GW_OWNER));

	// �����־����
	m_ModalShown=NULL;

	// ģ̬��μ�1
	ms_iModalLevel--; 
	if (ms_iModalLevel<0) ms_iModalLevel=0;		// �ݴ�
}


void CBForm::ConstructMenuIDItems( HMENU hMenuParent, bool bIsTop/*=false*/ )
{
	int i, iCount;
	UINT idItem;
	
	if (bIsTop) m_hashMenuIDs.Clear();	// ������м�¼
	if (0==hMenuParent) return;   

	// ��� hMenuParent �µĲ˵�������
	iCount = GetMenuItemCount(hMenuParent); 
	if (iCount == -1) return;		// GetMenuItemCount ����ʧ�ܣ����޲˵���
	
	// ������� hMenuParent �µ����в˵���
	for(i=0; i<=iCount-1; i++)
	{
		idItem = GetMenuItemID(hMenuParent, i);
		if (idItem == 0xFFFFFFFF)
		{
			// hMenu �µĵ� i ���м����˵�
			// �ݹ鴦���˼����˵��µ������Ӳ˵���
			ConstructMenuIDItems(GetSubMenu(hMenuParent, i));
		}
		else
		{
			// hMenu �µĵ� i ���ѻ�ò˵��� ID������� ID ����ϣ��
			// ��ϣ�� key = �˵������Դid��data = �ò˵��ĸ��˵��ľ��
			m_hashMenuIDs.Add((int)hMenuParent, idItem, 0, 0, 0, 0, 0, false);
		}
		
	}
}




void CBForm::ClearResource()
{
	// �ͷŸ���������Դ
	// ����ɾ��������ϵͳ���Զ�ɾ��
	// m_WndProp.hCursor
	// if (m_WndProp.cursorIdx) MousePointerSet(0);  // ���л� DestroyCursor
	m_WndProp.hCursor = NULL; m_WndProp.hCursor2 = NULL;
	m_WndProp.cursorIdx = 0;  m_WndProp.cursorIdx2 = 0;

	m_WndProp.backColor = -1;
	if (m_WndProp.hBrushBack) 
	{ DeleteObject(m_WndProp.hBrushBack); m_WndProp.hBrushBack = NULL; }

	m_WndProp.foreColor = -1;
	m_WndProp.bBackStyleTransparent = false;
	if (m_WndProp.hFont) 
	{ DeleteObject(m_WndProp.hFont); m_WndProp.hFont = NULL; }

	SetRect(&m_WndProp.rcTextOutClip, 0, 0, 0, 0);
	m_WndProp.iTextOutStartLeft = 0;
	m_WndProp.iTextOutFlags = 0;

	m_WndProp.stretch = false;
	if (m_WndProp.hBmpDisp) { DeleteObject((HGDIOBJ)m_WndProp.hBmpDisp); m_WndProp.hBmpDisp=NULL; }
	SetRect(&m_WndProp.rcPictureClip, -1, -1, -1, -1);
	m_WndProp.iTitleBarBehav = 0;

	if (m_WndProp.tagString)	// m_WndProp.tagString δ�� HM ���� 
	{ delete [](m_WndProp.tagString); m_WndProp.tagString = NULL; 	}
	m_WndProp.iTag1 = 0;
	m_WndProp.iTag2 = 0;
	m_WndProp.dTag = 0.0;	


	// �ͷŴ���
	if (m_hWnd)
	{
		//==============================================================
		// �ڴ˴������ٵ��� DestroyWindow ���� WM_DESTROY ��Ϣ
		// ���յ� WM_DESTROY ��Ϣʱ����ñ������������γ����ݹ�

		// �ָ��Ӵ��ڿؼ������ദ����lParam ��Ϊ 0 ��ʾж�ش���ʱ�ָ����ദ���Ӵ��ڿؼ�
		EnumChildWindows(m_hWnd, EnumChildProcSubClass, 0);

		// ���ǰ̨���ڸպ��Ǳ����ڣ�ȡ�����ټ���¼��ǰ̨���ڼ�¼
		if (CBForm::ms_hWndActiForm == m_hWnd)
		{
			CBForm::ms_hWndActiForm = NULL;
			CBForm::ms_hAccelCurrUsed = NULL;
		}
		
		// �ӹ�ϣ����ɾ���öԻ���ļ�¼
		ms_hashWnd.Remove((long)m_hWnd, false);
		
		// ����ô���Ϊģ̬���ָ����д��� Enabled
		if (m_ModalShown) RestoreModalWinEnabled();
		
		// �����Ա
		m_hashMenuIDs.Clear();
		
		m_hWnd = NULL;
		// m_hashEventNotify.Clear();
		// m_hashEventMessage.Clear();
	}
}








long CBForm::RoundRectForm()
{
	return m_RoundRect;
}


int CBForm::EdgeWidth()
{
	return m_EdgeWidth;
}

void CBForm::EdgeWidthSet( int newEdgeWidth )
{
	m_EdgeWidth = newEdgeWidth;
	InvalidateRect(m_hWnd, NULL, true);	// ʹ������Ч���� WM_PAINT �н����Ʊ߿�
}

COLORREF CBForm::EdgeColor()
{
	return m_EdgeColor;
}

void CBForm::EdgeColorSet( COLORREF newEdgeColor )
{
	m_EdgeColor = newEdgeColor;
	InvalidateRect(m_hWnd, NULL, true);	// ʹ������Ч���� WM_PAINT �н����Ʊ߿�
}


void CBForm::RoundRectFormSet(long newRoundCorner)
{
	if (m_hWnd==NULL) Load();					// ������ڻ�δ���أ����ڼ���

	m_RoundRect = newRoundCorner;

	if ( m_RoundRect )
	{
		// ����Բ�Ǵ���
		HRGN hrgnOld = m_hRgnRoundRect;
		
		if (m_RoundRect < 65536)
			m_hRgnRoundRect = CreateRoundRectRgn(0, 0, Width()+1, Height()+1, m_RoundRect, m_RoundRect);
		else
			m_hRgnRoundRect = CreateRoundRectRgn(0, 0, Width()+1, Height()+1, LOWORD(m_RoundRect), HIWORD(m_RoundRect));
		SetWindowRgn(m_hWnd, m_hRgnRoundRect, TRUE);
		
		if (hrgnOld) DeleteObject(hrgnOld);
	}
	else
	{
		// m_RoundRect == 0����ʹ��Բ�Ǵ���
		if (m_hRgnRoundRect)
		{
			// �Ѿ����Բ�Ǵ��壬�Żָ�
			SetWindowRgn(m_hWnd, NULL, TRUE);
			DeleteObject(m_hRgnRoundRect);
			m_hRgnRoundRect = NULL;
		}
		// else
		//     û�����Բ�Ǵ��壬���ػָ�
	}
}

void CBForm::MoveToScreenCenter( int width/*=-1*/, int height/*=-1*/, bool bHorizontalCenter/*=true*/, bool bVerticalCenter/*=true*/ )
{
	if (m_hWnd==NULL) Load();					// ������ڻ�δ���أ����ڼ���

	RECT rect;
	GetWindowRect(m_hWnd, &rect);

	// ������Ӧ�õĿ��ȡ��߶� => width, height
	if (width<=0) width = rect.right - rect.left;
	if (height<=0) height = rect.bottom - rect.top;

	// ������Ӧ��������Ļλ�õ����Ͻ����� => rect.left, rect.top
	if (bHorizontalCenter)
		rect.left = ( GetSystemMetrics(SM_CXSCREEN) - width ) / 2;
	if (bVerticalCenter)
		rect.top = ( GetSystemMetrics(SM_CYSCREEN) - height ) / 2;
	
	// һ���Ե��������λ�á���С
	MoveWindow(m_hWnd, rect.left, rect.top, width, height, 1);
}



// �����ޱ߿��壩�Ƿ����������϶��������ܸı䴰���С��������>0��ֵΪ�ɸı��С�ı߿�Χ��
int CBForm::BorderAutoResizable()
{
	return m_iBorderAutoResizable;
}

void CBForm::BorderAutoResizableSet( int iNewValue/*=1*/ )
{
	m_iBorderAutoResizable = iNewValue;
}








long CBForm::EventsGenerator( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	switch(uMsg)
	{
	case WM_COMMAND:	// �Ӵ��ڿؼ�֪ͨ��Ϣ����˵���Ϣ������ټ���Ϣ
		if (lParam)
		{
			// ======== �Ӵ��ڿؼ�֪ͨ��Ϣ ========
			// wparam ��λΪ֪ͨ�룬��λΪ�ؼ�ID��������Ϊ������
			// �� m_hashEventNotify �в������޼�Ϊ wparam ��������
			//   ��ʾ����¼���Ҫ�����������û����������¼�
			// �û������ĵ�ַ�� m_hashEventNotify �Ķ�Ӧ��� Data �б���
			if (m_hashEventNotify.IsKeyExist((long)wParam))
			{
				m_idRaisingEvent = LOWORD(wParam);

				ONEventVoid ptrFunc0 = NULL; 
				ONEventI ptrFunc1 = NULL;
				ONEventII ptrFunc2 = NULL;
				ONEventIII ptrFunc3 = NULL;
				ONEventIIII ptrFunc4 = NULL;
				switch(m_hashEventNotify.ItemLong((long)wParam, false))
				{
				case 0:		// 0 ������
					ptrFunc0 =
						(ONEventVoid)m_hashEventNotify.Item((long)wParam, false);
					if (ptrFunc0) (*ptrFunc0)();								// �� if �ݴ��ж�
					break;
				case 1:		// 1 �� int �Ͳ���
					ptrFunc1 = 
						(ONEventI)m_hashEventNotify.Item((long)wParam, false);
					if (ptrFunc1) (*ptrFunc1)((int)wParam);						// �� if �ݴ��ж�
					break;
				case 2:		// 2 �� int �Ͳ���
					ptrFunc2 = 
						(ONEventII)m_hashEventNotify.Item((long)wParam, false);
					if (ptrFunc2) (*ptrFunc2)((int)wParam, (int)lParam);		// �� if �ݴ��ж�
					break;
				case 3:		// 3 �� int �Ͳ���
					ptrFunc3 = 
						(ONEventIII)m_hashEventNotify.Item((long)wParam, false);
					if (ptrFunc3) (*ptrFunc3)((int)LOWORD(wParam), (int)HIWORD(wParam), 
						lParam);												// �� if �ݴ��ж�
					break;
				case 4:		// 4 �� int �Ͳ���
					ptrFunc4 = 
						(ONEventIIII)m_hashEventNotify.Item((long)wParam, false);
					if (ptrFunc4) (*ptrFunc4)((int)LOWORD(wParam), (int)HIWORD(wParam), 
						(int)LOWORD(lParam), (int)HIWORD(lParam));				// �� if �ݴ��ж�
					break;

				}	// end of switch
			}	// end of if (m_hashEventNotify.IsKeyExist((long)wparam))
		}	// end of if (lparam)
		else	// if (lparam) else��lparam==NULL
		{  
			// ======== �˵���Ϣ��ϵͳ�˵����ڴ˴�����������ټ���Ϣ ========
			ONEventIII ptrMenuClick;
			ptrMenuClick = (ONEventIII)m_hashEventNotify.Item(c_CBMenuClickEventKey, false);
			if (ptrMenuClick)
			{
				// ����1Ϊ�������Ĳ˵���ԴID��
				// ����2Ϊ��ֱ��ѡ��Ĳ˵�(=0)�����ǴӼ��ټ�ִ�е�(=1)
				// ����3=0 ��ʾ�Ǵ���ͨ�˵�ѡ��ģ�=1��ʾ�Ǵ�ϵͳ�˵�ѡ���
				(*ptrMenuClick)((int)LOWORD(wParam), (int)(HIWORD(wParam)), 0);
			}
		}

		return 0;   // ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

	case WM_NOTIFY:
		{
			long key = MAKELPARAM(((NMHDR *)lParam)->idFrom, ((NMHDR *)lParam)->code);
			if (m_hashEventNotify.IsKeyExist(key))
			{
				m_idRaisingEvent =((NMHDR *)lParam)->idFrom;

				ONEventVoid ptrFunc0 = NULL; 
				ONEventI ptrFunc1 = NULL;
				ONEventII ptrFunc2 = NULL;
				ONEventIII ptrFunc3 = NULL;
				ONEventIIII ptrFunc4 = NULL;
				switch(m_hashEventNotify.ItemLong(key, false))
				{
				case 0:		// 0 ������
					ptrFunc0 = (ONEventVoid)m_hashEventNotify.Item(key, false);
					break;
				case 1:		// 1 �� int �Ͳ���
					ptrFunc1 = (ONEventI)m_hashEventNotify.Item(key, false);
					break;
				case 2:		// 2 �� int �Ͳ���
					ptrFunc2 = (ONEventII)m_hashEventNotify.Item(key, false);
					break;
				case 3:		// 3 �� int �Ͳ���
					ptrFunc3 = (ONEventIII)m_hashEventNotify.Item(key, false);
					break;
				case 4:		// 4 �� int �Ͳ���
					ptrFunc4 = (ONEventIIII)m_hashEventNotify.Item(key, false);
					break;					
				}	// end of switch(m_hashEventNotify.ItemLong(key, false))

				switch (((NMHDR *)lParam)->code)
				{
				case NM_CLICK: case NM_RCLICK:				
				case NM_DBLCLK: case NM_RDBLCLK:
					{
						NMLISTVIEW * pnm = (NMLISTVIEW *)lParam;
						if (ptrFunc4) (*ptrFunc4)(pnm->iItem+1, pnm->iSubItem+1, 
							pnm->ptAction.x, pnm->ptAction.y);  // +1ת��Ϊ�±��1��ʼ
					}
					break;
				default:
					switch(m_hashEventNotify.ItemLong(key, false))
					{
					case 0:		// 0 ������
						if (ptrFunc0) (*ptrFunc0)();								// �� if �ݴ��ж�
						break;
					case 1:		// 1 �� int �Ͳ���
						if (ptrFunc1) (*ptrFunc1)((int)wParam);						// �� if �ݴ��ж�
						break;
					case 2:		// 2 �� int �Ͳ���
						if (ptrFunc2) (*ptrFunc2)((int)wParam, (int)lParam);		// �� if �ݴ��ж�
						break;
					case 3:		// 3 �� int �Ͳ���
						if (ptrFunc3) (*ptrFunc3)((int)LOWORD(wParam), (int)HIWORD(wParam), 
							lParam);												// �� if �ݴ��ж�
						break;
					case 4:		// 4 �� int �Ͳ���
						if (ptrFunc4) (*ptrFunc4)((int)LOWORD(wParam), (int)HIWORD(wParam), 
							(int)LOWORD(lParam), (int)HIWORD(lParam));				// �� if �ݴ��ж�
						break;					
					}	// end of switch(m_hashEventNotify.ItemLong(key, false))
				}  // end of switch (((NMHDR *)lParam)->code)
			}
		}	// end of case WM_NOTIFY

		return 0;   // ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

	case WM_DESTROY:
		// ������ WM_DESTROY �������̡��򸸴��屻�ر�ʱ�����Ӵ�����Զ��رչ���
		//   �����յ� WM_CLOSE ��Ϣ��ֻ���յ� WM_DESTROY ��Ϣ

		// ���� Form_UnLoad �¼�����������еĻ���
		{
			ONEventVoid ptrFuncForm0 = (ONEventVoid)m_hashEventMessage.Item((long)uMsg, false);
			if (ptrFuncForm0) (*ptrFuncForm0)();		// �� if �ݴ��ж�
		}
		
		// �� ClearResource �У���ö���Ӵ��ڣ��ָ������Ӵ��ڿؼ������ദ��
		ClearResource();
		return 0;   // ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

	case WM_CLOSE:
		
		// ���� Form_QueryLoad �¼�����������еĻ���
		{
			int bCancel = 0;
			ONEventI ptrFuncForm0 = (ONEventI)m_hashEventMessage.Item((long)uMsg, false);
			if (ptrFuncForm0) 
			{
				(*ptrFuncForm0)((int)(&bCancel));		// �� if �ݴ��ж�
				if (bCancel) return bCancel;			// ȡ�� WM_CLOSE ��Ϣ����ж�ش���
			}
		}

		// ���ǰ��û��ȡ����bCancelû��Ϊ��0ֵ��������ж�ضԻ���
		// ������ SendMessage(m_hWnd, WM_DESTROY, 0, 0); 
		DestroyWindow(m_hWnd);
		
		// ��ʱ��Ӧ���� CBForm_DlgProc ���յ��� WM_DESTROY ��Ϣ����ִ���� ClearResource()
		//   ��ʹ m_hWnd Ϊ 0�������ʱ m_hWnd �Բ�Ϊ 0��Ϊ�ݴ��������ٴ�ִ�� ClearResource()
		if (m_hWnd) ClearResource(); 

		return 0;   // ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

	case WM_ACTIVATE:
		
		// ���� eForm_ActiDeactivate �¼�������ʧ��ʱ��������еĻ���
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			ONEventI ptrFuncForm0 = (ONEventI)m_hashEventMessage.Item((long)uMsg, false);
			if (ptrFuncForm0) (*ptrFuncForm0)(0);		// �� if �ݴ��ж�
		}

		// �л���ǰǰ̨����
		ms_hWndActiForm=m_hWnd;

		// �л����ټ�
		if (LOWORD(wParam)==WA_ACTIVE || LOWORD(wParam)== WA_CLICKACTIVE)
		{
			ms_hAccelCurrUsed=(HACCEL)ms_hashWnd.ItemLong((long)m_hWnd, false); 
		}

		// ���� eForm_ActiDeactivate �¼�����������ʱ��������еĻ���
		if (LOWORD(wParam) != WA_INACTIVE)
		{
			ONEventI ptrFuncForm0 = (ONEventI)m_hashEventMessage.Item((long)uMsg, false);
			if (ptrFuncForm0) (*ptrFuncForm0)(1);		// �� if �ݴ��ж�
		}

		return 0;   // ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

	case WM_SHOWWINDOW:
		// ��������������أ����Ҹô���Ϊģ̬���ָ����д��� Enabled
		if (wParam==0)
			if (m_ModalShown) RestoreModalWinEnabled();
		
		break;		// ���� switch ��������¼���Ĭ�ϴ������� return 0; �������Ǹ�Ĭ�ϴ���
		
	case WM_SETCURSOR:
		if (pApp->MousePointerGlobal())
		{
			// ======== ʹ��ȫ������� ========
			if (pApp->MousePointerGlobalHCursor())
			{
				SetCursor(pApp->MousePointerGlobalHCursor());
				return 1;	// return 1; ������ switch ���Ĭ�������¼��Ĵ�����1��ʾҲ����Ĭ�ϴ��ڳ�������
			}
		}
		else if (m_WndProp.cursorIdx)  
		{
			// ======== ʹ��Ϊ�����������õ������ ========
			if (m_WndProp.hCursor)
			{					
				SetCursor(m_WndProp.hCursor);
				return 1;	// return 1; ������ switch ���Ĭ�������¼��Ĵ�����1��ʾҲ����Ĭ�ϴ��ڳ�������
			}
		}
		
		// ����ʹ��Ĭ�ϣ��������������꣨Ĭ�ϴ��ڳ����Զ�������
		break;		// ���� switch ��������¼���Ĭ�ϴ������� return 0; �������Ǹ�Ĭ�ϴ���
		
	case WM_ERASEBKGND:
		// ���ô��ڱ���ɫ
		if (m_WndProp.backColor != -1 && m_WndProp.hBrushBack)	// -1 ��ʾ����ϵͳĬ����ɫ�������Ԥ
		{
			RECT rectBK;
			GetClientRect(m_hWnd, &rectBK);
			FillRect((HDC)wParam, &rectBK, m_WndProp.hBrushBack);
			return 1;	// ��Ӧ�ó������в������������뷵�� 1
		}

		break;  // ���� switch ��������¼���Ĭ�ϴ������� return 0; �������Ǹ�Ĭ�ϴ���

	case WM_CTLCOLORSTATIC: 
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORSCROLLBAR:
	case WM_CTLCOLOREDIT:
		// �����Ӵ��ڿؼ�����ɫ��ǰ��ɫ����Щ��Ϣ�Ƿ��͵������ڵģ����Ƿ��͵��ؼ���
		{
			// ���� CBWndBase �ľ�̬���� PropertyMemCtrl���Կؼ��ľ����lParam����øÿؼ������Կռ�
			STWndProp *pPro = CBWndBase::PropertyMemCtrl((HWND)lParam, false);

			if (pPro)
			{
				// ======== ����ǰ��ɫ ========
				if (pPro->foreColor != -1) 
				{
					SetTextColor((HDC)wParam, pPro->foreColor);

					if (pPro->backColor == -1)
					{
						// ���û��ͬʱ���ñ���ɫ��Ϊʹǰ��ɫ��Ч
						// ����ѱ���ɫ����Ϊϵͳ��ɫ
						pPro->backColor = GetSysColor(COLOR_BTNFACE);
						if (pPro->hBrushBack) DeleteObject(pPro->hBrushBack);
						pPro->hBrushBack = CreateSolidBrush(pPro->backColor);	
						// pProp->backColor �� != -1������� if �λ᷵�ر���ɫ��ˢ
					}
				}

				// ======== ���ñ���ɫ ========
				if (pPro->bBackStyleTransparent)
				{
					// ����͸��
					SetBkMode((HDC)wParam, TRANSPARENT); 
					//hDC->SetTextColor(RGB(0,0,0)); 
					return (long)GetStockObject(HOLLOW_BRUSH); 
				}	// if (pProp->bBackStyleTransparent)
				else
				{
					// ������͸��
					if (pPro->backColor != -1)
					{
						// �û�ָ���˱���ɫ
						// �����ֱ�����Ϊ��͸�������ػ�ˢ
						SetBkColor((HDC)wParam, pPro->backColor);
						SetBkMode((HDC)wParam, OPAQUE);
						if (pPro->hBrushBack) return (long)(pPro->hBrushBack);
					}
					// else
						// �û�û��ָ������ɫ��ִ�� break; �� switch ��Ĭ�ϴ��������¼�
				}	// end if (pPro->bBackStyleTransparent) - els

			}  // end if (pPro)
		}  // end {

		break;	// ���� switch ��������¼���Ĭ�ϴ������� return 0; �������Ǹ�Ĭ�ϴ���

	// ==================================================================
	//	�����¼��Ĵ���
	case WM_PAINT:
		// ��� m_WndProp.hBmpDisp ��Ϊ0��˵��ָ����Ҫ�ڴ�������ʾ��λͼͼƬ
		// ��� m_EdgeWidth>0 ��˵��ָ����Ҫ�ڴ����ϻ��Ʊ߿�
		// ����������� BeginPaint 
		if (m_WndProp.hBmpDisp || m_EdgeWidth>0)	
		{
			PAINTSTRUCT ps;
 			BeginPaint(m_hWnd, &ps);
			
			// ���� CBWndBase �����е� RefreshPicture ��ʾλͼ���� 
			//   m_WndProp.hBmpDisp Ϊ 0 ���º������Զ�������
			RefreshPicture(ps.hdc, m_EdgeWidth, m_EdgeWidth);

			// ���ƴ���߿�
			if (m_EdgeWidth>0)
			{
				int iEdgeWidth;		// ����2���ı߿���ȣ���ʹ��PS_INSIDEFRAME������Բ�Ǿ���������©�ı�
				if (m_EdgeWidth==1) iEdgeWidth = m_EdgeWidth; else iEdgeWidth=m_EdgeWidth*2-1;
				HPEN hPen = CreatePen(PS_SOLID, iEdgeWidth, m_EdgeColor);
				HPEN hPenOld = (HPEN)SelectObject(ps.hdc, hPen);
				HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH); 
				HBRUSH hBrushOld = (HBRUSH)SelectObject(ps.hdc, hBrush);

				if ( m_RoundRect )
				{
					// ʹ��Բ�Ǵ���
					if (m_RoundRect < 65536)
						RoundRect(ps.hdc, 0, 0, Width(), Height(), m_RoundRect, m_RoundRect);
					else
						RoundRect(ps.hdc, 0, 0, Width(), Height(), LOWORD(m_RoundRect), HIWORD(m_RoundRect));
				}
				else
				{
					// m_RoundRect == 0����ʹ��Բ�Ǵ���
					Rectangle(ps.hdc, 0, 0, Width(), Height());
				}

				SelectObject(ps.hdc, hBrushOld);
				SelectObject(ps.hdc, hPenOld);
				DeleteObject(hPen);
			}	// end if (m_EdgeWidth>0)

			EndPaint(m_hWnd, &ps);
		}	// end if (m_hBmpBack || m_EdgeWidth>0)

		break;	// ���� switch ��������¼���Ĭ�ϴ������� return 0; �������Ǹ�Ĭ�ϴ���

	case WM_SIZE: 
		if ( m_RoundRect ) RoundRectFormSet(m_RoundRect);		// �����Բ�Ǿ��Σ���С�ı�����������ô����Բ������
		if ( m_WndProp.hBmpDisp || m_EdgeWidth>0 ) InvalidateRect(m_hWnd, NULL, FALSE);
		break;	// ���� switch ��������¼���Ĭ�ϴ������� return 0; �������Ǹ�Ĭ�ϴ���

	case WM_LBUTTONDOWN: 	case WM_RBUTTONDOWN:	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:		case WM_RBUTTONUP:		case WM_MBUTTONUP:
	case WM_LBUTTONDBLCLK:	case WM_MBUTTONDBLCLK:	case WM_RBUTTONDBLCLK:
	case WM_MOUSEMOVE:
		// ������ҡ��м����¡�̧��˫��������ƶ�
		{
			long key = 0;  int button = 0;
			if (uMsg == 0x200)				// ����ƶ�
			{
				key = WM_MOUSEMOVE;
				if ( wParam & MK_LBUTTON ) button = button | 1;
				if ( wParam & MK_RBUTTON ) button = button | 2;
				if ( wParam & MK_MBUTTON ) button = button | 4;

				//////////////////////////////////////////////////////////////////////////
				// ģ�� �ڱ��������϶� �ƶ�����λ����Ϊ���϶��߿�ı䴰���С��Ϊ
				if ( m_WndProp.iTitleBarBehav || m_iBorderAutoResizable )
				{
					static int iCursorIndexLast = m_WndProp.cursorIdx;  // ��¼�ϴ����ָ�룬����ÿ�� MouseMove ���仯

					// ������λ����λ�ڱ߿��Լ��ϡ��¡����ұ߿�λ��(��0)������λ�ڴ����ڲ�(0) => posi
					// ���ָ��Ӧ�õ���״��ϵͳָ����״�� => m_CursorIdx
					// ԭ�����ָ����״�� m_CursorIdx2 �б��棬�����δλ�ڱ߿򣬿��� m_CursorIdx2 �ָ�ָ����״
					const int c_iBorderRange = 8;
					POINT pt; WPARAM posi=0; int cursor=0; LPARAM lparamPt=0;
					GetCursorPos(&pt);  lparamPt=(LPARAM)(*(LPARAM *)&pt);
					ScreenToClient(m_hWnd, &pt);
					if (pt.y<c_iBorderRange)
					{
						if (pt.x<c_iBorderRange) { posi=HTTOPLEFT; m_WndProp.cursorIdx=IDC_SizeNWSE; }
						else if(pt.x>Width()-c_iBorderRange) { posi=HTTOPRIGHT; m_WndProp.cursorIdx=IDC_SizeNESW; }
						else { posi=HTTOP; m_WndProp.cursorIdx=IDC_SizeNS; }
					}
					else if(pt.y>Height()-c_iBorderRange)
					{
						if (pt.x<c_iBorderRange) { posi=HTBOTTOMLEFT; m_WndProp.cursorIdx=IDC_SizeNESW; }
						else if(pt.x>Width()-c_iBorderRange) { posi=HTBOTTOMRIGHT; m_WndProp.cursorIdx=IDC_SizeNWSE; }
						else { posi=HTBOTTOM; m_WndProp.cursorIdx=IDC_SizeNS; }
					}
					else
					{
						if (pt.x<c_iBorderRange) { posi=HTLEFT; m_WndProp.cursorIdx=IDC_SizeWE; }
						else if(pt.x>Width()-c_iBorderRange) { posi=HTRIGHT; m_WndProp.cursorIdx=IDC_SizeWE; }
						else
						{ posi=0; m_WndProp.cursorIdx=m_WndProp.cursorIdx2; }	// ���λ�ڴ����ڲ����򣨿����ƶ�����λ�ã�
					}
					
					if (m_iBorderAutoResizable && iCursorIndexLast!=m_WndProp.cursorIdx)
					{
						// �������ָ����״Ϊ m_CursorIdx ����״
						if (m_WndProp.cursorIdx)
						{
							if (m_WndProp.cursorIdx == m_WndProp.cursorIdx2)
								m_WndProp.hCursor = m_WndProp.hCursor2;		// ʹ�ø����������״
							else
								m_WndProp.hCursor = LoadCursor(NULL, 
								  MAKEINTRESOURCE(m_WndProp.cursorIdx - gc_IDStandCursorIDBase));
						}
						// �򱾴��ڷ��� WM_SETCURSOR����ʹ���������Ч
						//    �ڱ����ڲ��Ͻ��յ��� WM_SETCURSOR ��Ϣ�л�ı������
						SendMessage(m_hWnd, WM_SETCURSOR, (WPARAM)m_hWnd, 0);

						iCursorIndexLast = m_WndProp.cursorIdx;
					}

					if (button==1)
					{
						// ���� posi ���ô���λ�á���С
						ReleaseCapture();

						// ģ��ı䴰���С
						if (posi && m_iBorderAutoResizable) 
							SendMessage(m_hWnd, WM_NCLBUTTONDOWN, posi, lparamPt);	

						// ģ���ƶ�����λ��
						if (posi==0 && m_WndProp.iTitleBarBehav)
							SendMessage (m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, *((LPARAM *)(&pt)) );	
					}
				}  // if ( m_iTitleBarBehav || m_iBorderAutoResizable )
			}
			else if ((uMsg-0x200) % 3 == 1)	// ���£�0x201��0x204��0x207��(uMsg-0x200) % 3 == 1
			{
				key = WM_LBUTTONDOWN;		// ͳһ�� WM_LBUTTONDOWN ������Ϊ3��ö��ֵ
				button = 1 << (((uMsg-0x0200)-1)/3); 
			}
			else if ((uMsg-0x200) % 3 == 2)	// ̧��0x202��0x205��0x208��(uMsg-0x200) % 3 == 2
			{
				key = WM_LBUTTONUP;			// ͳһ�� WM_LBUTTONUP ������Ϊ3��ö��ֵ
				button = 1 << (((uMsg-0x0200)-2)/3);
			}
			else if ((uMsg-0x200) % 3 == 0)	// ˫����0x203��0x206��0x209��(uMsg-0x200) % 3 == 0
			{
				key = WM_LBUTTONDBLCLK;		// ͳһ�� WM_LBUTTONDBLCLK ������Ϊ3��ö��ֵ
				button = 1 << ((uMsg-0x0203)/3);
			}
			else
				return 0;	// �ݴ��������� switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

			if (m_hashEventMessage.IsKeyExist(key))
			{
				ONEventIIII ptrFuncForm4 = NULL;
				ptrFuncForm4 = 
					(ONEventIIII)m_hashEventMessage.Item(key, false);
				if (ptrFuncForm4)
				{
					POINT pt; UINT altKey=0;  
					GetCursorPos(&pt);  ScreenToClient(m_hWnd, &pt);
					if ( GetKeyState(VK_MENU)<0 ) altKey=4;		// Shift=1; Ctrl=2; Alt=4

					(*ptrFuncForm4)(button, 
						altKey | ( ( (wParam & (MK_CONTROL | MK_SHIFT))) /4 ), 
						pt.x, pt.y);
					
					return 0;
				}	// if (ptrFuncForm4)
			}	// if (m_hashEventMessage.IsKeyExist(key)
		}

		return 0;   // ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

	case WM_CHAR: 
		if (m_hashEventMessage.IsKeyExist(WM_CHAR))
		{
			ONEventII ptrFuncForm2 = NULL;
			int bCancel = 0;
			ptrFuncForm2 = 
				(ONEventII)m_hashEventMessage.Item(WM_CHAR, false);
			if (ptrFuncForm2)
			{
				(*ptrFuncForm2)((int)wParam, (int)(&bCancel)); 
				if (bCancel) return 1; else return 0;
			}
		}

		return 0;   // ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

	case WM_KEYDOWN: case WM_KEYUP: 		//   shift=1,2,4 �ֱ��ʾͬʱ Shift��Ctrl��Alt ��������
	case WM_SYSKEYDOWN: case WM_SYSKEYUP:
		{
			long key = uMsg;
			if (uMsg>=0x0104) key = (uMsg-0x4);	// ��WM_SYS��Ϣ�鲢��WM_��Ϣ
			if (m_hashEventMessage.IsKeyExist(key))
			{
				int iShift = 0;
				int bCancel = 0;
				if (GetKeyState(VK_SHIFT)<0 ) iShift |= 1;
				if (GetKeyState(VK_CONTROL)<0 ) iShift |= 2;
				if (GetKeyState(VK_MENU)<0 ) iShift |= 4;
				
				ONEventIII ptrFuncForm3 = NULL;
				ptrFuncForm3 = 
					(ONEventIII)m_hashEventMessage.Item(key, false);
				if (ptrFuncForm3)
				{
					(*ptrFuncForm3)((int)wParam, iShift, (int)(&bCancel)); 
					if (bCancel) return 1; else return 0;
				}
			}
		}

		return 0;   // ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

	case WM_SYSCOMMAND:
		{
			ONEventIII ptrMenuClick;
			ptrMenuClick = (ONEventIII)m_hashEventNotify.Item(c_CBMenuClickEventKey, false);
			if (ptrMenuClick)
			{
				// ����1Ϊ�������Ĳ˵���ID��SC_CLOSE��SC_MAXIMIZE ��
				// ����2Ϊ��ֱ��ѡ��Ĳ˵�(=0)�����ǴӼ��ټ�ִ�е�(=1)
				// ����3=0 ��ʾ�Ǵ���ͨ�˵�ѡ��ģ�=1��ʾ�Ǵ�ϵͳ�˵�ѡ���
				(*ptrMenuClick)((int)wParam, 0, 1);

				return 0;
			}
		}

		return 0;   // ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

	case WM_MENUSELECT:
		if (m_hashEventMessage.IsKeyExist((long)WM_MENUSELECT))
		{
			ONEventI ptrFuncForm1 = NULL;
			
			ptrFuncForm1 = 
				(ONEventI)m_hashEventMessage.Item((long)WM_MENUSELECT, false);
			if (ptrFuncForm1)		// �� if �ݴ��ж�
			{
				if (lParam)
				{
					// ʹ��ģ�鼶 m_MenuItemSel ��Ա�󶨵���ѡ�˵�
					if (HIWORD(wParam) & MF_POPUP)
						// ��ѡ�˵������һ������ʽ�˵���LOWORD(wParam) ��������
						m_MenuItemSel.SetFromPos((HMENU)lParam, LOWORD(wParam)+1, m_hWnd, &m_hashMenuIDs);	//+1 ת��Ϊλ�úŴ�1��ʼ
					else
						// ��ѡ�˵�������ͨ�����LOWORD(wParam) �� ResID
						m_MenuItemSel.SetFromResID((HMENU)lParam, LOWORD(wParam), m_hWnd, &m_hashMenuIDs);
					
					(*ptrFuncForm1)((int)&m_MenuItemSel);					
				}
				else
				{
					(*ptrFuncForm1)(NULL);
				}

				return 0;
			}
		}

		return 0;   // ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

	case WM_DROPFILES:
		if (m_hashEventMessage.IsKeyExist((long)WM_DROPFILES))
		{
			ONEventIIII ptrFuncForm4 = NULL;
			
			ptrFuncForm4 = 
				(ONEventIIII)m_hashEventMessage.Item((long)WM_DROPFILES, false);
			if (ptrFuncForm4)													// �� if �ݴ��ж�
			{
				// ����϶��ļ����ܸ���
				UINT count = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL, 0);
				if (count==0) return 0; 	// �ݴ��������� switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

				// ���ٱ����ַ���ָ��Ŀռ�
				unsigned long * ptrArr = new unsigned long [count+1]; // ������ [0] �Ŀռ�
				HM.AddPtr(ptrArr);
				HM.ZeroMem(ptrArr, sizeof(unsigned long)*(count+1));

				// ���ٸ����ַ����Ŀռ䣬����ø��ļ���
				UINT i=0; UINT sizeEach=0; TCHAR * pstr;
				for (i=0; i<count; i++)
				{
					sizeEach = DragQueryFile((HDROP)wParam, i, NULL, 0);
					pstr = new TCHAR [sizeEach+1];
					HM.AddPtr(pstr);
					HM.ZeroMem(pstr, sizeof(TCHAR)*sizeEach);
					ptrArr[i+1] = (unsigned long)pstr;
					DragQueryFile((HDROP)wParam, i, pstr, sizeEach+1);
				}

				// ����Ϸ��ļ�ʱ�������λ��
				POINT ptDrag;
				DragQueryPoint((HDROP)wParam, &ptDrag); 
					
				// �ͷž�� hDrop
				DragFinish((HDROP)wParam);

				// �����¼�
				(*ptrFuncForm4)((int)ptrArr, count, (int)ptDrag.x, (int)ptDrag.y);	
			}
		}

		return 0;   // ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

	case WM_USER_NOTIFYICON:
		if (m_hashEventMessage.IsKeyExist((long)WM_USER_NOTIFYICON))
		{
			ONEventIII ptrFuncForm3 = NULL;

			ptrFuncForm3 = 
				(ONEventIII)m_hashEventMessage.Item((long)WM_USER_NOTIFYICON, false);
			if (ptrFuncForm3)													// �� if �ݴ��ж�
			{	
				int action=0, button=0;
				if ((int)lParam == 0x200)	// ����ƶ�
				{ 
					action=0; button=0; 
				}	
				else						// ��갴�¡�̧��˫��
				{
					action = ((int)lParam-0x200) % 3;
					button = 1 << ((((int)lParam-0x0200)-action)/3);
					if (action==0) action=3;
				}

				// �����¼�
				(*ptrFuncForm3)(button, action, (int)wParam);	
			}
		}
		return 0;   // ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���
	case WM_CONTEXTMENU:
		// �����Ҽ��˵������Ƿ�ȡ�� WM_CONTEXTMENU ��Ϣ��
		{
			// ���� CBWndBase �ľ�̬���� PropertyMemCtrl���Կؼ��ľ����lParam����øÿؼ������Կռ�
			STWndProp *pPro = CBWndBase::PropertyMemCtrl((HWND)lParam, false);
			if (pPro)
			{			
				if (pPro->fDisableContextMenu) return 0;  // ȡ�� WM_CONTEXTMENU ��Ϣ
			}
		}
		break;	// ���� switch ��������¼���Ĭ�ϴ������� return 0; �������Ǹ�Ĭ�ϴ���
	}	// end switch
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////


	// �����¼���Ĭ�ϴ������������¼�����
	if (m_hashEventMessage.IsKeyExist((long)uMsg))
	{
		ONEventVoid ptrFuncForm0 = NULL;
		ONEventI ptrFuncForm1 = NULL;
		ONEventII ptrFuncForm2 = NULL;
		ONEventIII ptrFuncForm3 = NULL;
		ONEventIIII ptrFuncForm4 = NULL;
		switch(m_hashEventMessage.ItemLong((long)uMsg, false))
		{
		case 0:		// 0 ������
			ptrFuncForm0 =
				(ONEventVoid)m_hashEventMessage.Item((long)(uMsg), false);
			if (ptrFuncForm0) (*ptrFuncForm0)();							// �� if �ݴ��ж�
			break;
		case 1:		// 1 �� int �Ͳ���
			ptrFuncForm1 = 
				(ONEventI)m_hashEventMessage.Item((long)(uMsg), false);
			if (ptrFuncForm1) (*ptrFuncForm1)((int)wParam);					// �� if �ݴ��ж�
			break;
		case 2:		// 2 �� int �Ͳ���
			ptrFuncForm2 = 
				(ONEventII)m_hashEventMessage.Item((long)(uMsg), false);
			if (ptrFuncForm2) (*ptrFuncForm2)((int)wParam, (int)lParam);	// �� if �ݴ��ж�
			break;
		case 3:		// 3 �� int �Ͳ���
			ptrFuncForm3 = 
				(ONEventIII)m_hashEventMessage.Item((long)(uMsg), false);
			if (ptrFuncForm3) (*ptrFuncForm3)((int)LOWORD(wParam), (int)HIWORD(wParam),  
				(int)lParam);												// �� if �ݴ��ж�
			break;
		case 4:		// 4 �� int �Ͳ���
			ptrFuncForm4 = 
				(ONEventIIII)m_hashEventMessage.Item((long)(uMsg), false);
			if (ptrFuncForm4) (*ptrFuncForm4)((int)LOWORD(wParam), (int)HIWORD(wParam), 
				(int)LOWORD(lParam), (int)HIWORD(lParam));					// �� if �ݴ��ж�
			break;
		}
	}	// if (m_hashEventMessage.IsKeyExist((long)uMsg))

	return 0;
}



long CBForm::EventsGeneratorCtrl( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	long key = 0;											// ���ڼ�����ϣ���� key
	int idCtrl = GetDlgCtrlID(hWnd);						// ���Կؼ��Ŀؼ� ID
	if (idCtrl == 0 || idCtrl == -1) return gc_APICEventsGenDefautRet;		// �޷���ÿؼ� ID����ؼ��� ID���������¼���������������
	
	// ����������ϣ�� m_hashEventMessage����Ϊ���� 2 λΪ��Ϣ��WM_XXX������ 2 λΪ�ؼ� ID
	key = MAKELONG(uMsg, idCtrl);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	switch (uMsg)
	{
	case WM_COMMAND: case WM_NOTIFY:

		// �������������ؼ��ڲ��Ŀؼ��������ؼ�����֪ͨ��Ϣ
		// ���� EventsGenerator ͳһ����

		EventsGenerator(uMsg, wParam, lParam);

		return gc_APICEventsGenDefautRet;	// ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���


	// ==================================================================
	//	�����¼��Ĵ���
	case WM_LBUTTONDOWN: 	case WM_RBUTTONDOWN:	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:		case WM_RBUTTONUP:		case WM_MBUTTONUP:
	case WM_LBUTTONDBLCLK:	case WM_MBUTTONDBLCLK:	case WM_RBUTTONDBLCLK:
	case WM_MOUSEMOVE:
		// ������ҡ��м����¡�̧��˫��������ƶ�
		{
			//////////////////////////////////////////////////////////////////////////
			// ͼƬ��ť�Ĵ���
			STWndProp * pPro = CBWndBase::PropertyMemCtrl(hWnd, false);
			if (pPro)
			{
				if (pPro->ePicBtn)
				{
					CBControl ctrl;
					ctrl.SetResID(idCtrl);
					if ( ctrl.hWnd()==hWnd ) // �� idCtrl ��Ч�� ctrl.hWnd() ��Ϊ0����ҲΪ�ٴμ��飬ҲΪ�ݴ�����
					{
						POINT pt;  RECT rc;  
						GetCursorPos(&pt);  ScreenToClient(hWnd, &pt);
						GetWindowRect(hWnd, &rc);	// rect.right - rect.left Ϊ�ؼ� width	
													// rect.bottom - rect.top Ϊ�ؼ� height
						if (uMsg==WM_MOUSEMOVE)
						{	
							if (pt.x > 0 && pt.y > 0 && 
							  pt.x <= rc.right-rc.left && pt.y <= rc.bottom-rc.top)
							{
								if (pPro->iPicBtnCheckedState)
									ctrl.PicButtonStateSet(2, 1);	// ��̬��ť�İ���״̬����ƶ�����ť�ڲ�����ʾ����״̬
								else
									ctrl.PicButtonStateSet(1, 1);	// ��ͨ��ť����̬��ť��̧��״̬����ƶ�����ť�ڲ�����ʾ����״̬
							}
							else
							{
								if (pPro->iPicBtnCheckedState)
									ctrl.PicButtonStateSet(2, -1);	// ��̬��ť�İ���״̬����ƶ�����ť֮�⣺��ʾ����״̬
								else
									ctrl.PicButtonStateSet(0, -1);	// ��ͨ��ť����̬��ť��̧��״̬����ƶ�����ť֮�⣺��ʾ����״̬
							}
						}
						else if (uMsg==WM_LBUTTONDOWN || uMsg==WM_RBUTTONDOWN || uMsg==WM_MBUTTONDOWN)
						{
							ctrl.PicButtonStateSet(2);			// ������������ʾ����״̬
						}	
						else if (uMsg==WM_LBUTTONUP || uMsg==WM_RBUTTONUP || uMsg==WM_MBUTTONUP)
						{
							
							if (pPro->iPicBtnStateStyle>0)		// �������̬���任��̬
							{
								if (pPro->iPicBtnCheckedState==0) 
									pPro->iPicBtnCheckedState=1;
								else
									pPro->iPicBtnCheckedState=0;
							}

							if (pPro->iPicBtnCheckedState)
								ctrl.PicButtonStateSet(2);			// ��̬��ť�İ���״̧̬����������ʾ����״̬
							else
								ctrl.PicButtonStateSet(1);			// ��ͨ��ť����̬��ť��̧��״̧̬����������ʾ����״̬
							// ���� ReleaseCapture��ֻҪ��껹�ڿؼ��ϣ���ҪCapture����������ƿ�ʱ�Ͳ��ָܻ�״̬��
						}	
					}	// end if ( ctrl.hWnd()==hWnd ) 
				}	// end if (pPro->ePicBtn)
			}	// end if (pPro)


			//////////////////////////////////////////////////////////////////////////
			// ����ͨ���¼�
			long key = 0;  int button = 0;
			if (uMsg == 0x200)				// ����ƶ�
			{
				key = MAKELONG(WM_MOUSEMOVE, idCtrl);
				if ( wParam & MK_LBUTTON ) button = button | 1;
				if ( wParam & MK_MBUTTON ) button = button | 4;
				if ( wParam & MK_RBUTTON ) button = button | 2;

				// ģ�� �ڱ��������϶� �ƶ�����λ����Ϊ
				if (pPro)
				{
					if (pPro->iTitleBarBehav & button)
					{
						POINT pt;
						GetCursorPos(&pt);
						ReleaseCapture();
						SendMessage (m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, *((LPARAM *)(&pt)) );
					}
				}

			}
			else if ((uMsg-0x200) % 3 == 1)				// ���£�0x201��0x204��0x207��(uMsg-0x200) % 3 == 1
			{
				key = MAKELONG(WM_LBUTTONDOWN, idCtrl);	// ͳһ�� WM_LBUTTONDOWN ������Ϊ3��ö��ֵ
				button = 1 << ((uMsg-0x0200-1)/3); 
			}
			else if ((uMsg-0x200) % 3 == 2)				// ̧��0x202��0x205��0x208��(uMsg-0x200) % 3 == 2
			{
				key = MAKELONG(WM_LBUTTONUP, idCtrl);	// ͳһ�� WM_LBUTTONUP ������Ϊ3��ö��ֵ
				button = 1 << ((uMsg-0x0200-2)/3); 
			}
			else if ((uMsg-0x200) % 3 == 0)				// ˫����0x203��0x206��0x209��(uMsg-0x200) % 3 == 0
			{
				key = MAKELONG(WM_LBUTTONDBLCLK,idCtrl);// ͳһ�� WM_LBUTTONDBLCLK ������Ϊ3��ö��ֵ
				button = 1 << ((uMsg-0x0200-3)/3);
			}
			else
				return gc_APICEventsGenDefautRet;	// �ݴ��������� switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���
			
			if (m_hashEventMessage.IsKeyExist(key))
			{
				ONEventIIII ptrFuncForm4 = NULL;
				ptrFuncForm4 = 
					(ONEventIIII)m_hashEventMessage.Item(key, false);
				if (ptrFuncForm4)			// �� if �ݴ��ж�
				{
					POINT pt; UINT altKey=0;  
					GetCursorPos(&pt);  ScreenToClient(hWnd, &pt);
					if (GetKeyState(VK_MENU)<0) altKey=4;		// Shift=1; Ctrl=2; Alt=4
					
					m_idRaisingEvent = idCtrl;
					(*ptrFuncForm4)(button, 
						altKey | ( ( (wParam & (MK_CONTROL | MK_SHIFT))) /4 ), 
						pt.x, pt.y);
					
				}	// if (ptrFuncForm4)
			}	// if (m_hashEventMessage.IsKeyExist(key)
		}

		return gc_APICEventsGenDefautRet;	// ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

	case WM_CHAR: 
		// ��������ɴ��崦�������¼����ȴ�������ļ����¼�
		if (KeyPreview) if (EventsGenerator(uMsg, wParam, lParam)) return 1; 

		// �����ؼ��ļ����¼�
		if (m_hashEventMessage.IsKeyExist(key))
		{
			ONEventII ptrFuncForm2 = NULL;
			int bCancel = 0;
			ptrFuncForm2 = 
				(ONEventII)m_hashEventMessage.Item(key, false);
			if (ptrFuncForm2)
			{
				m_idRaisingEvent = idCtrl;
				(*ptrFuncForm2)((int)wParam, (int)(&bCancel)); 
				if (bCancel) return 1;
			}
		}

		return gc_APICEventsGenDefautRet;	// ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���
		
	case WM_KEYDOWN: case WM_KEYUP: 		//   shift=1,2,4 �ֱ��ʾͬʱ Shift��Ctrl��Alt ��������
	case WM_SYSKEYDOWN: case WM_SYSKEYUP:
		// ��������ɴ��崦�������¼����ȴ�������ļ����¼�
		if (KeyPreview) EventsGenerator(uMsg, wParam, lParam); 

		// �����ؼ��ļ����¼�
		if (uMsg>=0x0104) key = MAKELONG((uMsg-0x4), idCtrl);	// ��WM_SYS��Ϣ�鲢��WM_��Ϣ
		if (m_hashEventMessage.IsKeyExist(key))
		{
			int iShift = 0;
			int bCancel = 0;
			if (GetKeyState(VK_SHIFT)<0 ) iShift |= 1;
			if (GetKeyState(VK_CONTROL)<0 ) iShift |= 2;
			if (GetKeyState(VK_MENU)<0 ) iShift |= 4;
			
			ONEventIII ptrFuncForm3 = NULL;
			ptrFuncForm3 = 
				(ONEventIII)m_hashEventMessage.Item(key, false);
			if (ptrFuncForm3)
			{
				m_idRaisingEvent = idCtrl;
				(*ptrFuncForm3)((int)wParam, iShift, (int)(&bCancel)); 
				if (bCancel) return 1;
			}
		}

		// ������ MessageLoop �������� IsDialogMessage �Իس��Ĵ���
		//   ʹ�ڰ�ť�ϰ��»س�ʱ���ܴ�����ť�����µ��¼�
		//   �����޲���
		// ����ǰ�ť�ؼ����һس������£���ģ�ⰴť������
		if (wParam == VK_RETURN && uMsg==WM_KEYUP) 
		{
			TCHAR sClassName[255];		
			// ��������ַ��� => m_ClassName
			GetClassName(hWnd, sClassName, sizeof(sClassName)/sizeof(TCHAR)-1);
			if (_tcscmp(sClassName, TEXT("Button"))==0)
				SendMessage(hWnd, BM_CLICK, 0, 0);
		}

		return gc_APICEventsGenDefautRet;	// ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

	case WM_DROPFILES:
		
		if (m_hashEventMessage.IsKeyExist(key))
		{
			ONEventIIII ptrFuncForm4 = NULL;
			
			ptrFuncForm4 = 
				(ONEventIIII)m_hashEventMessage.Item(key, false);
			if (ptrFuncForm4)													// �� if �ݴ��ж�
			{
				// ����϶��ļ����ܸ���
				UINT count = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL, 0);
				if (count==0) return gc_APICEventsGenDefautRet;	// �ݴ��������� switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���
				
				// ���ٱ����ַ���ָ��Ŀռ�
				unsigned long * ptrArr = new unsigned long [count+1]; // ������ [0] �Ŀռ�
				HM.AddPtr(ptrArr);
				HM.ZeroMem(ptrArr, sizeof(unsigned long)*(count+1));
				
				// ���ٸ����ַ����Ŀռ䣬����ø��ļ���
				UINT i=0; UINT sizeEach=0; TCHAR * pstr;
				for (i=0; i<count; i++)
				{
					sizeEach = DragQueryFile((HDROP)wParam, i, NULL, 0);
					pstr = new TCHAR [sizeEach+1];
					HM.AddPtr(pstr);
					HM.ZeroMem(pstr, sizeof(TCHAR)*sizeEach);
					ptrArr[i+1] = (unsigned long)pstr;
					DragQueryFile((HDROP)wParam, i, pstr, sizeEach+1);
				}
				
				// ����Ϸ��ļ�ʱ�������λ��
				POINT ptDrag;
				DragQueryPoint((HDROP)wParam, &ptDrag); 
				
				// �ͷž�� hDrop
				DragFinish((HDROP)wParam);
				
				// �����¼�
				m_idRaisingEvent = idCtrl;
				(*ptrFuncForm4)((int)ptrArr, count, (int)ptDrag.x, (int)ptDrag.y);					
			}
		}
		
		return gc_APICEventsGenDefautRet;	// ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

	case WM_PAINT:
		if ( ms_hashCtrls.IsKeyExist((long)hWnd) )		// ����ÿؼ������������
		{
			CBControl ctrl;
			if (ctrl.SetResID(idCtrl))		// �ң�����ɹ����� idCtrl ����˿ؼ�(idCtrl ������Ч����ʧ��)
			{
				STWndProp * pPro = CBWndBase::PropertyMemCtrl(hWnd, false);
				bool fPicStatic = false;		// �Ƿ����� Static �ؼ���Ҫ��������ʾλͼ�����
				
				if (pPro)
					if (pPro->hBmpDisp) 
						if (ctrl.IsClassName(TEXT("Static"))) fPicStatic=true;
				
				if ( fPicStatic )
				{
					//////////////////////////////////////////////////////////////////////////
					// ���� Static �ؼ���Ҫ��������ʾλͼ�����

					PAINTSTRUCT ps;
					BeginPaint(hWnd, &ps);
					
					// ���� CBWndBase �����е� RefreshPicture ��ʾλͼ���� 
					//   pPro->hBmpDisp Ϊ 0 ���º������Զ�������
					ctrl.RefreshPicture(ps.hdc, 0, 0);

					EndPaint(hWnd, &ps);

					// ������Ĭ�ϴ��ڳ��������¼�������
					if (m_hashEventMessage.IsKeyExist(key))	// �������¼������¼��ӳ����ͼ
					{
						ONEventVoid ptrFunc0 = 
							(ONEventVoid)m_hashEventMessage.Item(key, false);
						if (ptrFunc0) { m_idRaisingEvent = idCtrl; (*ptrFunc0)(); }
					}
					return 0;								// ���ٽ���Ĭ�ϴ��ڳ�����
				}
				else	// else - if ( fPicStatic )
				{
					//////////////////////////////////////////////////////////////////////////
					// ������ Static �ؼ��������� Static �ؼ�������Ҫ��������ʾλͼ�����

					// ����Ĭ�ϴ��ڳ��������Ĭ�ϻ�ͼ����Ȼ�������¼�������
					CallWindowProc ((WNDPROC)(CBWndBase::ms_hashCtrls.Item((long)hWnd,false)), 
						hWnd, uMsg, wParam, lParam);		// ����Ĭ�ϴ��ڳ��������Ĭ�ϻ�ͼ
					if (m_hashEventMessage.IsKeyExist(key))	// �������¼������¼��ӳ����ͼ
					{
						ONEventVoid ptrFunc0 = 
							(ONEventVoid)m_hashEventMessage.Item(key, false);
						if (ptrFunc0) { m_idRaisingEvent = idCtrl; (*ptrFunc0)(); }
					}
					return 0;								// ���ٽ���Ĭ�ϴ��ڳ�����
				}	// end if ( fPicStatic )
			}	// end if (ctrl.SetResID(idCtrl))
		}	// end if ( ms_hashCtrls.IsKeyExist((long)hWnd) )  // ����ÿؼ������������

		// ����ÿؼ�������������������ߣ�δ�ɹ����� idCtrl ����˿ؼ�(idCtrl ������Ч����ʧ��)
		return gc_APICEventsGenDefautRet;	// ������ switch ��������¼���Ĭ�ϴ������� break; �����Ǹ�Ĭ�ϴ���

	// ==================================================================
	case WM_SETCURSOR:
		// ��������꣺���� SetCursor ���õģ�Windows ���Զ���ʾ������Ĺ��
		if (pApp->MousePointerGlobal())
		{
			// ======== ʹ��ȫ������� ms_hCursorGlobal ========
			if (pApp->MousePointerGlobalHCursor())
			{
				SetCursor(pApp->MousePointerGlobalHCursor());
				return 1;	// return 1; ��ʾ����Ĭ�ϴ��ڳ�������
			}
		}
		else
		{
			// �ж��Ƿ�Ϊ���Ӵ��ڿؼ������������
			STWndProp * pPro = CBWndBase::PropertyMemCtrl(hWnd, false); 
			if (pPro)
			{
				if (pPro->cursorIdx)
				{
					// ======== ʹ��Ϊ���Ӵ��ڿؼ����õ������ ========
					if (pPro->hCursor)
					{
						SetCursor(pPro->hCursor);
						return 1;	// return 1; ��ʾ����Ĭ�ϴ��ڳ�������
					}
				}
			}
			// ======== ����ʹ��Ĭ�ϣ��������������꣨Ĭ�ϴ��ڳ����Զ������� ========
		}
		
		break;	// ���� switch ��������¼���Ĭ�ϴ������� return 0; �������Ǹ�Ĭ�ϴ���
	case WM_CONTEXTMENU:
		// �����Ҽ��˵������Ƿ�ȡ�� WM_CONTEXTMENU ��Ϣ��
		{
			// ���� CBWndBase �ľ�̬���� PropertyMemCtrl���Կؼ��ľ����lParam����øÿؼ������Կռ�
			STWndProp * pPro = CBWndBase::PropertyMemCtrl(hWnd, false); 
			if (pPro)
			{			
				if (pPro->fDisableContextMenu) return 0;  // ȡ�� WM_CONTEXTMENU ��Ϣ
			}
		}
		break;	// ���� switch ��������¼���Ĭ�ϴ������� return 0; �������Ǹ�Ĭ�ϴ���
	} // end of switch (uMsg)
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	// �����¼���Ĭ�ϴ���
	if (m_hashEventMessage.IsKeyExist(key))
	{
		ONEventVoid ptrFunc0 = NULL; 
		ONEventI ptrFunc1 = NULL;
		ONEventII ptrFunc2 = NULL;
		ONEventIII ptrFunc3 = NULL;
		ONEventIIII ptrFunc4 = NULL;

		m_idRaisingEvent = idCtrl;

		switch(m_hashEventMessage.ItemLong(key, false))
		{
		case 0:		// 0 ������
			ptrFunc0 =
				(ONEventVoid)m_hashEventMessage.Item(key, false);
			if (ptrFunc0) (*ptrFunc0)();								// �� if �ݴ��ж�
			break;
		case 1:		// 1 �� int �Ͳ���
			ptrFunc1 = 
				(ONEventI)m_hashEventMessage.Item(key, false);
			if (ptrFunc1) (*ptrFunc1)((int)wParam);						// �� if �ݴ��ж�
			break;
		case 2:		// 2 �� int �Ͳ���
			ptrFunc2 = 
				(ONEventII)m_hashEventMessage.Item(key, false);
			if (ptrFunc2) (*ptrFunc2)((int)wParam, (int)lParam);		// �� if �ݴ��ж�
			break;
		case 3:		// 3 �� int �Ͳ���
			ptrFunc3 = 
				(ONEventIII)m_hashEventMessage.Item(key, false);
			if (ptrFunc3) (*ptrFunc3)((int)LOWORD(wParam), (int)HIWORD(wParam), 
				lParam);												// �� if �ݴ��ж�
			break;
		case 4:		// 4 �� int �Ͳ���
			ptrFunc4 = 
				(ONEventIIII)m_hashEventMessage.Item(key, false);
			if (ptrFunc4) (*ptrFunc4)((int)LOWORD(wParam), (int)HIWORD(wParam), 
				(int)LOWORD(lParam), (int)HIWORD(lParam));				// �� if �ݴ��ж�
			break;
			
		}	// end of switch(m_hashEventMessage.ItemLong(key, false))
	}	// end of if (m_hashEventMessage.IsKeyExist(key)

	return gc_APICEventsGenDefautRet;
}

bool CBForm::SysTrayAdd( LPCTSTR szTooltip, unsigned short idIconRes/*=0*/, UINT idSysTray /*= 0*/ )
{
	// m_NIData.cbSize !=0 ��ʾ�Ѿ�������ϵͳ����
	// һ������ֻ�ܱ�����һ��ϵͳ����ͼ�꣬��������ã��򷵻�
	if (m_NIData.cbSize) return false;

	// ����ͼ��
	memset(&m_NIData, 0, sizeof(m_NIData));			// �ṹ�����г�Ա��0
	m_NIData.cbSize = sizeof(NOTIFYICONDATA);		// �ṹ�峤�ȣ�!=0 ��ʾ����ϵͳ����
	m_NIData.hWnd = m_hWnd;
	m_NIData.uCallbackMessage = WM_USER_NOTIFYICON;	// Ҫ��������Ϣ
	_tcscpy(m_NIData.szTip, szTooltip);				// ��ʾ��Ϣ�ı�
	m_NIData.uID = idSysTray;						// ���������涨�� ID ֵ

	// ����ͼ�� => m_NIData.hIcon
	if (idIconRes)
	{
		// ʹ����Դ ID Ϊ idIconRes ��ͼ��
		m_NIData.hIcon = LoadIcon(pApp->hInstance, MAKEINTRESOURCE(idIconRes));
	}
	else
	{
		// ʹ�ñ������ͼ��
		m_NIData.hIcon = (HICON)SendMessage(m_hWnd, WM_GETICON, ICON_SMALL, 0);
		// �����������ͼ�꣬ʹ�� IDI_APPLICATION
		if (m_NIData.hIcon == NULL) m_NIData.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	}

	// ���� hIcon��uCallbackMessage��szTip ��Ա��Ч
	m_NIData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

	// ����API��������ϵͳ���̣��� Shell_NotifyIcon ���ط�0��ʾ�ɹ��������ʾʧ��
	return (Shell_NotifyIcon(NIM_ADD, &m_NIData) != 0);
}

void CBForm::SysTrayRemove()
{
	if(m_NIData.cbSize)
	{
		Shell_NotifyIcon(NIM_DELETE, &m_NIData);
		m_NIData.cbSize=0;	// �˳�Ա ==0 ��־��ϵͳ����
	}
}

bool CBForm::SysTraySetIcon( unsigned short idIconRes/*=0*/ )
{
	if( ! m_NIData.cbSize )		// û�д�������ͼ��ʱ�����Զ�����
		if ( ! SysTrayAdd(TEXT(""))) return false;	// ����ʧ��

	// ����ͼ�� => m_NIData.hIcon
	if (idIconRes)
	{
		// ʹ����Դ ID Ϊ idIconRes ��ͼ��
		m_NIData.hIcon = LoadIcon(pApp->hInstance, MAKEINTRESOURCE(idIconRes));
	}
	else
	{
		// ʹ�ñ������ͼ��
		m_NIData.hIcon = (HICON)SendMessage(m_hWnd, WM_GETICON, ICON_SMALL, 0);
		// �����������ͼ�꣬ʹ�� IDI_APPLICATION
		if (m_NIData.hIcon == NULL) m_NIData.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	}

	// ���� hIcon ��Ա��Ч
	m_NIData.uFlags = NIF_ICON;

	// ����API��������ϵͳ���̣��� Shell_NotifyIcon ���ط�0��ʾ�ɹ��������ʾʧ��
	return (Shell_NotifyIcon(NIM_MODIFY, &m_NIData) != 0);
}

LPTSTR CBForm::SysTrayToolTip()
{
	// ����ǰ�����̣����� ""�������� m_NIData.szTip Ϊ ""���󷵻أ�
	if( ! m_NIData.cbSize ) *m_NIData.szTip = 0;
	return m_NIData.szTip;
}

bool CBForm::SysTrayToolTipSet( tstring sTooltip )
{
	return SysTrayToolTipSet(sTooltip.c_str());
}

bool CBForm::SysTrayToolTipSet( LPCTSTR szTooltip )
{
	if( ! m_NIData.cbSize )		// û�д�������ͼ��ʱ�����Զ�����
		return SysTrayAdd(szTooltip);
	
	memset(m_NIData.szTip, 0, sizeof(m_NIData.szTip));
	_tcsncpy(m_NIData.szTip, szTooltip, 127);	// ���ı�̫����ֻ����ǰ127���ַ�
	
	// ���� szTip ��Ա��Ч
	m_NIData.uFlags = NIF_TIP;

	// ����API��������ϵͳ���̣��� Shell_NotifyIcon ���ط�0��ʾ�ɹ��������ʾʧ��
	return (Shell_NotifyIcon(NIM_MODIFY, &m_NIData) != 0);
}

BOOL CBForm::SysTrayPopupMenu( UINT idResMenu, int x/*=-1*/, int y/*=-1*/, bool bAllowRightClick/*=true*/ )
{
	if (x<0 || y<0)
	{
		// ʹ�����ָ�뵱ǰλ��
		POINT pt;
		GetCursorPos(&pt);
		if (x<0) x = pt.x;
		if (y<0) y = pt.y;
	}
		
	HMENU hMenuPop = LoadMenu(pApp->hInstance, MAKEINTRESOURCE(idResMenu));
	UINT flags = TPM_LEFTALIGN | TPM_BOTTOMALIGN;
	if (bAllowRightClick) flags = flags | TPM_RIGHTBUTTON;

	SetForegroundWindow(m_hWnd);
	// TrackPopupMenu ֻ�ܵ��� popup ʽ�˵���LoadMenu �õ��Ĳ��� popup ʽ�˵���
	//   Ӧ�� GetSubMenu ȡ���Ӳ˵�������ֻȡ��0����Ӳ˵���
	BOOL ret = TrackPopupMenu(GetSubMenu(hMenuPop, 0), flags, x, y, 0, m_hWnd, NULL);

	//�н��л�
	DestroyMenu(hMenuPop);
	return ret;
}











