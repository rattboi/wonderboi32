//---------------------------------------------------------------------------
#ifndef ConfigH
#define ConfigH
//---------------------------------------------------------------------------
#include <stdio.h>

#define FNAME_LENGTH 512

extern AnsiString Command;					// Ŀ��� ����
extern AnsiString AppDirectory;				// ���ø����̼� ���丮��
extern char IniName[FNAME_LENGTH+1];		// ���ø����̼� �ʱ�ȭ ���ϸ�

extern AnsiString DefaultDir;				// ����Ʈ ���丮��
extern AnsiString Title;					// īƮ������
extern char CartName[FNAME_LENGTH+1];		// īƮ���� ���ϸ�

#define LK_TMP 0
#define LK_SIO 1
#define LK_SOUND 2
#define LK_SPRITE 3
extern FILE *Log;
extern int LogKind;

extern int Verbose;							// ���� ǥ�� ����

//---------------------------------------------------------------------------
// ConfigCreate	ȯ���� �ʱ�ȭ
// �μ� ����
// ��ġ����
//
// ���ø����̼��� ���丮�� ����
// ���ø����̼� ������ ������ �̺�Ʈ �ڵ鷯�� �����Ѵ�
//---------------------------------------------------------------------------
extern "C" void __fastcall ConfigCreate(void);

//---------------------------------------------------------------------------
// ConfigRelease	ȯ���� ����
// �μ� ����
// ��ġ����
//
// ���ø����̼��� ���丮�� ����
// ���ø����̼� ������ ������ �̺�Ʈ �ڵ鷯�� �����Ѵ�
//---------------------------------------------------------------------------
extern "C" void __fastcall ConfigRelease(void);

extern "C" void __fastcall LogFile(int Kind, char *string);
//---------------------------------------------------------------------------
#endif
