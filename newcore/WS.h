//---------------------------------------------------------------------------
#ifndef GBH
#define GBH
#include "WSHard.h"
//---------------------------------------------------------------------------

extern int RunFlag;					// GB실행 프랙션″
extern int RunCount;
extern int SoundOn[7];
extern int FrameSkip;

//---------------------------------------------------------------------------
// GBCreate GB됫후″시″크트의 생성
// 인수 CartName	:카트릿시″파일명(테″레크트리+파일명+". gb")
// 반치 int			:정상 종료 0, 이상종료(ABEND)-1
//
// GB로 사용하는 메모리 환경을 생성한다.
// 윈트″우 생성의 이헤″트한트″라 에 실장하는 것이 바람직하다.
// Create 함수는, 반드시 Release 함수에 의해 오프″시″크트를 개방해야 한다.
//---------------------------------------------------------------------------
extern "C" int WsCreate(char *CartName);

//---------------------------------------------------------------------------
// GBRun GB의 실행
// 인수 없음
// 반치 int			:정상 종료 0, 이상종료(ABEND)-1
//
// GB를 실행한다.
// 종료, 정지 코만트″, 또는 정지 에러가 발생할 경우에 복귀한다.
//---------------------------------------------------------------------------
extern "C" int WsRun(void);

//---------------------------------------------------------------------------
// GBCreate GB됫후″시″크트의 개방
// 인수 없음
// 반치없음
//
// GB로 사용한 메모리 환경을 개방한다.
// 윈트″우 소멸의 이헤″트한트″라 에 실장하는 것이 바람직하다.
// 또, 아후˚리케이션 예외 처리에 실장해 두는 것으로 안전하게 종료 시킬 수가 있다.
// 벌써 개방 끝난 경우는, 아무것도 하지 않는다.
//---------------------------------------------------------------------------
extern "C" void WsRelease(void);

#endif
