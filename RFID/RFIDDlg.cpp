
// RFIDDlg.cpp: 구현 파일
#include "is_d2xx.h"
#include "pch.h"
#include "framework.h"
#include "RFID.h"
#include "RFIDDlg.h"
#include "afxdialogex.h"

// sound 출력용
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include "mmsystem.h"
#pragma comment(lib,"winmm.lib") // 라이브러리 불러오기

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CRFIDDlg 대화 상자
CRFIDDlg::CRFIDDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RFID_DIALOG, pParent)
	, m_strRfid(_T(""))
	, m_strUID(_T(""))
	, m_strSinger(_T(""))
	, m_strName(_T(""))
	, m_intStock(0)
	, m_strPrice(0)
	, m_picAddr(_T(""))
	, m_soundAddr(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
CRFIDDlg::~CRFIDDlg()
{
	//무선파워를 해제
	is_RfOff(ftHandle);
	// USB 포트를 Close
	if (is_Close(ftHandle) == IS_OK)
	{
		printf("연결을 닫습니다. ");
	}
}

void CRFIDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strRfid);
	DDX_Text(pDX, IDC_UID_EDIT, m_strUID);
	DDX_Text(pDX, IDC_SINGER_EDIT, m_strSinger);
	DDX_Text(pDX, IDC_NAME_EDIT, m_strName);
	DDX_Text(pDX, IDC_STOCK_EDIT, m_intStock);
	DDX_Text(pDX, IDC_PRICE_EDIT, m_strPrice);
	DDX_Control(pDX, IDC_PICTURE, m_picture);
}

BEGIN_MESSAGE_MAP(CRFIDDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_READ_BTN, &CRFIDDlg::OnClickedReadBtn)
	ON_BN_CLICKED(IDC_CHECK_BTN, &CRFIDDlg::OnClickedCheckBtn)
	ON_BN_CLICKED(IDC_CHECKOUT_BTN, &CRFIDDlg::OnClickedCheckoutBtn)
	ON_BN_CLICKED(IDC_RETURN_BTN, &CRFIDDlg::OnClickedReturnBtn)
	ON_BN_CLICKED(IDC_REGISTER_BTN, &CRFIDDlg::OnClickedRegisterBtn)
	ON_BN_CLICKED(IDC_DELETE_BTN, &CRFIDDlg::OnClickedDeleteBtn)
	ON_BN_CLICKED(IDC_PIC_BTN, &CRFIDDlg::OnClickedPicBtn)
	ON_BN_CLICKED(IDC_SOUND_BTN, &CRFIDDlg::OnClickedSoundBtn)
END_MESSAGE_MAP()


// CRFIDDlg 메시지 처리기
BOOL CRFIDDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.
	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	//열린 포트번호 찾기
	if (is_GetDeviceNumber(&usbnumber) == IS_OK)
	{
		printf("FTDI USB To Serial 연결된 개수 : %d\n", usbnumber);
		if (is_GetSerialNumber(0, readSerialNumber) == IS_OK)
		{
			if (memcmp(changeSerialNumber, readSerialNumber, sizeof(changeSerialNumber) != 0))
			{
				if (is_SetSerialNumber(0, changeSerialNumber) == IS_OK)
				{
					printf(" USB To Serial Number 를 변경 하였습니다.\n");
					printf(" FTDI SerialNumber :  %s \n", changeSerialNumber);
				}
			}
			else
				printf(" FTDI SerialNumber :  %s \n", changeSerialNumber);
		}
	}

	//열린 포트번호로 연결
	unsigned long portNumber;
	if (is_GetCOMPort_NoConnect(0, &portNumber) == IS_OK)
	{
		printf("COM Port : %d\n", portNumber);
	}
	if (is_OpenSerialNumber(&ftHandle, readSerialNumber, 115200) != IS_OK)
	{
		printf("USB To Serial과 통신 연결 실패\n");
	}
	else {
		printf("Serial포트 %d와 통신 연결성공!! \n", portNumber);
	}
	Sleep(100);
	flag_r = 0;

	// DB 연결
	MYSQL* conn;
	MYSQL* conn_result;
	unsigned int timeout_sec = 1;

	conn = mysql_init(NULL);
	mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);
	conn_result = mysql_real_connect(conn, "127.0.0.1", "root", "1234", "RecordDB", 3306, NULL, 0);

	if (conn_result == NULL)
	{
		printf("DB Connection Fail\n");
	}
	else
	{
		printf("DB Connection Success\n");
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CRFIDDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CRFIDDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CRFIDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRFIDDlg::OnClickedReadBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	PlaySound(NULL, NULL, SND_ASYNC);
	CString temp, temp1 = _T("");
	//ISO15693모드로 읽기( 싱글모드 읽기 )
	if (is_WriteReadCommand(ftHandle, CM1_ISO15693, CM2_ISO15693_ACTIVE// + BUZZER_ON,
		, writeLength, wirteData, &readLength, readData) == IS_OK)
	{
		int i;
		printf("ISO 15693 UID : ");
		for (i = 0; i < readLength; i++)
		{
			temp.Format(_T("%02x "), readData[i]);
			temp1 += temp;
		}
		m_strUID = temp1;
		UpdateData(FALSE);

		printf("\n");
	}

	// DB 연결
	MYSQL* conn;
	MYSQL* conn_result;
	unsigned int timeout_sec = 1;

	conn = mysql_init(NULL);
	mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);
	conn_result = mysql_real_connect(conn, "127.0.0.1", "root", "1234", "RecordDB", 3306, NULL, 0);

	if (conn_result == NULL)
	{
		printf("DB Connection Fail\n");
	}
	else
	{
		printf("DB Connection Success\n");
	}
	// 쿼리 생성
	char query[1024];
	MYSQL_RES* result;
	MYSQL_ROW row;

	sprintf_s(query, 1024, "SELECT * FROM recordTBL WHERE recordID = '%S'", temp1);
	
	if (mysql_query(conn, query))
	{
		printf("PICTURE LOAD FAIL\n");
		
	}
	else
	{
		// 사진 가져오기
		printf("PICTURE LOAD SUCCESS\n");
		result = mysql_store_result(conn);
		int fields = mysql_num_fields(result);
		CString pic_addr, sound_addr;
		row = mysql_fetch_row(result);
		CRect rect;
		m_picture.GetWindowRect(rect);
		CDC* dc;
		dc = m_picture.GetDC();
		CImage image;
		if (row == NULL)
		{
			image.Load(L"img\\unregisterd_card.bmp");
			image.StretchBlt(dc->m_hDC, 0, 0, rect.Width(), rect.Height(), SRCCOPY);
			ReleaseDC(dc);
		}
		else if ((pic_addr = row[5]) == "")
		{
			image.Load(L"img\\require_pic.bmp");
			image.StretchBlt(dc->m_hDC, 0, 0, rect.Width(), rect.Height(), SRCCOPY);
			ReleaseDC(dc);
		}
		else
		{
			image.Load(pic_addr);
			image.StretchBlt(dc->m_hDC, 0, 0, rect.Width(), rect.Height(), SRCCOPY);
			ReleaseDC(dc);

			// 노래 재생
			sound_addr = row[6];
			PlaySoundW(sound_addr, NULL, SND_FILENAME | SND_ASYNC);
		}
	}
	
}


void CRFIDDlg::OnClickedCheckBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// DB 연결
	MYSQL* conn;
	MYSQL* conn_result;
	unsigned int timeout_sec = 1;

	conn = mysql_init(NULL);
	mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);
	conn_result = mysql_real_connect(conn, "127.0.0.1", "root", "1234", "RecordDB", 3306, NULL, 0);

	if (conn_result == NULL)
	{
		printf("DB Connection Fail\n");
	}
	else
	{
		printf("DB Connection Success\n");
	}

	// 쿼리 생성
	char query[1024];
	MYSQL_RES* result;
	MYSQL_ROW row;

	sprintf_s(query, 1024, "SELECT * FROM recordTBL WHERE recordID = '%S'", m_strUID);

	if (mysql_query(conn, query))
	{
		printf("CHECK FAIL\n");
	}
	else
	{
		printf("CHECK SUCCESS\n");
		result = mysql_store_result(conn);
		int fields = mysql_num_fields(result);
		row = mysql_fetch_row(result);
		if (row == NULL)
		{
			AfxMessageBox(L"등록되지 않은 카드입니다.");
		}
		else
		{
			CString str_check;

			str_check += row[1];
			str_check += L",  ";
			str_check += row[2];
			str_check += L",  ";
			str_check += L"재고 : ";
			str_check += row[3];
			str_check += L",  ";
			str_check += L"가격 : ";
			str_check += row[4];

			MessageBox(str_check);
		}
	}

}


void CRFIDDlg::OnClickedCheckoutBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// DB 연결
	MYSQL* conn;
	MYSQL* conn_result;
	unsigned int timeout_sec = 1;

	conn = mysql_init(NULL);
	mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);
	conn_result = mysql_real_connect(conn, "127.0.0.1", "root", "1234", "RecordDB", 3306, NULL, 0);

	if (conn_result == NULL)
	{
		printf("DB Connection Fail\n");
	}
	else
	{
		printf("DB Connection Success\n");
	}

	// 쿼리 생성
	char query[1024];
	MYSQL_RES* result;
	MYSQL_ROW row;

	sprintf_s(query, 1024, "UPDATE recordtbl SET recordStock= recordStock - 1 WHERE recordID = '%S'", m_strUID);

	if (mysql_query(conn, query))
	{
		printf("CHECKOUT FAIL\n");
	}
	else
	{
		printf("CHECKOUT SUCCESS\n");
		MessageBox(L"대출");
	}


}


void CRFIDDlg::OnClickedReturnBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

		// DB 연결
	MYSQL* conn;
	MYSQL* conn_result;
	unsigned int timeout_sec = 1;

	conn = mysql_init(NULL);
	mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);
	conn_result = mysql_real_connect(conn, "127.0.0.1", "root", "1234", "RecordDB", 3306, NULL, 0);

	if (conn_result == NULL)
	{
		printf("DB Connection Fail\n");
	}
	else
	{
		printf("DB Connection Success\n");
	}

	// 쿼리 생성
	char query[1024];
	MYSQL_RES* result;
	MYSQL_ROW row;

	sprintf_s(query, 1024, "UPDATE recordtbl SET recordStock= recordStock + 1 WHERE recordID = '%S'", m_strUID);

	if (mysql_query(conn, query))
	{
		printf("RETURN FAIL\n");
	}
	else
	{
		printf("RETURN SUCCESS\n");
		MessageBox(L"반납");
	}
}


void CRFIDDlg::OnClickedRegisterBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// DB 연결
	MYSQL* conn;
	MYSQL* conn_result;
	unsigned int timeout_sec = 1;

	conn = mysql_init(NULL);
	mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);
	conn_result = mysql_real_connect(conn, "127.0.0.1", "root", "1234", "RecordDB", 3306, NULL, 0);

	if (conn_result == NULL)
	{
		printf("DB Connection Fail\n");
	}
	else
	{
		printf("DB Connection Success\n");
	}

	// 쿼리 생성
	char query[1024];
	MYSQL_RES* result;

	CString uid, singer, name, image, sound;

	GetDlgItemText(IDC_UID_EDIT, uid);
	GetDlgItemText(IDC_SINGER_EDIT, singer);
	GetDlgItemText(IDC_NAME_EDIT, name);
	int stock = GetDlgItemInt(IDC_STOCK_EDIT);
	int price = GetDlgItemInt(IDC_PRICE_EDIT);
	image = m_picAddr;
	sound = m_soundAddr;
	
	sprintf_s(query, 1024, "INSERT INTO recordTBL VALUES ('%S', '%S', '%S', '%d', '%d', '%S', '%S')", uid, singer, name, stock, price, image, sound);

	if (mysql_query(conn, query))
	{
		printf("INSERT FAIL\n");
	}
	else
	{
		printf("INSERT SUCCESS\n");
	}
	SetDlgItemText(IDC_SINGER_EDIT, L"");
	SetDlgItemText(IDC_NAME_EDIT, L"");
	SetDlgItemText(IDC_STOCK_EDIT, L"");
	SetDlgItemText(IDC_PRICE_EDIT, L"");
	m_picAddr = "";
	m_soundAddr = "";

}


void CRFIDDlg::OnClickedDeleteBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// DB 연결
	MYSQL* conn;
	MYSQL* conn_result;
	unsigned int timeout_sec = 1;

	conn = mysql_init(NULL);
	mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);
	conn_result = mysql_real_connect(conn, "127.0.0.1", "root", "1234", "RecordDB", 3306, NULL, 0);

	if (conn_result == NULL)
	{
		printf("DB Connection Fail\n");
	}
	else
	{
		printf("DB Connection Success\n");
	}

	// 쿼리 생성
	char query[1024];
	MYSQL_RES* result;

	CString uid;
	GetDlgItemText(IDC_UID_EDIT, uid);

	sprintf_s(query, 1024, "DELETE FROM recordTBL WHERE recordID = '%S'", uid);

	if (mysql_query(conn, query))
	{
		printf("DELETE FAIL\n");
	}
	else
	{
		printf("DELETE SUCCESS\n");
	}
}


void CRFIDDlg::OnClickedPicBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TCHAR szFile[] = _T("BMP 파일(*.bmp)|*.bmp||");

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFile);

	if (IDOK == dlg.DoModal())
	{
		CString pathName = dlg.GetPathName();
		CString temp;
		temp = pathName.Right(pathName.GetLength() - pathName.ReverseFind('\\') - 1);
		CString storePath = L"D:\\project\\C++\\RFID\\RFID\\img\\";
		storePath += temp;
		if (CopyFile(pathName, storePath, FALSE))
		{
			printf("PICTURE COPY SUCCESS\n");
			m_picAddr += L"img/";
			m_picAddr += temp;
		}
		else
		{
			printf("PICTURE COPY FAIL\n");
		}
	}
}


void CRFIDDlg::OnClickedSoundBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TCHAR szFile[] = _T("WAV 파일(*.wav)|*.wav||");

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFile);

	if (IDOK == dlg.DoModal())
	{
		CString pathName = dlg.GetPathName();
		CString temp;
		temp = pathName.Right(pathName.GetLength() - pathName.ReverseFind('\\') - 1);
		CString storePath = L"D:\\project\\C++\\RFID\\RFID\\wav\\";
		storePath += temp;
		if (CopyFile(pathName, storePath, FALSE))
		{
			printf("WAV COPY SUCCESS\n");
			m_soundAddr += L"wav/";
			m_soundAddr += temp;
		}
		else
		{
			printf("WAV COPY FAIL\n");
		}		
	}
}

