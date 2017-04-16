
// MFCChatroomDlg.h : header file
//

#pragma once


// CMFCChatroomDlg dialog
class CMFCChatroomDlg : public CDialogEx
{
// Construction
public:
	CMFCChatroomDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCCHATROOM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton2();
	CString edtLocalPort;
	CString edtServerIP;
	afx_msg void OnBnClickedButton3();
	CString edtUser;
	CString edtMsg;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CString edtText;
};