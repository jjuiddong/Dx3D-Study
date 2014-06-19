//-----------------------------------------------------------------------------//
// 2009-08-11  programer: jaejung ¦®(¢Á¬Õ¢Á)¦°
// 
//-----------------------------------------------------------------------------//

#if !defined(__UTILITY_H__)
#define __UTILITY_H__

#include <list>


BOOL CollectFile( std::list<std::string> *pFindExt, char *szDirectory, std::list<std::string> *pFileList );
void NormalizeRect( RECT *prt );
float Lerp(float p1, float p2, float alpha);


// file
BOOL CompareExtendName(char *srcFileName, int srcStringMaxLength, char *compareExtendName);
void ChangeExtendName(char *srcFileName, int srcStringMaxLength, char *extendName, char *newFileName, int newFileNameLength);
void GetFileNameNoPath(char *srcFileName, char *dest);
void GetFilePath(char *srcFileName, char *dest);


#endif // __UTILITY_H__
