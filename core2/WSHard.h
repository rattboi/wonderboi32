//---------------------------------------------------------------------------
#ifndef WSHARDH
#define WSHARDH
//---------------------------------------------------------------------------
// WS의 공통 정의
// 각 모듈이 공유한다.
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// 데이터 사이즈 정의
//---------------------------------------------------------------------------
typedef signed char offset;

//---------------------------------------------------------------------------
// 데이터 인터페이스 정의
//---------------------------------------------------------------------------
extern void *Debug;
extern ubyte *Page[0x10];		//리드 address 공간 0 x10000 단위
extern ubyte *ROMMap[0x100];		//C-ROM 뱅크 번호에 대한 C-ROM의 포인터(1024까지 서포트)
extern ubyte *RAMMap[0x100];		//C-RAM 뱅크 번호에 대한 C-RAM의 포인터(1024까지 서포트)
extern ubyte IRAM[0x10000];		//내장 IO공간의 64kB
extern ubyte IO[0x100];		//IO포토 에리어

typedef void  (*WriteMemFn) (unsigned int A, byte V);
extern WriteMemFn WriteMemFnTable[0x10];
byte  ReadMem(unsigned int A);
void  WriteMem(unsigned int A, byte V);
void  WriteIO(unsigned A, byte V);

#define cpu_readop(A)				(ReadMem(A)) 									//8bit　read
#define cpu_readop_arg(A)			(ReadMem(A)) 									//8bit　read
#define cpu_readmem20(A)			(ReadMem(A)) 									//8bit　read
#define cpu_writemem20(A, D) 		(WriteMem((A), (byte)(D)))						//8bit　기입
#define cpu_readport(port)			(IO[(port)])									//8bit　read
#define cpu_writeport(port, val)		(WriteIO((port), (byte)(val)))					//8bit　기입
#define cpu_readport16(port)		(IO[(port)])									//8bit　read
#define cpu_writeport16(port, val)	(WriteIO((port), (byte)(val)))					//8bit　기입

#define TILE0	(IRAM+0x4000)	// 타일 데이터 뱅크 0
#define TILE1	(IRAM+0x8000)	// 타일 데이터 뱅크 1
#define COLTBL	(IRAM+0xFE00)	// 칼라 테이블

//---------------------------------------------------------------------------
// I/O靡시″스튜디오 정의
//---------------------------------------------------------------------------
#define DSPCTL	IO[0x00]		// 표시 콘트롤
#define BORDER	IO[0x01]	 	// 경계색
#define RSTRL	IO[0x02]	 	// 묘화 라인
#define RSTRLC	IO[0x03]	 	// 묘화 라인 비교값
#define SPRTAB	IO[0x04]		// 스프라이트 테이블 베이스 주소
#define SPRBGN	IO[0x05]		// 스프라이트 개시 번호
#define SPRCNT	IO[0x06]		// 스프라이트 묘화수
#define SCRMAP	IO[0x07]		// 맵 베이스 주소
#define SCR2WL	IO[0x08]		// foreground 윈도우 좌표 좌상 X
#define SCR2WT	IO[0x09]		// foreground 윈도우 좌표 좌상 Y
#define SCR2WR	IO[0x0A]		// foreground 윈도우 좌표 우하 X
#define SCR2WB	IO[0x0B]		// foreground 윈도우 좌표 우하 Y
#define SPRWL	IO[0x0C]		// 스프라이트 윈도우 좌표 좌상 X
#define SPRWT	IO[0x0D]		// 스프라이트 윈도우 좌표 좌상 Y
#define SPRWR	IO[0x0E]		// 스프라이트 윈도우 좌표 우하 X
#define SPRWB	IO[0x0F]		// 스프라이트 윈도우 좌표 우하 Y
#define SCR1X	IO[0x10]		// 백그라운드 X스크롤
#define SCR1Y	IO[0x11]	 	// 백그라운드 Y스크롤
#define SCR2X	IO[0x12]	 	// foreground X스크롤
#define SCR2Y	IO[0x13]	 	// foreground Y스크롤
#define LCDSLP	IO[0x14]		// LCD 콘트롤
#define LCDSEG	IO[0x15]		// segment 표시

#define COL		(byte*)(IO+0x1C)	// 그레이 스케일
#define PAL		(byte*)(IO+0x20)	// 팔레트

#define DMASL	IO[0x40]		// DMA 전송 전주소 L
#define DMASH	IO[0x41]		// DMA 전송 전주소 H
#define DMASB	IO[0x42]		// DMA 전송 전뱅크
#define DMADB	IO[0x43]		// DMA 전송처 뱅크
#define DMADL	IO[0x44]		// DMA 전송처 주소 L
#define DMADH	IO[0x45]		// DMA 전송처 주소 H
#define DMACL	IO[0x46]		// DMA 전송 바이트수L
#define DMACH	IO[0x47]		// DMA 전송 바이트수H
#define DMACTL	IO[0x48]		// DMA 전송 콘트롤

#define SDMASL	IO[0x4A]		// 사운드 DMA 전송 전주소 L
#define SDMASH	IO[0x4B]		// 사운드 DMA 전송 전주소 H
#define SDMASB	IO[0x4C]		// 사운드 DMA 전송 전뱅크
#define SDMACL	IO[0x4E]		// 사운드 DMA 전송 바이트수L
#define SDMACH	IO[0x4F]		// 사운드 DMA 전송 바이트수H
#define SDMACTL	IO[0x52]		// 사운드 DMA 전송 콘트롤

#define COLCTL	IO[0x60]		// 비디오 모드

#define SNDP	IO[0x80]		// 사운드 피치
#define SNDV	IO[0x88]		// 사운드 볼륨
#define SNDSWP	IO[0x8C]		// 사운드 스위프 볼륨
#define SWPSTP	IO[0x8D]		// 스위프 스텝
#define NSCTL	IO[0x8E]		// 노이즈 콘트롤
#define WAVDTP	IO[0x8F]		// WAVE 패턴 베이스 주소
#define SNDMOD	IO[0x90]		// 사운드 채널 모드
#define SNDOUT	IO[0x91]		// 사운드 출력
#define PCSRL	IO[0x92]		// 다항식 카운터 L
#define PCSRH	IO[0x93]		// 다항식 카운터 H

#define HWARCH	IO[0xA0]		// 하드웨어 타입

#define TIMCTL	IO[0xA2]		// 공백 타이머 콘트롤
#define HPREL	IO[0xA4]		// 수평 공백 pre-set L
#define HPREH	IO[0xA5]		// 수평 공백 pre-set H
#define VPREL	IO[0xA6]		// 수직 공백 pre-set L
#define VPREH	IO[0xA7]		// 수직 공백 pre-set H
#define HCNTL	IO[0xA8]		// 수평 공백 카운터 L
#define HCNTH	IO[0xA9]		// 수평 공백 카운터 H
#define VCNTL	IO[0xAA]		// 수직 공백 카운터 L
#define VCNTH	IO[0xAB]		// 수직 공백 카운터 H

#define IRQBSE	IO[0xB0]		// 세치기 베이스 벡터
#define COMDT	IO[0xB1]		// 시리얼 데이터
#define IRQENA	IO[0xB2]		// 세치기 허가
#define COMCTL	IO[0xB3]		// 시리얼 콘트롤
#define KEYCTL	IO[0xB5]		// 키 입력 콘트롤
#define IRQACK	IO[0xB6]		// 세치기 요구 클리어

#define EEPDTL	IO[0xBA]		// 내장 EEPROM 데이터 L
#define EEPDTH	IO[0xBA]		// 내장 EEPROM 데이터 H
#define EEPADL	IO[0xBA]		// 내장 EEPROM 주소 L
#define EEPADH	IO[0xBA]		// 내장 EEPROM 주소 H
#define EEPCTL	IO[0xBA]		// 내장 EEPROM 콘트롤

#define BNK1SEL	IO[0xC1]		// 뱅크 1 실렉터
#define BNK2SEL	IO[0xC2]		// 뱅크 2 실렉터
#define BNK3SEL	IO[0xC3]		// 뱅크 3 실렉터

//---------------------------------------------------------------------------
// 세치기 콘트롤러 정의
//---------------------------------------------------------------------------
#define TXD_IFLAG 0x01
#define KEY_IFLAG 0x02
#define CRT_IFLAG 0x04
#define RXD_IFLAG 0x08
#define RST_IFLAG 0x10
#define VTM_IFLAG 0x20
#define VBB_IFLAG 0x40
#define HTM_IFLAG 0x80

//---------------------------------------------------------------------------
#endif
