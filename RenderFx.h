// PocketWS rendering functions
// to do : Mono-mode rendering
// 2004. 4. 4. trapung



void render16PackedMode(int Line, void *lpSurface);
void render16LayeredMode(int Line, void *lpSurface);
void render4ColorMode(int Line, void *lpSurface);
void renderMonoMode(int Line, void *lpSurface);

void (*renderLineCE[4]) (int Line, void *lpSurface) = {
	render4ColorMode,
	render4ColorMode,
	render16LayeredMode,
	render16PackedMode
};

void render16PackedMode(int Line, void *lpSurface) {
// COLCTL = 0xE0
	register unsigned short *pSBuf;			// 서페스하″파의 (0, Y) 좌표호˚인터(8艱″실 포함하지 않고)
	register unsigned short *pSWrBuf;			// 서페스하″파의 라이트호˚인터
	register unsigned short * lpScBf = ScreenBuffer;

	int *pZ;				// ZBuf의 라이트호˚인터
	int ZBuf[0x100];		// Z絿스크
	int *pW;				// write-pointer of WBuf 
	int WBuf[0x100];		// Window Mask

	int OffsetX;			// X좌표 오프셋(0∼7)
	int OffsetY;			// Y좌표 오프셋(0∼E)

	register byte *pbTMap;			// 독 붐비어 타이르맛후˚의 호˚인터
	int TMap;				// 타이르맛후˚
	int TMapX;				// 타이르맛후˚의 메쉬 X값
	int TMapXEnd;			// 타이르맛후˚의 하″크크″라운트″영역 종료의 메쉬 X값

	register byte *pbTData;			// 독 붐비어 타이르테″타의 호˚인터


    unsigned int i, j, k, index[8];

	unsigned short *pbPal;			// 하˚렛호˚인터
	unsigned short defpal[1];

	defpal[0] = 0;
	pSBuf=(lpScBf+Line*240)+8;
	pSWrBuf=pSBuf;
	if(LCDSLP&0x01) pbPal=&Palette[(BORDER&0xF0)>>4][BORDER&0x0F];
	else pbPal=defpal;

	unsigned short bkCol = pbPal[0];

	if(! (LCDSLP&0x01)) return;

 	if(DSPCTL&0x01)	// render background layer
	{
		OffsetX=SCR1X&0x07;						// X좌표 오프셋을 설정
		pSWrBuf=pSBuf-OffsetX;					// 서페스하″파의 기입호˚인터를 X됫후셋트
		i=Line+SCR1Y;
		OffsetY=(i&0x07);						// Y좌표 오프셋을 설정

		pbTMap=Scr1TMap+((i&0xF8)<<3);			// 타이르맛후˚의 (0, Y) 좌표호˚인터
		TMapX=(SCR1X&0xF8)>>2;					// 타이르맛후˚의 X값을 설정
		TMapXEnd=((SCR1X+224+7)>>2) &0xFFE;		// 하″크크″라운트″영역 종료의 X값을 설정

		for(;TMapX<TMapXEnd;)					// 하″크크″라운트″영역을 묘화
		{
			TMap=*(pbTMap+(TMapX++&0x3F));		// 타이르맛후˚를 읽어들인다
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;	// 타이르맛후˚를 읽어들인다

			pbTData = (TMap&MAP_BANK) ? (IRAM+0x08000) : (IRAM+0x4000);
			pbTData+= (TMap&MAP_TILE)<<5;
			pbTData+= (TMap&MAP_VREV) ? ((7-OffsetY)<<2) : (OffsetY<<2);

			unsigned short * palt = Palette[(TMap&MAP_PAL)>>9];

			if(TMap&MAP_HREV) {
				*pSWrBuf++ = (j=((pbTData[3]&0x0F)))	? palt[j] : bkCol;
				*pSWrBuf++ = (j=((pbTData[3]&0xF0)>>4)) ? palt[j] : bkCol;
				*pSWrBuf++ = (j=((pbTData[2]&0x0F)))	? palt[j] : bkCol;
				*pSWrBuf++ = (j=((pbTData[2]&0xF0)>>4)) ? palt[j] : bkCol;
				*pSWrBuf++ = (j=((pbTData[1]&0x0F)))	? palt[j] : bkCol;
				*pSWrBuf++ = (j=((pbTData[1]&0xF0)>>4)) ? palt[j] : bkCol;
				*pSWrBuf++ = (j=((pbTData[0]&0x0F)))	? palt[j] : bkCol;
				*pSWrBuf++ = (j=((pbTData[0]&0xF0)>>4)) ? palt[j] : bkCol;
			} else	{
				*pSWrBuf++ = (j=((pbTData[0]&0xF0)>>4)) ? palt[j] : bkCol;
				*pSWrBuf++ = (j=((pbTData[0]&0x0F)))	? palt[j] : bkCol;
				*pSWrBuf++ = (j=((pbTData[1]&0xF0)>>4)) ? palt[j] : bkCol;
				*pSWrBuf++ = (j=((pbTData[1]&0x0F)))	? palt[j] : bkCol;
				*pSWrBuf++ = (j=((pbTData[2]&0xF0)>>4)) ? palt[j] : bkCol;
				*pSWrBuf++ = (j=((pbTData[2]&0x0F)))	? palt[j] : bkCol;
				*pSWrBuf++ = (j=((pbTData[3]&0xF0)>>4)) ? palt[j] : bkCol;
				*pSWrBuf++ = (j=((pbTData[3]&0x0F)))	? palt[j] : bkCol;
			}

		}
	}

	for (i=0;i<0x100;i++) ZBuf[i] = 0;

	if(DSPCTL&0x02)
	{
		if((DSPCTL&0x30) ==0x20)
		{
            for(i=0, pW=WBuf+8;i<224;i++)
				*pW++=1;
			if((Line>=SCR2WT)&&(Line<=SCR2WB))
				for(i=SCR2WL, pW=WBuf+8+i;i<=SCR2WR;i++)
					*pW++=0;
		} 
		else if((DSPCTL&0x30) ==0x30) 
		{
            for(i=0, pW=WBuf+8;i<224;i++)
				*pW++=0;
			if((Line>=SCR2WT)&&(Line<=SCR2WB))
            	for(i=SCR2WL, pW=WBuf+8+i;i<=SCR2WR;i++)
					*pW++=1;
		}
		else 
		{
			for(i=0, pW=WBuf+8;i<0x100;i++)
				*pW++=0;
		}

		OffsetX=SCR2X&0x07;						// X좌표 오프셋을 설정
		pSWrBuf=pSBuf-OffsetX;					// 서페스하″파의 기입호˚인터를 X됫후셋트
		i=Line+SCR2Y;
		OffsetY=(i&0x07);						// Y좌표 오프셋을 설정

		pbTMap=Scr2TMap+((i&0xF8)<<3);			// 타이르맛후˚의 (0, Y) 좌표호˚인터
		TMapX=(SCR2X&0xF8)>>2;					// 타이르맛후˚의 X값을 설정
		TMapXEnd=((SCR2X+224+7)>>2) &0xFFE;				// 하″크크″라운트″영역 종료의 X값을 설정

		pW=WBuf+8-OffsetX;
		pZ=ZBuf+8-OffsetX;
        
		for(;TMapX<TMapXEnd;)					// 하″크크″라운트″영역을 묘화
		{
			TMap=*(pbTMap+(TMapX++&0x3F));		// 타이르맛후˚를 읽어들인다
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;	// 타이르맛후˚를 읽어들인다

			pbTData = (TMap&MAP_BANK) ? (IRAM+0x08000) : (IRAM+0x4000);
			pbTData+= (TMap&MAP_TILE)<<5;
			pbTData+= (TMap&MAP_VREV) ? ((7-OffsetY)<<2) : (OffsetY<<2);

			unsigned short * palt = Palette[(TMap&MAP_PAL)>>9];
			
            if(TMap&MAP_HREV) {
				if (!pW[0]&&(j=(pbTData[3]&0x0F)))	  { pZ[0]=1; pSWrBuf[0]=palt[j]; }
				if (!pW[1]&&(j=(pbTData[3]&0xF0)>>4)) { pZ[1]=1; pSWrBuf[1]=palt[j]; }
				if (!pW[2]&&(j=(pbTData[2]&0x0F)))	  { pZ[2]=1; pSWrBuf[2]=palt[j]; }
				if (!pW[3]&&(j=(pbTData[2]&0xF0)>>4)) { pZ[3]=1; pSWrBuf[3]=palt[j]; }
				if (!pW[4]&&(j=(pbTData[1]&0x0F)))	  { pZ[4]=1; pSWrBuf[4]=palt[j]; }
				if (!pW[5]&&(j=(pbTData[1]&0xF0)>>4)) { pZ[5]=1; pSWrBuf[5]=palt[j]; }
				if (!pW[6]&&(j=(pbTData[0]&0x0F)))	  { pZ[6]=1; pSWrBuf[6]=palt[j]; }
				if (!pW[7]&&(j=(pbTData[0]&0xF0)>>4)) { pZ[7]=1; pSWrBuf[7]=palt[j]; }
			} else {
				if (!pW[0]&&(j=(pbTData[0]&0xF0)>>4)) { pZ[0]=1; pSWrBuf[0]=palt[j]; }
				if (!pW[1]&&(j=(pbTData[0]&0x0F)))	  { pZ[1]=1; pSWrBuf[1]=palt[j]; }
				if (!pW[2]&&(j=(pbTData[1]&0xF0)>>4)) { pZ[2]=1; pSWrBuf[2]=palt[j]; }
				if (!pW[3]&&(j=(pbTData[1]&0x0F)))	  { pZ[3]=1; pSWrBuf[3]=palt[j]; }
				if (!pW[4]&&(j=(pbTData[2]&0xF0)>>4)) { pZ[4]=1; pSWrBuf[4]=palt[j]; }
				if (!pW[5]&&(j=(pbTData[2]&0x0F)))	  { pZ[5]=1; pSWrBuf[5]=palt[j]; }
				if (!pW[6]&&(j=(pbTData[3]&0xF0)>>4)) { pZ[6]=1; pSWrBuf[6]=palt[j]; }
				if (!pW[7]&&(j=(pbTData[3]&0x0F)))	  { pZ[7]=1; pSWrBuf[7]=palt[j]; }
			} 

			pW += 8;
			pZ += 8;
			pSWrBuf += 8;
		}
	}

	if(DSPCTL&0x04)  // Sprite On/Off
	{
		if(DSPCTL&0x08)	// Sprite Window On/Off
        {
			
            for(i=0, pW=WBuf+8;i<224;i++)
			{
        		*pW++=1;
			}
			if((Line>=SPRWT)&&(Line<=SPRWB))
			//if ( ((unsigned int)(Line-SPRWT)) <= ((unsigned int)(SPRWB-SPRWT)) )
            {
				for(i=SPRWL, pW=WBuf+8+i;i<=SPRWR;i++)
            	{
            		*pW++=0;
            	}
            }
			
        }
				// for cancel
				// (DSPCTL&0x08)&&(TMap&SPR_CLIP)&&(!*pW)
				// (DSPCTL&0x08)&&(!(TMap&SPR_CLIP))&&(*pW)
				// (!index[i])
				// (*pZ)&&(!(TMap&SPR_LAYR))
if (DSPCTL&0x08) {
/*
		for(pbTMap=SprETMap;pbTMap>=SprTTMap;pbTMap-=4) // render sprites
		{
			TMap=pbTMap[0];TMap|=pbTMap[1]<<8;					
			if(pbTMap[2]>0xF8) j=pbTMap[2]-0x100; else j=pbTMap[2];
			if(pbTMap[3]>0xF8) k=pbTMap[3]-0x100; else k=pbTMap[3];
			if(Line<j) continue;
			if(Line>=j+8) continue;
			if(224<=k) continue;
			i=k;
			pSWrBuf=pSBuf+i;
           	pbTData=IRAM+0x4000;
            pbTData+=(TMap&SPR_TILE)<<5;
			if(TMap&SPR_VREV) pbTData+=(7-Line+j)<<2;else pbTData+=(Line-j)<<2;

			if(TMap&SPR_HREV) {
				index[0]=(pbTData[0]&0xF0)>>4;
				index[1]=pbTData[0]&0x0F;
				index[2]=(pbTData[1]&0xF0)>>4;
				index[3]=pbTData[1]&0x0F;
				index[4]=(pbTData[2]&0xF0)>>4;
				index[5]=pbTData[2]&0x0F;
				index[6]=(pbTData[3]&0xF0)>>4;
				index[7]=pbTData[3]&0x0F; 
			} else {
				index[7]=(pbTData[0]&0xF0)>>4;
				index[6]=pbTData[0]&0x0F;
				index[5]=(pbTData[1]&0xF0)>>4;
				index[4]=pbTData[1]&0x0F;
				index[3]=(pbTData[2]&0xF0)>>4;
				index[2]=pbTData[2]&0x0F;
				index[1]=(pbTData[3]&0xF0)>>4;
				index[0]=pbTData[3]&0x0F; 
			}

			pW=WBuf+8+k;
			pZ=ZBuf+k+8;
			unsigned short * palt = Palette[((TMap&SPR_PAL)>>9)+8];

            for(i=0;i<8;i++, pZ++, pW++)
            {
           		if(TMap&SPR_CLIP)	// sprite window clipping on 
           		{
               		if(! *pW)
               		{
               			pSWrBuf++;
						continue;
               		}
           		}
           		else				// sprite window clipping off
				{
					if(*pW)
					{
						pSWrBuf++;
               			continue;
					}
           		}


				if( (!index[i]) )
                {
                	pSWrBuf++;
                    continue;
                }

				if((*pZ)&&(! (TMap&SPR_LAYR)))
                {
                	pSWrBuf++;
                    continue;
                }

                *pSWrBuf++=Palette[((TMap&SPR_PAL)>>9)+8][index[i]];
			}
		}
*/
}  // DSPCTL&0x08 On
else {
/*
		for(pbTMap=SprETMap;pbTMap>=SprTTMap;pbTMap-=4) // render sprites
		{
			TMap=pbTMap[0];TMap|=pbTMap[1]<<8;					
			if(pbTMap[2]>0xF8) j=pbTMap[2]-0x100; else j=pbTMap[2];
			if(pbTMap[3]>0xF8) k=pbTMap[3]-0x100; else k=pbTMap[3];
			if(Line<j) continue;
			if(Line>=j+8) continue;
			if(224<=k) continue;
			i=k;
			pSWrBuf=pSBuf+i;
           	pbTData=IRAM+0x4000;
            pbTData+=(TMap&SPR_TILE)<<5;
			if(TMap&SPR_VREV) pbTData+=(7-Line+j)<<2;else pbTData+=(Line-j)<<2;

			pW=WBuf+8+k;
			pZ=ZBuf+k+8;
			unsigned short * palt = Palette[((TMap&SPR_PAL)>>9)+8];

			if (TMap&SPR_LAYR) {
				if(TMap&SPR_HREV) {
					if ((j=(pbTData[3]&0x0F)))	  { pSWrBuf[0]=palt[j]; }
					if ((j=(pbTData[3]&0xF0)>>4)) { pSWrBuf[1]=palt[j]; }
					if ((j=(pbTData[2]&0x0F)))	  { pSWrBuf[2]=palt[j]; }
					if ((j=(pbTData[2]&0xF0)>>4)) { pSWrBuf[3]=palt[j]; }
					if ((j=(pbTData[1]&0x0F)))	  { pSWrBuf[4]=palt[j]; }
					if ((j=(pbTData[1]&0xF0)>>4)) { pSWrBuf[5]=palt[j]; }
					if ((j=(pbTData[0]&0x0F)))	  { pSWrBuf[6]=palt[j]; }
					if ((j=(pbTData[0]&0xF0)>>4)) { pSWrBuf[7]=palt[j]; }
				} else {
					if ((j=(pbTData[0]&0xF0)>>4)) { pSWrBuf[0]=palt[j]; }
					if ((j=(pbTData[0]&0x0F)))	  { pSWrBuf[1]=palt[j]; }
					if ((j=(pbTData[1]&0xF0)>>4)) { pSWrBuf[2]=palt[j]; }
					if ((j=(pbTData[1]&0x0F)))	  { pSWrBuf[3]=palt[j]; }
					if ((j=(pbTData[2]&0xF0)>>4)) { pSWrBuf[4]=palt[j]; }
					if ((j=(pbTData[2]&0x0F)))	  { pSWrBuf[5]=palt[j]; }
					if ((j=(pbTData[3]&0xF0)>>4)) { pSWrBuf[6]=palt[j]; }
					if ((j=(pbTData[3]&0x0F)))	  { pSWrBuf[7]=palt[j]; }
				} 
			} else {
				if(TMap&SPR_HREV) {
					if (!pZ[0]&&(j=(pbTData[3]&0x0F)))	  { pSWrBuf[0]=palt[j]; }
					if (!pZ[1]&&(j=(pbTData[3]&0xF0)>>4)) { pSWrBuf[1]=palt[j]; }
					if (!pZ[2]&&(j=(pbTData[2]&0x0F)))	  { pSWrBuf[2]=palt[j]; }
					if (!pZ[3]&&(j=(pbTData[2]&0xF0)>>4)) { pSWrBuf[3]=palt[j]; }
					if (!pZ[4]&&(j=(pbTData[1]&0x0F)))	  { pSWrBuf[4]=palt[j]; }
					if (!pZ[5]&&(j=(pbTData[1]&0xF0)>>4)) { pSWrBuf[5]=palt[j]; }
					if (!pZ[6]&&(j=(pbTData[0]&0x0F)))	  { pSWrBuf[6]=palt[j]; }
					if (!pZ[7]&&(j=(pbTData[0]&0xF0)>>4)) { pSWrBuf[7]=palt[j]; }
				} else {
					if (!pZ[0]&&(j=(pbTData[0]&0xF0)>>4)) { pSWrBuf[0]=palt[j]; }
					if (!pZ[1]&&(j=(pbTData[0]&0x0F)))	  { pSWrBuf[1]=palt[j]; }
					if (!pZ[2]&&(j=(pbTData[1]&0xF0)>>4)) { pSWrBuf[2]=palt[j]; }
					if (!pZ[3]&&(j=(pbTData[1]&0x0F)))	  { pSWrBuf[3]=palt[j]; }
					if (!pZ[4]&&(j=(pbTData[2]&0xF0)>>4)) { pSWrBuf[4]=palt[j]; }
					if (!pZ[5]&&(j=(pbTData[2]&0x0F)))	  { pSWrBuf[5]=palt[j]; }
					if (!pZ[6]&&(j=(pbTData[3]&0xF0)>>4)) { pSWrBuf[6]=palt[j]; }
					if (!pZ[7]&&(j=(pbTData[3]&0x0F)))	  { pSWrBuf[7]=palt[j]; }
				} 
			}
		}
*/
}  // DSPCTL&0x08 Off

		
	} // Sprite On/Off

}

extern char WSRenderMode;
void render16LayeredMode(int Line, void *lpSurface) 
{// COLCTL = 0xC0	// 1100 8 4 
	register unsigned short *pSBuf;			// 서페스하″파의 (0, Y) 좌표호˚인터(8艱″실 포함하지 않고)
	register unsigned short *pSWrBuf;			// 서페스하″파의 라이트호˚인터
	register unsigned short * lpScBf = ScreenBuffer;

	int *pZ;				// ZBuf의 라이트호˚인터
	int ZBuf[0x100];		// Z絿스크
	int *pW;				// write-pointer of WBuf 
	int WBuf[0x100];		// Window Mask

	int OffsetX;			// X좌표 오프셋(0∼7)
	int OffsetY;			// Y좌표 오프셋(0∼E)

	register byte *pbTMap;			// 독 붐비어 타이르맛후˚의 호˚인터
	int TMap;				// 타이르맛후˚
	int TMapX;				// 타이르맛후˚의 메쉬 X값
	int TMapXEnd;			// 타이르맛후˚의 하″크크″라운트″영역 종료의 메쉬 X값

	register byte *pbTData;			// 독 붐비어 타이르테″타의 호˚인터


    unsigned int i, j, k, index[8];

	unsigned short *pbPal;			// 하˚렛호˚인터
	unsigned short defpal[1];


	defpal[0] = 0;
	pSBuf=(lpScBf+Line*240)+8;
	pSWrBuf=pSBuf;

	if(LCDSLP&0x01)
	{
		if(COLCTL&0xE0)	pbPal=&Palette[(BORDER&0xF0)>>4][BORDER&0x0F];			
	} else {
		pbPal=defpal;
	}
	unsigned short bkCol = pbPal[0];
	
	if(! (LCDSLP&0x01)) return;

 	if(DSPCTL&0x01)
	{
		OffsetX=SCR1X&0x07;						// X좌표 오프셋을 설정
		pSWrBuf=pSBuf-OffsetX;					// 서페스하″파의 기입호˚인터를 X됫후셋트
		i=Line+SCR1Y;
		OffsetY=(i&0x07);						// Y좌표 오프셋을 설정

		pbTMap=Scr1TMap+((i&0xF8)<<3);			// 타이르맛후˚의 (0, Y) 좌표호˚인터
		TMapX=(SCR1X&0xF8)>>2;					// 타이르맛후˚의 X값을 설정
		TMapXEnd=((SCR1X+224+7)>>2) &0xFFE;		// 하″크크″라운트″영역 종료의 X값을 설정

		for(;TMapX<TMapXEnd;)					// 하″크크″라운트″영역을 묘화
		{
			TMap=*(pbTMap+(TMapX++&0x3F));		// 타이르맛후˚를 읽어들인다
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;	// 타이르맛후˚를 읽어들인다

			pbTData = (TMap&MAP_BANK) ? (IRAM+0x08000) : (IRAM+0x4000);
			pbTData+= (TMap&MAP_TILE)<<5;
			pbTData+= (TMap&MAP_VREV) ? ((7-OffsetY)<<2) : (OffsetY<<2);
			unsigned short * palt = Palette[(TMap&MAP_PAL)>>9];

			unsigned int *pp = (unsigned int*) &pbTData[0];
			if(TMap&MAP_HREV) {
				*pSWrBuf++ = (j=((((j=pp[0]&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F))		? palt[j] : bkCol;
				*pSWrBuf++ = (j=((((j=pp[0]&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F))	? palt[j] : bkCol;
				*pSWrBuf++ = (j=((((j=pp[0]&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F))	? palt[j] : bkCol;
				*pSWrBuf++ = (j=((((j=pp[0]&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F)) ? palt[j] : bkCol;
				*pSWrBuf++ = (j=((((j=pp[0]&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F)) ? palt[j] : bkCol;
				*pSWrBuf++ = (j=((((j=pp[0]&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F)) ? palt[j] : bkCol;
				*pSWrBuf++ = (j=((((j=pp[0]&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F)) ? palt[j] : bkCol;
				*pSWrBuf++ = (j=((((j=pp[0]&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F)) ? palt[j] : bkCol;				
			} else 	{
				*pSWrBuf++ = (j=((((j=pp[0]&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F)) ? palt[j] : bkCol;
				*pSWrBuf++ = (j=((((j=pp[0]&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F)) ? palt[j] : bkCol;
				*pSWrBuf++ = (j=((((j=pp[0]&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F)) ? palt[j] : bkCol;
				*pSWrBuf++ = (j=((((j=pp[0]&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F)) ? palt[j] : bkCol;
				*pSWrBuf++ = (j=((((j=pp[0]&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F)) ? palt[j] : bkCol;
				*pSWrBuf++ = (j=((((j=pp[0]&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F))	? palt[j] : bkCol;
				*pSWrBuf++ = (j=((((j=pp[0]&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F))	? palt[j] : bkCol;
				*pSWrBuf++ = (j=((((j=pp[0]&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F))		? palt[j] : bkCol;
			}
		}
	}

	for (i=0;i<0x100;i++) ZBuf[i] = 0;

	if(DSPCTL&0x02)
	{
		if((DSPCTL&0x30) ==0x20)
		{
            for(i=0, pW=WBuf+8;i<224;i++)
			{
        		*pW++=1;
			}
			if((Line>=SCR2WT)&&(Line<=SCR2WB))
            {
				for(i=SCR2WL, pW=WBuf+8+i;i<=SCR2WR;i++)
            	{
            		*pW++=0;
            	}
            }
		}
		else if((DSPCTL&0x30) ==0x30)
		{
            for(i=0, pW=WBuf+8;i<224;i++)
			{
        		*pW++=0;
			}
			if((Line>=SCR2WT)&&(Line<=SCR2WB))
            {
				for(i=SCR2WL, pW=WBuf+8+i;i<=SCR2WR;i++)
            	{
            		*pW++=1;
            	}
            }
		}
		else
		{
			for(i=0, pW=WBuf+8;i<0x100;i++)
			{
				*pW++=0;
			}
		}

		OffsetX=SCR2X&0x07;						// X좌표 오프셋을 설정
		pSWrBuf=pSBuf-OffsetX;					// 서페스하″파의 기입호˚인터를 X됫후셋트
		i=Line+SCR2Y;
		OffsetY=(i&0x07);						// Y좌표 오프셋을 설정

		pbTMap=Scr2TMap+((i&0xF8)<<3);			// 타이르맛후˚의 (0, Y) 좌표호˚인터
		TMapX=(SCR2X&0xF8)>>2;					// 타이르맛후˚의 X값을 설정
		TMapXEnd=((SCR2X+224+7)>>2) &0xFFE;				// 하″크크″라운트″영역 종료의 X값을 설정

		pW=WBuf+8-OffsetX;
		pZ=ZBuf+8-OffsetX;
        
		for(;TMapX<TMapXEnd;)					// 하″크크″라운트″영역을 묘화
		{
			TMap=*(pbTMap+(TMapX++&0x3F));		// 타이르맛후˚를 읽어들인다
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;	// 타이르맛후˚를 읽어들인다

			pbTData = (TMap&MAP_BANK) ? (IRAM+0x08000) : (IRAM+0x4000);
			pbTData+= (TMap&MAP_TILE)<<5;
			pbTData+= (TMap&MAP_VREV) ? ((7-OffsetY)<<2) : (OffsetY<<2);
			unsigned short * palt = Palette[(TMap&MAP_PAL)>>9];

// 4 : 16/4 , 2 : pack/layer , 1 : col/mono
			unsigned int *pp = (unsigned int*) &pbTData[0];
			if (TMap&MAP_HREV) {
				if (!pW[0]&&(j=((((j=pp[0]&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F)))		{ pZ[0]=1; pSWrBuf[0]=palt[j]; }
				if (!pW[1]&&(j=((((j=pp[0]&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F)))	{ pZ[1]=1; pSWrBuf[1]=palt[j]; }
				if (!pW[2]&&(j=((((j=pp[0]&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F)))	{ pZ[2]=1; pSWrBuf[2]=palt[j]; }
				if (!pW[3]&&(j=((((j=pp[0]&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F))) { pZ[3]=1; pSWrBuf[3]=palt[j]; }
				if (!pW[4]&&(j=((((j=pp[0]&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F)))	{ pZ[4]=1; pSWrBuf[4]=palt[j]; }
				if (!pW[5]&&(j=((((j=pp[0]&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F))) { pZ[5]=1; pSWrBuf[5]=palt[j]; }
				if (!pW[6]&&(j=((((j=pp[0]&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F)))	{ pZ[6]=1; pSWrBuf[6]=palt[j]; }
				if (!pW[7]&&(j=((((j=pp[0]&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F))) { pZ[7]=1; pSWrBuf[7]=palt[j]; }
			} else {
				if (!pW[0]&&(j=((((j=pp[0]&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F))) { pZ[0]=1; pSWrBuf[0]=palt[j]; }
				if (!pW[1]&&(j=((((j=pp[0]&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F)))	{ pZ[1]=1; pSWrBuf[1]=palt[j]; }
				if (!pW[2]&&(j=((((j=pp[0]&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F))) { pZ[2]=1; pSWrBuf[2]=palt[j]; }
				if (!pW[3]&&(j=((((j=pp[0]&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F)))	{ pZ[3]=1; pSWrBuf[3]=palt[j]; }
				if (!pW[4]&&(j=((((j=pp[0]&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F))) { pZ[4]=1; pSWrBuf[4]=palt[j]; }
				if (!pW[5]&&(j=((((j=pp[0]&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F)))	{ pZ[5]=1; pSWrBuf[5]=palt[j]; }
				if (!pW[6]&&(j=((((j=pp[0]&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F)))	{ pZ[6]=1; pSWrBuf[6]=palt[j]; }
				if (!pW[7]&&(j=((((j=pp[0]&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F)))		{ pZ[7]=1; pSWrBuf[7]=palt[j]; }
			}

			pW += 8;
			pZ += 8;
			pSWrBuf += 8;
		}
	}

	if(DSPCTL&0x04)  // Sprite On/Off
	{
		if(DSPCTL&0x08)	// Sprite Window On/Off
        {
			
            for(i=0, pW=WBuf+8;i<224;i++)
			{
        		*pW++=1;
			}
			if((Line>=SPRWT)&&(Line<=SPRWB))
            {
				for(i=SPRWL, pW=WBuf+8+i;i<=SPRWR;i++)
            	{
            		*pW++=0;
            	}
            }
			
        }

if(DSPCTL&0x08) {
		for(pbTMap=SprETMap;pbTMap>=SprTTMap;pbTMap-=4) // render sprite
		{
			TMap=pbTMap[0];	TMap|=pbTMap[1]<<8;					
			if(pbTMap[2]>0xF8) j=pbTMap[2]-0x100; else j=pbTMap[2];
			if(pbTMap[3]>0xF8) k=pbTMap[3]-0x100; else k=pbTMap[3];
			if(Line<j) continue;
			if(Line>=j+8) continue;
			if(224<=k) continue;
			i=k; pSWrBuf=pSBuf+i;
			pbTData=IRAM+0x4000;
			pbTData+=(TMap&SPR_TILE)<<5;
			if(TMap&SPR_VREV) pbTData+=(7-Line+j)<<2; else pbTData+=(Line-j)<<2;

			pW=WBuf+8+k;
			pZ=ZBuf+k+8;
			unsigned short *palt = Palette[((TMap&SPR_PAL)>>9)+8];
			unsigned int *pp = (unsigned int*) &pbTData[0];
			if(TMap&SPR_CLIP) {
				if (TMap&SPR_LAYR){
					if (TMap&SPR_HREV) {
						if ((*pW++)&&(j=((((j=pp[0]&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F)))		pSWrBuf[0]=palt[j];
						if ((*pW++)&&(j=((((j=pp[0]&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F)))		pSWrBuf[1]=palt[j];
						if ((*pW++)&&(j=((((j=pp[0]&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F)))		pSWrBuf[2]=palt[j];
						if ((*pW++)&&(j=((((j=pp[0]&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F)))	pSWrBuf[3]=palt[j];
						if ((*pW++)&&(j=((((j=pp[0]&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F)))	pSWrBuf[4]=palt[j];
						if ((*pW++)&&(j=((((j=pp[0]&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F)))	pSWrBuf[5]=palt[j];
						if ((*pW++)&&(j=((((j=pp[0]&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F)))	pSWrBuf[6]=palt[j];
						if ((*pW++)&&(j=((((j=pp[0]&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F)))	pSWrBuf[7]=palt[j];				
					} else 	{
						if ((*pW++)&&(j=((((j=pp[0]&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F)))	pSWrBuf[0]=palt[j];
						if ((*pW++)&&(j=((((j=pp[0]&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F)))	pSWrBuf[1]=palt[j];
						if ((*pW++)&&(j=((((j=pp[0]&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F)))	pSWrBuf[2]=palt[j];
						if ((*pW++)&&(j=((((j=pp[0]&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F)))	pSWrBuf[3]=palt[j];
						if ((*pW++)&&(j=((((j=pp[0]&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F)))	pSWrBuf[4]=palt[j];
						if ((*pW++)&&(j=((((j=pp[0]&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F)))		pSWrBuf[5]=palt[j];
						if ((*pW++)&&(j=((((j=pp[0]&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F)))		pSWrBuf[6]=palt[j];
						if ((*pW++)&&(j=((((j=pp[0]&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F)))		pSWrBuf[7]=palt[j];
					}
					pZ+=8;
				} else {
					if (TMap&SPR_HREV) {
						if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F)))		pSWrBuf[0]=palt[j];
						if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F)))	pSWrBuf[1]=palt[j];
						if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F)))	pSWrBuf[2]=palt[j];
						if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F)))	pSWrBuf[3]=palt[j];
						if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F)))	pSWrBuf[4]=palt[j];
						if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F)))	pSWrBuf[5]=palt[j];
						if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F)))	pSWrBuf[6]=palt[j];
						if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F)))	pSWrBuf[7]=palt[j];				
					} else 	{
						if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F)))	pSWrBuf[0]=palt[j];
						if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F)))	pSWrBuf[1]=palt[j];
						if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F)))	pSWrBuf[2]=palt[j];
						if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F)))	pSWrBuf[3]=palt[j];
						if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F)))	pSWrBuf[4]=palt[j];
						if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F)))	pSWrBuf[5]=palt[j];
						if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F)))	pSWrBuf[6]=palt[j];
						if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F)))		pSWrBuf[7]=palt[j];
					}
				}
			} else {
				if (TMap&SPR_LAYR){
					if (TMap&SPR_HREV) {
						if ((!*pW++)&&(j=((((j=pp[0]&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F)))		pSWrBuf[0]=palt[j];
						if ((!*pW++)&&(j=((((j=pp[0]&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F)))	pSWrBuf[1]=palt[j];
						if ((!*pW++)&&(j=((((j=pp[0]&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F)))	pSWrBuf[2]=palt[j];
						if ((!*pW++)&&(j=((((j=pp[0]&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F)))	pSWrBuf[3]=palt[j];
						if ((!*pW++)&&(j=((((j=pp[0]&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F)))	pSWrBuf[4]=palt[j];
						if ((!*pW++)&&(j=((((j=pp[0]&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F)))	pSWrBuf[5]=palt[j];
						if ((!*pW++)&&(j=((((j=pp[0]&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F)))	pSWrBuf[6]=palt[j];
						if ((!*pW++)&&(j=((((j=pp[0]&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F)))	pSWrBuf[7]=palt[j];				
					} else 	{
						if ((!*pW++)&&(j=((((j=pp[0]&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F)))	pSWrBuf[0]=palt[j];
						if ((!*pW++)&&(j=((((j=pp[0]&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F)))	pSWrBuf[1]=palt[j];
						if ((!*pW++)&&(j=((((j=pp[0]&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F)))	pSWrBuf[2]=palt[j];
						if ((!*pW++)&&(j=((((j=pp[0]&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F)))	pSWrBuf[3]=palt[j];
						if ((!*pW++)&&(j=((((j=pp[0]&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F)))	pSWrBuf[4]=palt[j];
						if ((!*pW++)&&(j=((((j=pp[0]&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F)))	pSWrBuf[5]=palt[j];
						if ((!*pW++)&&(j=((((j=pp[0]&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F)))	pSWrBuf[6]=palt[j];
						if ((!*pW++)&&(j=((((j=pp[0]&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F)))		pSWrBuf[7]=palt[j];
					}
					pZ+=8;
				} else {
					if (TMap&SPR_HREV) {
						if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F)))		pSWrBuf[0]=palt[j];
						if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F)))	pSWrBuf[1]=palt[j];
						if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F)))	pSWrBuf[2]=palt[j];
						if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F)))	pSWrBuf[3]=palt[j];
						if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F)))	pSWrBuf[4]=palt[j];
						if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F)))	pSWrBuf[5]=palt[j];
						if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F)))	pSWrBuf[6]=palt[j];
						if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F)))	pSWrBuf[7]=palt[j];				
					} else 	{
						if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F)))	pSWrBuf[0]=palt[j];
						if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F)))	pSWrBuf[1]=palt[j];
						if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F)))	pSWrBuf[2]=palt[j];
						if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F)))	pSWrBuf[3]=palt[j];
						if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F)))	pSWrBuf[4]=palt[j];
						if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F)))	pSWrBuf[5]=palt[j];
						if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F)))	pSWrBuf[6]=palt[j];
						if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F)))		pSWrBuf[7]=palt[j];
					}
				}
			}
		}
} else { //(DSPCTL&0x08)
		for(pbTMap=SprETMap;pbTMap>=SprTTMap;pbTMap-=4) // render sprite
		{
			TMap=pbTMap[0];	TMap|=pbTMap[1]<<8;					
			if(pbTMap[2]>0xF8) j=pbTMap[2]-0x100; else j=pbTMap[2];
			if(pbTMap[3]>0xF8) k=pbTMap[3]-0x100; else k=pbTMap[3];
			if(Line<j) continue;
			if(Line>=j+8) continue;
			if(224<=k) continue;
			i=k; pSWrBuf=pSBuf+i;
			pbTData=IRAM+0x4000;
			pbTData+=(TMap&SPR_TILE)<<5;
			if(TMap&SPR_VREV) pbTData+=(7-Line+j)<<2; else pbTData+=(Line-j)<<2;

			pW=WBuf+8+k;
			pZ=ZBuf+k+8;
			unsigned short *palt = Palette[((TMap&SPR_PAL)>>9)+8];
			unsigned int *pp = (unsigned int*) &pbTData[0];
			if (TMap&SPR_LAYR) {
				if (TMap&SPR_HREV) {
					if (j=((((j=pp[0]&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F))		pSWrBuf[0]=palt[j];
					if (j=((((j=pp[0]&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F))	pSWrBuf[1]=palt[j];
					if (j=((((j=pp[0]&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F))	pSWrBuf[2]=palt[j];
					if (j=((((j=pp[0]&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F))	pSWrBuf[3]=palt[j];
					if (j=((((j=pp[0]&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F))	pSWrBuf[4]=palt[j];
					if (j=((((j=pp[0]&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F))	pSWrBuf[5]=palt[j];
					if (j=((((j=pp[0]&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F))	pSWrBuf[6]=palt[j];
					if (j=((((j=pp[0]&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F))	pSWrBuf[7]=palt[j];				
				} else 	{
					if (j=((((j=pp[0]&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F))	pSWrBuf[0]=palt[j];
					if (j=((((j=pp[0]&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F))	pSWrBuf[1]=palt[j];
					if (j=((((j=pp[0]&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F))	pSWrBuf[2]=palt[j];
					if (j=((((j=pp[0]&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F))	pSWrBuf[3]=palt[j];
					if (j=((((j=pp[0]&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F))	pSWrBuf[4]=palt[j];
					if (j=((((j=pp[0]&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F))	pSWrBuf[5]=palt[j];
					if (j=((((j=pp[0]&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F))	pSWrBuf[6]=palt[j];
					if (j=((((j=pp[0]&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F))		pSWrBuf[7]=palt[j];
				}
				pZ+=8;
			} else {
				if (TMap&SPR_HREV) {
					if ((!*pZ++)&&(j=((((j=pp[0]&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F)))		pSWrBuf[0]=palt[j];
					if ((!*pZ++)&&(j=((((j=pp[0]&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F)))	pSWrBuf[1]=palt[j];
					if ((!*pZ++)&&(j=((((j=pp[0]&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F)))	pSWrBuf[2]=palt[j];
					if ((!*pZ++)&&(j=((((j=pp[0]&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F)))	pSWrBuf[3]=palt[j];
					if ((!*pZ++)&&(j=((((j=pp[0]&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F)))	pSWrBuf[4]=palt[j];
					if ((!*pZ++)&&(j=((((j=pp[0]&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F)))	pSWrBuf[5]=palt[j];
					if ((!*pZ++)&&(j=((((j=pp[0]&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F)))	pSWrBuf[6]=palt[j];
					if ((!*pZ++)&&(j=((((j=pp[0]&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F)))	pSWrBuf[7]=palt[j];				
				} else 	{
					if ((!*pZ++)&&(j=((((j=pp[0]&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F)))	pSWrBuf[0]=palt[j];
					if ((!*pZ++)&&(j=((((j=pp[0]&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F)))	pSWrBuf[1]=palt[j];
					if ((!*pZ++)&&(j=((((j=pp[0]&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F)))	pSWrBuf[2]=palt[j];
					if ((!*pZ++)&&(j=((((j=pp[0]&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F)))	pSWrBuf[3]=palt[j];
					if ((!*pZ++)&&(j=((((j=pp[0]&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F)))	pSWrBuf[4]=palt[j];
					if ((!*pZ++)&&(j=((((j=pp[0]&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F)))	pSWrBuf[5]=palt[j];
					if ((!*pZ++)&&(j=((((j=pp[0]&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F)))	pSWrBuf[6]=palt[j];
					if ((!*pZ++)&&(j=((((j=pp[0]&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F)))		pSWrBuf[7]=palt[j];
				}
			}
		}
}//(DSPCTL&0x08)
	}


}

void render4ColorMode(int Line, void *lpSurface) 
{// for 4gray/color mode
 // COLCTL = 0x00 or 0x80(b1000)
	register unsigned short *pSBuf;			// 서페스하″파의 (0, Y) 좌표호˚인터(8艱″실 포함하지 않고)
	register unsigned short *pSWrBuf;			// 서페스하″파의 라이트호˚인터
	register unsigned short * lpScBf = ScreenBuffer;

	int *pZ;				// ZBuf의 라이트호˚인터
	int ZBuf[0x100];		// Z絿스크
	int *pW;				// write-pointer of WBuf 
	int WBuf[0x100];		// Window Mask

	int OffsetX;			// X좌표 오프셋(0∼7)
	int OffsetY;			// Y좌표 오프셋(0∼E)

	register byte *pbTMap;			// 독 붐비어 타이르맛후˚의 호˚인터
	int TMap;				// 타이르맛후˚
	int TMapX;				// 타이르맛후˚의 메쉬 X값
	int TMapXEnd;			// 타이르맛후˚의 하″크크″라운트″영역 종료의 메쉬 X값

	register byte *pbTData;			// 독 붐비어 타이르테″타의 호˚인터


    unsigned int i, j, k, index[8];

	unsigned short *pbPal;			// 하˚렛호˚인터
	unsigned short defpal[1];

	defpal[0] = 0;
	pSBuf=(lpScBf+Line*240)+8;
	pSWrBuf=pSBuf;
	if(LCDSLP&0x01)
	{
		if(COLCTL&0xE0) pbPal=&Palette[(BORDER&0xF0)>>4][BORDER&0x0F];
		else pbPal=&Palette[16][BORDER&0x07];	// mono color (no palette->[16][*])
	} else pbPal=defpal;
	
	//for(i=0;i<224;i++) *pSWrBuf++=pbPal[0];
	unsigned short bkCol = pbPal[0];

	if(! (LCDSLP&0x01)) return;

 	if(DSPCTL&0x01)
	{
		OffsetX=SCR1X&0x07;						// X좌표 오프셋을 설정
		pSWrBuf=pSBuf-OffsetX;					// 서페스하″파의 기입호˚인터를 X됫후셋트
		i=Line+SCR1Y;
		OffsetY=(i&0x07);						// Y좌표 오프셋을 설정

		pbTMap=Scr1TMap+((i&0xF8)<<3);			// 타이르맛후˚의 (0, Y) 좌표호˚인터
		TMapX=(SCR1X&0xF8)>>2;					// 타이르맛후˚의 X값을 설정
		TMapXEnd=((SCR1X+224+7)>>2) &0xFFE;		// 하″크크″라운트″영역 종료의 X값을 설정

		for(;TMapX<TMapXEnd;)					// 하″크크″라운트″영역을 묘화
		{
			TMap=*(pbTMap+(TMapX++&0x3F));		// 타이르맛후˚를 읽어들인다
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;	// 타이르맛후˚를 읽어들인다

			pbTData = IRAM+0x2000;
			pbTData+= (TMap&MAP_TILE)<<4;
			pbTData+= (TMap&MAP_VREV) ? ((7-OffsetY)<<1) : (OffsetY<<1);

			unsigned short * palt = Palette[(TMap&MAP_PAL)>>9];
			unsigned short *pp = (unsigned short*) &pbTData[0];
			if(TMap&MAP_HREV) {
				*pSWrBuf++ = (j=(((j=pp[0]&0x0101)>>7)|(j))&0x03)		? palt[j] : bkCol;
				*pSWrBuf++ = (j=(((j=pp[0]&0x0202)>>8)|(j>>1))&0x03)	? palt[j] : bkCol;
				*pSWrBuf++ = (j=(((j=pp[0]&0x0404)>>9)|(j>>2))&0x03)	? palt[j] : bkCol;
				*pSWrBuf++ = (j=(((j=pp[0]&0x0808)>>10)|(j>>3))&0x03)	? palt[j] : bkCol;
				*pSWrBuf++ = (j=(((j=pp[0]&0x1010)>>11)|(j>>4))&0x03)	? palt[j] : bkCol;
				*pSWrBuf++ = (j=(((j=pp[0]&0x2020)>>12)|(j>>5))&0x03)	? palt[j] : bkCol;
				*pSWrBuf++ = (j=(((j=pp[0]&0x4040)>>13)|(j>>6))&0x03)	? palt[j] : bkCol;
				*pSWrBuf++ = (j=(((j=pp[0]&0x8080)>>14)|(j>>7))&0x03)	? palt[j] : bkCol;
			} else {
				*pSWrBuf++ = (j=(((j=pp[0]&0x8080)>>14)|(j>>7))&0x03)	? palt[j] : bkCol;
				*pSWrBuf++ = (j=(((j=pp[0]&0x4040)>>13)|(j>>6))&0x03)	? palt[j] : bkCol;
				*pSWrBuf++ = (j=(((j=pp[0]&0x2020)>>12)|(j>>5))&0x03)	? palt[j] : bkCol;
				*pSWrBuf++ = (j=(((j=pp[0]&0x1010)>>11)|(j>>4))&0x03)	? palt[j] : bkCol;
				*pSWrBuf++ = (j=(((j=pp[0]&0x0808)>>10)|(j>>3))&0x03)	? palt[j] : bkCol;
				*pSWrBuf++ = (j=(((j=pp[0]&0x0404)>>9)|(j>>2))&0x03)	? palt[j] : bkCol;
				*pSWrBuf++ = (j=(((j=pp[0]&0x0202)>>8)|(j>>1))&0x03)	? palt[j] : bkCol;
				*pSWrBuf++ = (j=(((j=pp[0]&0x0101)>>7)|(j))&0x03)		? palt[j] : bkCol;
			}
		}
	}

	for (i=0;i<0x100;i++) ZBuf[i] = 0;

	if(DSPCTL&0x02)
	{
		if((DSPCTL&0x30) ==0x20)
		{
            for(i=0, pW=WBuf+8;i<224;i++)
			{
        		*pW++=1;
			}
			if((Line>=SCR2WT)&&(Line<=SCR2WB))
            {
				for(i=SCR2WL, pW=WBuf+8+i;i<=SCR2WR;i++)
            	{
            		*pW++=0;
            	}
            }
		}
		else if((DSPCTL&0x30) ==0x30)
		{
            for(i=0, pW=WBuf+8;i<224;i++)
			{
        		*pW++=0;
			}
			if((Line>=SCR2WT)&&(Line<=SCR2WB))
            {
				for(i=SCR2WL, pW=WBuf+8+i;i<=SCR2WR;i++)
            	{
            		*pW++=1;
            	}
            }
		}
		else
		{
			for(i=0, pW=WBuf+8;i<0x100;i++)
			{
				*pW++=0;
			}
		}

		OffsetX=SCR2X&0x07;						// X좌표 오프셋을 설정
		pSWrBuf=pSBuf-OffsetX;					// 서페스하″파의 기입호˚인터를 X됫후셋트
		i=Line+SCR2Y;
		OffsetY=(i&0x07);						// Y좌표 오프셋을 설정

		pbTMap=Scr2TMap+((i&0xF8)<<3);			// 타이르맛후˚의 (0, Y) 좌표호˚인터
		TMapX=(SCR2X&0xF8)>>2;					// 타이르맛후˚의 X값을 설정
		TMapXEnd=((SCR2X+224+7)>>2) &0xFFE;				// 하″크크″라운트″영역 종료의 X값을 설정

		pW=WBuf+8-OffsetX;
		pZ=ZBuf+8-OffsetX;
        
		for(;TMapX<TMapXEnd;)					// 하″크크″라운트″영역을 묘화
		{
			TMap=*(pbTMap+(TMapX++&0x3F));		// 타이르맛후˚를 읽어들인다
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;	// 타이르맛후˚를 읽어들인다

			pbTData = IRAM+0x2000;
			pbTData+= (TMap&MAP_TILE)<<4;
			pbTData+= (TMap&MAP_VREV) ? ((7-OffsetY)<<1) : (OffsetY<<1);

// 4 : 16/4 , 2 : pack/layer , 1 : col/mono
			unsigned short *pp = (unsigned short*) &pbTData[0];
			unsigned short * palt = Palette[(TMap&MAP_PAL)>>9];
            if(TMap&MAP_HREV) {
				if (!pW[0]&&(j=(((j=pp[0]&0x0101)>>7)|(j))&0x03))		{ pZ[0]=1; pSWrBuf[0]=palt[j]; }
				if (!pW[1]&&(j=(((j=pp[0]&0x0202)>>8)|(j>>1))&0x03))	{ pZ[1]=1; pSWrBuf[1]=palt[j]; }
				if (!pW[2]&&(j=(((j=pp[0]&0x0404)>>9)|(j>>2))&0x03))	{ pZ[2]=1; pSWrBuf[2]=palt[j]; }
				if (!pW[3]&&(j=(((j=pp[0]&0x0808)>>10)|(j>>3))&0x03))	{ pZ[3]=1; pSWrBuf[3]=palt[j]; }
				if (!pW[4]&&(j=(((j=pp[0]&0x1010)>>11)|(j>>4))&0x03))	{ pZ[4]=1; pSWrBuf[4]=palt[j]; }
				if (!pW[5]&&(j=(((j=pp[0]&0x2020)>>12)|(j>>5))&0x03))	{ pZ[5]=1; pSWrBuf[5]=palt[j]; }
				if (!pW[6]&&(j=(((j=pp[0]&0x4040)>>13)|(j>>6))&0x03))	{ pZ[6]=1; pSWrBuf[6]=palt[j]; }
				if (!pW[7]&&(j=(((j=pp[0]&0x8080)>>14)|(j>>7))&0x03))	{ pZ[7]=1; pSWrBuf[7]=palt[j]; }
			} else {
				if (!pW[0]&&(j=(((j=pp[0]&0x8080)>>14)|(j>>7))&0x03))	{ pZ[0]=1; pSWrBuf[0]=palt[j]; }
				if (!pW[1]&&(j=(((j=pp[0]&0x4040)>>13)|(j>>6))&0x03))	{ pZ[1]=1; pSWrBuf[1]=palt[j]; }
				if (!pW[2]&&(j=(((j=pp[0]&0x2020)>>12)|(j>>5))&0x03))	{ pZ[2]=1; pSWrBuf[2]=palt[j]; }
				if (!pW[3]&&(j=(((j=pp[0]&0x1010)>>11)|(j>>4))&0x03))	{ pZ[3]=1; pSWrBuf[3]=palt[j]; }
				if (!pW[4]&&(j=(((j=pp[0]&0x0808)>>10)|(j>>3))&0x03))	{ pZ[4]=1; pSWrBuf[4]=palt[j]; }
				if (!pW[5]&&(j=(((j=pp[0]&0x0404)>>9)|(j>>2))&0x03))	{ pZ[5]=1; pSWrBuf[5]=palt[j]; }
				if (!pW[6]&&(j=(((j=pp[0]&0x0202)>>8)|(j>>1))&0x03))	{ pZ[6]=1; pSWrBuf[6]=palt[j]; }
				if (!pW[7]&&(j=(((j=pp[0]&0x0101)>>7)|(j))&0x03))		{ pZ[7]=1; pSWrBuf[7]=palt[j]; }
			}

			pW += 8;
			pZ += 8;
			pSWrBuf += 8;

		}
	}

	if(DSPCTL&0x04)  // Sprite On/Off
	{
		if(DSPCTL&0x08)	// Sprite Window On/Off
        {
			
            for(i=0, pW=WBuf+8;i<224;i++)
			{
        		*pW++=1;
			}
			if((Line>=SPRWT)&&(Line<=SPRWB))
            {
				for(i=SPRWL, pW=WBuf+8+i;i<=SPRWR;i++)
            	{
            		*pW++=0;
            	}
            }
			
        }

		for(pbTMap=SprETMap;pbTMap>=SprTTMap;pbTMap-=4) // 스후˚라이트의 묘화
		{
			TMap=pbTMap[0];						// 타이르맛후˚를 읽어들인다
			TMap|=pbTMap[1]<<8;					// 타이르맛후˚를 읽어들인다

			if(pbTMap[2]>0xF8)
            {
            	j=pbTMap[2]-0x100;
            }
            else
            {
            	j=pbTMap[2];
            }
			if(pbTMap[3]>0xF8)
            {
            	k=pbTMap[3]-0x100;
            }
            else
            {
            	k=pbTMap[3];
            }

			if(Line<j)
				continue;
			if(Line>=j+8)
				continue;
			if(224<=k)
				continue;

			i=k;
			pSWrBuf=pSBuf+i;

			if(COLCTL&0x40)
			{
            	pbTData=IRAM+0x4000;
            	pbTData+=(TMap&SPR_TILE)<<5;
				if(TMap&SPR_VREV)
       			{
            		pbTData+=(7-Line+j)<<2;// 타이르테″타의 호˚인터를 설정
       			}
				else
				{
					pbTData+=(Line-j)<<2;// 타이르테″타의 호˚인터를 설정
				}
			}
			else
			{
            	pbTData=IRAM+0x2000;
            	pbTData+=(TMap&SPR_TILE)<<4;
				if(TMap&SPR_VREV)
       			{
            		pbTData+=(7-Line+j)<<1;// 타이르테″타의 호˚인터를 설정
       			}
				else
				{
					pbTData+=(Line-j)<<1;// 타이르테″타의 호˚인터를 설정
				}
			}

			if(COLCTL&0x20)						// Packed Mode
			{
				if(COLCTL&0x40)					// 16 Color
				{
					index[0]=(pbTData[0]&0xF0)>>4;
					index[1]=pbTData[0]&0x0F;
					index[2]=(pbTData[1]&0xF0)>>4;
					index[3]=pbTData[1]&0x0F;
					index[4]=(pbTData[2]&0xF0)>>4;
					index[5]=pbTData[2]&0x0F;
					index[6]=(pbTData[3]&0xF0)>>4;
					index[7]=pbTData[3]&0x0F;
				}
				else							// 4 Color
				{
					index[0]=(pbTData[0]&0xC0)>>6;
					index[1]=(pbTData[0]&0x30)>>4;
					index[2]=(pbTData[0]&0x0C)>>2;
					index[3]=pbTData[0]&0x03;
					index[4]=(pbTData[1]&0xC0)>>6;
					index[5]=(pbTData[1]&0x30)>>4;
					index[6]=(pbTData[1]&0x0C)>>2;
					index[7]=pbTData[1]&0x03;
				}
			}
			else
			{
				if(COLCTL&0x40)					// 16 Color
				{
					
					index[0] =(pbTData[0]&0x80)? 0x1:0;
					index[0]|=(pbTData[1]&0x80)? 0x2:0;
					index[0]|=(pbTData[2]&0x80)? 0x4:0;
					index[0]|=(pbTData[3]&0x80)? 0x8:0;
					index[1] =(pbTData[0]&0x40)? 0x1:0;
					index[1]|=(pbTData[1]&0x40)? 0x2:0;
					index[1]|=(pbTData[2]&0x40)? 0x4:0;
					index[1]|=(pbTData[3]&0x40)? 0x8:0;
					index[2] =(pbTData[0]&0x20)? 0x1:0;
					index[2]|=(pbTData[1]&0x20)? 0x2:0;
					index[2]|=(pbTData[2]&0x20)? 0x4:0;
					index[2]|=(pbTData[3]&0x20)? 0x8:0;
					index[3] =(pbTData[0]&0x10)? 0x1:0;
					index[3]|=(pbTData[1]&0x10)? 0x2:0;
					index[3]|=(pbTData[2]&0x10)? 0x4:0;
					index[3]|=(pbTData[3]&0x10)? 0x8:0;
					index[4] =(pbTData[0]&0x08)? 0x1:0;
					index[4]|=(pbTData[1]&0x08)? 0x2:0;
					index[4]|=(pbTData[2]&0x08)? 0x4:0;
					index[4]|=(pbTData[3]&0x08)? 0x8:0;
					index[5] =(pbTData[0]&0x04)? 0x1:0;
					index[5]|=(pbTData[1]&0x04)? 0x2:0;
					index[5]|=(pbTData[2]&0x04)? 0x4:0;
					index[5]|=(pbTData[3]&0x04)? 0x8:0;
					index[6] =(pbTData[0]&0x02)? 0x1:0;
					index[6]|=(pbTData[1]&0x02)? 0x2:0;
					index[6]|=(pbTData[2]&0x02)? 0x4:0;
					index[6]|=(pbTData[3]&0x02)? 0x8:0;
					index[7] =(pbTData[0]&0x01)? 0x1:0;
					index[7]|=(pbTData[1]&0x01)? 0x2:0;
					index[7]|=(pbTData[2]&0x01)? 0x4:0;
					index[7]|=(pbTData[3]&0x01)? 0x8:0;
					
				}
				else							// 4 Color
				{
					index[0] =(pbTData[0]&0x80)? 0x1:0;
					index[0]|=(pbTData[1]&0x80)? 0x2:0;
					index[1] =(pbTData[0]&0x40)? 0x1:0;
					index[1]|=(pbTData[1]&0x40)? 0x2:0;
					index[2] =(pbTData[0]&0x20)? 0x1:0;
					index[2]|=(pbTData[1]&0x20)? 0x2:0;
					index[3] =(pbTData[0]&0x10)? 0x1:0;
					index[3]|=(pbTData[1]&0x10)? 0x2:0;
					index[4] =(pbTData[0]&0x08)? 0x1:0;
					index[4]|=(pbTData[1]&0x08)? 0x2:0;
					index[5] =(pbTData[0]&0x04)? 0x1:0;
					index[5]|=(pbTData[1]&0x04)? 0x2:0;
					index[6] =(pbTData[0]&0x02)? 0x1:0;
					index[6]|=(pbTData[1]&0x02)? 0x2:0;
					index[7] =(pbTData[0]&0x01)? 0x1:0;
					index[7]|=(pbTData[1]&0x01)? 0x2:0;
				}
			}

            if(TMap&SPR_HREV)
           	{
				j=index[0];
				index[0]=index[7];
				index[7]=j;
				j=index[1];
				index[1]=index[6];
				index[6]=j;
				j=index[2];
				index[2]=index[5];
				index[5]=j;
				j=index[3];
				index[3]=index[4];
				index[4]=j;
			}

			pW=WBuf+8+k;
			pZ=ZBuf+k+8;
            for(i=0;i<8;i++, pZ++, pW++)
            {
            	if(DSPCTL&0x08)
            	{
            		if(TMap&SPR_CLIP)
            		{
                		if(! *pW)
                		{
                			pSWrBuf++;
							continue;
                		}
            		}
            		else
					{
						if(*pW)
						{
							pSWrBuf++;
                   			continue;
						}
            		}
                }

				if((! index[i])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))
                {
                	pSWrBuf++;
                    continue;
                }

				if((*pZ)&&(! (TMap&SPR_LAYR)))
                {
                	pSWrBuf++;
                    continue;
                }

                *pSWrBuf++=Palette[((TMap&SPR_PAL)>>9)+8][index[i]];
			}
		}
	}

}


void renderMonoMode(int Line, void *lpSurface) {}