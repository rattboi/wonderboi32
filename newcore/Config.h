//---------------------------------------------------------------------------
#ifndef ConfigH
#define ConfigH
//---------------------------------------------------------------------------
#include <stdio.h>

#define FNAME_LENGTH 512

extern AnsiString Command;					// 커멘드 라인
extern AnsiString AppDirectory;				// 어플리케이션 디렉토리명
extern char IniName[FNAME_LENGTH+1];		// 어플리케이션 초기화 파일명

extern AnsiString DefaultDir;				// 디폴트 디렉토리명
extern AnsiString Title;					// 카트리지명
extern char CartName[FNAME_LENGTH+1];		// 카트리지 파일명

#define LK_TMP 0
#define LK_SIO 1
#define LK_SOUND 2
#define LK_SPRITE 3
extern FILE *Log;
extern int LogKind;

extern int Verbose;							// 에러 표시 제어

//---------------------------------------------------------------------------
// ConfigCreate	환경의 초기화
// 인수 없음
// 반치없음
//
// 어플리케이션의 디렉토리를 설정
// 어플리케이션 윈도우 생성의 이벤트 핸들러에 실장한다
//---------------------------------------------------------------------------
extern "C" void __fastcall ConfigCreate(void);

//---------------------------------------------------------------------------
// ConfigRelease	환경의 설정
// 인수 없음
// 반치없음
//
// 어플리케이션의 디렉토리를 설정
// 어플리케이션 윈도우 생성의 이벤트 핸들러에 실장한다
//---------------------------------------------------------------------------
extern "C" void __fastcall ConfigRelease(void);

extern "C" void __fastcall LogFile(int Kind, char *string);
//---------------------------------------------------------------------------
#endif
