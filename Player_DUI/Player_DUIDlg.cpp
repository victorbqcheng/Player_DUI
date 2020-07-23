
// Player_DUIDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include <functional>
#include <algorithm>

#include "Player_DUI.h"
#include "Player_DUIDlg.h"
#include "afxdialogex.h"
#include "Div.h"
#include "CDUIButton.h"
#include "CDUIProgress.h"
#include "CTools.h"


#define NOMINMAX
#undef max
#undef min

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::string ID_DIV_BK = "background";
std::string ID_DIV_VOLUME = "volume";
std::string ID_DIV_DRAG = "draggable";
std::string ID_BTN_PLAY = "btn_play";
std::string ID_BTN_PAUSE = "btn_pause";
std::string ID_BTN_CONTINUE = "btn_continue";
std::string ID_BTN_STOP = "btn_stop";
std::string ID_BTN_SPEED = "btn_speed";
std::string ID_DIV_TIME = "time";
std::string ID_PLAY_BAR = "play_bar";
std::string ID_PROGRESS_BAR = "progress_bar";
std::string ID_PROGRESS_HANDLE = "progress_handle";

std::string ID_SPEED_MENU_CONTAINER = "speed_menu";
std::string ID_SPEED_MENU_ITEM_1_5X = "speed_1.5X";
std::string ID_SPEED_MENU_ITEM_1_25X = "speed_1.25X";
std::string ID_SPEED_MENU_ITEM_1X = "speed_1X";
std::string ID_SPEED_MENU_ITEM_0_75X = "speed_0.75X";
std::string ID_SPEED_MENU_ITEM_0_5X = "speed_0.5X";

int const BTN_WIDTH = 48;
int const BTN_HEIGHT = 48;

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


// CPlayerDUIDlg 对话框

CPlayerDUIDlg::CPlayerDUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PLAYER_DUI_DIALOG, pParent), uiMgr(IDD_PLAYER_DUI_DIALOG)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bTracking = false;
}

void CPlayerDUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
void CPlayerDUIDlg::onBtnPlayClicked(CMouseEvent e)
{
	if (m_fileName == "")
	{
		return;
	}
	DivPtr pDivBtnPlay = uiMgr.getElementByID(ID_BTN_PLAY);
	if(pDivBtnPlay != NULL)
		pDivBtnPlay->setVisible(false);

	DivPtr pDivBtnPause = uiMgr.getElementByID(ID_BTN_PAUSE);
	if (pDivBtnPause != NULL)
		pDivBtnPause->setVisible(true);

	
	if (m_player.open(m_fileName) == 0)
	{
		m_player.set_render_callback(std::bind(&CPlayerDUIDlg::on_video_render_cb, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		showPlayTime(0, m_player.get_duration());
		m_player.set_volume(m_nVolume);
		m_player.play();
		m_bPlaying = true;


		int w = m_player.get_width();
		int h = m_player.get_height();
		resizeWindow(w, h);
	}
}
void CPlayerDUIDlg::onBtnPauseClicked(CMouseEvent e)
{
	e.pTarget->setVisible(false);
	DivPtr pDivBtnContinue = uiMgr.getElementByID(ID_BTN_CONTINUE);
	if (pDivBtnContinue != NULL)
	{
		pDivBtnContinue->setVisible(true);
	}
	m_player.pause();
}

void CPlayerDUIDlg::onBtnContinueClicked(CMouseEvent e)
{
	e.pTarget->setVisible(false);
	DivPtr pDivBtnPause = uiMgr.getElementByID(ID_BTN_PAUSE);
	if (pDivBtnPause != NULL)
	{
		pDivBtnPause->setVisible(true);
	}
	m_player.continue_play();
}

void CPlayerDUIDlg::onBtnStopClicked(CMouseEvent e)
{
	DivPtr pDivBk = uiMgr.getElementByID(ID_DIV_BK);
	pDivBk->setBackgroundImage(NULL, 0, 0);

	m_bPlaying = false;
	m_player.stop();
	m_player.close();

	DivPtr pDivPlayBar = uiMgr.getElementByID(ID_PLAY_BAR);
	if(pDivPlayBar != NULL)
	{
		DivPtr pDivBtnPlay = uiMgr.getElementByID(ID_BTN_PLAY);
		pDivBtnPlay->setVisible(true);

		DivPtr pDivBtnPause = uiMgr.getElementByID(ID_BTN_PAUSE);
		pDivBtnPause->setVisible(false);

		DivPtr pDivBtnContinue = uiMgr.getElementByID(ID_BTN_CONTINUE);
		pDivBtnContinue->setVisible(false);
	}
}

void CPlayerDUIDlg::onBtnSpeedClicked(CMouseEvent e)
{
}

void CPlayerDUIDlg::onBtnSpeedMouseMove(CMouseEvent e)
{
	DivPtr pSpeedMenu = uiMgr.getElementByID(ID_SPEED_MENU_CONTAINER);
	if (pSpeedMenu != NULL)
	{
		pSpeedMenu->setVisible(true);
	}

}

void CPlayerDUIDlg::onBtnSpeedMouseLeave(CMouseEvent e)
{
	DivPtr pSpeedMenu = uiMgr.getElementByID(ID_SPEED_MENU_CONTAINER);
	if (pSpeedMenu != NULL)
	{
		pSpeedMenu->setVisible(false);
	}
}

void CPlayerDUIDlg::onProgressBarClicked(CMouseEvent e)
{
	CDUIProgress* pProgressBar = (CDUIProgress*)uiMgr.getElementByID(ID_PROGRESS_BAR).get();
	if (pProgressBar != NULL)
	{
		if (m_bPlaying)
		{
			int dist = e.nMouseX - pProgressBar->getAbsPosition().x;
			int nWidth = pProgressBar->getWidth();
			double percentage = double(dist) / nWidth;
			int64_t time = int64_t(m_player.get_duration() * percentage);
			m_player.seek_to(time);
		}
	}
}

void CPlayerDUIDlg::onDragDbClick(CMouseEvent e)
{
	resizeWindow(m_nMaxWidth, m_nMaxHeight);
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE);
}

void CPlayerDUIDlg::createSpeedMenuItem(std::string strID, std::wstring strText, int index)
{
	DivPtr pSpeedMenu = uiMgr.getElementByID(ID_SPEED_MENU_CONTAINER);
	if (pSpeedMenu != NULL)
	{
		auto onItemMouseMove = [](CMouseEvent e)
		{
			e.pTarget->setTextColor(Corona::Color(0, 255, 0));
		};
		auto onItemMouseLeave = [](CMouseEvent e)
		{
			e.pTarget->setTextColor(Corona::Color(0, 0, 0));
		};
		auto onItemClicked = [this, pSpeedMenu, strText, strID](CMouseEvent e)
		{
			DivPtr pBtnSpeed = uiMgr.getElementByID(ID_BTN_SPEED);
			if (pBtnSpeed != NULL)
			{
				pBtnSpeed->setText(strText);
				pSpeedMenu->setVisible(false);
				std::map<std::string, double> speedMap{
					{ID_SPEED_MENU_ITEM_1_5X, 1.5},
					{ID_SPEED_MENU_ITEM_1_25X, 1.25},
					{ID_SPEED_MENU_ITEM_1X, 1.0},
					{ID_SPEED_MENU_ITEM_0_75X, 0.75},
					{ID_SPEED_MENU_ITEM_0_5X, 0.5},
				};
				double speed = speedMap.find(strID)->second;
				m_player.set_speed(speed);
			}
		};

		const int ITEM_HEIGHT = 40;
		auto pMenuItem = CUIMgr::buildDiv(strID);
		pMenuItem->setWidth(BTN_WIDTH+20);
		pMenuItem->setHeight(ITEM_HEIGHT);
		pMenuItem->setPosition(0, ITEM_HEIGHT*index);
		pMenuItem->setText(strText);
		pMenuItem->setTextColor(Corona::Color(0, 0, 0));
		pMenuItem->setFontSize(12);
		pMenuItem->setAlignment(Corona::ALIGNMENT_CENTER, Corona::ALIGNMENT_CENTER);
		pMenuItem->setMouseMoveCb(std::function<void(CMouseEvent)>(onItemMouseMove));
		pMenuItem->setMouseLeaveCb(std::function<void(CMouseEvent)>(onItemMouseLeave));
		pMenuItem->setClickCb(std::function<void(CMouseEvent e)>(onItemClicked));

		pSpeedMenu->addChild(pMenuItem);
	}
}
void CPlayerDUIDlg::on_video_render_cb(char* data, int width, int height)
{
	DivPtr pBk = uiMgr.getElementByID(ID_DIV_BK);
	pBk->setBackgroundImage(data, width, height);
}
void CPlayerDUIDlg::createDUIElement()
{
	RECT rc = {0};
	this->GetClientRect(&rc);
	using Rect = Corona::Rect;
	auto pDivBkGround = CUIMgr::buildDiv(ID_DIV_BK);
	{
		pDivBkGround->setBackgroundColor(Corona::Color(255, 0, 0, 0));
		pDivBkGround->setPosition(0, 0);
		pDivBkGround->setWidth(rc.right);
		pDivBkGround->setHeight(rc.bottom);
		pDivBkGround->setClip(true);
		//pDivBkGround->setDraggable(true);
		//uiMgr.addElement(pDivBkGround);
		uiMgr.setRoot(pDivBkGround);
	}
	auto pDivDrag = CUIMgr::buildDiv(ID_DIV_DRAG);
	{
		pDivDrag->setPosition(0, 0);
		pDivDrag->setWidth(rc.right);
		pDivDrag->setHeight(rc.bottom);
		pDivDrag->setDraggable(true);
		pDivDrag->setLButtonDbClickCb(std::bind(&CPlayerDUIDlg::onDragDbClick, this, std::placeholders::_1));
		pDivBkGround->addChild(pDivDrag);
	}
	auto pDivVolume = CUIMgr::buildDiv(ID_DIV_VOLUME);
	{
		pDivVolume->setWidth(130);
		pDivVolume->setHeight(30);
		pDivVolume->setPosition(10, 10);
		pDivVolume->setTransparent(true);
		wchar_t buf[32]{ 0 };
		wsprintf(buf, L"volume:%d", m_nVolume);
		pDivVolume->setText(buf);
		pDivVolume->setTextColor(Corona::Color(200, 200, 200));
		pDivVolume->setAlignment(Corona::ALIGNMENT_NEAR, Corona::ALIGNMENT_NEAR);
		pDivBkGround->addChild(pDivVolume);
	}
	auto pProgressBar = CUIMgr::buildElement<CDUIProgress>(ID_PROGRESS_BAR);
	{
		pProgressBar->setWidth(rc.right - rc.left);
		pProgressBar->setHeight(10);
		pProgressBar->setPosition(0, rc.bottom - BTN_HEIGHT - 30);
		pProgressBar->setTransparent(false);
		pProgressBar->setBackgroundColor(Corona::Color(100, 100, 100, 100));
		pProgressBar->setHandleLen(15);
		pProgressBar->setClickCb(std::bind(&CPlayerDUIDlg::onProgressBarClicked, this, std::placeholders::_1));
		pDivBkGround->addChild(pProgressBar);
	}

	auto pPlayBar = CUIMgr::buildDiv(ID_PLAY_BAR);
	{
		//pPlayBar->setClip(true);
		pPlayBar->setVisible(false);
		pPlayBar->setWidth(rc.right - rc.left);
		pPlayBar->setHeight(BTN_HEIGHT);
		pPlayBar->setPosition(0, 300);
		pPlayBar->setTransparent(true);
		pPlayBar->setBackgroundColor(Corona::Color(100, 100, 100, 100));
		pDivBkGround->addChild(pPlayBar);
	}

	auto pBtnPlay = CUIMgr::buildElement<CDUIButton>(ID_BTN_PLAY);
	{
		pBtnPlay->setVisible(true);
		pBtnPlay->setButtonWidth(BTN_WIDTH);
		pBtnPlay->setButtonHeight(BTN_HEIGHT);
		pBtnPlay->setPosition(0, 0);
		std::wstring play_normal = L".\\res\\play_normal.png";
		std::wstring play_hover = L".\\res\\play_hover.png";
		pBtnPlay->setBackgroundImage(play_normal, Rect(0, 0, BTN_WIDTH, BTN_HEIGHT),
			play_hover, Rect(0, 0, BTN_WIDTH, BTN_HEIGHT),
			play_hover, Rect(0, 0, BTN_WIDTH, BTN_HEIGHT));
		pBtnPlay->setClickCb(std::bind(&CPlayerDUIDlg::onBtnPlayClicked, this, std::placeholders::_1));
		pPlayBar->addChild(pBtnPlay);
	}

	auto pBtnPause = CUIMgr::buildElement<CDUIButton>(ID_BTN_PAUSE);
	{
		pBtnPause->setVisible(false);
		pBtnPause->setButtonWidth(BTN_WIDTH);
		pBtnPause->setButtonHeight(BTN_HEIGHT);
		pBtnPause->setPosition(0, 0);
		std::wstring pause_normal = L".\\res\\pause_normal.png";
		std::wstring pause_hover = L".\\res\\pause_hover.png";
		pBtnPause->setBackgroundImage(pause_normal, Rect(0, 0, BTN_WIDTH, BTN_HEIGHT),
			pause_hover, Rect(0, 0, BTN_WIDTH, BTN_HEIGHT),
			pause_hover, Rect(0, 0, BTN_WIDTH, BTN_HEIGHT));
		pBtnPause->setClickCb(std::bind(&CPlayerDUIDlg::onBtnPauseClicked, this, std::placeholders::_1));
		pPlayBar->addChild(pBtnPause);
	}
	auto pBtnContinue = CUIMgr::buildElement<CDUIButton>(ID_BTN_CONTINUE);
	{
		pBtnContinue->setVisible(false);
		pBtnContinue->setButtonWidth(BTN_WIDTH);
		pBtnContinue->setButtonHeight(BTN_HEIGHT);
		pBtnContinue->setPosition(0, 0);
		std::wstring play_normal = L".\\res\\play_normal.png";
		std::wstring play_hover = L".\\res\\play_hover.png";
		pBtnContinue->setBackgroundImage(play_normal, Rect(0, 0, BTN_WIDTH, BTN_HEIGHT),
			play_hover, Rect(0, 0, BTN_WIDTH, BTN_HEIGHT),
			play_hover, Rect(0, 0, BTN_WIDTH, BTN_HEIGHT));
		pBtnContinue->setClickCb(std::bind(&CPlayerDUIDlg::onBtnContinueClicked, this, std::placeholders::_1));
		pPlayBar->addChild(pBtnContinue);
	}
	auto pBtnStop = CUIMgr::buildElement<CDUIButton>(ID_BTN_STOP);
	{
		pBtnStop->setVisible(true);
		pBtnStop->setButtonWidth(BTN_WIDTH);
		pBtnStop->setButtonHeight(BTN_HEIGHT);
		pBtnStop->setPosition(BTN_WIDTH, 0);
		std::wstring stop_normal = L".\\res\\stop_normal.png";
		std::wstring stop_hover = L".\\res\\stop_hover.png";
		pBtnStop->setBackgroundImage(stop_normal, Rect(0, 0, BTN_WIDTH, BTN_HEIGHT),
			stop_hover, Rect(0, 0, BTN_WIDTH, BTN_HEIGHT),
			stop_hover, Rect(0, 0, BTN_WIDTH, BTN_HEIGHT));
		pBtnStop->setClickCb(std::bind(&CPlayerDUIDlg::onBtnStopClicked, this, std::placeholders::_1));
		pPlayBar->addChild(pBtnStop);
	}
	auto pBtnSpeed = CUIMgr::buildDiv(ID_BTN_SPEED);
	{
		pBtnSpeed->setWidth(BTN_WIDTH+20);
		pBtnSpeed->setHeight(BTN_HEIGHT);
		pBtnSpeed->setPosition(BTN_WIDTH*2, 0);
		pBtnSpeed->setBackgroundColor(Corona::Color(0, 255, 0, 0));
		pBtnSpeed->setFontSize(12);
		pBtnSpeed->setAlignment(Corona::ALIGNMENT_CENTER, Corona::ALIGNMENT_CENTER);
		pBtnSpeed->setText(L"1X");
		pBtnSpeed->setTextColor(Corona::Color(0, 255, 0));
		pBtnSpeed->setMouseMoveCb(std::bind(&CPlayerDUIDlg::onBtnSpeedMouseMove, this, std::placeholders::_1));
		pBtnSpeed->setMouseLeaveCb(std::bind(&CPlayerDUIDlg::onBtnSpeedMouseLeave, this, std::placeholders::_1));
		pPlayBar->addChild(pBtnSpeed);

		auto pSpeedMenu = CUIMgr::buildDiv(ID_SPEED_MENU_CONTAINER);
		{
			const int ITEM_HEIGHT = 40;
			pSpeedMenu->setWidth(BTN_WIDTH+20);
			pSpeedMenu->setHeight(ITEM_HEIGHT * 5);
			pSpeedMenu->setPosition(0, -200+1);
			pSpeedMenu->setBackgroundColor(Corona::Color(0, 100, 100));
			pSpeedMenu->setVisible(false);
			pBtnSpeed->addChild(pSpeedMenu);
			
			createSpeedMenuItem(ID_SPEED_MENU_ITEM_1_5X, L"1.5X", 0);
			createSpeedMenuItem(ID_SPEED_MENU_ITEM_1_25X, L"1.25X", 1);
			createSpeedMenuItem(ID_SPEED_MENU_ITEM_1X, L"1X", 2);
			createSpeedMenuItem(ID_SPEED_MENU_ITEM_0_75X, L"0.75X", 3);
			createSpeedMenuItem(ID_SPEED_MENU_ITEM_0_5X, L"0.5X", 4);
		}
	}
	auto pDivTime = CUIMgr::buildDiv(ID_DIV_TIME);
	{
		int const w = 250;
		pDivTime->setWidth(w);
		pDivTime->setHeight(BTN_HEIGHT);
		pDivTime->setPosition(pPlayBar->getWidth() - w, 0);
		pDivTime->setTransparent(true);
		pDivTime->setText(L"00:00:00/00:00:00");
		pDivTime->setTextColor(Corona::Color(255, 255, 255));
		pDivTime->setFontSize(12);
		pDivTime->setFontName(L"宋体");
		pDivTime->setAlignment(Corona::ALIGNMENT_CENTER, Corona::ALIGNMENT_CENTER);
		pPlayBar->addChild(pDivTime);
	}
}

void CPlayerDUIDlg::showPlayTime(int64_t play_time, int64_t duration)
{
	using namespace std::chrono;
	DivPtr pDivTime = uiMgr.getElementByID(ID_DIV_TIME);
	if(pDivTime!=NULL)
	{
		milliseconds mill_dr = std::chrono::milliseconds(duration);

		hours h_dr = duration_cast<hours>(mill_dr);
		minutes m_dr = duration_cast<minutes>(mill_dr) - h_dr;
		seconds s_dr = duration_cast<seconds>(mill_dr) - h_dr - m_dr;

		milliseconds mill_pt = milliseconds(play_time);
		hours h_pt = duration_cast<hours>(mill_pt);
		minutes m_pt = duration_cast<minutes>(mill_pt) - h_pt;
		seconds s_pt = duration_cast<seconds>(mill_pt) - h_pt - m_pt;

		char buf[64]{ 0 };
		sprintf(buf, "%02d:%02d:%02lld / %02d:%02d:%02lld", h_pt.count(), m_pt.count(), s_pt.count(),
										  h_dr.count(), m_dr.count(), s_dr.count());
		pDivTime->setText(CTools::str_2_wstr(buf));
		//更新进度条
 		double percentage = double(play_time) / double(duration);
 		CDUIProgress* pProgressBar = (CDUIProgress*)uiMgr.getElementByID(ID_PROGRESS_BAR).get();
		pProgressBar->setPercentage(percentage);		
	}
}

void CPlayerDUIDlg::resizeWindow(int w, int h)
{
	//MoveWindow()
	w = std::min(std::max(w, 500), m_nMaxWidth);
	h = std::min(std::max(h, 250), m_nMaxHeight);
	SetWindowPos(NULL, 0, 0, w, h, SWP_NOMOVE);
}

void CPlayerDUIDlg::updateVolume()
{
	wchar_t buf[32]{ 0 };
	wsprintf(buf, L"volume:%02d", m_nVolume);
	DivPtr pDivVolume = uiMgr.getElementByID(ID_DIV_VOLUME);
	pDivVolume->setText(buf);

	int volume =int( double(m_nVolume) / 100 * m_player.get_max_volume());
	m_player.set_volume(volume);
	//m_player.set_volume(m_nVolume);
}

BEGIN_MESSAGE_MAP(CPlayerDUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_DROPFILES()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CPlayerDUIDlg 消息处理程序

BOOL CPlayerDUIDlg::OnInitDialog()
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
	createDUIElement();
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	m_nMaxWidth = rect.right - rect.left;
	m_nMaxHeight = rect.bottom - rect.top;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPlayerDUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPlayerDUIDlg::OnPaint()
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
HCURSOR CPlayerDUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CPlayerDUIDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	uiMgr.hookWndMsg(this->GetSafeHwnd(), message, wParam, lParam);
	if (message == WM_NCHITTEST)
	{
		//return HTCAPTION;
	}
	if (message == WM_LBUTTONDOWN)
	{
		//SendMessage(WM_SYSCOMMAND, SC_MOVE | 0x02, 0);
	}
	if (message == WM_KEYDOWN)
	{
		
	}
	return CDialogEx::WindowProc(message, wParam, lParam);
}


void CPlayerDUIDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bPlaying)
	{
		showPlayTime(m_player.get_play_time(), m_player.get_duration());
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CPlayerDUIDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	//int nTop = cy - BTN_HEIGHT;
	DivPtr pDivBk = uiMgr.getElementByID(ID_DIV_BK);
	if (pDivBk != NULL)
	{
		pDivBk->setWidth(cx);
		pDivBk->setHeight(cy);
	}
	DivPtr pDivDrag = uiMgr.getElementByID(ID_DIV_DRAG);
	if (pDivDrag != NULL)
	{
		pDivDrag->setWidth(cx);
		pDivDrag->setHeight(cy);
	}
	DivPtr pProgressBar = uiMgr.getElementByID(ID_PROGRESS_BAR);
	if(pProgressBar != NULL)
	{ 
		pProgressBar->setWidth(cx);
		pProgressBar->setPosition(0, cy - BTN_HEIGHT - 30);
	}
	DivPtr pPlayBar = uiMgr.getElementByID(ID_PLAY_BAR);
	if (pPlayBar != NULL)
	{
		pPlayBar->setWidth(cx);
	}
	DivPtr pDivTime = uiMgr.getElementByID(ID_DIV_TIME);
	if (pDivTime != NULL)
	{
		pDivTime->setPosition(pPlayBar->getWidth() - pDivTime->getWidth() - 5, 0);
	}
	
}


void CPlayerDUIDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	DivPtr pPlayBar = uiMgr.getElementByID(ID_PLAY_BAR);
	RECT rc = {0};
	GetClientRect(&rc);
	if (pPlayBar != NULL)
	{
		pPlayBar->setPosition(0, rc.bottom - BTN_HEIGHT);
		pPlayBar->setVisible(true);
	}
	if (m_bTracking == false)
	{
		m_bTracking = true;
		TRACKMOUSEEVENT t;
		t.cbSize = sizeof(t);
		t.dwFlags = TME_LEAVE;
		t.hwndTrack = m_hWnd;
		::TrackMouseEvent(&t);
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CPlayerDUIDlg::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	DivPtr pPlayBar = uiMgr.getElementByID(ID_PLAY_BAR);
	RECT rc = { 0 };
	GetClientRect(&rc);
	if (pPlayBar != NULL)
	{
		pPlayBar->setPosition(0, rc.bottom);
		pPlayBar->setVisible(false);
	}
	m_bTracking = false;
	CDialogEx::OnMouseLeave();
}


void CPlayerDUIDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnClose();
}


void CPlayerDUIDlg::OnDestroy()
{
	
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	m_player.stop();
	m_player.close();
}


void CPlayerDUIDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CPlayerDUIDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	
	if (pMsg->message == WM_KEYDOWN)
	{
		int key = pMsg->wParam;
		if (key == VK_UP)
		{
			m_nVolume++;
			if (m_nVolume >= VOLUME_MAX)
			{
				m_nVolume = VOLUME_MAX;
			}
			updateVolume();
		}
		else if (key == VK_DOWN)
		{
			m_nVolume--;
			if (m_nVolume <= VOLUME_MIN)
			{
				m_nVolume = VOLUME_MIN;
			}
			updateVolume();
		}
		else if (key == VK_RIGHT)
		{
			m_player.seek_by(20000);
		}
		else if (key == VK_LEFT)
		{
			m_player.seek_by(-10000);
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CPlayerDUIDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	UINT  nFileCount = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
	char szFileName[_MAX_PATH] = "";
	DWORD dwAttribute;
	for (UINT i = 0; i < 1; i++)	//只处理一个文件
	{
		::DragQueryFileA(hDropInfo, i, szFileName, sizeof(szFileName));
		dwAttribute = ::GetFileAttributesA(szFileName);

		// 是否为文件夹
		if (dwAttribute & FILE_ATTRIBUTE_DIRECTORY)
		{
			//::SetCurrentDirectoryA(szFileName);
		}
		else
		{
			m_fileName = szFileName;
		}
	}
	::DragFinish(hDropInfo);

	CMouseEvent e{0};
	this->onBtnStopClicked(e);
	this->onBtnPlayClicked(e);

	CDialogEx::OnDropFiles(hDropInfo);
}

//与OnNcHitTest配合实现无border窗口的拖动
void CPlayerDUIDlg::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nHitTest)
	{
	case HTTOP:
		SendMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_TOP, MAKELPARAM(point.x, point.y));
		return;
	case HTBOTTOM:
		SendMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOM, MAKELPARAM(point.x, point.y));
		return;
	case HTLEFT:
		SendMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_LEFT, MAKELPARAM(point.x, point.y));
		return;
	case HTRIGHT:
		SendMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_RIGHT, MAKELPARAM(point.x, point.y));
		return;
	case HTTOPLEFT:
		SendMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPLEFT, MAKELPARAM(point.x, point.y));
		return;
	case HTTOPRIGHT:
		SendMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPRIGHT, MAKELPARAM(point.x, point.y));
		return;
	case HTBOTTOMLEFT:
		SendMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMLEFT, MAKELPARAM(point.x, point.y));
		return;
	case HTBOTTOMRIGHT:
		SendMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMRIGHT, MAKELPARAM(point.x, point.y));
		return;
	}
	CDialogEx::OnNcLButtonDown(nHitTest, point);
}


LRESULT CPlayerDUIDlg::OnNcHitTest(CPoint point)
{
	return CDialogEx::OnNcHitTest(point);
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CPoint ptCur;
	CRect rect;
	GetCursorPos(&ptCur);
	GetWindowRect(&rect);

	if (CRect(rect.left, rect.top, rect.left + 3, rect.top + 3).PtInRect(ptCur))
		return HTTOPLEFT;
	else if (CRect(rect.right - 3, rect.top, rect.right, rect.top + 3).PtInRect(ptCur))
		return HTTOPRIGHT;
	else if (CRect(rect.left, rect.bottom - 3, rect.left + 3, rect.bottom).PtInRect(ptCur))
		return HTBOTTOMLEFT;
	else if (CRect(rect.right - 3, rect.bottom - 3, rect.right, rect.bottom).PtInRect(ptCur))
		return HTBOTTOMRIGHT;
	else if (CRect(rect.left, rect.top, rect.left + 3, rect.bottom).PtInRect(ptCur))
		return HTLEFT;
	else if (CRect(rect.right - 3, rect.top, rect.right, rect.bottom).PtInRect(ptCur))
		return HTRIGHT;
	else if (CRect(rect.left, rect.top, rect.right - 128, rect.top + 3).PtInRect(ptCur)) // 128 Min,Max,Close
		return HTTOP;
	else if (CRect(rect.left, rect.bottom - 3, rect.right, rect.bottom).PtInRect(ptCur))
		return HTBOTTOM;

	return CDialogEx::OnNcHitTest(point);
}
