//---------------------------------------------------------------------------
#ifndef WSHARDH
#define WSHARDH
//---------------------------------------------------------------------------
// WS�� ���� ����
// �� ����� �����Ѵ�.
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// ������ ������ ����
//---------------------------------------------------------------------------
typedef signed char offset;

//---------------------------------------------------------------------------
// ������ �������̽� ����
//---------------------------------------------------------------------------
extern void *Debug;
extern ubyte *Page[0x10];		//���� address ���� 0 x10000 ����
extern ubyte *ROMMap[0x100];		//C-ROM ��ũ ��ȣ�� ���� C-ROM�� ������(1024���� ����Ʈ)
extern ubyte *RAMMap[0x100];		//C-RAM ��ũ ��ȣ�� ���� C-RAM�� ������(1024���� ����Ʈ)
extern ubyte IRAM[0x10000];		//���� IO������ 64kB
extern ubyte IO[0x100];		//IO���� ������

typedef void  (*WriteMemFn) (unsigned int A, byte V);
extern WriteMemFn WriteMemFnTable[0x10];
byte  ReadMem(unsigned int A);
void  WriteMem(unsigned int A, byte V);
void  WriteIO(unsigned A, byte V);

#define cpu_readop(A)				(ReadMem(A)) 									//8bit��read
#define cpu_readop_arg(A)			(ReadMem(A)) 									//8bit��read
#define cpu_readmem20(A)			(ReadMem(A)) 									//8bit��read
#define cpu_writemem20(A, D) 		(WriteMem((A), (byte)(D)))						//8bit������
#define cpu_readport(port)			(IO[(port)])									//8bit��read
#define cpu_writeport(port, val)		(WriteIO((port), (byte)(val)))					//8bit������
#define cpu_readport16(port)		(IO[(port)])									//8bit��read
#define cpu_writeport16(port, val)	(WriteIO((port), (byte)(val)))					//8bit������

#define TILE0	(IRAM+0x4000)	// Ÿ�� ������ ��ũ 0
#define TILE1	(IRAM+0x8000)	// Ÿ�� ������ ��ũ 1
#define COLTBL	(IRAM+0xFE00)	// Į�� ���̺�

//---------------------------------------------------------------------------
// I/Oڼ�áȽ�Ʃ��� ����
//---------------------------------------------------------------------------
#define DSPCTL	IO[0x00]		// ǥ�� ��Ʈ��
#define BORDER	IO[0x01]	 	// ����
#define RSTRL	IO[0x02]	 	// ��ȭ ����
#define RSTRLC	IO[0x03]	 	// ��ȭ ���� �񱳰�
#define SPRTAB	IO[0x04]		// ��������Ʈ ���̺� ���̽� �ּ�
#define SPRBGN	IO[0x05]		// ��������Ʈ ���� ��ȣ
#define SPRCNT	IO[0x06]		// ��������Ʈ ��ȭ��
#define SCRMAP	IO[0x07]		// �� ���̽� �ּ�
#define SCR2WL	IO[0x08]		// foreground ������ ��ǥ �»� X
#define SCR2WT	IO[0x09]		// foreground ������ ��ǥ �»� Y
#define SCR2WR	IO[0x0A]		// foreground ������ ��ǥ ���� X
#define SCR2WB	IO[0x0B]		// foreground ������ ��ǥ ���� Y
#define SPRWL	IO[0x0C]		// ��������Ʈ ������ ��ǥ �»� X
#define SPRWT	IO[0x0D]		// ��������Ʈ ������ ��ǥ �»� Y
#define SPRWR	IO[0x0E]		// ��������Ʈ ������ ��ǥ ���� X
#define SPRWB	IO[0x0F]		// ��������Ʈ ������ ��ǥ ���� Y
#define SCR1X	IO[0x10]		// ��׶��� X��ũ��
#define SCR1Y	IO[0x11]	 	// ��׶��� Y��ũ��
#define SCR2X	IO[0x12]	 	// foreground X��ũ��
#define SCR2Y	IO[0x13]	 	// foreground Y��ũ��
#define LCDSLP	IO[0x14]		// LCD ��Ʈ��
#define LCDSEG	IO[0x15]		// segment ǥ��

#define COL		(byte*)(IO+0x1C)	// �׷��� ������
#define PAL		(byte*)(IO+0x20)	// �ȷ�Ʈ

#define DMASL	IO[0x40]		// DMA ���� ���ּ� L
#define DMASH	IO[0x41]		// DMA ���� ���ּ� H
#define DMASB	IO[0x42]		// DMA ���� ����ũ
#define DMADB	IO[0x43]		// DMA ����ó ��ũ
#define DMADL	IO[0x44]		// DMA ����ó �ּ� L
#define DMADH	IO[0x45]		// DMA ����ó �ּ� H
#define DMACL	IO[0x46]		// DMA ���� ����Ʈ��L
#define DMACH	IO[0x47]		// DMA ���� ����Ʈ��H
#define DMACTL	IO[0x48]		// DMA ���� ��Ʈ��

#define SDMASL	IO[0x4A]		// ���� DMA ���� ���ּ� L
#define SDMASH	IO[0x4B]		// ���� DMA ���� ���ּ� H
#define SDMASB	IO[0x4C]		// ���� DMA ���� ����ũ
#define SDMACL	IO[0x4E]		// ���� DMA ���� ����Ʈ��L
#define SDMACH	IO[0x4F]		// ���� DMA ���� ����Ʈ��H
#define SDMACTL	IO[0x52]		// ���� DMA ���� ��Ʈ��

#define COLCTL	IO[0x60]		// ���� ���

#define SNDP	IO[0x80]		// ���� ��ġ
#define SNDV	IO[0x88]		// ���� ����
#define SNDSWP	IO[0x8C]		// ���� ������ ����
#define SWPSTP	IO[0x8D]		// ������ ����
#define NSCTL	IO[0x8E]		// ������ ��Ʈ��
#define WAVDTP	IO[0x8F]		// WAVE ���� ���̽� �ּ�
#define SNDMOD	IO[0x90]		// ���� ä�� ���
#define SNDOUT	IO[0x91]		// ���� ���
#define PCSRL	IO[0x92]		// ���׽� ī���� L
#define PCSRH	IO[0x93]		// ���׽� ī���� H

#define HWARCH	IO[0xA0]		// �ϵ���� Ÿ��

#define TIMCTL	IO[0xA2]		// ���� Ÿ�̸� ��Ʈ��
#define HPREL	IO[0xA4]		// ���� ���� pre-set L
#define HPREH	IO[0xA5]		// ���� ���� pre-set H
#define VPREL	IO[0xA6]		// ���� ���� pre-set L
#define VPREH	IO[0xA7]		// ���� ���� pre-set H
#define HCNTL	IO[0xA8]		// ���� ���� ī���� L
#define HCNTH	IO[0xA9]		// ���� ���� ī���� H
#define VCNTL	IO[0xAA]		// ���� ���� ī���� L
#define VCNTH	IO[0xAB]		// ���� ���� ī���� H

#define IRQBSE	IO[0xB0]		// ��ġ�� ���̽� ����
#define COMDT	IO[0xB1]		// �ø��� ������
#define IRQENA	IO[0xB2]		// ��ġ�� �㰡
#define COMCTL	IO[0xB3]		// �ø��� ��Ʈ��
#define KEYCTL	IO[0xB5]		// Ű �Է� ��Ʈ��
#define IRQACK	IO[0xB6]		// ��ġ�� �䱸 Ŭ����

#define EEPDTL	IO[0xBA]		// ���� EEPROM ������ L
#define EEPDTH	IO[0xBA]		// ���� EEPROM ������ H
#define EEPADL	IO[0xBA]		// ���� EEPROM �ּ� L
#define EEPADH	IO[0xBA]		// ���� EEPROM �ּ� H
#define EEPCTL	IO[0xBA]		// ���� EEPROM ��Ʈ��

#define BNK1SEL	IO[0xC1]		// ��ũ 1 �Ƿ���
#define BNK2SEL	IO[0xC2]		// ��ũ 2 �Ƿ���
#define BNK3SEL	IO[0xC3]		// ��ũ 3 �Ƿ���

//---------------------------------------------------------------------------
// ��ġ�� ��Ʈ�ѷ� ����
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
