//---------------------------------------------------------------------------
#ifndef WsSoundH
#define WsSoundH

//#include <mmsystem.h>
//#include <Dsound.h>
#include "WSHard.h"

extern int SwpTime;							// 스이후˚시간
extern int SwpStep;							// 스이후˚주기 스텟후˚
extern int MainVol;							// 현재의 메인 호″류무

//---------------------------------------------------------------------------
// WsSoundCreate DirectSound됫후″시″크트의 생성
// 인수 hWnd		:윈트″우의 헌트″르
// 반치 int			:정상 종료 0, 이상종료(ABEND)-1
//
// WS로 사용하는 오프″시″크트, 채널하″크하″파, 도 생성하는.
// 윈트″우 생성의 이헤″트한트″라 에 실장하는 것이 바람직하다.
// Create 함수는, 반드시 Release 함수에 의해 오프″시″크트를 개방해야 한다.
//
//	예)	WsDrawCreate(Form1->Handle);
//---------------------------------------------------------------------------
extern  int  WsSoundCreate();

//---------------------------------------------------------------------------
// WsSoundRelease DirectSound됫후″시″크트의 개방
// 인수 없음
// 반치없음
//
// GB로 사용하는 오프″시″크트, 채널하″크하″파, 를 해방하는.
// 윈트″우 소멸의 이헤″트한트″라 에 실장하는 것이 바람직하다.
// 또, 아후˚리케이션 예외 처리에 실장해 두는 것으로 안전하게 종료 시킬 수가 있다.
// 벌써 개방 끝난 경우는, 아무것도 하지 않는다.
//---------------------------------------------------------------------------
extern  void  WsSoundRelease(void);

//---------------------------------------------------------------------------
// WsSoundPlay 채널하″크하″파 기동
// 인수 없음
// 반치 int			:정상 종료 0, 이상종료(ABEND)-1
//
// GB음스크상에서 기동을 제어한다
//---------------------------------------------------------------------------
extern  int  WsSoundPlay(int Channel);

//---------------------------------------------------------------------------
// WsSoundStop 채널하″크하″파 정지
// 인수
// 반치 int			:정상 종료 0, 이상종료(ABEND)-1
//
// GB음스크상에서 정지를 제어한다
//---------------------------------------------------------------------------
extern  int  WsSoundStop(int Channel);

//---------------------------------------------------------------------------
// WsSoundClear 채널하″크하″파의 클리어
// 인수 없음
// 반치없음
//
// 채널하″크하″파를 무음 상태로 하는.
//---------------------------------------------------------------------------
extern  void  WsSoundClear(int Channel);

//---------------------------------------------------------------------------
extern  void  SetSoundFreq(int Channel, int Period);

extern  void  SetSoundVol(int Channel, int Vol);
//---------------------------------------------------------------------------
// SetSoundPan 채널하˚설정
// 인수 Left	왼쪽 음량(0∼7)
// 인수 Right	오른쪽 음량(0∼7)
// 반치없음
//
// 채널 좌우의 음량 설정
//---------------------------------------------------------------------------
extern  void  SetSoundPan(int Channel, int Left, int Right);

//---------------------------------------------------------------------------
extern  void  SetSoundPData(int Channel, int Index);

//---------------------------------------------------------------------------
// SetSoundPBuf 임의하˚턴 설정
// 인수 Addr	하˚탄아트″Respond(0∼15)
// 인수 Data	하˚탄테″타
// 반치없음
//
// 채널의 선택
//---------------------------------------------------------------------------
extern  void  SetSoundPBuf(int Addr, int Data);

//---------------------------------------------------------------------------
// WsSoundInt 1/256초 마다의 사운트″처리
// 인수 없음
// 반치 int			:하위 4拒″트가 사운트″ON敬라크″에 상당
//
//---------------------------------------------------------------------------
extern  int  WsSoundInt(void);

//---------------------------------------------------------------------------
extern  void  SetPCM(int Line);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
extern  void  FlashPCM(void);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// M계열 신호 발생기
// 불려 갈 때에 M계열 신호를 돌려줍니다
//---------------------------------------------------------------------------
extern  unsigned int  Mrand(unsigned int Degree);
//---------------------------------------------------------------------------

#endif
