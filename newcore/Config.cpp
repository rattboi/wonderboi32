//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#include <dir.h>
#pragma hdrstop

#include "Config.h"
#include "WSForm.h"
#include "WS.h"
#include "WSInput.h"
//---------------------------------------------------------------------------
AnsiString Command;					// 커멘드 라인
AnsiString AppDirectory;			// 어플리케이션 디렉토리명
char IniName[FNAME_LENGTH+1];		// 어플리케이션 초기화 파일명

AnsiString DefaultDir;				// 디폴트 디렉토리명
AnsiString Title;					// 카트리지명
char CartName[FNAME_LENGTH+1];		// 카트리지 파일명

FILE *Log;
int LogKind=LK_TMP;

int Verbose;						// 에러 표시 제어
//---------------------------------------------------------------------------
void __fastcall ConfigCreate(void)
{
	char buf[FNAME_LENGTH+1];
    AnsiString ini;
	FILE *fp;
    int i;

    CartName[0]='\0';

    AppDirectory=Command;
    i=AppDirectory.LastDelimiter(" ");
    AppDirectory.Delete(i, AppDirectory.Length() -i);
    i=AppDirectory.LastDelimiter("\\");
    AppDirectory.Delete(i+1, AppDirectory.Length() -i);
    AppDirectory.Delete(1,1);

    ini=AppDirectory+"WSCamp.INI";
    strcpy(IniName, ini.c_str());

    fp=fopen(IniName,"r");
    fclose(fp);
    if(fp==NULL)
    {
    	WritePrivateProfileString(NULL, NULL, NULL, IniName);
    }

    if(LogKind! =LK_TMP)
    {
    	strcpy(buf, AppDirectory.c_str());
        strcat(buf,"Log.txt");
    	Log=fopen(buf,"w");
    }

    GetPrivateProfileString("CONFIG","DefaultDir","", buf, FNAME_LENGTH, IniName);
    DefaultDir=buf;

    i=GetPrivateProfileInt("CONFIG","DrawSize",-1, IniName);
    if(i==-1)
    {
    	WritePrivateProfileString("CONFIG","DrawSize","0", IniName);
        MainForm->SetScale(1,0);
    }
    else if(i==3)
    {
        MainForm->SetScale(3,0);
    }
    else if(i==2)
    {
        MainForm->SetScale(2,0);
    }
    else
    {
        MainForm->SetScale(1,0);
    }

    i=GetPrivateProfileInt("CONFIG","FrameSkip",-1, IniName);
    if(i==-1)
    {
    	WritePrivateProfileString("CONFIG","FrameSkip","1", IniName);
        MainForm->SetSkip(1);
    }
    else if(i==4)
    {
        MainForm->SetSkip(4);
    }
    else if(i==3)
    {
        MainForm->SetSkip(3);
    }
    else if(i==2)
    {
        MainForm->SetSkip(2);
    }
    else if(i==1)
    {
        MainForm->SetSkip(1);
    }
    else
    {
        MainForm->SetSkip(0);
    }

    i=GetPrivateProfileInt("CONFIG","Sound1",-1, IniName);
    if(i==-1)
    {
    	WritePrivateProfileString("CONFIG","Sound1","1", IniName);
        MainForm->Channel1->Checked=true;
        SoundOn[0]=1;
    }
    else if(i)
    {
        MainForm->Channel1->Checked=true;
        SoundOn[0]=1;
    }
    else
    {
        MainForm->Channel1->Checked=false;
        SoundOn[0]=0;
    }
    i=GetPrivateProfileInt("CONFIG","Sound2",-1, IniName);
    if(i==-1)
    {
    	WritePrivateProfileString("CONFIG","Sound2","1", IniName);
        MainForm->Channel2->Checked=true;
        SoundOn[1]=1;
    }
    else if(i)
    {
        MainForm->Channel2->Checked=true;
        SoundOn[1]=1;
    }
    else
    {
        MainForm->Channel2->Checked=false;
        SoundOn[1]=0;
    }
    i=GetPrivateProfileInt("CONFIG","Sound3",-1, IniName);
    if(i==-1)
    {
    	WritePrivateProfileString("CONFIG","Sound3","1", IniName);
        MainForm->Channel3->Checked=true;
        SoundOn[2]=1;
    }
    else if(i)
    {
        MainForm->Channel3->Checked=true;
        SoundOn[2]=1;
    }
    else
    {
        MainForm->Channel3->Checked=false;
        SoundOn[2]=0;
    }
    i=GetPrivateProfileInt("CONFIG","Sound4",-1, IniName);
    if(i==-1)
    {
    	WritePrivateProfileString("CONFIG","Sound4","1", IniName);
        MainForm->Channel4->Checked=true;
        SoundOn[3]=1;
    }
    else if(i)
    {
        MainForm->Channel4->Checked=true;
        SoundOn[3]=1;
    }
    else
    {
        MainForm->Channel4->Checked=false;
        SoundOn[3]=0;
    }
    i=GetPrivateProfileInt("CONFIG","SoundNoise",-1, IniName);
    if(i==-1)
    {
    	WritePrivateProfileString("CONFIG","SoundNoise","1", IniName);
        MainForm->Noise->Checked=true;
        SoundOn[4]=1;
    }
    else if(i)
    {
        MainForm->Noise->Checked=true;
        SoundOn[4]=1;
    }
    else
    {
        MainForm->Noise->Checked=false;
        SoundOn[4]=0;
    }
    i=GetPrivateProfileInt("CONFIG","SoundPCM",-1, IniName);
    if(i==-1)
    {
    	WritePrivateProfileString("CONFIG","SoundPCM","1", IniName);
        MainForm->PCM->Checked=true;
        SoundOn[5]=1;
    }
    else if(i)
    {
        MainForm->PCM->Checked=true;
        SoundOn[5]=1;
    }
    else
    {
        MainForm->PCM->Checked=false;
        SoundOn[5]=0;
    }
    i=GetPrivateProfileInt("CONFIG","SoundDMA",-1, IniName);
    if(i==-1)
    {
    	WritePrivateProfileString("CONFIG","SoundDMA","1", IniName);
        MainForm->DMA->Checked=true;
        SoundOn[6]=1;
    }
    else if(i)
    {
        MainForm->DMA->Checked=true;
        SoundOn[6]=1;
    }
    else
    {
        MainForm->DMA->Checked=false;
        SoundOn[6]=0;
    }

    MainForm->Top=GetPrivateProfileInt("CONFIG","DrawTop", 0, IniName);
    if(MainForm->Top==0)
    {
    	WritePrivateProfileString("CONFIG","DrawTop","0", IniName);
    }

    MainForm->Left=GetPrivateProfileInt("CONFIG","DrawLeft", 0, IniName);
    if(MainForm->Left==0)
    {
    	WritePrivateProfileString("CONFIG","DrawLeft","0", IniName);
    }

    Verbose=GetPrivateProfileInt("CONFIG","Verbose", 0, IniName);
    if(Verbose==0)
    {
    	WritePrivateProfileString("CONFIG","Verbose","0", IniName);
    }


    GetPrivateProfileString("KEYBOARD","HYUp","", KeyConfig[0], 64, IniName);
    if(KeyConfig[0][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","HYUp","W", IniName);
        strcpy(KeyConfig[0],"W");
    }
    GetPrivateProfileString("KEYBOARD","HYRight","", KeyConfig[1], 64, IniName);
    if(KeyConfig[1][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","HYRight","D", IniName);
        strcpy(KeyConfig[1],"D");
    }
    GetPrivateProfileString("KEYBOARD","HYDown","", KeyConfig[2], 64, IniName);
    if(KeyConfig[2][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","HYDown","S", IniName);
        strcpy(KeyConfig[2],"S");
    }
    GetPrivateProfileString("KEYBOARD","HYLeft","", KeyConfig[3], 64, IniName);
    if(KeyConfig[3][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","HYLeft","A", IniName);
        strcpy(KeyConfig[3],"A");
    }
    GetPrivateProfileString("KEYBOARD","HXUp","", KeyConfig[4], 64, IniName);
    if(KeyConfig[4][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","HXUp","UP", IniName);
        strcpy(KeyConfig[4],"UP");
    }
    GetPrivateProfileString("KEYBOARD","HXRight","", KeyConfig[5], 64, IniName);
    if(KeyConfig[5][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","HXRight","RIGHT", IniName);
        strcpy(KeyConfig[5],"RIGHT");
    }
    GetPrivateProfileString("KEYBOARD","HXDown","", KeyConfig[6], 64, IniName);
    if(KeyConfig[6][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","HXDown","DOWN", IniName);
        strcpy(KeyConfig[6],"DOWN");
    }
    GetPrivateProfileString("KEYBOARD","HXLeft","", KeyConfig[7], 64, IniName);
    if(KeyConfig[7][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","HXLeft","LEFT", IniName);
        strcpy(KeyConfig[7],"LEFT");
    }
    GetPrivateProfileString("KEYBOARD","HOption","", KeyConfig[8], 64, IniName);
    if(KeyConfig[8][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","HOption","CONTROL", IniName);
        strcpy(KeyConfig[8],"CONTROL");
    }
    GetPrivateProfileString("KEYBOARD","HStart","", KeyConfig[9], 64, IniName);
    if(KeyConfig[9][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","HStart","RETURN", IniName);
        strcpy(KeyConfig[9],"RETURN");
    }
    GetPrivateProfileString("KEYBOARD","HAButton","", KeyConfig[10], 64, IniName);
    if(KeyConfig[10][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","HAButton","X", IniName);
        strcpy(KeyConfig[10],"X");
    }
    GetPrivateProfileString("KEYBOARD","HBButton","", KeyConfig[11], 64, IniName);
    if(KeyConfig[11][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","HBButton","Z", IniName);
        strcpy(KeyConfig[11],"Z");
    }

    GetPrivateProfileString("KEYBOARD","VYLeft","", KeyConfig[12], 64, IniName);
    if(KeyConfig[12][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","VYLeft","A", IniName);
        strcpy(KeyConfig[12],"A");
    }
    GetPrivateProfileString("KEYBOARD","VYUp","", KeyConfig[13], 64, IniName);
    if(KeyConfig[13][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","VYUp","W", IniName);
        strcpy(KeyConfig[13],"W");
    }
    GetPrivateProfileString("KEYBOARD","VYRight","", KeyConfig[14], 64, IniName);
    if(KeyConfig[14][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","VYRight","D", IniName);
        strcpy(KeyConfig[14],"D");
    }
    GetPrivateProfileString("KEYBOARD","VYDown","", KeyConfig[15], 64, IniName);
    if(KeyConfig[15][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","VYDown","S", IniName);
        strcpy(KeyConfig[15],"S");
    }
    GetPrivateProfileString("KEYBOARD","VXLeft","", KeyConfig[16], 64, IniName);
    if(KeyConfig[16][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","VXLeft","LEFT", IniName);
        strcpy(KeyConfig[16],"LEFT");
    }
    GetPrivateProfileString("KEYBOARD","VXUp","", KeyConfig[17], 64, IniName);
    if(KeyConfig[17][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","VXUp","UP", IniName);
        strcpy(KeyConfig[17],"UP");
    }
    GetPrivateProfileString("KEYBOARD","VXRight","", KeyConfig[18], 64, IniName);
    if(KeyConfig[18][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","VXRight","RIGHT", IniName);
        strcpy(KeyConfig[18],"RIGHT");
    }
    GetPrivateProfileString("KEYBOARD","VXDown","", KeyConfig[19], 64, IniName);
    if(KeyConfig[19][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","VXDown","DOWN", IniName);
        strcpy(KeyConfig[19],"DOWN");
    }
    GetPrivateProfileString("KEYBOARD","VOption","", KeyConfig[20], 64, IniName);
    if(KeyConfig[20][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","VOption","CONTROL", IniName);
        strcpy(KeyConfig[20],"CONTROL");
    }
    GetPrivateProfileString("KEYBOARD","VStart","", KeyConfig[21], 64, IniName);
    if(KeyConfig[21][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","VStart","RETURN", IniName);
        strcpy(KeyConfig[21],"RETURN");
    }
    GetPrivateProfileString("KEYBOARD","VAButton","", KeyConfig[22], 64, IniName);
    if(KeyConfig[22][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","VAButton","X", IniName);
        strcpy(KeyConfig[22],"X");
    }
    GetPrivateProfileString("KEYBOARD","VBButton","", KeyConfig[23], 64, IniName);
    if(KeyConfig[23][0]=='\0')
    {
    	WritePrivateProfileString("KEYBOARD","VBButton","Z", IniName);
        strcpy(KeyConfig[23],"Z");
    }


    GetPrivateProfileString("JOYPAD","HYUp","", JoyConfig[0], 64, IniName);
    if(JoyConfig[0][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","HYUp","6", IniName);
        strcpy(JoyConfig[0],"6");
    }
    GetPrivateProfileString("JOYPAD","HYRight","", JoyConfig[1], 64, IniName);
    if(JoyConfig[1][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","HYRight","8", IniName);
        strcpy(JoyConfig[1],"8");
    }
    GetPrivateProfileString("JOYPAD","HYDown","", JoyConfig[2], 64, IniName);
    if(JoyConfig[2][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","HYDown","3", IniName);
        strcpy(JoyConfig[2],"3");
    }
    GetPrivateProfileString("JOYPAD","HYLeft","", JoyConfig[3], 64, IniName);
    if(JoyConfig[3][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","HYLeft","7", IniName);
        strcpy(JoyConfig[3],"7");
    }
    GetPrivateProfileString("JOYPAD","HXUp","", JoyConfig[4], 64, IniName);
    if(JoyConfig[4][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","HXUp","UP", IniName);
        strcpy(JoyConfig[4],"UP");
    }
    GetPrivateProfileString("JOYPAD","HXRight","", JoyConfig[5], 64, IniName);
    if(JoyConfig[5][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","HXRight","RIGHT", IniName);
        strcpy(JoyConfig[5],"RIGHT");
    }
    GetPrivateProfileString("JOYPAD","HXDown","", JoyConfig[6], 64, IniName);
    if(JoyConfig[6][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","HXDown","DOWN", IniName);
        strcpy(JoyConfig[6],"DOWN");
    }
    GetPrivateProfileString("JOYPAD","HXLeft","", JoyConfig[7], 64, IniName);
    if(JoyConfig[7][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","HXLeft","LEFT", IniName);
        strcpy(JoyConfig[7],"LEFT");
    }
    GetPrivateProfileString("JOYPAD","HOption","", JoyConfig[8], 64, IniName);
    if(JoyConfig[8][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","HOption","4", IniName);
        strcpy(JoyConfig[8],"4");
    }
    GetPrivateProfileString("JOYPAD","HStart","", JoyConfig[9], 64, IniName);
    if(JoyConfig[9][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","HStart","5,9", IniName);
        strcpy(JoyConfig[9],"5,9");
    }
    GetPrivateProfileString("JOYPAD","HAButton","", JoyConfig[10], 64, IniName);
    if(JoyConfig[10][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","HAButton","2", IniName);
        strcpy(JoyConfig[10],"2");
    }
    GetPrivateProfileString("JOYPAD","HBButton","", JoyConfig[11], 64, IniName);
    if(JoyConfig[11][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","HBButton","1", IniName);
        strcpy(JoyConfig[11],"1");
    }

    GetPrivateProfileString("JOYPAD","VYLeft","", JoyConfig[12], 64, IniName);
    if(JoyConfig[12][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","VYLeft","LEFT", IniName);
        strcpy(JoyConfig[12],"LEFT");
    }
    GetPrivateProfileString("JOYPAD","VYUp","", JoyConfig[13], 64, IniName);
    if(JoyConfig[13][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","VYUp","UP", IniName);
        strcpy(JoyConfig[13],"UP");
    }
    GetPrivateProfileString("JOYPAD","VYRight","", JoyConfig[14], 64, IniName);
    if(JoyConfig[14][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","VYRight","RIGHT", IniName);
        strcpy(JoyConfig[14],"RIGHT");
    }
    GetPrivateProfileString("JOYPAD","VYDown","", JoyConfig[15], 64, IniName);
    if(JoyConfig[15][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","VYDown","DOWN", IniName);
        strcpy(JoyConfig[15],"DOWN");
    }
    GetPrivateProfileString("JOYPAD","VXLeft","", JoyConfig[16], 64, IniName);
    if(JoyConfig[16][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","VXLeft","1", IniName);
        strcpy(JoyConfig[16],"1");
    }
    GetPrivateProfileString("JOYPAD","VXUp","", JoyConfig[17], 64, IniName);
    if(JoyConfig[17][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","VXUp","4", IniName);
        strcpy(JoyConfig[17],"4");
    }
    GetPrivateProfileString("JOYPAD","VXRight","", JoyConfig[18], 64, IniName);
    if(JoyConfig[18][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","VXRight","5", IniName);
        strcpy(JoyConfig[18],"5");
    }
    GetPrivateProfileString("JOYPAD","VXDown","", JoyConfig[19], 64, IniName);
    if(JoyConfig[19][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","VXDown","2", IniName);
        strcpy(JoyConfig[19],"2");
    }
    GetPrivateProfileString("JOYPAD","VOption","", JoyConfig[20], 64, IniName);
    if(JoyConfig[20][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","VOption","3", IniName);
        strcpy(JoyConfig[20],"3");
    }
    GetPrivateProfileString("JOYPAD","VStart","", JoyConfig[21], 64, IniName);
    if(JoyConfig[21][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","VStart","6,9", IniName);
        strcpy(JoyConfig[21],"6,9");
    }
    GetPrivateProfileString("JOYPAD","VAButton","", JoyConfig[22], 64, IniName);
    if(JoyConfig[22][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","VAButton","8", IniName);
        strcpy(JoyConfig[22],"8");
    }
    GetPrivateProfileString("JOYPAD","VBButton","", JoyConfig[23], 64, IniName);
    if(JoyConfig[23][0]=='\0')
    {
    	WritePrivateProfileString("JOYPAD","VBButton","7", IniName);
        strcpy(JoyConfig[23],"7");
    }

    WritePrivateProfileString(NULL, NULL, NULL, IniName);

}

//---------------------------------------------------------------------------
void __fastcall ConfigRelease(void)
{
	char buf[FNAME_LENGTH+1];

    WritePrivateProfileString("CONFIG","DefaultDir", DefaultDir.c_str(), IniName);
    sprintf(buf,"%d", MainForm->Scale);
    WritePrivateProfileString("CONFIG","DrawSize", buf, IniName);
    sprintf(buf,"%d", FrameSkip);
    WritePrivateProfileString("CONFIG","FrameSkip", buf, IniName);

    sprintf(buf,"%d", SoundOn[0]);
    WritePrivateProfileString("CONFIG","Sound1", buf, IniName);
    sprintf(buf,"%d", SoundOn[1]);
    WritePrivateProfileString("CONFIG","Sound2", buf, IniName);
    sprintf(buf,"%d", SoundOn[2]);
    WritePrivateProfileString("CONFIG","Sound3", buf, IniName);
    sprintf(buf,"%d", SoundOn[3]);
    WritePrivateProfileString("CONFIG","Sound4", buf, IniName);
    sprintf(buf,"%d", SoundOn[4]);
    WritePrivateProfileString("CONFIG","SoundNoise", buf, IniName);
    sprintf(buf,"%d", SoundOn[5]);
    WritePrivateProfileString("CONFIG","SoundPCM", buf, IniName);
    sprintf(buf,"%d", SoundOn[6]);
    WritePrivateProfileString("CONFIG","SoundDMA", buf, IniName);

    sprintf(buf,"%d", MainForm->Top);
    WritePrivateProfileString("CONFIG","DrawTop", buf, IniName);
    sprintf(buf,"%d", MainForm->Left);
    WritePrivateProfileString("CONFIG","DrawLeft", buf, IniName);

	WritePrivateProfileString(NULL, NULL, NULL, IniName);

	if(Log)
    {
    	fclose(Log);
    }
    Log=NULL;
}

//---------------------------------------------------------------------------
void __fastcall LogFile(int Kind, char *string)
{
	if((LogKind==Kind)&&(Log))
    {
    	fprintf(Log, string);
    }
}

//---------------------------------------------------------------------------
