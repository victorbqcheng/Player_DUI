
// Player_DUIDlg.h: 头文件
//

#pragma once

#include "UIMgr.h"
#include "CPlayer.h"

// CPlayerDUIDlg 对话框
class CPlayerDUIDlg : public CDialogEx
{
	int VOLUME_MAX = 100;
	int const VOLUME_MIN = 0;
// 构造
public:
	CPlayerDUIDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLAYER_DUI_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
private:
	void createDUIElement();
	void createSpeedMenuItem(std::string strID, std::string strText, int index);
	void showPlayTime(int64_t play_time, int64_t duration);
	void resizeWindow(int w, int h);
	void updateVolume();
// 实现
protected:
	HICON m_hIcon;
	CUIMgr uiMgr;
	bool m_bTracking;
	CPlayer m_player;
	int m_nVolume = VOLUME_MAX/2;
	double play_speed = 1.0;
	bool m_bPlaying = false;
	std::string m_fileName = "";
	//最大宽高(桌面宽高, 不包括任务栏)
	int m_nMaxWidth = 0;
	int m_nMaxHeight = 0;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	void onBtnPlayClicked(CMouseEvent e);
	void onBtnPauseClicked(CMouseEvent e);
	void onBtnContinueClicked(CMouseEvent e);
	void onBtnStopClicked(CMouseEvent e);
	void onBtnSpeedClicked(CMouseEvent e);
	void onBtnSpeedMouseMove(CMouseEvent e);
	void onBtnSpeedMouseLeave(CMouseEvent e);	
	void onProgressBarClicked(CMouseEvent e);
	void onDragDbClick(CMouseEvent e);

	void on_video_render_cb(char* data, int width, int height);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
};
