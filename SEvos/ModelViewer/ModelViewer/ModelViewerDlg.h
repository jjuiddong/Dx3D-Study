
// CModelViewerDlg.h : 헤더 파일
//

#pragma once


// CModelViewerDlg 대화 상자
class CModelViewerDlg : public CDialog
{
// 생성입니다.
public:
	CModelViewerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MODELVIEWER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

protected:
	BOOL m_bLoop;
	int m_nDeltaTime;
	char m_CurrentDirectory[ MAX_PATH];

public:
	BOOL Init();
	void MainProc();
	BOOL Setup();


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};
