// CCrTerrainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool2.h"
#include "CrTerrainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCrTerrainDlg dialog


CCrTerrainDlg::CCrTerrainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCrTerrainDlg::IDD, pParent)
	, m_strTextureName(_T(""))
	, m_CellSize(0)
	, m_strWidth(_T(""))
	, m_strHeight(_T(""))
	, m_strVtxPerRow(_T(""))
	, m_strVtxPerCol(_T(""))
	, m_strFilePath(_T(""))
{
	//{{AFX_DATA_INIT(CCrTerrainDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCrTerrainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCrTerrainDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_TEXTURE, m_strTextureName);
	DDX_Text(pDX, IDC_EDIT_FILEPATH, m_strFilePath);
	DDX_Text(pDX, IDC_STATIC_CELLSIZE, m_CellSize);
	DDX_CBString(pDX, IDC_COMBO_WIDTH, m_strWidth);
	DDX_CBString(pDX, IDC_COMBO_HEIGHT, m_strHeight);
	DDX_CBString(pDX, IDC_COMBO_VTXPERROW, m_strVtxPerRow);
	DDX_CBString(pDX, IDC_COMBO_VTXPERCOL, m_strVtxPerCol);
}


BEGIN_MESSAGE_MAP(CCrTerrainDlg, CDialog)
	//{{AFX_MSG_MAP(CCrTerrainDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, &CCrTerrainDlg::OnBnClickedButtonTexture)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCrTerrainDlg message handlers
BOOL CCrTerrainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_strWidth = "640";
	m_strHeight = "640";
	m_strVtxPerCol = "65";
	m_strVtxPerRow = "65";
	m_CellSize = atoi(m_strWidth) / (atoi(m_strVtxPerRow)-1);
	m_strFilePath = "data/map/data1";
	m_strTextureName = "data/map/tile/detail.tga";
	UpdateData( FALSE );

	return TRUE;
}


void CCrTerrainDlg::OnOK() 
{
	UpdateData( TRUE );
	m_CreateData.nWidth = atoi(m_strWidth);
	m_CreateData.nHeight = atoi(m_strHeight);
	m_CreateData.nVtxPerRow = atoi(m_strVtxPerRow);
	m_CreateData.nVtxPerCol = atoi(m_strVtxPerCol);
	strcpy_s(m_CreateData.szFilePath, sizeof(m_CreateData.szFilePath), m_strFilePath);
	strcpy_s(m_CreateData.szTexture, sizeof(m_CreateData.szTexture), m_strTextureName);

	if ((0 == m_CreateData.nVtxPerRow % 2) || (0 == m_CreateData.nVtxPerCol % 2))
	{
		::AfxMessageBox("Vertex 개수는 짝수여야 합니다.");
		return;
	}

	if (PathFileExists(m_strFilePath))
	{
		const int result = MessageBox("이미 같은 파일이 존재합니다. 그래도 만드시겠습니까?", "Confirm", MB_YESNO);
		if (IDNO == result)
		{
			return;
		}
	}

	CDialog::OnOK();
}

void CCrTerrainDlg::OnCancel() 
{
	
	CDialog::OnCancel();
}

void CCrTerrainDlg::OnBnClickedButtonTexture()
{

}

