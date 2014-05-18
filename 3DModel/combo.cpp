
#include <windows.h>
#include "combo.h"
#include <queue>
using namespace std;


//-----------------------------------------------------------------------------//
// SActInfo 저장
//-----------------------------------------------------------------------------//
BOOL Write_ActInfoData( FILE *fp, SActInfo *pAct )
{
	queue< SActInfo* > q;
	q.push( pAct );
	while( !q.empty() )
	{
		SActInfo *p = q.front(); q.pop();
		for( int i=0; i < p->nextcount; ++i )
		{
			fwrite( p->next[ i], 1, sizeof(SActInfo), fp );
			q.push( p->next[ i] );
		}
	}
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 콤보트리를 파일에 저장 한다.
//-----------------------------------------------------------------------------//
BOOL Write_ComboData( char *pFileName, int ComboSize, SCombo *pCombo[] )
{
	if( 0 >= ComboSize ) return FALSE;
	if( !pCombo ) return FALSE;

	FILE *fp = fopen( pFileName, "wb" );
	if( !fp ) return FALSE;

	// 콤보갯수를 저장한다.
	fwrite( &ComboSize, 1, sizeof(int), fp );

	for( int i=0; i < ComboSize; ++i )
	{
		fwrite( pCombo[ i], 1, sizeof(SCombo), fp );
		Write_ActInfoData( fp, &pCombo[ i]->act );
	}

	fclose( fp );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 파일에 저장된 SActInfo 데이타를 읽어온다
//-----------------------------------------------------------------------------//
BOOL Read_ActInfoData( FILE *fp, SActInfo *pAct )
{
	queue< SActInfo* > q;
	q.push( pAct );

	while( !q.empty() )
	{
		SActInfo *act = q.front(); q.pop();
		for( int i=0; i < act->nextcount; ++i )
		{
			SActInfo *pnew = new SActInfo;
			ZeroMemory( pnew, sizeof(SActInfo) );
			int r = fread( pnew, 1, sizeof(SActInfo), fp );
			pnew->id = i;
			act->next[ i] = pnew;
			q.push( pnew );
		}
	}
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 저장된 콤보트리를 읽어온다.
// 콤보트리수를 리턴한다.
//-----------------------------------------------------------------------------//
int Read_ComboData( char *pFileName, SCombo *pCombo[] )
{
	FILE *fp = fopen( pFileName, "rb" );
	if( !fp ) return FALSE;
	
	// 콤보갯수를 저장한다.
	int ComboSize = 0;
	fread( &ComboSize, 1, sizeof(int), fp );

	for( int i=0; i < ComboSize; ++i )
	{
		pCombo[ i] = new SCombo;
		int s = fread( pCombo[ i], 1, sizeof(SCombo), fp );
		pCombo[ i]->id = i;
		Read_ActInfoData( fp, &pCombo[ i]->act );
	}

	fclose( fp );
	return ComboSize;
}

void Release_ActInfoRec2( SActInfo *pAct )
{
	if( !pAct ) return;
	for( int i=0; i < pAct->nextcount; ++i )
		Release_ActInfoRec2( pAct->next[ i] );
	delete pAct;
}

void Release_ActInfoRec1( SActInfo *pAct )
{
	for( int i=0; i < pAct->nextcount; ++i )
		Release_ActInfoRec2( pAct->next[ i] );
}

void Release_Combo( SCombo *pCombo )
{
	if( !pCombo ) return;
	Release_ActInfoRec1( &pCombo->act );
	delete pCombo;
}


BOOL Combo_CompareBtn( int SrcBtn[ MAX_BTN], int Btn, int PrevBtn )
{
	for( int i=0; i < MAX_BTN; ++i )
	{
		if( 0 == SrcBtn[ i] ) return FALSE;
		if( ((SrcBtn[ i] & KEY_PREV) && (Btn == PrevBtn)) || 
			((SrcBtn[ i] & KEY_NOT) && (Btn != PrevBtn) && (Btn & KEY_DIR)) || 
			( ((!(SrcBtn[ i] & KEY_PREV))) && 
			  ((!(SrcBtn[ i] & KEY_NOT))) && 
			  (SrcBtn[ i] == Btn)) )
			return TRUE;
	}
	return FALSE;
}
