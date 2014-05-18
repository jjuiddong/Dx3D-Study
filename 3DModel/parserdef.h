
#if !defined(__PARSERDEF_H__)
#define __PARSERDEF_H__

/*
	날짜: 2002-11-28 programer: 이재정
	날짜: 2003-09-19 programer: 이재정
		- refactoring
*/


#include <string.h>
#define SCRIPT_VERSION		"1.04"		// 2003-09-20


/////////////////////////////////////////////////////////////////////////
// 판별 문자

// 명령어 리스트
static char *g_szComlist = "{}";

// 주석 판별
static char *g_szComment = "/";

// 공백 문자 판별, 공백, tab, carriage return, line feed를 판별
static char *g_szWhite = " \t\n\r";

// 수치 자료 판별
static char *g_szDigit = "0123456789.";

// 명령어나 변수명 판별문자
static char *g_szAlpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz#_";

static char *g_szOperator = "[]#*";

// 콤마 판별
static char *g_szComma = ",";

// 문자열 판별문자
static char *g_szString = "'";

// 값설정 판별문자
static char *g_szAssign = "=";

/////////////////////////////////////////////////////////////////////////
// 판별함수

// 명령어 리스트 "{}"
static char* is_comlist( char c )
{
	return strchr( g_szComlist, c );
}

// 주석 판별 '//'
static char* is_comment( char c )
{
	return strchr( g_szComment, c );
}

// 공백 문자 판별, 공백, tab, carriage return, line feed를 판별
static char* is_white( char c )
{
	return strchr( g_szWhite, c );
}

// 수치 자료 판별
static char* is_digit( char c )
{
	return strchr( g_szDigit, c );
}

// 명령어와 변수명을 판별을 위한 문자 판별 함수
static char* is_alpha( char c )
{
	return strchr( g_szAlpha, c );
}

// 콤마 판별
static char* is_comma( char c )
{
	return strchr( g_szComma, c );
}

// 문자열 판별함수
static char* is_string( char c )
{
	return strchr( g_szString, c );
}

// 값설정 판별 '='
static char* is_assign( char c )
{
	return strchr( g_szAssign, c );
}

// 오퍼레이터 판별
static char* is_operator( char c )
{
	return strchr( g_szOperator, c );
}

#endif // __PARSERDEF_H__
