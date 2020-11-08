
// joyAdmissionTestDlg.h: 头文件
//

#pragma once
#include "CMSComm.h"
#include "config_com.h"
// CjoyAdmissionTestDlg 对话框
class CjoyAdmissionTestDlg : public CDialogEx
{
// 构造
public:
	CjoyAdmissionTestDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JOYADMISSIONTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CMSComm m_ctrComm;
	CString m_strShow;
	CComboBox m_comboCom;
	DECLARE_EVENTSINK_MAP()
        
	void OnCommUart();
    
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeCombo1();
    afx_msg void OnBnClickedOk();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnBnClickedButCalib();
    afx_msg void OnBnClicked_startTest();
    afx_msg void OnBnClickedUartClose();

	unsigned  char checksum(unsigned  char *p, unsigned  int len);
	CString getCurSysTime(void);


// 成功true 失败为false
bool m_UartOk;
bool m_Stop;

#pragma pack(1)
typedef enum _eGIF_LED_
{
	GIF_LED_OFF = 0,
	GIF_LED_ERR = 1,
	GIF_LED_OK  = 2,
}eGIF_LED;

#define FRAEM_SIZE			9	
#define CMD_HEAD				0xFF
#define HALL_DATA_RANGE     (0x3DF - 0x020)

#define TIMER1_OXYZB		1	//定时器
#define JOY_RANGE_MAX	95	//摇杆最大量程

#define TIME_MS	8000	//摇杆最大量程


typedef union
{
	char        u8_v[2];
	uint16_t		uV;
	short		sV;
} un2_type;

typedef struct _CMD_BUF_
{
	uint8_t		head;	//2Byte:7e7f 帧同步: 以0xFF, 0x34 特殊数据为帧起始或帧同步.
	un2_type	dataX;	//2Byte: dataX
	un2_type	dataY;	//2Byte:dataY
	un2_type	dataZ;	//2Byte: dataZ
	uint8_t		button; //1Byte:按钮
	uint8_t		sum;	//校验和
}BUF_TypeDef;

typedef union _FMC_BUF_
{
	uint8_t			buff[FRAEM_SIZE];
	BUF_TypeDef		sCmd;
}JOY_BUF_TypeDef, *pJoyBuf;

JOY_BUF_TypeDef joyBuf;


#define			TEST_MAX_SIZE 	200	//摇杆最大量程
uint8_t			testBuff[TEST_MAX_SIZE];

#pragma pack()

void OnUartComSend(unsigned char const *buff, int len);
void joy_pose(BYTE _data);
void parseFrame(pJoyBuf pJoy, int len);
void parseJoyData(pJoyBuf pJdata);
void xytsShowValue(uint8_t num, int32_t value, CString decs);


void test_pose(BYTE _data);
void testParseFrame(uint8_t *buf, int len);
void printfBuf(uint8_t *buf, int len);

typedef enum _eBMP_LIST_
{
	BMP_LED_CALIB	= 0,    //标定灯
	BMP_LED_X_TEST	= 1,    //X轴指示灯
	BMP_LED_Y_TEST	= 2,    //Y轴指示灯
	BMP_LED_Z_TEST  = 3,    //Z轴指示灯
	BMP_LED_BUTTUN	= 4,    //按钮指示灯
	
	BMP_LED_NUM 
}eBMP_LIST;

enum _eSTART_TEST
{
    eTEST_INIT,
    eTEST_START,
    eO_START	,    // 原点开始
    eO_END	,    // 原点结束
    eX_START ,    // x开始
    eX_END   ,    // x结束
    eY_START ,    // y开始
    eY_END   ,    // y结束
    eZ_START ,    // z开始
    eZ_END   ,    // z结束
    eB_START ,    // b开始
    eB_END   ,    // b结束

    eTEST_MAX, 
};


CStatic m_nIDC_LedCalib;// 标定指示bmp
CStatic m_nIDC_LedXTest;// x轴指示灯图片
CStatic m_nIDC_LedYTest;// y轴指示灯图片
CStatic m_nIDC_LedZTest;// Z轴指示灯图片
CStatic m_nIDC_LedBTest;// 按钮

// 图片指示灯 指针数组
CStatic *m_arryBmpLed[BMP_LED_NUM];//
//保存上一次是否显示一致
uint8_t bmpLast_led[BMP_LED_NUM];

// xyz轴操作提示区
CString m_strXYShow;
int  nStart_test;   //开始测试


typedef enum _eOXYZB
{
	eT_INIT = 0,
	eT_OFF = 1,
	eT_ON  = 2,
};

int isO_testOK; //是否开始原点测试
int isX_testOK;
int isY_testOK;
int isZ_testOK;
int isB_testOK;

void ledBmpShow_ErrOk(int num, bool isLed);

void hall_origin_test(int _valueX,int _valueY,int _valueZ);
void uart_x_test(int _value);
void uart_y_test(int _value);
void uart_z_test(int _value);
void uart_b_test(int _value);


void GetCom();
int uart_num = 0;

afx_msg void OnBnClickedButton1();
};
