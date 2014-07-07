#include "terrain.h"


CTerrain::CTerrain(LPDIRECT3DDEVICE9 lpDevice)
{		
	m_pd3dDevice		= lpDevice;
	m_pVB			    = NULL;
	m_pIB			    = NULL;
	m_wWidth			= 0;
	m_wHeight			= 0;
	m_wWidthVertices	= 0;
	m_wHeightVertices	= 0;
	m_dwTotalVertices   = 0x00;
	m_dwTotalIndices	= 0x00;
	m_fLength			= 0.0f;
	m_dwPolygonsCount	= 0;
	m_pVertices			= NULL;


	m_nCurSelIndex		= 0;
	for( int i=0 ; i<4 ; ++i )
		m_pDrawIndex[i]= i;

	m_fBrushSize		= 2.0f;
	m_fSmoothSize		= 1.0f;

	m_blPickOK			= FALSE;             
	m_vGetPos			= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_fGetU				= 0.0f;
	m_fGetV				= 0.0f;
	m_blClicked			= FALSE;
	m_nTexPosX			= 0;
	m_nTexPosY			= 0;
}

CTerrain::~CTerrain(void)
{
	SAFE_RELEASE_( m_pBaseTexture );
	for( int i=0; i<MAXTEXNUM ; ++i )
	{
		SAFE_RELEASE_( m_Texture[i].pTexture );
		SAFE_RELEASE_( m_Texture[i].pAlphamap );
	}
	SAFE_RELEASE_(m_pVB);
	SAFE_RELEASE_(m_pIB);
	SAFE_DELETE_( m_pVertices );
}

HRESULT CTerrain::TerrainInit( WORD wWidth, WORD wHeight, float fLength/*, char *chFileName */)
{
	WORD x,y;

	m_wWidth		  = wWidth;
	m_wHeight		  = wHeight;
	m_wWidthVertices  = wWidth + 1;										//맵의 가로 정점수
	m_wHeightVertices = wHeight + 1;									//맵의 세로 정점수
	m_dwTotalVertices = m_wWidthVertices * m_wHeightVertices;			//맵의 전체정점수

	m_fLength = fLength;
	
	//if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, chFileName, &m_Texture)))
	//{ return false;}

	//ex)2*2		

	//		6---7---8
	//		| ＼| ＼|
	//		3---4---5
	//		| ＼| ＼|
	//		0---1---2

	//2.정점좌표,텍스쳐좌표 설정

	m_pVertices = new CUSTOMVERTEX[m_dwTotalVertices];

	for ( y=0; y<m_wHeightVertices; ++y )
	{
		for ( x=0; x<m_wWidthVertices; ++x )
		{
			m_pVertices[(y*m_wWidthVertices)+x].x  = (float)(x); //+ m_fLength;
			m_pVertices[(y*m_wWidthVertices)+x].y  = 0.0f;
			m_pVertices[(y*m_wWidthVertices)+x].z  = (float)(y); //+ m_fLength;

			m_pVertices[(y*m_wWidthVertices)+x].u1 = (float)x / (float)m_wWidth;
			m_pVertices[(y*m_wWidthVertices)+x].v1 = 1.0f -(float)y / (float)m_wHeight;
			m_pVertices[(y*m_wWidthVertices)+x].u2 = (float)x / (float)m_wWidth;
			m_pVertices[(y*m_wWidthVertices)+x].v2 = 1.0f -(float)y / (float)m_wHeight;
		} 
	}

	//3.정점버퍼 설정

	void *pVertices;

	if(FAILED(m_pd3dDevice->CreateVertexBuffer(
			m_dwTotalVertices * sizeof(CUSTOMVERTEX),
			0,
			D3DFVF_CUSTOMVERTEX, 
			D3DPOOL_DEFAULT, 
			&m_pVB,
			NULL) ) )
		{ return false; }

		if( FAILED(m_pVB->Lock( 0, m_dwTotalVertices * sizeof(CUSTOMVERTEX), (void**)&pVertices,0 ) ) )
		{ return false; }

		memcpy(pVertices, m_pVertices,
			m_dwTotalVertices * sizeof(CUSTOMVERTEX));
		m_pVB->Unlock();
		

		//4.인덱스 설정
		
		int total_strips = 
			m_wWidthVertices-1;
		int total_indexes_per_strip = 
			m_wWidthVertices<<1;

		// the total number of indices is equal
		// to the number of strips times the
		// indices used per strip plus one
		// degenerate triangle between each strip
		m_dwTotalIndices = 
			(total_indexes_per_strip * total_strips) 
			+ (total_strips<<1) - 2;

		unsigned short* pIndexValues = 
			new unsigned short[m_dwTotalIndices];

		unsigned short* index = pIndexValues;
		unsigned short start_vert = 0;
		unsigned short lineStep = (unsigned short)(m_fLength*m_wHeightVertices);

		for ( y=0; y<total_strips; ++y )
		{
			unsigned short vert=start_vert;

			// create a strip for this row
			for ( x=0; x<m_wWidthVertices; ++x )
			{
				*(index++) = vert;
				*(index++) = vert + lineStep;
				vert += 1;
			}
			start_vert += lineStep;

			if (y+1<total_strips)
			{
				// add a degenerate to attach to 
				// the next row
				*(index++) = (vert-1)+lineStep;
				*(index++) = start_vert;
			}
		}

		m_dwPolygonsCount = m_dwTotalIndices - 2; //스트립으로 그릴때는 2를빼준다
		//인덱스버퍼설정
		if(FAILED( m_pd3dDevice->CreateIndexBuffer(
			m_dwTotalIndices * sizeof(WORD),0,
			D3DFMT_INDEX16,D3DPOOL_DEFAULT, &m_pIB ,NULL ) ) )
		{ return false; }

		void *pIndices;
		m_pIB->Lock(0,0,(void**)&pIndices,0);
		memcpy( pIndices, pIndexValues, m_dwTotalIndices * sizeof(WORD));
		m_pIB->Unlock();

		delete []pIndexValues;

		return S_OK;
}

void CTerrain::TerrainRender()
{
	D3DXMATRIXA16	matTemp;

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );

	// Allow multiple passes to blend together correctly
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pd3dDevice->SetIndices(m_pIB);
	m_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP  );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP  );

	D3DXMatrixScaling( &matTemp, m_wWidth, m_wHeight, 1.0f );

	m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matTemp );
	

	//m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	

	m_pd3dDevice->SetTexture( 0,m_pBaseTexture);
	m_pd3dDevice->SetStreamSource( 0,m_pVB, 0,sizeof(CUSTOMVERTEX) );
	m_pd3dDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLESTRIP,		//프리미티브타입
		0,
		0,
		m_dwTotalVertices,
		0,
		m_dwPolygonsCount
		);

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
}

//---------------------------------

BOOL CTerrain::CreateAlphaMaps()
{
	for( int i=0 ; i<MAXTEXNUM ; ++i)
	{
		if( FAILED(m_pd3dDevice->CreateTexture( TEXALPHASIZE, TEXALPHASIZE, 1, 0, D3DFMT_A8R8G8B8, 
			D3DPOOL_MANAGED, &m_Texture[i].pAlphamap, NULL )) )
			return MessageBox(NULL, "alphaTexture make error", "Error", MB_OK);

		D3DLOCKED_RECT		AlphaMap_Locked;

		memset( &AlphaMap_Locked, 0, sizeof(D3DLOCKED_RECT) );

		if( FAILED(m_Texture[i].pAlphamap->LockRect(0, &AlphaMap_Locked, NULL, 0)) )
			return MessageBox(NULL, "alphaTexture Lock error", "Error", MB_OK);

		LPBYTE pDataDST = (LPBYTE) AlphaMap_Locked.pBits;

		for( int j=0; j<TEXALPHASIZE; ++j )
		{
			LPDWORD pDWordDST = (LPDWORD) (pDataDST + j * AlphaMap_Locked.Pitch);

			for( int i=0; i<TEXALPHASIZE; ++i )
			{
				*(pDWordDST + i) = 0x00000000;
			}
		}

		if( FAILED(m_Texture[i].pAlphamap->UnlockRect(0)) )	
			return MessageBox(NULL, "alphaTexture UnLock error", "Error", MB_OK);
	}

	return TRUE;
}

BOOL CTerrain::IntersectTriangle( const D3DXVECTOR3& orig,
								  const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
								  D3DXVECTOR3& v1, D3DXVECTOR3& v2,
								  FLOAT* t, FLOAT* u, FLOAT* v )
{
	// Find vectors for two edges sharing vert0
	D3DXVECTOR3 edge1 = v1 - v0;
	D3DXVECTOR3 edge2 = v2 - v0;

	// Begin calculating determinant - also used to calculate U parameter
	D3DXVECTOR3 pvec;
	D3DXVec3Cross( &pvec, &dir, &edge2 );

	// If determinant is near zero, ray lies in plane of triangle
	FLOAT det = D3DXVec3Dot( &edge1, &pvec );

	D3DXVECTOR3 tvec;
	if( det > 0 )
	{
		tvec = orig - v0;
	}
	else
	{
		tvec = v0 - orig;
		det = -det;
	}

	if( det < 0.0001f )
		return FALSE;

	// Calculate U parameter and test bounds
	*u = D3DXVec3Dot( &tvec, &pvec );
	if( *u < 0.0f || *u > det )
		return FALSE;

	// Prepare to test V parameter
	D3DXVECTOR3 qvec;
	D3DXVec3Cross( &qvec, &tvec, &edge1 );

	// Calculate V parameter and test bounds
	*v = D3DXVec3Dot( &dir, &qvec );
	if( *v < 0.0f || *u + *v > det )
		return FALSE;

	// Calculate t, scale parameters, ray intersects triangle
	*t = D3DXVec3Dot( &edge2, &qvec );
	FLOAT fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return TRUE;
}


//-----------------------------------------------------------------------------
// Picking Texture
//-----------------------------------------------------------------------------
void CTerrain::PickingProcess( RECT &rtRect , float fCurPosX, float fCurPosY )
{
	D3DXVECTOR3 v, vOrg, vDir, vPicked;
	D3DXMATRIXA16 matWorld,matView, matProj;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matProj);


	// Get Picking Ray.

	m_pd3dDevice->GetTransform( D3DTS_PROJECTION , &matProj );
	m_pd3dDevice->GetTransform( D3DTS_VIEW , &matView );

	D3DXMatrixInverse( &matWorld, NULL, &matView);

	FLOAT fWidth = FLOAT(rtRect.right - rtRect.left);
	FLOAT fHeight = FLOAT(rtRect.bottom - rtRect.top);	

	v.x = ( ( ( fCurPosX * 2.0f / fWidth  ) - 1.0f )) / matProj._11;
	v.y = (-( ( fCurPosY * 2.0f / fHeight ) - 1.0f )) / matProj._22;
	v.z = 1.0f;


	vDir.x = v.x*matWorld._11 + v.y*matWorld._21 + v.z*matWorld._31;
	vDir.y = v.x*matWorld._12 + v.y*matWorld._22 + v.z*matWorld._32;
	vDir.z = v.x*matWorld._13 + v.y*matWorld._23 + v.z*matWorld._33;

	vOrg.x = matWorld._41;  
	vOrg.y = matWorld._42;  
	vOrg.z = matWorld._43;


	D3DXVECTOR3 v0,v1,v2,v3;
	WORD x,y;
	float fDist;

	for( y =0 ; y<MAPHEIGHT ; ++y )
	{
		for( x=0; x<MAPWIDTH ; ++x)
		{
			v0= D3DXVECTOR3( m_pVertices[y*m_wWidthVertices+x].x, 
							 m_pVertices[y*m_wWidthVertices+x].y, 
							 m_pVertices[y*m_wWidthVertices+x].z );
			
			v1= D3DXVECTOR3( m_pVertices[(y+1)*m_wWidthVertices+x].x, 
							 m_pVertices[(y+1)*m_wWidthVertices+x].y, 
							 m_pVertices[(y+1)*m_wWidthVertices+x].z );
			
			v2= D3DXVECTOR3( m_pVertices[y*m_wWidthVertices+(x+1)].x, 
							 m_pVertices[y*m_wWidthVertices+(x+1)].y, 
							 m_pVertices[y*m_wWidthVertices+(x+1)].z );

			v3= D3DXVECTOR3( m_pVertices[(y+1)*m_wWidthVertices+(x+1)].x, 
				             m_pVertices[(y+1)*m_wWidthVertices+(x+1)].y, 
							 m_pVertices[(y+1)*m_wWidthVertices+(x+1)].z );

			if ( IntersectTriangle( vOrg, vDir,
				v0, v1, v2,
				&fDist, &m_fGetU, &m_fGetV ) ) 
			{
				m_vGetPos = vOrg  + vDir * fDist;
				m_blPickOK = TRUE;
				break;
			} 
			else if ( IntersectTriangle( vOrg, vDir,
				v1, v3, v2,
				&fDist, &m_fGetU, &m_fGetV ) )  
			{
				m_vGetPos = vOrg  + vDir * fDist;
				m_blPickOK = TRUE;
				break;
			} 
			else 
				m_blPickOK = FALSE;
		}
	}

}

//-----------------------------------------------------------------------------
// Draw the alphamaps
//-----------------------------------------------------------------------------
bool CTerrain::DrawAlphamaps( int nIndex ) 
{	
	// 알파맵의 한 픽셀이 월드 상의 한점의 크기를 구한다.
	// 1.0f <-현재 사각형1개만그렸으므로
	//float		PixSize		= 1.0f/(float)TEXALPHASIZE;
	float		PixSize		= (float)MAPWIDTH/(float)TEXALPHASIZE;

	//전역브러시 와 전역스무스는 현재 브러쉬할원의 지름을 넣기대문에 반지름을 사용해야한다.
	float 	    fHalfBrushSize	= m_fBrushSize/2.0f;
	float		fHalfSmoothSize = m_fSmoothSize/2.0f;

	// 에디터의 원의 최대 크기에 한점의 크기를 나누워서
	// 몇픽셀을 에디터 하는지 크기를 구한다.
	float		EditSize = fHalfBrushSize/PixSize;

	float	PosU = m_vGetPos.x / (float)(MAPWIDTH);
	float	PosV = 1 - m_vGetPos.z / (float)(MAPHEIGHT);
	//float	PosV = m_vGetPos.z / (float)(MAPHEIGHT);


	m_nTexPosX = int(PosU  * TEXALPHASIZE);
	m_nTexPosY = int(PosV  * TEXALPHASIZE);

	int StartPosX = int(( (m_nTexPosX - EditSize) < 0 )? 0 : m_nTexPosX - EditSize);
	int StartPosY = int(( (m_nTexPosY - EditSize) < 0 )? 0 : m_nTexPosY - EditSize);
	int EndPosX   = int(( (m_nTexPosX + EditSize) >= TEXALPHASIZE )? TEXALPHASIZE - 1: m_nTexPosX + EditSize);
	int EndPosY   = int(( (m_nTexPosY + EditSize) >= TEXALPHASIZE )? TEXALPHASIZE - 1: m_nTexPosY + EditSize);

	DWORD dwChangeColor = 0x00;
	float Smooth		= 0.0f;
	DWORD dwA			= 0x00;

	D3DLOCKED_RECT		AlphaMap_Locked;

	memset( &AlphaMap_Locked, 0, sizeof(D3DLOCKED_RECT) );

	if( FAILED(m_Texture[nIndex].pAlphamap->LockRect(0, &AlphaMap_Locked, NULL, 0)) )
	{
		MessageBox(NULL, "alphaTexture Lock error", "Error", MB_OK);
		return FALSE;
	}


	LPBYTE pDataDST = (LPBYTE) AlphaMap_Locked.pBits;

	for( int j=StartPosY; j<=EndPosY; ++j )
	{
		LPDWORD pDWordDST = (LPDWORD) (pDataDST + j * AlphaMap_Locked.Pitch);

		for( int i=StartPosX; i<=EndPosX; ++i )
		{
			D3DXVECTOR3 Pix = D3DXVECTOR3( i * PixSize, 0.0f, j * PixSize ) -
				D3DXVECTOR3( m_nTexPosX * PixSize, 0.0f, m_nTexPosY * PixSize ); 

			float Len = D3DXVec3Length( &Pix );

			if( Len <= fHalfSmoothSize )
			{
				dwChangeColor = 0xFFFFFFFF;
			}
			else if(  Len <= fHalfBrushSize  )
			{
				// 최대사이즈에 포함될 우 최대사이즈까지의
				// 선영보간한 알파값을 구한다.
				Len  -= fHalfSmoothSize;
				Smooth = fHalfBrushSize - fHalfSmoothSize;
				dwChangeColor= DWORD(( Smooth - Len ) / Smooth  * 0xFF000000);
				dwA = (dwChangeColor & 0xFF000000) >> 24;
				dwChangeColor = (dwA << 24) | (dwA << 16) | (dwA << 8) | dwA;
			}
			else
			{
				continue;
			}

			*(pDWordDST + i) = ( *(pDWordDST + i) < dwChangeColor ) ? dwChangeColor : *(pDWordDST + i);
		}
	}

	if( FAILED(m_Texture[nIndex].pAlphamap->UnlockRect(0)) )	
	{
		MessageBox(NULL, "alphaTexture UnLock error", "Error", MB_OK);
		return FALSE;
	}

	return TRUE;
}


//피킹할곳에 원을 그린다.
void CTerrain::DrawPickCircle( int Count, float size, D3DCOLOR Col )
{

	if ( Count < 4 ) Count = 4;
	float Step = (D3DX_PI * 2) / Count;

	Line3D          S_Line[2];
	D3DXVECTOR3     NewPos, Pos;
	D3DXMATRIXA16   matRot;

	Pos = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );

	//size도 원의 지름이 넘어오므로 반지름으로 만들어줘야한다
	size = size/2.0f;

	S_Line[1].Pos = Pos * size + m_vGetPos;
	S_Line[1].Col = S_Line[0].Col = Col;
	S_Line[1].Pos.y = 0.001f;
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

	for ( int i = 1; i < Count + 1; ++i) {
		S_Line[0].Pos = S_Line[1].Pos;

		D3DXMatrixRotationY( &matRot, i * Step );
		D3DXVec3TransformCoord( &NewPos, &Pos, &matRot );
		D3DXVec3Normalize( &NewPos, &NewPos );

		S_Line[1].Pos = (NewPos * size) + m_vGetPos;
		S_Line[1].Pos.y = 0.001f;

		

		m_pd3dDevice->SetFVF( D3DFVF_LINE3D );
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, S_Line, sizeof( Line3D ) );

		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	}
}


void CTerrain::AlphaTextureRender()
{
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	// Texture: take the color from the texture, take the alpha from the previous stage
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);

	m_pd3dDevice->SetIndices(m_pIB);
	m_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

	m_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// 0번 인덱스 알파 텍스쳐는 전체에 한번만 그려져야 하므로
	// 타일화 셋팅 하지 않음
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP  );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP  );

	D3DXMATRIXA16	matTemp;

	D3DXMatrixScaling( &matTemp, m_wWidth, m_wHeight, 1.0f );

	m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matTemp );
	m_pd3dDevice->SetTransform( D3DTS_TEXTURE1, &matTemp );


	for( int i=0 ; i<MAXTEXNUM ; ++i )
	{
		m_pd3dDevice->SetTexture(0, m_Texture[m_pDrawIndex[i]].pAlphamap);
		m_pd3dDevice->SetTexture(1, m_Texture[m_pDrawIndex[i]].pTexture);
		m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(CUSTOMVERTEX) );

		m_pd3dDevice->DrawIndexedPrimitive(
			D3DPT_TRIANGLESTRIP,		//프리미티브타입
			0,
			0,
			m_dwTotalVertices,
			0,
			m_dwPolygonsCount
			);

	}

	
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

	// 멀티텍스쳐 0,1번 셋팅을 사용을 중지 한다.
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	// 알파 사용을 중지 한다.
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
}

void CTerrain::MiniAlphaTextureRender()
{
	static float MiniAlphaTex[4][6] = 
	{
		{ WIDTH-MINIALPHASIZE,			0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
		{ WIDTH,						0.0f, 0.0f, 1.0f, 1.0f, 0.0f },
		{ WIDTH-MINIALPHASIZE, MINIALPHASIZE, 0.0f, 1.0f, 0.0f, 1.0f },
		{ WIDTH,			   MINIALPHASIZE, 0.0f, 1.0f, 1.0f, 1.0f },
	};

	m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );	
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTexture( 0, m_Texture[m_nCurSelIndex].pAlphamap );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, MiniAlphaTex, sizeof(MiniAlphaTex[0]) );	
}

//-----------------------------------------------------------------------------
// Load the terrain textures
//-----------------------------------------------------------------------------
bool CTerrain::LoadTextures() 
{
	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, "Detail.tga", &m_pBaseTexture))) {
		MessageBox(NULL, "Failed to load 'Detail.tga'", "Error", MB_OK);
		return FALSE;
	}

	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, "Terrain0.tga", &m_Texture[0].pTexture))) {
		MessageBox(NULL, "Failed to load 'Terrain0.tga'", "Error", MB_OK);
		return FALSE;
	}

	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, "Terrain1.tga", &m_Texture[1].pTexture))) {
		MessageBox(NULL, "Failed to load 'Terrain1.tga'", "Error", MB_OK);
		return FALSE;
	}

	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, "Terrain2.tga", &m_Texture[2].pTexture))) {
		MessageBox(NULL, "Failed to load 'Terrain2.tga'", "Error", MB_OK);
		return FALSE;
	}

	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, "Terrain3.tga", &m_Texture[3].pTexture))) {
		MessageBox(NULL, "Failed to load 'Terrain3.tga'", "Error", MB_OK);
		return FALSE;
	}

	CreateAlphaMaps();

	return true;
}

