#include "gpdef.h"
#include "gpstdlib.h"
#include "gpstdio.h"

int yo_atoi(char *s)
{
	int i,r;
	if (s==NULL) return 0;
	i=0;
	r=0;
	while (s[i])
	{
	 if ((s[i]>='0')&&(s[i]<='9')) r=r*10+(s[i]-'0');
	 i++;
	}
	return r;
}

/*special, part de la fin*/
char *yo_strrchr(char *s, char c)
{
	int i;
	if (s==NULL) return NULL;
	i=0;
	while (s[i]) i++;
	i--;
	while (i && (s[i]!=c) ) i--;
	if (s[i]!=c) return NULL;
	return &s[i];
}

void yo_strlwr(char *s)
{
   int i;
   if(s==NULL) return;
   i=0;
   while (s[i])
   {
   	if ((s[i]>='A') && (s[i]<='Z')) s[i]+='a'-'A';
   	i++;
   }
}

void yo_strupr(char *s)
{
   int i;
   if(s==NULL) return;
   i=0;
   while (s[i])
   {
   	if ((s[i]>='a') && (s[i]<='z')) s[i]+='A'-'a';
   	i++;
   }
}

int yo_strncmp(char *s1,char *s2, int lg)
{
	int i;
	if ((s1==NULL)||(s2==NULL)) return -1;
	i=0;
	while ((s1[i]==s2[i])&&((i+1)<lg))
	{
		if ((!s1[i])||(!s2[i])) break;
		i++;
	}
	return s1[i]-s2[i];
}
