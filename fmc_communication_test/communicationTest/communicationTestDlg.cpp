
// joyAdmissionTestDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "joyAdmissionTest.h"
#include "joyAdmissionTestDlg.h"
#include "afxdialogex.h"
#include "config_com.h"
#include "xsleep.h"

#include <setupapi.h>
#pragma comment(lib, "Setupapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const uint16_t   mIDC_STATIC_XYYS[4] =
{
	IDC_STATIC1 ,IDC_STATIC2 ,IDC_STATIC3 ,IDC_STATIC4 
};

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CjoyAdmissionTestDlg 对话框

CjoyAdmissionTestDlg::CjoyAdmissionTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_JOYADMISSIONTEST_DIALOG, pParent)
	, m_strShow(_T(""))
	, m_UartOk(FALSE)
	, m_Stop(FALSE)	
	, nStart_test(0)
	, isO_testOK(eT_INIT)
	, isX_testOK(eT_INIT)
	, isY_testOK(eT_INIT)
	, isZ_testOK(eT_INIT)
	, isB_testOK(eT_INIT)
	, m_strXYShow(_T(""))
{
	
	m_arryBmpLed[0] = &m_nIDC_LedCalib;
	m_arryBmpLed[1] = &m_nIDC_LedXTest;
	m_arryBmpLed[2] = &m_nIDC_LedYTest;
	m_arryBmpLed[3] = &m_nIDC_LedZTest;
	m_arryBmpLed[4] = &m_nIDC_LedBTest;

	memset(bmpLast_led, GIF_LED_OFF, sizeof(bmpLast_led));
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CjoyAdmissionTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSCOMM1, m_ctrComm);//DISPID(),
	DDX_Text(pDX, IDC_Recv, m_strShow);
	DDX_Text(pDX, IDC_EDIT_XY_SHOW, m_strXYShow);
	DDX_Control(pDX, IDC_COMBO1, m_comboCom);
	DDX_Control(pDX, IDC_STATIC_CALIB, m_nIDC_LedCalib);
	DDX_Control(pDX, IDC_STATIC_X_TEST, m_nIDC_LedXTest);
	DDX_Control(pDX, IDC_STATIC_Y_TEST, m_nIDC_LedYTest);
	DDX_Control(pDX, IDC_STATIC_B_TEST, m_nIDC_LedBTest);
	DDX_Control(pDX, IDC_STATIC_Z_TEST, m_nIDC_LedZTest);
}

BEGIN_MESSAGE_MAP(CjoyAdmissionTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CjoyAdmissionTestDlg::OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUT_CALIB,  &CjoyAdmissionTestDlg::OnBnClickedButCalib)
	ON_BN_CLICKED(IDC_BUN_Y_TEST, &CjoyAdmissionTestDlg::OnBnClicked_startTest)
	ON_BN_CLICKED(IDC_Uart_Close, &CjoyAdmissionTestDlg::OnBnClickedUartClose)
	ON_BN_CLICKED(IDC_BUTTON1, &CjoyAdmissionTestDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CjoyAdmissionTestDlg 消息处理程序

BOOL CjoyAdmissionTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	GetCom();
	OnBnClickedUartClose();


	GetDlgItem(IDC_STATIC_X_TEST)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
	GetDlgItem(IDC_STATIC_Y_TEST)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
	GetDlgItem(IDC_STATIC_CALIB)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
	GetDlgItem(IDC_STATIC_Z_TEST)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
	GetDlgItem(IDC_STATIC_B_TEST)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
	GetDlgItem(IDC_BUT_CALIB)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW

	//GetDlgItem(IDC_Uart_Close)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
	
    CFont cfont;
    cfont.CreatePointFont(220, _T("华文行楷"), NULL);
    GetDlgItem(IDC_STATIC_SHOW)->SetFont(&cfont);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CjoyAdmissionTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CjoyAdmissionTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CjoyAdmissionTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BEGIN_EVENTSINK_MAP(CjoyAdmissionTestDlg, CDialogEx)
	ON_EVENT(CjoyAdmissionTestDlg, IDC_MSCOMM1, 1, CjoyAdmissionTestDlg::OnCommUart, VTS_NONE)
END_EVENTSINK_MAP()


unsigned  char CjoyAdmissionTestDlg::checksum(unsigned  char *p, unsigned  int len)
{
	unsigned  char rt = 0;
	unsigned  int i = 0;
	for (i = 0; i < len; i++)
	{
		rt = (p[i] + rt) & 0xff;
	}
	return rt;
}

CString CjoyAdmissionTestDlg::getCurSysTime(void)
{
	CString str_time; //获取系统时间 　　
	CTime tm;
	tm = CTime::GetCurrentTime();
	str_time = tm.Format("%Y年%m月%d日 %X :");
	return str_time;
}


//解析xyx state显示
void CjoyAdmissionTestDlg::xytsShowValue(uint8_t num, int32_t value, CString decs)
{
	if (GetDlgItem(mIDC_STATIC_XYYS[num]))
	{
		CFont font;//声明在头文件中 不再使用指针9
		CString strJoy(decs);
		CString str;
		font.CreatePointFont(110, _T("仿宋"));
		GetDlgItem(mIDC_STATIC_XYYS[num])->SetFont(&font);
		str.Format(_T(" \n%d"), value);
		strJoy += str;
		GetDlgItem(mIDC_STATIC_XYYS[num])->SetWindowTextW(strJoy);
	}

}

void CjoyAdmissionTestDlg::parseJoyData(pJoyBuf pJdata)
{

	JOY_BUF_TypeDef sJoy;
	uint16_t jx = 0, jy = 0, jz = 0, btn = 0;
	float   temp = 0;

	jx = (pJdata->buff[1] << 8 | pJdata->buff[2]);
	jy = (pJdata->buff[3] << 8 | pJdata->buff[4]);
	jz = (pJdata->buff[5] << 8 | pJdata->buff[6]);
	btn = pJdata->buff[7];

	temp = ((jx - 0x20) * 200 / HALL_DATA_RANGE) - 100;
	//霍尔摇杆x轴信号与原电位器摇杆数据相反，此处将数据反向
	sJoy.sCmd.dataX.sV = -temp;
	temp = ((jy - 0x20) * 200 / HALL_DATA_RANGE) - 100;
	sJoy.sCmd.dataY.sV = temp;
	temp = ((jz - 0x20) * 200 / HALL_DATA_RANGE) - 100;
	sJoy.sCmd.dataZ.sV = temp;
	sJoy.sCmd.button = (btn >> 5);


    //实时显示
	xytsShowValue(0, sJoy.sCmd.dataX.sV, _T("X轴："));
	xytsShowValue(1, sJoy.sCmd.dataY.sV, _T("Y轴："));
	xytsShowValue(2, sJoy.sCmd.dataZ.sV, _T("Z轴："));
	xytsShowValue(3, sJoy.sCmd.button, _T("按钮 松开[0] 按下[1]:"));


    //中心原点测试
    hall_origin_test(sJoy.sCmd.dataX.sV,sJoy.sCmd.dataY.sV,sJoy.sCmd.dataZ.sV);
	uart_x_test(sJoy.sCmd.dataX.sV);
	uart_y_test(sJoy.sCmd.dataY.sV);
    uart_z_test(sJoy.sCmd.dataZ.sV);
    uart_b_test(sJoy.sCmd.button);
}

//数据帧接收解析
//FF XXH	XXL		YYH		YYL		ZZH		ZZL			Button	CH
//头 X 高位 X 低位	Y 高位	Y 低位  Z 高位 Z 低位		按钮	校验和
void CjoyAdmissionTestDlg::parseFrame(pJoyBuf pJoy, int len)
{

	int rev_len = 0, crc_len = 0;
	unsigned  char revSum;
	//接收到帧长度
	rev_len = len;
	//计算sum
	if (checksum(&pJoy->buff[1], (len - 2)) == pJoy->sCmd.sum)
	{
		parseJoyData(pJoy);
	}

}
void CjoyAdmissionTestDlg::printfBuf(uint8_t *buf, int len)
{
    if(!m_Stop)
    {
    	CString tmStr;
        CString tmhex;

        CString tmInt1;
        
    	for (int i = 0; i < len; i++)
    	{
			if (i == 6 || i == 7)
			{
				tmhex.Format(_T("%02x "), buf[i]);
				tmStr += tmhex;
			}

    	}

        u16 m16;
		u16 m116;
        u16 m_16;
		u16 m1_16;

		u16 m57_16;
		u16 mfe_16;

		m57_16.B[0] = 0x57;
		m57_16.B[1] = 0xfe;

		mfe_16.B[1] = 0x57;
		mfe_16.B[0] = 0xfe;

        m16.B[0] = buf[6];
        m16.B[1] = buf[7];

		m116.B[1] = buf[6];
		m116.B[0] = buf[7];

        m_16.B[1] = buf[6];
        m_16.B[0] = buf[7];

		m1_16.B[0] = buf[6];
		m1_16.B[1] = buf[7];




		//tmInt1.Format(_T(" %d  %d  %d  %d  %d  %d  差值 v57= %d  v57= %d 差值 vfe= %d  vfe= %d"), buf[6], buf[7], m16.u16_v, m116.u16_v, m_16.s16_v, m1_16.s16_v
		//	, (m_16.s16_v - m57_16.s16_v), (m_16.s16_v - mfe_16.s16_v), (m116.s16_v - m57_16.s16_v), (m116.s16_v - mfe_16.s16_v));
            
		tmInt1.Format(_T("  %d"),( m57_16.s16_v - m16.s16_v));

        m_strShow   += tmStr + tmInt1;
        m_strShow += _T("\r\n\n");
        UpdateData(FALSE);
    }
}

//解析具体数据字段 
void CjoyAdmissionTestDlg::testParseFrame(uint8_t *buf, int len)
{

    u16 sum,rev_sum;
    uint16_t sum_len = 0;
	int rev_len = 0;
    
    uint8_t  tmpBuff[TEST_MAX_SIZE] ={0};
	unsigned  char revSum;
    
	//接收到帧长度
	rev_len = len;
    memcpy((char *)tmpBuff, (char *)&buf[0], rev_len);

    sum_len = rev_len-4;
	rev_sum.B[0] = tmpBuff[rev_len - 2];
	rev_sum.B[1] = tmpBuff[rev_len - 1];


    //检验和
    sum.u16_v =  CheckSum16(&buf[2],sum_len);
    
	//计算sum
	if (sum.u16_v== rev_sum.u16_v )
	{
        printfBuf(buf,len);
	}

}
void CjoyAdmissionTestDlg::joy_pose(BYTE _data)
{

	static unsigned char lastRcv = 0;
	static bool bStart = false;
	static unsigned short index = 0, oneLen = 0;
	static unsigned short bsize = 0;
	static unsigned short size = 0, i = 0;

	joyBuf.buff[index] = (unsigned char)_data;
	//只识别帧头
	if (lastRcv != CMD_HEAD && _data == CMD_HEAD && (!bStart))
	{
		bStart = true;
		joyBuf.buff[0] = CMD_HEAD;
		index = 1;
		lastRcv = 0x00;
		return;
	}
	if (bStart)
	{
		joyBuf.buff[index] = _data;
		index++;
		//只识别帧帧尾
		if (index == FRAEM_SIZE)
		{
			//获取一帧数据
			parseFrame(&joyBuf, index);
			bStart = false;
			index = 0;
			lastRcv = 0;
			memset((char *)joyBuf.buff, 0, FRAEM_SIZE);
		}
		if (index > FRAEM_SIZE)
		{
			bStart = false;
			index = 0;
			lastRcv = 0;
			memset((char *)joyBuf.buff, 0, FRAEM_SIZE);
		}
	}
	lastRcv = _data;

}

//串口解析数据帧
void CjoyAdmissionTestDlg::test_pose(BYTE _data)
{
	static unsigned char lastRcv = 0;
	static bool bStart = false;
	static unsigned short index = 0, oneLen = 0,	 bsize = 0,, size = 0, i = 0;

	if (index > TEST_MAX_SIZE)
	{
		bStart = false;
		index = 0;
		lastRcv = 0;
		memset((char *)testBuff, 0, TEST_MAX_SIZE);
	}
	testBuff[index] = (unsigned char)_data;
	//只识别帧头 55 AA
	if (lastRcv == 0x55 && _data == 0xAA && (!bStart))
	{
		bStart = true;
		testBuff[0] = 0x55;
		testBuff[1] = 0xaa;
		index = 2;
		lastRcv = 0x00;
		return;
	}
	if (bStart)
	{
		testBuff[index] = _data;
		index++;
		//只对0x08功能码进行识别筛选
		if ((index == (testBuff[2]+6)) && testBuff[2] == 0x08 )
		{
			//获取一帧数据
			testParseFrame(testBuff, index);
			bStart = false;
			index = 0;
			lastRcv = 0;
			memset((char *)testBuff, 0, TEST_MAX_SIZE);
		}
	}
	lastRcv = _data;
}

void CjoyAdmissionTestDlg::OnCommUart()
{
	// TODO: 在此处添加消息处理程序代码
	VARIANT variant_inp;   //Variant 是一种特殊的数据类型，除了定长String数据及用户定义类型外，可以包含任何种类的数据。
	COleSafeArray safearray_inp;
	LONG len, k;
	BYTE rxdata[2048]; //设置BYTE数组 An 8-bit integer that is not signed.     
	CString strtemp;

	if (m_ctrComm.get_CommEvent() == 2) //事件值为2表示接收缓冲区内有字符     
	{
		////////以下你可以根据自己的通信协议加入处理代码 
		variant_inp = m_ctrComm.get_Input();     //读缓冲区  
		safearray_inp = variant_inp;              //VARIANT型变量转换为ColeSafeArray型变量
		len = safearray_inp.GetOneDimSize();      //得到有效数据长度    

		for (k = 0; k < len; k++)
		{
			safearray_inp.GetElement(&k, rxdata + k);//转换为BYTE型数组
			BYTE bt = *(char*)(rxdata + k);//字符型 
			//joy_pose(bt);

			//串口接收数据接口
            test_pose(bt);
		}
        
		m_ctrComm.put_InputLen(0);  //设置当前接收区数据长度为0 
		m_ctrComm.get_Input();      //先预读缓冲区以清除残留数据  
	}
}

void CjoyAdmissionTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
		case TIMER1_OXYZB:
    		{
    			if (isO_testOK != eT_ON && nStart_test == eTEST_START)
    			{
					//等待超时   
					isO_testOK = eT_OFF;
					GetDlgItem(IDC_STATIC_CALIB)->ShowWindow(SW_SHOW);// SW_HIDE SW_SHOW
					ledBmpShow_ErrOk(BMP_LED_CALIB,false);
                
					m_strXYShow += _T("中心原点数据测试超时,失败!\r\n\n");
					m_strShow   += _T("中心原点数据测试超时,失败!\r\n\n");

					UpdateData(FALSE);
    			}
    			else if (isX_testOK != eT_ON && nStart_test == eO_END)
    			{
    				GetDlgItem(IDC_STATIC_X_TEST)->ShowWindow(SW_SHOW);// SW_HIDE SW_SHOW
    				ledBmpShow_ErrOk(BMP_LED_X_TEST, false);
    				isX_testOK = eT_OFF;
                
    				m_strXYShow += _T("摇杆X轴测试超时，失败！！\r\n\n");
    				m_strShow   += _T("摇杆X轴测试超时，失败！！\r\n\n");
    				UpdateData(FALSE);
    			}
    			else if (isY_testOK != eT_ON && nStart_test == eX_END)
    			{
    				GetDlgItem(IDC_STATIC_Y_TEST)->ShowWindow(SW_SHOW);// SW_HIDE SW_SHOW
    				ledBmpShow_ErrOk(BMP_LED_Y_TEST, false);
    				isY_testOK = eT_OFF;
                
    				m_strXYShow += _T("摇杆Y轴测试超时，失败！！\r\n\n");
    				m_strShow   += _T("摇杆Y轴测试超时，失败！！\r\n\n");
    				UpdateData(FALSE);
    			}
				else if (isZ_testOK != eT_ON && nStart_test == eY_END)
    			{
    				GetDlgItem(IDC_STATIC_Z_TEST)->ShowWindow(SW_SHOW);// SW_HIDE SW_SHOW
    				ledBmpShow_ErrOk(BMP_LED_Z_TEST, false);
    				isZ_testOK = eT_OFF;
                
    				m_strXYShow += _T("摇杆旋转测试超时，失败！！\r\n");
    				m_strShow   += _T("摇杆旋转测试超时，失败！！\r\n");
    				UpdateData(FALSE);
    			}
    			else if (isB_testOK != eT_ON && nStart_test == eZ_END)
    			{
    				GetDlgItem(IDC_STATIC_B_TEST)->ShowWindow(SW_SHOW);// SW_HIDE SW_SHOW
    				ledBmpShow_ErrOk(BMP_LED_BUTTUN, false);
    				isB_testOK = eT_OFF;
                
    				m_strXYShow += _T("摇杆按钮测试超时，失败！！\r\n\n");
    				m_strShow   += _T("摇杆按钮测试超时，失败！！\r\n\n");
    				UpdateData(FALSE);
    			}
            
    			KillTimer(TIMER1_OXYZB); //关闭定时器3。
    			break;
            
    		}
		default:
			break;
	}
	CDialogEx::OnTimer(nIDEvent);
	
}

void CjoyAdmissionTestDlg::OnUartComSend(unsigned char const *buff, int len)
{
	CByteArray HexDataBuf;
	if (m_UartOk)    //判断是否打开并初始化串口
	{
		//数据发送函数
		HexDataBuf.SetSize(len);   //设置数组大小为帧长度 
		for (int i = 0; i < len; i++)
		{
			HexDataBuf.SetAt(i, buff[i]);
		}
		//发送十六进制数据
		m_ctrComm.put_Output(COleVariant(HexDataBuf));
	}
	else
	{
		MessageBox(_T("请先选择COM口"));
	}
}


void CjoyAdmissionTestDlg::OnCbnSelchangeCombo1()
{
    
}

void CjoyAdmissionTestDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
	m_strShow = _T("");
	m_strXYShow = _T("");
	nStart_test = eTEST_INIT;
    GetDlgItem(IDC_STATIC_Z_TEST)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
	GetDlgItem(IDC_STATIC_X_TEST)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
	GetDlgItem(IDC_STATIC_Y_TEST)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
	GetDlgItem(IDC_STATIC_CALIB)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
	GetDlgItem(IDC_STATIC_B_TEST)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
	memset(bmpLast_led, GIF_LED_OFF, sizeof(bmpLast_led));
	UpdateData(FALSE);
}


HBRUSH CjoyAdmissionTestDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
		// TODO:  在此更改 DC 的任何特性
	for (uint8_t i = 0; i < 4; i++)
	{
		if (mIDC_STATIC_XYYS[i] == pWnd->GetDlgCtrlID())
		{
			pDC->SetTextColor(RGB(0, 50, 200));  //设置文本颜色
			//pDC->SetBkColor(RGB(150, 150, 255));  //设置文本背景颜色
			pDC->SetBkMode(OPAQUE);   //设置文本背景模式为透明
			//hbr = CreateSolidBrush(RGB(0, 60, 255));  //设置控件的被那个颜色
		}
		else if (IDC_EDIT_XY_SHOW == pWnd->GetDlgCtrlID())
		{
			pDC->SetTextColor(RGB(200, 100, 0));  //设置文本颜色
			pDC->SetBkMode(OPAQUE);   //设置文本背景模式为透明
		}
	}

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_SHOW) //控件ID
	{
        pDC->SetTextColor(RGB(200, 100, 200)); //设置颜色
	}
		

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}



// 一键摇杆标定
void CjoyAdmissionTestDlg::OnBnClickedButCalib()
{
	if (m_UartOk)    //判断是否打开并初始化串口
	{
		// TODO: 在此添加控件通知处理程序代码
		//GetDlgItem(IDC_BUN_GET_VERSION)->EnableWindow(FALSE);
		//uint8_t  sendbuff[2] = { 1 };
		////12 34 21 3c 01 ff ff 00 4c  01 01 43 21
		//OnSendUltCmd(CMD_PUB_JOY_RESET_ID, 1, sendbuff, 1);
		//XSleep(1000);
		//GetDlgItem(IDC_BUN_GET_VERSION)->EnableWindow(TRUE);
	}
	else
	{
		MessageBox(_T("请先选择COM口"));
	}

}
void CjoyAdmissionTestDlg::ledBmpShow_ErrOk(int num, bool isLed)
{
    
	uint8_t ledStatus = 0;
	ledStatus = isLed ? GIF_LED_OK : GIF_LED_ERR;
    
	if (bmpLast_led[num] != ledStatus)
	{
		bmpLast_led[num] = ledStatus;
        
		CBitmap bitmap;  // CBitmap对象，用于加载位图   
		HBITMAP hBmp;    // 保存CBitmap加载的位图的句柄  
		
		if (isLed)
		{
			bitmap.LoadBitmap(IDB_BMP_OK);  // 将位图IDB_BITMAP1加载到bitmap   
		}
		else
		{
			bitmap.LoadBitmap(IDB_BMP_ERR);  // 将位图IDB_BITMAP1加载到bitmap   
		}
        
		hBmp = (HBITMAP)bitmap.GetSafeHandle();// 获取bitmap加载位图的句柄   
		m_arryBmpLed[num]->SetBitmap(hBmp);				// 设置图片控件m_testLed1的位图图片为IDB_BITMAP_OK
	}

}

void CjoyAdmissionTestDlg::OnBnClicked_startTest()
{
    
	// TODO: 在此添加控件通知处理程序代码
	if (m_UartOk)    //判断是否打开并初始化串口
	{
		CFont font;//声明在头文件中 不再使用指针9
		CString strUlt;
		font.CreatePointFont(110, _T("仿宋"));
		GetDlgItem(IDC_EDIT_XY_SHOW)->SetFont(&font);
        
		//GetDlgItem(IDC_STATIC_Y_TEST)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
		
        if(nStart_test == eTEST_INIT)
        {
            nStart_test = eTEST_START;
			m_strXYShow += _T("开始测试 \r\n");
        }
        else
        {
			//OnBnClickedOk();

			m_strShow = _T("");
			m_strXYShow = _T("");
			//nStart_test = eTEST_INIT;
			nStart_test = eTEST_START;
			GetDlgItem(IDC_STATIC_Z_TEST)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
			GetDlgItem(IDC_STATIC_X_TEST)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
			GetDlgItem(IDC_STATIC_Y_TEST)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
			GetDlgItem(IDC_STATIC_CALIB)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
			GetDlgItem(IDC_STATIC_B_TEST)->ShowWindow(SW_HIDE);// SW_HIDE SW_SHOW
			memset(bmpLast_led, GIF_LED_OFF, sizeof(bmpLast_led));
            m_strXYShow += _T(" 重新测试 开始测试 或者重新测试 \r\n");

        }

		GetDlgItem(IDC_EDIT_XY_SHOW)->SetWindowTextW(m_strXYShow);
		UpdateData(FALSE);
	}
	else
	{
		MessageBox(_T("请先选择COM口"));
	}
    
}

void CjoyAdmissionTestDlg::hall_origin_test(int _valueX,int _valueY,int _valueZ)
{
    static bool isTimeStart = false;
    //开始测试
    if(nStart_test == eTEST_START)
    {
        if(!isTimeStart)
        {
            isTimeStart = true;
            SetTimer(TIMER1_OXYZB, TIME_MS, 0);//启动定时器1,定时时间是timeout_ms 毫秒
        }
        
        if ((abs(_valueX) < 2) && (abs(_valueY) < 2) && (abs(_valueZ) < 2))
        {
            KillTimer(TIMER1_OXYZB); //关闭定时器1。
			GetDlgItem(IDC_STATIC_CALIB)->ShowWindow(SW_SHOW);// SW_HIDE SW_SHOW
			
            m_strXYShow += _T("中心原点数据正常\r\n");
			m_strShow   += _T("中心原点数据正常\r\n");
            
            nStart_test = eO_END; //下一个流程使用
            isO_testOK = eT_ON;
            isTimeStart = false;
			ledBmpShow_ErrOk(BMP_LED_CALIB,true);

            UpdateData(FALSE);
        }
    }

    
}


void CjoyAdmissionTestDlg::uart_x_test(int _value)
{
    
	static bool iszXtestOk = false; //正方向
	static bool isfXtestOK = false; //负方向

    static bool isTimeStartX = false;
    
    //开始测试
    if(nStart_test == eO_END)
    {
        if(!isTimeStartX)
        {
            isTimeStartX = true;
            SetTimer(TIMER1_OXYZB, TIME_MS, 0);//启动定时器1,定时时间是timeout_ms 毫秒
        }

		if (_value > JOY_RANGE_MAX && (!iszXtestOk))
		{
			m_strXYShow += _T("完成X+轴数据测试\r\n\n");
			m_strShow   += _T("完成X+轴数据测试\r\n\n");
			iszXtestOk  = true;
			UpdateData(FALSE);
		}
		else if (_value < -JOY_RANGE_MAX && (!isfXtestOK))
		{
			m_strXYShow += _T("完成X-轴数据测试\r\n");
			m_strShow += _T("完成X-轴数据测试\r\n");
			isfXtestOK = true;
			UpdateData(FALSE);
		}
		else if (isfXtestOK && iszXtestOk)
		{
            KillTimer(TIMER1_OXYZB); //关闭定时器1。
			GetDlgItem(IDC_STATIC_X_TEST)->ShowWindow(SW_SHOW);// SW_HIDE SW_SHOW
			ledBmpShow_ErrOk(BMP_LED_X_TEST,true);
			m_strXYShow += _T("完成X轴数据测试\r\n\n");
			m_strShow += _T("X轴测试成功\r\n\n");
            nStart_test = eX_END; //下一个流程使用
            isX_testOK = eT_ON;
            isTimeStartX = false;
			UpdateData(FALSE);
		}
	}
	else
	{
		isfXtestOK = false;
		iszXtestOk = false;
	}

}

void CjoyAdmissionTestDlg::uart_y_test(int _value)
{
    
	static bool iszYtestOk = false; //正方向
	static bool isfYtestOK = false; //负方向
	
    static bool isTimeStartY = false;
    //开始测试
    if(nStart_test == eX_END)
    {
        if(!isTimeStartY)
        {
            isTimeStartY = true;
            SetTimer(TIMER1_OXYZB, TIME_MS, 0);//启动定时器1,定时时间是timeout_ms 毫秒
        }


		if (_value > JOY_RANGE_MAX && (!iszYtestOk))
		{
			m_strXYShow += _T("完成Y+轴数据测试\r\n\n");
			m_strShow   += _T("完成Y+轴数据测试\r\n\n");
			iszYtestOk  = true;
			UpdateData(FALSE);
		}
		else if (_value < -JOY_RANGE_MAX && (!isfYtestOK))
		{
			m_strXYShow += _T("完成Y-轴数据测试\r\n\n");
			m_strShow   += _T("完成Y-轴数据测试\r\n\n");
			isfYtestOK = true;
			UpdateData(FALSE);
		}
		else if (isfYtestOK && iszYtestOk)
		{
            KillTimer(TIMER1_OXYZB); //关闭定时器1。
			GetDlgItem(IDC_STATIC_Y_TEST)->ShowWindow(SW_SHOW);// SW_HIDE SW_SHOW
			ledBmpShow_ErrOk(BMP_LED_Y_TEST, true);
			m_strXYShow += _T("完成Y轴数据测试\r\n");
			m_strShow   += _T("Y轴测试成功\r\n\n");
            nStart_test = eY_END; //下一个流程使用
            isY_testOK = eT_ON;
            isTimeStartY = false;
			UpdateData(FALSE);
		}
	}
	else
	{
		isfYtestOK = false;
		iszYtestOk = false;
	}
}


void CjoyAdmissionTestDlg::uart_z_test(int _value)
{
    
    static bool iszZtestOk = false; //正方向
    static bool isfZtestOK = false; //负方向
    
    static bool isTimeStartZ = false;
    //开始测试
    if(nStart_test == eY_END)
    {
        if(!isTimeStartZ)
        {
            isTimeStartZ = true;
            SetTimer(TIMER1_OXYZB, TIME_MS, 0);//启动定时器1,定时时间是timeout_ms 毫秒
        }

		if (_value > JOY_RANGE_MAX && (!iszZtestOk))
		{
			m_strXYShow += _T("完成左旋转测试\r\n");
			m_strShow   += _T("完成左旋转测试\r\n");
			iszZtestOk = true;
			UpdateData(FALSE);
		}
		else if (_value < -JOY_RANGE_MAX && (!isfZtestOK))
		{
			m_strXYShow += _T("完成右旋转测试\r\n");
			m_strShow   += _T("完成右旋转测试\r\n");
			isfZtestOK = true;
			UpdateData(FALSE);
		}
		else if (isfZtestOK && iszZtestOk)
		{
            KillTimer(TIMER1_OXYZB); //关闭定时器1。
			GetDlgItem(IDC_STATIC_Z_TEST)->ShowWindow(SW_SHOW);// SW_HIDE SW_SHOW
			ledBmpShow_ErrOk(BMP_LED_Z_TEST, true);
			m_strXYShow += _T("完成左右旋据测试\r\n\n");
			m_strShow   += _T("完成左右旋据测试\r\n\n");
            nStart_test = eZ_END; //下一个流程使用
            isZ_testOK = eT_ON;
            isTimeStartZ = false;
			UpdateData(FALSE);
            
		}
	}
	else
	{
		isfZtestOK = false;
		iszZtestOk = false;
	}
    
}

void CjoyAdmissionTestDlg::uart_b_test(int _value)
{
    
	static bool iszBtestOk = false; //正方向
	static bool isfBtestOK = false; //负方向
	
	static int curB  = 0; //方向
	static int lastB = 0; //方向
	
    static bool isTimeStartB = false;
    //开始测试
    if(nStart_test == eZ_END)
    {
        if(!isTimeStartB)
        {
            isTimeStartB = true;
            SetTimer(TIMER1_OXYZB, TIME_MS, 0);//启动定时器1,定时时间是timeout_ms 毫秒
        }

		if ( (_value == 1) && (lastB == 0) && (!iszBtestOk))
		{
			m_strXYShow += _T("完成按钮按下测试\r\n");
			m_strShow   += _T("完成按钮按下测试\r\n");
			iszBtestOk = true;
			UpdateData(FALSE);
		}
		else if ( (_value == 0) && (lastB == 1) && (!isfBtestOK))
		{
			m_strXYShow += _T("完成按钮松开测试\r\n");
			m_strShow   += _T("完成按钮松开测试\r\n");
			isfBtestOK = true;
			UpdateData(FALSE);
		}
		else if (isfBtestOK && iszBtestOk)
		{
            KillTimer(TIMER1_OXYZB); //关闭定时器1。
			GetDlgItem(IDC_STATIC_B_TEST)->ShowWindow(SW_SHOW);// SW_HIDE SW_SHOW
			ledBmpShow_ErrOk(BMP_LED_BUTTUN, true);
			m_strXYShow += _T("完成按钮测试\r\n\n");
			m_strShow   += _T("完成按钮测试\r\n\n");
            
            m_strXYShow += _T("===完成测试,请点击重新测试===\r\n\n");
			m_strShow   += _T("===完成测试,请点击重新测试===\r\n\n");
            nStart_test = eB_END; //下一个流程使用
            isB_testOK = eT_ON;
            
            isTimeStartB = false;
			UpdateData(FALSE);
            
		}
        
        lastB = _value;
	}
	else
	{
		iszBtestOk = false;
		isfBtestOK = false;
	}
    
}


void CjoyAdmissionTestDlg::OnBnClickedUartClose()
{
    
	CString str, n;					//定义字符串
	GetDlgItemText(IDC_Uart_Close, str);  //获取给定控件的文本
	CWnd *h1;
	h1 = GetDlgItem(IDC_Uart_Close);		//指向控件的caption
	if (m_UartOk)
	{
		m_ctrComm.put_PortOpen(false);		//关闭串口
		if (str != _T("打开串口"))
		{
			str = _T("打开串口");
			UpdateData(true);				//将控件的状态传给其关联的变量
			h1->SetWindowText(str);			//改变按钮名称为打开串口
		}
        
		m_UartOk = FALSE;
		OnBnClickedOk();
		return;
	}
	
	m_ctrComm.put_CommPort(uart_num);//选择串口号(这里因为列表框的索引号是从0开始，所以(nSel+1)对应的才是我们所选的串口号)
	m_ctrComm.put_InBufferSize(1024);//输入缓冲区大小为1024byte
	m_ctrComm.put_OutBufferSize(512);//输出缓冲区大小为512byte
	m_ctrComm.put_PortOpen(TRUE);//打开串口
	m_ctrComm.put_RThreshold(2);//收到两个字节引发OnComm事件 
	m_ctrComm.put_InputMode(1);//输入模式选为二进制 
	m_ctrComm.put_Settings(_T("115200,n,8,1"));//设置串口参数，波特率，无奇偶校验，位停止位，位数据位
	m_ctrComm.put_InputMode(1);  // 以二进制方式检取数据 
	m_ctrComm.put_RThreshold(1); //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件 
	m_ctrComm.put_InputLen(0);  //设置当前接收区数据长度为0 
	m_ctrComm.get_Input(); //先预读缓冲区以清除残留数据  
	
	if (m_ctrComm.get_PortOpen())
	{
		str = _T("关闭串口");
		UpdateData(true);
		h1->SetWindowText(str);			//改变按钮名称为‘’关闭串口”

		CString strCom; 
		m_UartOk = TRUE;
		//strCom.Format(_T("打开串口COM%d 成功==> OK \r\n"), (int)(m_ctrComm.get__CommPort()));
		strCom= _T("打开串口 成功==> OK \r\n");
		//strCom.Format(_T("打开串口COM%d 成功==> OK \r\n"), uart_num);
		m_strShow += strCom;
		UpdateData(FALSE);
		//GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
	}
	
}
void CjoyAdmissionTestDlg::GetCom()
{

	//程序启动时获取全部可用串口
	HANDLE hCom;
	int i, k;
	CString str;
	BOOL flag;
	CString strCom;

	((CComboBox *)GetDlgItem(IDC_COMBO1))->ResetContent();
	flag = FALSE;
	uart_num = 0;
	for (i = 1; i <= 16; i++)
	{//此程序支持16个串口
		str.Format(L"\\\\.\\COM%d", i);
		hCom = CreateFile(str, 0, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (INVALID_HANDLE_VALUE != hCom)
		{//能打开该串口，则添加该串口
			CloseHandle(hCom);
			str = str.Mid(4);
			((CComboBox *)GetDlgItem(IDC_COMBO1))->AddString(str);
			if (flag == FALSE)
			{
				flag = TRUE;
				uart_num = i;
				strCom.Format(_T("扫描COM%d \r\n"), uart_num);
				m_strShow += strCom;
				UpdateData(FALSE);
			}
		}
	}
	i = ((CComboBox *)GetDlgItem(IDC_COMBO1))->GetCount();
	if (i == 0)
	{//若找不到可用串口则禁用“打开串口”功能
		((CComboBox *)GetDlgItem(IDC_COMBO1))->EnableWindow(FALSE);
	}
	else
	{
		k = ((CComboBox *)GetDlgItem((IDC_COMBO1)))->GetCount();
		((CComboBox *)GetDlgItem(IDC_COMBO1))->SetCurSel(k - 1);
		//m_ctrComm.BindCommPort(uart_num);
		//m_ctrComm.BindProperty(uart_num);

	}
}






void CjoyAdmissionTestDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	m_Stop = !m_Stop;


}
