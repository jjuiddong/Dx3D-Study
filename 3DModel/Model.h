//-----------------------------------------------------------------------------//
// 2005-10-04  programer: jaejung ┏(⊙д⊙)┛
// 
//-----------------------------------------------------------------------------//

#if !defined(__MODEL_H__)
#define __MODEL_H__


#include "collision.h"


//-----------------------------------------------------------------------------//
// CModel
//-----------------------------------------------------------------------------//
class CBone_;
class CBoneNode;
class CMesh;
class CModel : public ICollisionable
{
public:
	CModel();
	virtual ~CModel();

protected:
	ANI_TYPE m_eType;
	int m_ObjId;
	int m_nMeshCount;
	CMesh **m_pMesh;
	CBone_ *m_pBone;
	SAniLoader *m_pAniLoader;
	Vector3 m_Dir;					// 모델의 방향
	float m_fSize;					// 모델크기 (0-1 값)
	Matrix44 m_matWorld;
	Matrix44 m_matTmpWorld;
	Matrix44 m_matUpdate;			// 에니메이션 Matrix를 임시 저장하기 위한 변수
	Matrix44 *m_pLinkPTM;			// 다른 모델과 연결되었을때 설정되며, 부모 TM이다.
	Matrix44 *m_pLinkSTM;			// 다른 모델과 연결되었을때 설정되며, 부모의 서브 TM이다.
	BOOL m_bUpdateMatrix;			// 에니메이션이 끝난후 WorldMatrix를 업데이트하기 위한 플래그
	BOOL m_bQuickAni;

public:
	BOOL LoadModel( char *szFileName );
	BOOL LoadAni( char *szFileName, int nAniIdx=0, BOOL bLoop=FALSE );
	BOOL LoadDynamicModel( SBMMLoader *pLoader, BOOL bCollision=FALSE );
	BOOL LoadDynamicAni( SAniLoader *pAniLoader, int nAniIdx, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick );
	BOOL LoadDynamicAni( SAniLoader *pAniLoader, char *pAniName, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick );
	void SetAnimation( int nAniIdx, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick );
	void SetAnimation( char *pAniName, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick );
	void SetLink( CModel *pModel, CBoneNode *pBoneNode );
	CBoneNode* GetBoneNode( char *pBoneName );
	int  GetCurrentFrame();
	BOOL Animate( int nDelta );
	void Render();

	Matrix44* GetWoldTM() { return &m_matWorld; }
	void SetWorldTM( Matrix44 *pTM );
	void SetSize( const float size );
	void SetDirection( const Vector3& dir );		// rot: 라디안 값
	void SetDirection( const Quaternion& q );
	const Vector3& GetDirection() const { return m_Dir; }
	void SetPos( const Vector3 &pos );
	Vector3 GetPos();
	void MovePos( const Vector3 &pos );
	void MutliplyWorldTM( Matrix44 *pTM );
	void MutliplyBoneTM( int nBoneId, Matrix44 *pTM );
	void Clear();

	// ICollisionable 인터페이스 구현
	virtual BOOL IsTest( int TestNum ) { return TRUE; }
	virtual int GetObjId();
	virtual Matrix44* GetWorldTM();
	virtual Sphere* GetSphere() { return NULL; }
	virtual Sphere* GetMinimumSphere() { return NULL; }
	virtual void UpdateCollisionBox();
	virtual void GetCollisionBox( std::list<Box*> *pList );
	virtual void Collision( int TestNum, ICollisionable *pDst ) {}
};

#endif // __MODEL_H__
