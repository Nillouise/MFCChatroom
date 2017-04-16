
// MFCChatroomDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCChatroom.h"
#include "MFCChatroomDlg.h"
#include "afxdialogex.h"
#include"NetControl.h"
#include<string>
#include<sstream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCChatroomDlg dialog



CMFCChatroomDlg::CMFCChatroomDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCCHATROOM_DIALOG, pParent)
	, edtLocalPort(_T(""))
	, edtServerIP(_T(""))
	, edtUser(_T(""))
	, edtMsg(_T(""))
	, edtText(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCChatroomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, edtLocalPort);
	DDX_Text(pDX, IDC_EDIT4, edtServerIP);
	DDX_Text(pDX, IDC_EDIT6, edtUser);
	DDX_Text(pDX, edt_input, edtMsg);
	DDX_Text(pDX, edt_Output, edtText);
}

BEGIN_MESSAGE_MAP(CMFCChatroomDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON4, &CMFCChatroomDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCChatroomDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCChatroomDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCChatroomDlg::OnBnClickedButton1)
	ON_WM_TIMER()
END_MESSAGE_MAP()


//void CALLBACK CMFCChatroomDlg::TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
//{
////	MessageBox(NULL, TEXT("你是好人吗？"), TEXT("CHOOSE"), MB_YESNO | MB_ICONQUESTION);
//	CString s = RecvMsgFromClients();
//	if (s[s.GetLength() - 1] != '\n'&&s.GetLength()>0)s += '\n';
//	RecvMsgFromServer();
//	if (s[s.GetLength() - 1] != '\n'&&s.GetLength()>0)s += '\n';
//
//
//}

// CMFCChatroomDlg message handlers

BOOL CMFCChatroomDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	initChatRoomSocket();//初始化socket 包和其他的东西
	SetTimer(1, 200, NULL);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCChatroomDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCChatroomDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCChatroomDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCChatroomDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString IP = edtServerIP.Left(edtServerIP.Find(_T(":")));
	std::string sport = WChar2Ansi( edtServerIP.Right(edtServerIP.GetLength()-1- edtServerIP.Find(_T(":"))).GetBuffer(0));
	int port;
	std::stringstream ss;
	ss << sport;
	ss >> port;

	if (!setServerIP(IP, port))
	{
		MessageBox(_T("设置服务器不成功"));
	}

}


void CMFCChatroomDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int port;
	std::string s= WChar2Ansi(edtLocalPort.GetBuffer(0));
	std::stringstream ss;
	ss << s;
	ss >> port;

	if (!setLocalMachineToServer(port))
	{
		MessageBox(_T("绑定端口不成功"));
	}

}


void CMFCChatroomDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here


}


void CMFCChatroomDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString s = edtUser + ": " + edtMsg+"\r\n";
	if (SendMsgToClients(s))
	{
		UpdateData(TRUE);
		edtText += s;
		UpdateData(FALSE);
	}
	SendMsgToServer(s);
}


void CMFCChatroomDlg::OnTimer(UINT_PTR nIDEvent)
{
	//TODO: Add your message handler code here and/or call default
	UpdateData(TRUE);
	CString s = RecvMsgFromClients();
	if (s.GetLength() > 0)
	{
		SendMsgToClients(s);
	}
	if (s.GetLength() > 0&&s[s.GetLength() - 1] != _T('\n'))
		s += _T("\r\n");
	edtText += s;
	s = RecvMsgFromServer();
	if (s.GetLength()>0&&s[s.GetLength() -1] != _T('\n'))
		s += _T("\r\n");
	edtText += s;
	
	UpdateData(FALSE);

//	RecvMsgFromClients();

	CDialogEx::OnTimer(nIDEvent);
}
