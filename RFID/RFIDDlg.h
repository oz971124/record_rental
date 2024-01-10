
// RFIDDlg.h: 헤더 파일
//

#pragma once
#include "is_d2xx.h"

// CRFIDDlg 대화 상자
class CRFIDDlg : public CDialogEx
{
// 생성입니다.
public:
	CRFIDDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	~CRFIDDlg();
	IS_HANDLE ftHandle = 0;
	char readSerialNumber[100] = "COM07";
	char changeSerialNumber[100] = "RFID01";
	short usbnumber;
	unsigned char wirteData[1024];
	unsigned short writeLength = 0;
	unsigned char readData[1024];
	unsigned short readLength = 0;
	BOOL flag_r=0;
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RFID_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strRfid;
	CString m_strUID;
	CString m_strSinger;
	CString m_strName;
	int m_intStock;
	int m_strPrice;
	afx_msg void OnClickedReadBtn();
	afx_msg void OnClickedCheckBtn();
	afx_msg void OnClickedCheckoutBtn();
	afx_msg void OnClickedReturnBtn();
	afx_msg void OnClickedRegisterBtn();
	CStatic m_picture;
	afx_msg void OnClickedDeleteBtn();
	afx_msg void OnClickedPicBtn();

	CString m_picAddr;
	afx_msg void OnClickedSoundBtn();
	CString m_soundAddr;
};
