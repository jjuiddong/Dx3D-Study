
#include "stdafx.h"

#include "terrain.h"
#include "../model/model.h"
#include "common/utility.h"
#include "terrainCursor.h"
#include "chunkManager.h"
#include "shadowsystem.h"
#include "terraineditor.h"


CTerrainEditor::CTerrainEditor():
m_bDrawChunkEdge( TRUE )
{

}


CTerrainEditor::~CTerrainEditor()
{
	Clear();
}


//------------------------------------------------------------------------
// 
// [2011/2/20 jjuiddong]
//------------------------------------------------------------------------
BOOL CTerrainEditor::Load( char *szFileName )
{
	CLinearMemLoader loader;
	loader.LoadTokenFile( "data//script//def_map.txt" );

	STerrainLoader *ploader = (STerrainLoader*)loader.ReadBin(szFileName, "TERRAIN");
	if (!ploader) return FALSE;

	// load terrain
	Create(ploader->nVtxPerRow, ploader->nVtxPerCol, (float)ploader->nCellSize, 1.0f, "data//map//tile//detail.tga");

	const int vertexSize = GetVertexCount();
	IDirect3DVertexBuffer9 *pvtxBuff = GetVertexBuffer();
	SVtxNormTex *pv;
	pvtxBuff->Lock(0, 0, (void**)&pv, 0);
	for (int i=0; i < vertexSize; ++i)
		pv[ i].v.y = ploader->pMap[ i];
	pvtxBuff->Unlock();

	// load rigid model
	for (int i=0; i < ploader->nRigidSize; ++i)
	{
		STModelLoader *pTStructure = &ploader->pRigid[ i];
		CModel *pModel = new CModel();
		if (pModel->LoadModel(pTStructure->szObjName))
		{
			AddTerrainStructure(pTStructure->vPos, OT_RIGID, pTStructure->szObjName, pModel);
		}
		else
		{
			delete pModel;
		}
	}

	// load dynamic model
	for (int i=0; i < ploader->nDynSize; ++i)
	{
		STModelLoader *pTStructure = &ploader->pDyn[ i];
		CModel *pModel = new CModel();
		if (pModel->LoadModel(pTStructure->szObjName))
		{
			AddTerrainStructure(pTStructure->vPos, OT_MODEL, pTStructure->szObjName, pModel);
		}
		else
		{
			delete pModel;
		}
	}

	// load chunk
	m_pChunkMng->Load(this, &ploader->chunkGroup);

	UpdateNormals();
	m_pChunkMng->UpdateTerrainToChunk();
	UpdateEdge();

	delete[] ploader;
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
BOOL CTerrainEditor::Save( char *szFileName )
{
	const int vertexCount = GetVertexCount();
	STerrainLoader *pTLoader = (STerrainLoader*)new BYTE[ sizeof(STerrainLoader) + (vertexCount * sizeof(float))];
	ZeroMemory( pTLoader, sizeof(STerrainLoader) );

	pTLoader->nWidth = (int)GetRegionWidth();
	pTLoader->nHeight = (int)GetRegionHeight();
	m_fHeightScale = 1.f;
	pTLoader->nVtxPerRow = m_rowVtxCount;
	pTLoader->nVtxPerCol = m_columnVtxCount;
	pTLoader->nCellPerRow = m_rowCellCount;
	pTLoader->nCellPerCol = m_columnCellCount;
	pTLoader->nCellSize = (int)m_cellSize;
	pTLoader->nMapSize = GetVertexCount();
	pTLoader->pMap = (float*)((BYTE*)pTLoader + sizeof(STerrainLoader));
	ZeroMemory( pTLoader->pMap, GetVertexCount() * sizeof(float) );

	IDirect3DVertexBuffer9 *pvtxBuff = GetVertexBuffer();
	SVtxNormTex *pv;
	pvtxBuff->Lock(0, 0, (void**)&pv, 0);
	for (int i=0; i < vertexCount; ++i)
	{
		pTLoader->pMap[ i] = pv[ i].v.y;
	}
	pvtxBuff->Unlock();

	// rigid 저장
	pTLoader->nRigidSize = m_RigidList.size();
	if (0 < m_RigidList.size())
	{
		pTLoader->pRigid = new STModelLoader[ m_RigidList.size()];
		int i=0;
		ModelItor itor = m_RigidList.begin();
		while( m_RigidList.end() != itor )
		{
			Vector3 v = itor->pModel->GetPos();
			pTLoader->pRigid[ i++] = STModelLoader( v, itor->nModelIdx, itor->szObjName );
			++itor;
		}
	}

	// dynamic 저장
	pTLoader->nDynSize = m_DynList.size();
	if( 0 < m_DynList.size() )
	{
		pTLoader->pDyn = new STModelLoader[ m_DynList.size()];

		int i=0;
		ModelItor itor = m_DynList.begin();
		while( m_DynList.end() != itor )
		{
			Vector3 v = itor->pModel->GetPos();
			pTLoader->pDyn[ i++] = STModelLoader( v, itor->nModelIdx, itor->szObjName );
			++itor;
		}
	}

	// chunk alpha folder 생성
	char szChunkAlphaFilePath[ MAX_PATH];
	GetFilePath(szFileName, szChunkAlphaFilePath);
	strcat_s(szChunkAlphaFilePath, sizeof(szChunkAlphaFilePath), "/alpha");
	if (!PathFileExists(szChunkAlphaFilePath))
	{
		BOOL result = CreateDirectory(szChunkAlphaFilePath, NULL);
		if (!result) return FALSE;
	}

	// chunk data save
	SChunkGroupLoader *pChunkGroupLoader = m_pChunkMng->GetChunkGroupLoader(szChunkAlphaFilePath);
	pTLoader->chunkGroup = *pChunkGroupLoader;

	CLinearMemLoader loader;
	loader.LoadTokenFile( "data//script//def_map.txt" );
	BOOL writeResult = loader.WriteBin(szFileName, pTLoader, "TERRAIN" );

	return writeResult;
}


//-----------------------------------------------------------------------------//
// 출력
//-----------------------------------------------------------------------------//
void CTerrainEditor::Render()
{
	CTerrain::Render();

	if (m_bDrawChunkEdge && m_pChunkMng)
		m_pChunkMng->RenderEdge();

	RenderSelectModel();

	//RenderPostEffect();
	if (m_pShadowSystem)
		m_pShadowSystem->EdgeFinalRender();

/*
	itor = m_CaptureList.begin();
	while( m_CaptureList.end() != itor )
		(itor++)->pModel->RenderBoundBox();
/**/
}


//-----------------------------------------------------------------------------//
// 에니메이션
//-----------------------------------------------------------------------------//
void CTerrainEditor::Update( int nElapsTick )
{
	CTerrain::Update( nElapsTick );
	UpdateSelectModel(nElapsTick);

}


//-----------------------------------------------------------------------------//
// 높이 설정
// vPos: map x,y axis
// eType: cursor type {square, circle}
// fSize: up/down cursor size
//-----------------------------------------------------------------------------//
void CTerrainEditor::SetHeight( CTerrainCursor *pcursor, int nElapsTick )
{
	if (!m_pChunkMng) return;

	m_pChunkMng->SetHeight(pcursor, nElapsTick);
	m_pChunkMng->UpdateChunkToTerrain();
	UpdateNormals();
	m_pChunkMng->UpdateTerrainToChunk();
}


//-----------------------------------------------------------------------------//
// SelectModel 을 지형에 추가한다.
//-----------------------------------------------------------------------------//
BOOL CTerrainEditor::LocateModel( CTerrainCursor *pCursor )
{
	SelectItor itor = m_SelectList.begin();
	while (m_SelectList.end() != itor)
	{
		SSelectObj obj = *itor++;
		CModel *pNewModel = obj.pModel->Clone();
		if (!pNewModel) continue;

		AddTerrainStructure( pCursor->GetCursorPos(), obj.eOType, obj.szObjName, pNewModel );
	}
	return TRUE;
}


//-----------------------------------------------------------------------------//
// Capture된 Model을 HoverList에 추가한다.
// 
//-----------------------------------------------------------------------------//
BOOL CTerrainEditor::CaptureToHover( LPPOINT lpPos )
{
	SelectModelCancel();

/*
	HoverItor itor = m_CaptureList.begin();
	while( m_CaptureList.end() != itor )
	{
		// 임시코드 {하나만 복사한다.}
		SHoverObj hover;
		hover.eOType = itor->eOType;
		hover.pModel = CopyModel( itor->eOType, itor->pModel );
		m_HoverList.push_back( hover );

		DelTModel( itor->eOType, itor->nId );

		++itor;
		break;
	}
	m_CaptureList.clear();
/**/
	return TRUE;
}


//-----------------------------------------------------------------------------//
// Hover 이동
//-----------------------------------------------------------------------------//
void CTerrainEditor::MoveSelectModel( CTerrainCursor *pCursor )
{
	Vector3 cusorPos = pCursor->GetCursorPos();

	SelectItor itor = m_SelectList.begin();
	while( m_SelectList.end() != itor )
	{
		CModel *pModel = (itor++)->pModel;
		Matrix44 *pMat = pModel->GetWorldTM();
		pMat->SetWorld(cusorPos);
		pModel->SetWorldTM(pMat);
	}
}


//-----------------------------------------------------------------------------//
// Dyn, Rigid Model capture검사
// return 0 = capture 안됨
//		  1 = Dyn,Rigid Capture
//		  2 = Hover 설정됨
//-----------------------------------------------------------------------------//
int CTerrainEditor::Capture( Vector2 screenPos )
{
	SelectModelCancel();

	// m_DynList 검사
	m_CaptureList.clear();
	ModelItor itor = m_DynList.begin();
	while( m_DynList.end() != itor )
	{
		if( itor->pModel->Pick(screenPos) )
		{
			m_SelectList.push_back( SSelectObj(itor->nId,OT_MODEL,itor->szObjName,itor->pModel) );
			itor = m_DynList.erase(itor);
		}
		else
		{
			++itor;
		}
	}

	// m_RigidList 검사
	itor = m_RigidList.begin();
	while( m_RigidList.end() != itor )
	{
		if( itor->pModel->Pick(screenPos) )
			m_SelectList.push_back( SSelectObj(itor->nId,OT_RIGID,itor->szObjName,itor->pModel) );
		++itor;
	}

/*
	// Capture된 Model을 다시 capture할 경우 CaptureToHover() 함수가 호출되고 리턴된다.
	HoverItor citor = m_CaptureList.begin();
	while( m_CaptureList.end() != citor )
	{
		if( citor->pModel->Pick(lpRect) )
		{
			POINT pos = {lpRect->left, lpRect->top};
			CaptureToHover( &pos );
			return 2;
		}

		++citor;
	}
	//

	// m_DynList 검사
	m_CaptureList.clear();
	ModelItor itor = m_DynList.begin();
	while( m_DynList.end() != itor )
	{
		if( itor->pModel->Pick(lpRect) )
			m_CaptureList.push_back( SHoverObj(itor->nId,OT_MODEL,itor->szObjName,itor->pModel) );
		++itor;
	}

	// m_RigidList 검사
	itor = m_RigidList.begin();
	while( m_RigidList.end() != itor )
	{
		if( itor->pModel->Pick(lpRect) )
			m_CaptureList.push_back( SHoverObj(itor->nId,OT_RIGID,itor->szObjName,itor->pModel) );
		++itor;
	}

	return (m_CaptureList.size()>0)? 1 : 0;
/**/

	return 0;
}


//-----------------------------------------------------------------------------//
// HoverObject 취소함
//-----------------------------------------------------------------------------//
void CTerrainEditor::SelectModelCancel()
{
	SelectItor itor = m_SelectList.begin();
	while( m_SelectList.end() != itor )
		delete (itor++)->pModel;
	m_SelectList.clear();
}


//-----------------------------------------------------------------------------//
// Hover Object 설정
//-----------------------------------------------------------------------------//
void CTerrainEditor::SetSelectModel( OBJ_TYPE eOType, char *szObjName )
{
	SelectModelCancel();

	BOOL isLoadSuccess = FALSE;
	CModel *pModel = NULL;
	if (eOType == OT_MODEL)
	{
		pModel = new CModel();
		isLoadSuccess = pModel->LoadModel(szObjName);
	}
	else if (eOType == OT_RIGID)
	{
//		pModel = new SD_CRigidModel;
//		pModel->Load( szObjName );
	}

	if (!isLoadSuccess)
	{
		SAFE_DELETE(pModel);
		return;
	}

	if (pModel)
	{
		m_SelectList.push_back( SSelectObj(0, eOType, szObjName, pModel) );
	}
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CTerrainEditor::UpdateObjPos()
{
/*
	ModelItor itor = m_DynList.begin();
	while( m_DynList.end() != itor )
	{
		Vector3 vPos;
		itor->pModel->GetPos( &vPos );
		itor->pModel->SetPos( &Vector3(vPos.x, vPos.y, GetHeight(vPos.x,vPos.y)) );
		++itor;
	}

	itor = m_RigidList.begin();
	while( m_RigidList.end() != itor )
	{
		Vector3 vPos;
		itor->pModel->GetPos( &vPos );
		itor->pModel->SetPos( &Vector3(vPos.x, vPos.y, GetHeight(vPos.x,vPos.y)) );
		++itor;
	}
/**/

}


//-----------------------------------------------------------------------------//
// 청크 최적화
//-----------------------------------------------------------------------------//
void CTerrainEditor::OptimizeChunk()
{

}


//-----------------------------------------------------------------------------//
// 타일 블러쉬 출력
//-----------------------------------------------------------------------------//
void CTerrainEditor::setVisibleBrush( BOOL visible )
{

}


//-----------------------------------------------------------------------------//
// 스플래팅 기술로 지형에 텍스쳐를 입힌다.
// pbrush: 브러쉬 정보
// screenpos: 브러쉬로 그린 스크린 좌표
//-----------------------------------------------------------------------------//
BOOL CTerrainEditor::DrawBrush( CTerrainCursor *pcursor )
{
	if (!m_pChunkMng) return FALSE;

	if( !pcursor->IsLoad() )
	{
		::AfxMessageBox( "타일을 먼저 선택하세요" );
		return FALSE;
	}

	m_pChunkMng->DrawBrush(pcursor);

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Chunk Edge를 출력할지를 결정한다.
//-----------------------------------------------------------------------------//
void CTerrainEditor::SetDrawChunkEdgeOption( BOOL chunkdraw )
{
	m_bDrawChunkEdge = chunkdraw;
}


//------------------------------------------------------------------------
// 
// [2011/2/24 jjuiddong]
//------------------------------------------------------------------------
void CTerrainEditor::Clear()
{
	CTerrain::Clear();

	SelectItor itor = m_SelectList.begin();
	while( m_SelectList.end() != itor )
		delete (itor++)->pModel;
	m_SelectList.clear();
}


//------------------------------------------------------------------------
// 
// [2011/2/24 jjuiddong]
//------------------------------------------------------------------------
void CTerrainEditor::RenderSelectModel()
{
	SelectItor itor = m_SelectList.begin();
	while( m_SelectList.end() != itor )
		(itor++)->pModel->Render();
}


//------------------------------------------------------------------------
// 
// [2011/2/24 jjuiddong]
//------------------------------------------------------------------------
void CTerrainEditor::UpdateSelectModel(int nElapsTick)
{
	SelectItor itor = m_SelectList.begin();
	while( m_SelectList.end() != itor )
		(itor++)->pModel->Update( nElapsTick );
}


//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
CChunk* CTerrainEditor::SetChunkFocusing( CTerrainCursor *pcursor )
{
	if (!m_pChunkMng) return NULL;

	CChunk *pchunk = m_pChunkMng->SetFocus(pcursor);
	return pchunk;

}
