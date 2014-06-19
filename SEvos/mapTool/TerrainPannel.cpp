// TerrainPanel.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool2.h"
#include "mapview.h"
#include "terrain/terrainCursor.h"
#include "terrain/TerrainEditor.h"
#include "common//light.h"

#include "TerrainPannel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTerrainPanel dialog


CTerrainPanel::CTerrainPanel(CWnd* pParent /*=NULL*/)
	: CPanelBase(TERRAIN, CTerrainPanel::IDD, pParent)
	, m_strLightDirection(_T(""))
{
	//{{AFX_DATA_INIT(CTerrainPanel)
	m_nHOffset = 0;
	m_EditTerrain = FALSE;
	m_strHOffset = _T("");
	m_strBrushSize = _T("");
	m_nBrushSize = 0;
	m_strCursorPos = _T("");
	m_bUpDown = TRUE;
	//}}AFX_DATA_INIT
}

CTerrainPanel::~CTerrainPanel()
{

}


void CTerrainPanel::DoDataExchange(CDataExchange* pDX)
{
	CPanelBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerrainPanel)
	DDX_Slider(pDX, IDC_SLIDER_HOFFSET, m_nHOffset);
	DDX_Check(pDX, IDC_CHECK_EDIT, m_EditTerrain);
	DDX_Text(pDX, IDC_EDIT_HOFFSET, m_strHOffset);
	DDX_Text(pDX, IDC_EDIT_BRUSHSIZE, m_strBrushSize);
	DDX_Slider(pDX, IDC_SLIDER_BRUSHSIZE, m_nBrushSize);
	DDX_Text(pDX, IDC_STATIC_CURSORPOS, m_strCursorPos);
	DDX_Check(pDX, IDC_CHECK_UPDOWN, m_bUpDown);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_STATIC_LIGHTDIR, m_strLightDirection);
}


BEGIN_MESSAGE_MAP(CTerrainPanel, CPanelBase)
	//{{AFX_MSG_MAP(CTerrainPanel)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_HOFFSET, OnReleasedcaptureSliderHoffset)
	ON_BN_CLICKED(IDC_CHECK_EDIT, OnCheckEdit)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_BRUSHSIZE, OnReleasedcaptureSliderBrushsize)
	ON_BN_CLICKED(IDC_CHECK_UPDOWN, OnCheckUpdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrainPanel message handlers

BOOL CTerrainPanel::OnInitDialog() 
{
	CPanelBase::OnInitDialog();

	m_nHOffset = 66;
	m_nBrushSize = 50;
	m_strHOffset = "20.0";
	m_strBrushSize = "20.0";

	CTerrainCursor *pcursor = g_pTerrainView->GetTerrainCursor();
	if (!pcursor) return FALSE;

	pcursor->SetHeightIncrementOffset(20.f);
//	g_pTerrainView->SetBrushSize( 20.f );

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CTerrainPanel::OnReleasedcaptureSliderHoffset(NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdateData();

	float fOffset = ((float)m_nHOffset * 0.3f) + 1.f;
	CTerrainCursor *pcursor = g_pTerrainView->GetTerrainCursor();
	if (!pcursor) return;

	pcursor->SetHeightIncrementOffset(fOffset);
	m_strHOffset.Format( "%.1f", fOffset );
	UpdateData( FALSE );

	*pResult = 0;
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CTerrainPanel::OnCheckEdit() 
{
	UpdateData();
	Update();	// listner에게 알린다.
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CTerrainPanel::OnReleasedcaptureSliderBrushsize(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData();

//	float fSize = ((float)m_nBrushSize*0.2f) + 10.f;
//	g_pTerrainView->SetBrushSize( fSize );
//	m_strBrushSize.Format( "%.1f", fSize );
	UpdateData( FALSE );

	*pResult = 0;
}


//-----------------------------------------------------------------------------//
// Cursor정보 Update
//-----------------------------------------------------------------------------//
void CTerrainPanel::UpdateCursorInfo()
{
	CTerrainCursor *pcursor = g_pTerrainView->GetTerrainCursor();
	if (!pcursor) return;

	Vector3 pos = pcursor->GetCursorPos();
	m_strCursorPos.Format( "{ %.1f, %.1f, %.1f }", pos.x, pos.y, pos.z );

	CTerrainEditor *pTerrain = g_pTerrainView->GetTerrain();
	CLight *plight = pTerrain->GetLight();
	if (!plight) return;
	Vector3 dir = plight->GetDirection();
	m_strLightDirection.Format( "{ %.1f, %.1f, %.1f }", dir.x, dir.y, dir.z );

	UpdateData( FALSE );
}


void CTerrainPanel::OnCheckUpdown() 
{
	UpdateData();
	CTerrainCursor *pcursor = g_pTerrainView->GetTerrainCursor();
	if (!pcursor) return;
	pcursor->SetTerrainUp(m_bUpDown);
}


BOOL CTerrainPanel::IsEditTerrainMode()
{
	UpdateData();
	return m_EditTerrain;
}


//------------------------------------------------------------------------
// 
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
void CTerrainPanel::OnShowWindow()
{

}


//------------------------------------------------------------------------
// 
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
void CTerrainPanel::OnHideWindow()
{
	m_EditTerrain = FALSE;
	UpdateData(FALSE);
}
