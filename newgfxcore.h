#if (BUFFER_MODE == HORZ_BUFFER_MODE)
	#undef REFRESHLINE16PACKED 
	#define REFRESHLINE16PACKED RefreshLine16PackedHorz
	#undef REFRESHLINE16LAYERED
	#define REFRESHLINE16LAYERED RefreshLine16LayeredHorz
	#undef REFRESHLINE4COLORLAYERED
	#define REFRESHLINE4COLORLAYERED RefreshLine4ColorLayeredHorz
#elif (BUFFER_MODE == VERT_BUFFER_MODE)
	#undef REFRESHLINE16PACKED 
	#define REFRESHLINE16PACKED RefreshLine16PackedVert
	#undef REFRESHLINE16LAYERED
	#define REFRESHLINE16LAYERED RefreshLine16LayeredVert
	#undef REFRESHLINE4COLORLAYERED
	#define REFRESHLINE4COLORLAYERED RefreshLine4ColorLayeredVert
#endif

void  REFRESHLINE16PACKED(int Line, void* buffer);
void  REFRESHLINE16LAYERED(int Line, void* buffer);
void  REFRESHLINE4COLORLAYERED(int Line, void* buffer);
void  RefreshLineOld(int Line, void* buffer);

void (*renderLine[]) (int Line, void *buffer) = {
	REFRESHLINE16PACKED,
	REFRESHLINE16LAYERED,
	REFRESHLINE4COLORLAYERED,
	RefreshLineOld
};

uint8 *ws_tileCache_getTileRow(uint32 TMap, 
							   uint32 line, 
							   uint32 sprite)
{
	int forline;
	int vFlip;
	int hFlip;
	int bank;
	int tileIndex;
	
	if (sprite == 0)
	{
		tileIndex	= TMap&MAP_TILE;
		vFlip		= TMap&MAP_VREV;
		hFlip		= TMap&MAP_HREV;
		bank		= TMap&MAP_BANK;
	}
	else
	{
		tileIndex	= TMap&SPR_TILE;
		vFlip		= TMap&SPR_VREV;
		hFlip		= TMap&SPR_HREV;
		bank		= 0;
	}

	if (bank)
		tileIndex+=512;

	switch(COLCTL & 0xE0)
	{
	case 0xE0:		// 16 bit packed (so easy!)
		{
			if ((wsc_modified_tile[tileIndex] == 1))//|| sprite == 1 )
			{

				uint8	*tileInCachePtr = &wsc_tile_cache[tileIndex<<6];
				uint8	*hflippedTileInCachePtr = &wsc_hflipped_tile_cache[tileIndex<<6];
				uint32	*tileInRamPtr   = (uint32*)&IRAM[0x4000+(tileIndex<<5)];
				uint32	tileLine;

				for (forline=0;forline<8;forline++)
				{
					tileLine=*tileInRamPtr++;
					
					tileInCachePtr[0]=(tileLine>>4)&0x0f;
					tileInCachePtr[1]=(tileLine>>0)&0x0f;
					tileInCachePtr[2]=(tileLine>>12)&0x0f;
					tileInCachePtr[3]=(tileLine>>8)&0x0f;
					tileInCachePtr[4]=(tileLine>>20)&0x0f;
					tileInCachePtr[5]=(tileLine>>16)&0x0f;
					tileInCachePtr[6]=(tileLine>>28)&0x0f;
					tileInCachePtr[7]=(tileLine>>24)&0x0f;

					hflippedTileInCachePtr[7]=tileInCachePtr[0];
					hflippedTileInCachePtr[6]=tileInCachePtr[1];
					hflippedTileInCachePtr[5]=tileInCachePtr[2];
					hflippedTileInCachePtr[4]=tileInCachePtr[3];
					hflippedTileInCachePtr[3]=tileInCachePtr[4];
					hflippedTileInCachePtr[2]=tileInCachePtr[5];
					hflippedTileInCachePtr[1]=tileInCachePtr[6];
					hflippedTileInCachePtr[0]=tileInCachePtr[7];

					tileInCachePtr+=8;
					hflippedTileInCachePtr+=8;				
				}
				wsc_modified_tile[tileIndex]=0;
			}
			if (vFlip)
				line=7-line;
			if (hFlip)
				return(&wsc_hflipped_tile_cache[(tileIndex<<6)+(line<<3)]);
			else
				return(&wsc_tile_cache[(tileIndex<<6)+(line<<3)]);
		}
		break;
	case 0xC0:		// 16 bit layered (not so easy!)
		{
			if ((wsc_modified_tile[tileIndex] == 1))// || sprite == 1 )
			{
				uint8	*tileInCachePtr			= &wsc_tile_cache[tileIndex<<6];
				uint8	*hflippedTileInCachePtr = &wsc_hflipped_tile_cache[tileIndex<<6];
				uint32	*tileInRamPtr			= (uint32*)&IRAM[0x4000+(tileIndex<<5)];
				uint32	tileLine;
				uint32	j;

				for (forline=0;forline<8;forline++)
				{
					tileLine=*tileInRamPtr++;
					
					tileInCachePtr[0] = ((((j=tileLine&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F);
					tileInCachePtr[1] = ((((j=tileLine&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F);
					tileInCachePtr[2] = ((((j=tileLine&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F);
					tileInCachePtr[3] = ((((j=tileLine&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F);
					tileInCachePtr[4] = ((((j=tileLine&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F);
					tileInCachePtr[5] = ((((j=tileLine&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F);
					tileInCachePtr[6] = ((((j=tileLine&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F);
					tileInCachePtr[7] = ((((j=tileLine&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F);

					hflippedTileInCachePtr[7]=tileInCachePtr[0];
					hflippedTileInCachePtr[6]=tileInCachePtr[1];
					hflippedTileInCachePtr[5]=tileInCachePtr[2];
					hflippedTileInCachePtr[4]=tileInCachePtr[3];
					hflippedTileInCachePtr[3]=tileInCachePtr[4];
					hflippedTileInCachePtr[2]=tileInCachePtr[5];
					hflippedTileInCachePtr[1]=tileInCachePtr[6];
					hflippedTileInCachePtr[0]=tileInCachePtr[7];

					tileInCachePtr+=8;
					hflippedTileInCachePtr+=8;
				}
				wsc_modified_tile[tileIndex]=0;
			}
			if (vFlip)
				line=7-line;
			if (hFlip)
				return(&wsc_hflipped_tile_cache[(tileIndex<<6)+(line<<3)]);
			else
				return(&wsc_tile_cache[(tileIndex<<6)+(line<<3)]);
		}
		break;
	default:
		break;
	}
	return(NULL);
}

// for 16 color packed mode
// COLCTL = 0xE0
void  REFRESHLINE16PACKED(int Line, void* buffer)
{
	register uint8	*pSBuf;			// pointer to the screen buffer
	register uint8	*pSWrBuf;		// pointer to where to write to in the buffer

	int *pZ;				// ZBuf pointer
	int ZBuf[0x100];		// clip mask for sprite window 
	int *pW;				// WBuf pointer
	int WBuf[0x100];		// clip mask for FG window 

	int OffsetX;			// X Offset (0-7)
	int OffsetY;			// Y Offset (0-7)

	register byte *pbTMap;
	int TMap;
	int TMapX;
	int TMapXEnd;

	register uint8	*ws_tileRow;

    unsigned int i, j, k, index[8];

	uint8 *pbPal;
	uint8 defpal[1];
	uint8 bkCol;

	defpal[0] = 0;
	pSBuf=(uint8 *) buffer+(8*BUFFER_MODE);
	pSWrBuf=pSBuf;


	if(LCDSLP&0x01)	
		bkCol = BORDER;
	else
		bkCol = 0;

	if(!(LCDSLP&0x01)) return;

#define ColorTILEInfo (((TMap&MAP_PAL)>>9)<<4)
 	if(DSPCTL&0x01) // BG Layer on
	{
		OffsetX=SCR1X&0x07;
		pSWrBuf=pSBuf-(OffsetX*BUFFER_MODE);
		i=Line+SCR1Y;
		OffsetY=(i&0x07);

		pbTMap=Scr1TMap+((i&0xF8)<<3);
		TMapX=(SCR1X&0xF8)>>2;
		TMapXEnd=((SCR1X+224+7)>>2) &0xFFE;

		for(;TMapX<TMapXEnd;)
		{
			TMap=*(pbTMap+(TMapX++&0x3F));
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;

			ws_tileRow	=	ws_tileCache_getTileRow(TMap, OffsetY, 0);

			*pSWrBuf = ((j=(*ws_tileRow++))		? j+ColorTILEInfo : bkCol); pSWrBuf += BUFFER_MODE;
			*pSWrBuf = ((j=(*ws_tileRow++))		? j+ColorTILEInfo : bkCol); pSWrBuf += BUFFER_MODE;
			*pSWrBuf = ((j=(*ws_tileRow++))		? j+ColorTILEInfo : bkCol); pSWrBuf += BUFFER_MODE;
			*pSWrBuf = ((j=(*ws_tileRow++))		? j+ColorTILEInfo : bkCol); pSWrBuf += BUFFER_MODE;
			*pSWrBuf = ((j=(*ws_tileRow++))		? j+ColorTILEInfo : bkCol); pSWrBuf += BUFFER_MODE;
			*pSWrBuf = ((j=(*ws_tileRow++))		? j+ColorTILEInfo : bkCol); pSWrBuf += BUFFER_MODE;
			*pSWrBuf = ((j=(*ws_tileRow++))		? j+ColorTILEInfo : bkCol); pSWrBuf += BUFFER_MODE;
			*pSWrBuf = ((j=(*ws_tileRow++))		? j+ColorTILEInfo : bkCol); pSWrBuf += BUFFER_MODE;
		}
	}

	for (i=0;i<0x100;i++) ZBuf[i] = 0;

	if(DSPCTL&0x02)			// FG Layer on
	{
		if((DSPCTL&0x30) ==0x20) // FG Layer displayed only inside window
		{
            for(i=0, pW=WBuf+8;i<224;i++)
        		*pW++=1;
			if((Line>=SCR2WT)&&(Line<=SCR2WB))
				for(i=SCR2WL, pW=WBuf+8+i;i<=SCR2WR;i++)
            		*pW++=0;
		}
		else if((DSPCTL&0x30) ==0x30) // FG Layer displayed only outside window
		{
            for(i=0, pW=WBuf+8;i<224;i++)
        		*pW++=0;
			if((Line>=SCR2WT)&&(Line<=SCR2WB))
				for(i=SCR2WL, pW=WBuf+8+i;i<=SCR2WR;i++)
            		*pW++=1;
		}
		else  // no FG window
		{
			for(i=0, pW=WBuf+8;i<0x100;i++)
				*pW++=0;
		}

		OffsetX=SCR2X&0x07;
		pSWrBuf=pSBuf-(OffsetX * BUFFER_MODE);
		i=Line+SCR2Y;
		OffsetY=(i&0x07);

		pbTMap=Scr2TMap+((i&0xF8)<<3);
		TMapX=(SCR2X&0xF8)>>2;
		TMapXEnd=((SCR2X+224+7)>>2) &0xFFE;

		pW=WBuf+8-OffsetX;
		pZ=ZBuf+8-OffsetX;
        
		for(;TMapX<TMapXEnd;)
		{
			TMap=*(pbTMap+(TMapX++&0x3F));
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;

			ws_tileRow	=	ws_tileCache_getTileRow(TMap, OffsetY, 0);

			if (!pW[0]&&(j=(*ws_tileRow++)))	  { pZ[0]=1; pSWrBuf[0*BUFFER_MODE]=j+ColorTILEInfo; }
			if (!pW[1]&&(j=(*ws_tileRow++)))	  { pZ[1]=1; pSWrBuf[1*BUFFER_MODE]=j+ColorTILEInfo; }
			if (!pW[2]&&(j=(*ws_tileRow++)))	  { pZ[2]=1; pSWrBuf[2*BUFFER_MODE]=j+ColorTILEInfo; }
			if (!pW[3]&&(j=(*ws_tileRow++)))	  { pZ[3]=1; pSWrBuf[3*BUFFER_MODE]=j+ColorTILEInfo; }
			if (!pW[4]&&(j=(*ws_tileRow++)))	  { pZ[4]=1; pSWrBuf[4*BUFFER_MODE]=j+ColorTILEInfo; }
			if (!pW[5]&&(j=(*ws_tileRow++)))	  { pZ[5]=1; pSWrBuf[5*BUFFER_MODE]=j+ColorTILEInfo; }
			if (!pW[6]&&(j=(*ws_tileRow++)))	  { pZ[6]=1; pSWrBuf[6*BUFFER_MODE]=j+ColorTILEInfo; }
			if (!pW[7]&&(j=(*ws_tileRow++)))	  { pZ[7]=1; pSWrBuf[7*BUFFER_MODE]=j+ColorTILEInfo; }

			pW += 8;
			pZ += 8;
			pSWrBuf += (8*BUFFER_MODE);
		}
	}

#undef	ColorTILEInfo
#define ColorTILEInfo ((((TMap&SPR_PAL)>>9)+8)<<4)

	if(DSPCTL&0x04) // sprites on
	{
		if(DSPCTL&0x08) //sprite window on
        {
            for(i=0, pW=WBuf+8;i<224;i++)
        		*pW++=1;
			if((Line>=SPRWT)&&(Line<=SPRWB))
				for(i=SPRWL, pW=WBuf+8+i;i<=SPRWR;i++)
            		*pW++=0;
        }

		if (DSPCTL&0x08) 
		{
			for(pbTMap=SprETMap;pbTMap>=SprTTMap;pbTMap-=4) // render sprites
			{
				TMap=pbTMap[0];TMap|=pbTMap[1]<<8;					
				if(pbTMap[2]>0xF8) j=pbTMap[2]-0x100; else j=pbTMap[2];
				if(pbTMap[3]>0xF8) k=pbTMap[3]-0x100; else k=pbTMap[3];
				if(Line<j) continue;
				if(Line>=j+8) continue;
				if(224<=k) continue;
				i=k;
				pSWrBuf=pSBuf+(i*BUFFER_MODE);

				ws_tileRow	=	ws_tileCache_getTileRow(TMap, (Line-j)&0x07, 1);

				index[0]=*ws_tileRow++;
				index[1]=*ws_tileRow++;
				index[2]=*ws_tileRow++;
				index[3]=*ws_tileRow++;
				index[4]=*ws_tileRow++;
				index[5]=*ws_tileRow++;
				index[6]=*ws_tileRow++;
				index[7]=*ws_tileRow++;

				pW=WBuf+8+k;
				pZ=ZBuf+8+k;

				for(i=0;i<8;i++, pZ++, pW++)
				{
           			if(TMap&SPR_CLIP)	// sprite window clipping on 
           				if(! *pW)	{ pSWrBuf+=BUFFER_MODE; continue; }
           			else				// sprite window clipping off
						if(*pW)		{ pSWrBuf+=BUFFER_MODE; continue; }

					if(!index[i])	{ pSWrBuf+=BUFFER_MODE; continue; }

					if((*pZ)&&(! (TMap&SPR_LAYR)))
									{ pSWrBuf+=BUFFER_MODE; continue; }

					*pSWrBuf	=	ColorTILEInfo+index[i];
					pSWrBuf		+=	BUFFER_MODE;
				}
			}
		}  // DSPCTL&0x08 On
		else 
		{
			for(pbTMap=SprETMap;pbTMap>=SprTTMap;pbTMap-=4) // render sprites
			{
				TMap=pbTMap[0];TMap|=pbTMap[1]<<8;					
				if(pbTMap[2]>0xF8) j=pbTMap[2]-0x100; else j=pbTMap[2];
				if(pbTMap[3]>0xF8) k=pbTMap[3]-0x100; else k=pbTMap[3];
				if(Line<j) continue;
				if(Line>=j+8) continue;
				if(224<=k) continue;
				i=k;
				pSWrBuf=pSBuf+(i*BUFFER_MODE);

				ws_tileRow	=	ws_tileCache_getTileRow(TMap, (Line-j)&0x07, 1);

//				pW=WBuf+8+k;
				pZ=ZBuf+k+8;

				if (TMap&SPR_LAYR) 
				{
					if ((j=(*ws_tileRow++)))	  { pSWrBuf[0*BUFFER_MODE]=j+ColorTILEInfo; }
					if ((j=(*ws_tileRow++)))	  { pSWrBuf[1*BUFFER_MODE]=j+ColorTILEInfo; }
					if ((j=(*ws_tileRow++)))	  { pSWrBuf[2*BUFFER_MODE]=j+ColorTILEInfo; }
					if ((j=(*ws_tileRow++)))	  { pSWrBuf[3*BUFFER_MODE]=j+ColorTILEInfo; }
					if ((j=(*ws_tileRow++)))	  { pSWrBuf[4*BUFFER_MODE]=j+ColorTILEInfo; }
					if ((j=(*ws_tileRow++)))	  { pSWrBuf[5*BUFFER_MODE]=j+ColorTILEInfo; }
					if ((j=(*ws_tileRow++)))	  { pSWrBuf[6*BUFFER_MODE]=j+ColorTILEInfo; }
					if ((j=(*ws_tileRow++)))	  { pSWrBuf[7*BUFFER_MODE]=j+ColorTILEInfo; }
				}
				else 
				{
					if (!pZ[0]&&(j=(*ws_tileRow++)))	  { pSWrBuf[0*BUFFER_MODE]=j+ColorTILEInfo; }
					if (!pZ[1]&&(j=(*ws_tileRow++)))	  { pSWrBuf[1*BUFFER_MODE]=j+ColorTILEInfo; }
					if (!pZ[2]&&(j=(*ws_tileRow++)))	  { pSWrBuf[2*BUFFER_MODE]=j+ColorTILEInfo; }
					if (!pZ[3]&&(j=(*ws_tileRow++)))	  { pSWrBuf[3*BUFFER_MODE]=j+ColorTILEInfo; }
					if (!pZ[4]&&(j=(*ws_tileRow++)))	  { pSWrBuf[4*BUFFER_MODE]=j+ColorTILEInfo; }
					if (!pZ[5]&&(j=(*ws_tileRow++)))	  { pSWrBuf[5*BUFFER_MODE]=j+ColorTILEInfo; }
					if (!pZ[6]&&(j=(*ws_tileRow++)))	  { pSWrBuf[6*BUFFER_MODE]=j+ColorTILEInfo; }
					if (!pZ[7]&&(j=(*ws_tileRow++)))	  { pSWrBuf[7*BUFFER_MODE]=j+ColorTILEInfo; }
				}
			}
		}  // DSPCTL&0x08 Off
	}
}
// for 16 color layered mode
// COLCTL = 0xC0
void REFRESHLINE16LAYERED(int Line, void *buffer) 
{
	register uint8 *pSBuf;
	register uint8 *pSWrBuf;

	int *pZ;
	int ZBuf[0x100];
	int *pW;
	int WBuf[0x100];

	int OffsetX;
	int OffsetY;

	register byte *pbTMap;
	int TMap;
	int TMapX;
	int TMapXEnd;

	register uint8	*ws_tileRow;

    uint32 i, j, k, index[8];

	uint8 *pbPal;
	uint8 defpal = 0;

	pSBuf=(uint8 *) buffer+(8*BUFFER_MODE);
	pSWrBuf=pSBuf;

	uint8 bkCol;

	if(LCDSLP&0x01)	
		bkCol = BORDER;
	else	
		bkCol = 0;


	if(! (LCDSLP&0x01)) return;

#undef	ColorTILEInfo
#define ColorTILEInfo (((TMap&MAP_PAL)>>9)<<4)

 	if(DSPCTL&0x01)
	{
		OffsetX=SCR1X&0x07;
		pSWrBuf=pSBuf-(OffsetX*BUFFER_MODE);
		i=Line+SCR1Y;
		OffsetY=(i&0x07);

		pbTMap=Scr1TMap+((i&0xF8)<<3);
		TMapX=(SCR1X&0xF8)>>2;
		TMapXEnd=((SCR1X+224+7)>>2) &0xFFE;

		for(;TMapX<TMapXEnd;)
		{
			TMap=*(pbTMap+(TMapX++&0x3F));
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;

			ws_tileRow	=	ws_tileCache_getTileRow(TMap, OffsetY, 0);

			*pSWrBuf = ((j=(*ws_tileRow++))		? j+ColorTILEInfo : bkCol); pSWrBuf += BUFFER_MODE;
			*pSWrBuf = ((j=(*ws_tileRow++))		? j+ColorTILEInfo : bkCol); pSWrBuf += BUFFER_MODE;
			*pSWrBuf = ((j=(*ws_tileRow++))		? j+ColorTILEInfo : bkCol); pSWrBuf += BUFFER_MODE;
			*pSWrBuf = ((j=(*ws_tileRow++))		? j+ColorTILEInfo : bkCol); pSWrBuf += BUFFER_MODE;
			*pSWrBuf = ((j=(*ws_tileRow++))		? j+ColorTILEInfo : bkCol); pSWrBuf += BUFFER_MODE;
			*pSWrBuf = ((j=(*ws_tileRow++))		? j+ColorTILEInfo : bkCol); pSWrBuf += BUFFER_MODE;
			*pSWrBuf = ((j=(*ws_tileRow++))		? j+ColorTILEInfo : bkCol); pSWrBuf += BUFFER_MODE;
			*pSWrBuf = ((j=(*ws_tileRow++))		? j+ColorTILEInfo : bkCol); pSWrBuf += BUFFER_MODE;
		}
	}

	for (i=0;i<0x100;i++) ZBuf[i] = 0;

	if(DSPCTL&0x02)			// FG Layer on
	{
		if((DSPCTL&0x30) ==0x20) // FG Layer displayed only inside window
		{
            for(i=0, pW=WBuf+8;i<224;i++)
        		*pW++=1;
			if((Line>=SCR2WT)&&(Line<=SCR2WB))
				for(i=SCR2WL, pW=WBuf+8+i;i<=SCR2WR;i++)
            		*pW++=0;
		}
		else if((DSPCTL&0x30) ==0x30) // FG Layer displayed only outside window
		{
            for(i=0, pW=WBuf+8;i<224;i++)
        		*pW++=0;
			if((Line>=SCR2WT)&&(Line<=SCR2WB))
				for(i=SCR2WL, pW=WBuf+8+i;i<=SCR2WR;i++)
            		*pW++=1;
		}
		else  // no FG window
		{
			for(i=0, pW=WBuf+8;i<0x100;i++)
				*pW++=0;
		}

		OffsetX=SCR2X&0x07;
		pSWrBuf=pSBuf-(OffsetX* BUFFER_MODE);
		i=Line+SCR2Y;
		OffsetY=(i&0x07);

		pbTMap=Scr2TMap+((i&0xF8)<<3);
		TMapX=(SCR2X&0xF8)>>2;
		TMapXEnd=((SCR2X+224+7)>>2) &0xFFE;

		pW=WBuf+8-OffsetX;
		pZ=ZBuf+8-OffsetX;
        
		for(;TMapX<TMapXEnd;)
		{
			TMap=*(pbTMap+(TMapX++&0x3F));
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;

			ws_tileRow	=	ws_tileCache_getTileRow(TMap, OffsetY, 0);

			if (!pW[0]&&(j=(*ws_tileRow++)))	  { pZ[0]=1; pSWrBuf[0*BUFFER_MODE]=j+ColorTILEInfo; }
			if (!pW[1]&&(j=(*ws_tileRow++)))	  { pZ[1]=1; pSWrBuf[1*BUFFER_MODE]=j+ColorTILEInfo; }
			if (!pW[2]&&(j=(*ws_tileRow++)))	  { pZ[2]=1; pSWrBuf[2*BUFFER_MODE]=j+ColorTILEInfo; }
			if (!pW[3]&&(j=(*ws_tileRow++)))	  { pZ[3]=1; pSWrBuf[3*BUFFER_MODE]=j+ColorTILEInfo; }
			if (!pW[4]&&(j=(*ws_tileRow++)))	  { pZ[4]=1; pSWrBuf[4*BUFFER_MODE]=j+ColorTILEInfo; }
			if (!pW[5]&&(j=(*ws_tileRow++)))	  { pZ[5]=1; pSWrBuf[5*BUFFER_MODE]=j+ColorTILEInfo; }
			if (!pW[6]&&(j=(*ws_tileRow++)))	  { pZ[6]=1; pSWrBuf[6*BUFFER_MODE]=j+ColorTILEInfo; }
			if (!pW[7]&&(j=(*ws_tileRow++)))	  { pZ[7]=1; pSWrBuf[7*BUFFER_MODE]=j+ColorTILEInfo; }

			pW += 8;
			pZ += 8;
			pSWrBuf += (8*BUFFER_MODE);
		}
	}

#undef	ColorTILEInfo
#define ColorTILEInfo ((((TMap&SPR_PAL)>>9)+8)<<4)

	if(DSPCTL&0x04) // sprites on
	{
		if(DSPCTL&0x08) //sprite window on
        {
            for(i=0, pW=WBuf+8;i<224;i++)
        		*pW++=1;
			if((Line>=SPRWT)&&(Line<=SPRWB))
				for(i=SPRWL, pW=WBuf+8+i;i<=SPRWR;i++)
            		*pW++=0;
        }

		if(DSPCTL&0x08) // sprite window on
		{
			for(pbTMap=SprETMap;pbTMap>=SprTTMap;pbTMap-=4) // render sprite
			{
				TMap=pbTMap[0];	TMap|=pbTMap[1]<<8;					
				if(pbTMap[2]>0xF8) j=pbTMap[2]-0x100; else j=pbTMap[2];
				if(pbTMap[3]>0xF8) k=pbTMap[3]-0x100; else k=pbTMap[3];
				if(Line<j) continue;
				if(Line>=j+8) continue;
				if(224<=k) continue;
				i=k; 
				pSWrBuf=pSBuf+(i*BUFFER_MODE);

				pW=WBuf+8+k;
				pZ=ZBuf+k+8;

				ws_tileRow	=	ws_tileCache_getTileRow(TMap, (Line-j)&0x07, 1);

				if(TMap&SPR_CLIP) {
					if (TMap&SPR_LAYR){
						if ((*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[0*BUFFER_MODE]=j+ColorTILEInfo;
						if ((*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[1*BUFFER_MODE]=j+ColorTILEInfo;
						if ((*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[2*BUFFER_MODE]=j+ColorTILEInfo;
						if ((*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[3*BUFFER_MODE]=j+ColorTILEInfo;
						if ((*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[4*BUFFER_MODE]=j+ColorTILEInfo;
						if ((*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[5*BUFFER_MODE]=j+ColorTILEInfo;
						if ((*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[6*BUFFER_MODE]=j+ColorTILEInfo;
						if ((*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[7*BUFFER_MODE]=j+ColorTILEInfo;
						pZ+=8;
					} else {
						if ((!*pZ++)&&(*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[0*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pZ++)&&(*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[1*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pZ++)&&(*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[2*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pZ++)&&(*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[3*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pZ++)&&(*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[4*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pZ++)&&(*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[5*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pZ++)&&(*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[6*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pZ++)&&(*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[7*BUFFER_MODE]=j+ColorTILEInfo;
					}
				} else {
					if (TMap&SPR_LAYR){
						if ((!*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[0*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[1*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[2*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[3*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[4*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[5*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[6*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[7*BUFFER_MODE]=j+ColorTILEInfo;
						pZ+=8;
					} else {
						if ((!*pZ++)&&(!*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[0*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pZ++)&&(!*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[1*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pZ++)&&(!*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[2*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pZ++)&&(!*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[3*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pZ++)&&(!*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[4*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pZ++)&&(!*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[5*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pZ++)&&(!*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[6*BUFFER_MODE]=j+ColorTILEInfo;
						if ((!*pZ++)&&(!*pW++)&&(j=(*ws_tileRow++)))		pSWrBuf[7*BUFFER_MODE]=j+ColorTILEInfo;
					}
				}
			}
		} 
		else  // sprite window off
		{ 
			for(pbTMap=SprETMap;pbTMap>=SprTTMap;pbTMap-=4) // render sprite
			{
				TMap=pbTMap[0];	TMap|=pbTMap[1]<<8;					
				if(pbTMap[2]>0xF8) j=pbTMap[2]-0x100; else j=pbTMap[2];
				if(pbTMap[3]>0xF8) k=pbTMap[3]-0x100; else k=pbTMap[3];
				if(Line<j) continue;
				if(Line>=j+8) continue;
				if(224<=k) continue;
				i=k; 
				pSWrBuf=pSBuf+(i*BUFFER_MODE);

				pW=WBuf+8+k;
				pZ=ZBuf+k+8;

				ws_tileRow	=	ws_tileCache_getTileRow(TMap, (Line-j)&0x07, 1);

				if (TMap&SPR_LAYR) {
					if (j=(*ws_tileRow++))		pSWrBuf[0*BUFFER_MODE]=j+ColorTILEInfo;
					if (j=(*ws_tileRow++))		pSWrBuf[1*BUFFER_MODE]=j+ColorTILEInfo;
					if (j=(*ws_tileRow++))		pSWrBuf[2*BUFFER_MODE]=j+ColorTILEInfo;
					if (j=(*ws_tileRow++))		pSWrBuf[3*BUFFER_MODE]=j+ColorTILEInfo;
					if (j=(*ws_tileRow++))		pSWrBuf[4*BUFFER_MODE]=j+ColorTILEInfo;
					if (j=(*ws_tileRow++))		pSWrBuf[5*BUFFER_MODE]=j+ColorTILEInfo;
					if (j=(*ws_tileRow++))		pSWrBuf[6*BUFFER_MODE]=j+ColorTILEInfo;
					if (j=(*ws_tileRow++))		pSWrBuf[7*BUFFER_MODE]=j+ColorTILEInfo;
					pZ+=8;
				} else {
					if ((!*pZ++)&&(j=(*ws_tileRow++)))		pSWrBuf[0*BUFFER_MODE]=j+ColorTILEInfo;
					if ((!*pZ++)&&(j=(*ws_tileRow++)))		pSWrBuf[1*BUFFER_MODE]=j+ColorTILEInfo;
					if ((!*pZ++)&&(j=(*ws_tileRow++)))		pSWrBuf[2*BUFFER_MODE]=j+ColorTILEInfo;
					if ((!*pZ++)&&(j=(*ws_tileRow++)))		pSWrBuf[3*BUFFER_MODE]=j+ColorTILEInfo;
					if ((!*pZ++)&&(j=(*ws_tileRow++)))		pSWrBuf[4*BUFFER_MODE]=j+ColorTILEInfo;
					if ((!*pZ++)&&(j=(*ws_tileRow++)))		pSWrBuf[5*BUFFER_MODE]=j+ColorTILEInfo;
					if ((!*pZ++)&&(j=(*ws_tileRow++)))		pSWrBuf[6*BUFFER_MODE]=j+ColorTILEInfo;
					if ((!*pZ++)&&(j=(*ws_tileRow++)))		pSWrBuf[7*BUFFER_MODE]=j+ColorTILEInfo;
				}
			}
		}//(DSPCTL&0x08)
	}
}

// for 4 color layered mode
// COLCTL = 0x80(b1000)
void REFRESHLINE4COLORLAYERED(int Line, void *buffer) 
{
	register uint16 *pSBuf;
	register uint16 *pSWrBuf;

	int *pZ;
	int ZBuf[0x100];
	int *pW;
	int WBuf[0x100];

	int OffsetX;
	int OffsetY;

	register byte *pbTMap;
	int TMap;
	int TMapX;
	int TMapXEnd;

	register byte *pbTData;

    unsigned int i, j, k, index[8];

	uint16 *pbPal;
	uint16 defpal[1];

	defpal[0] = 0;
	pSBuf=(uint16 *) buffer+8;
	pSWrBuf=pSBuf;

	if(LCDSLP&0x01)	pbPal=&Palette[(BORDER&0xF0)>>4][BORDER&0x0F];
	else pbPal=defpal;
	
	uint16 bkCol = pbPal[0];

	if(! (LCDSLP&0x01)) return;

 	if(DSPCTL&0x01)
	{
		OffsetX=SCR1X&0x07;
		pSWrBuf=pSBuf-(OffsetX*BUFFER_MODE);
		i=Line+SCR1Y;
		OffsetY=(i&0x07);

		pbTMap=Scr1TMap+((i&0xF8)<<3);
		TMapX=(SCR1X&0xF8)>>2;
		TMapXEnd=((SCR1X+224+7)>>2) &0xFFE;

		for(;TMapX<TMapXEnd;)
		{
			TMap=*(pbTMap+(TMapX++&0x3F));
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;

			pbTData = IRAM+0x2000;
			pbTData+= (TMap&MAP_TILE)<<4;
			pbTData+= (TMap&MAP_VREV) ? ((7-OffsetY)<<1) : (OffsetY<<1);

			uint16 * palt = Palette[(TMap&MAP_PAL)>>9];
			uint16 *pp = (uint16*) &pbTData[0];
			if(TMap&MAP_HREV) {
				*pSWrBuf = (j=(((j=pp[0]&0x0101)>>7)|(j))&0x03)		? palt[j] : bkCol; pSWrBuf+=BUFFER_MODE;
				*pSWrBuf = (j=(((j=pp[0]&0x0202)>>8)|(j>>1))&0x03)	? palt[j] : bkCol; pSWrBuf+=BUFFER_MODE;
				*pSWrBuf = (j=(((j=pp[0]&0x0404)>>9)|(j>>2))&0x03)	? palt[j] : bkCol; pSWrBuf+=BUFFER_MODE;
				*pSWrBuf = (j=(((j=pp[0]&0x0808)>>10)|(j>>3))&0x03)	? palt[j] : bkCol; pSWrBuf+=BUFFER_MODE;
				*pSWrBuf = (j=(((j=pp[0]&0x1010)>>11)|(j>>4))&0x03)	? palt[j] : bkCol; pSWrBuf+=BUFFER_MODE;
				*pSWrBuf = (j=(((j=pp[0]&0x2020)>>12)|(j>>5))&0x03)	? palt[j] : bkCol; pSWrBuf+=BUFFER_MODE;
				*pSWrBuf = (j=(((j=pp[0]&0x4040)>>13)|(j>>6))&0x03)	? palt[j] : bkCol; pSWrBuf+=BUFFER_MODE;
				*pSWrBuf = (j=(((j=pp[0]&0x8080)>>14)|(j>>7))&0x03)	? palt[j] : bkCol; pSWrBuf+=BUFFER_MODE;
			} else {
				*pSWrBuf = (j=(((j=pp[0]&0x8080)>>14)|(j>>7))&0x03)	? palt[j] : bkCol; pSWrBuf+=BUFFER_MODE;
				*pSWrBuf = (j=(((j=pp[0]&0x4040)>>13)|(j>>6))&0x03)	? palt[j] : bkCol; pSWrBuf+=BUFFER_MODE;
				*pSWrBuf = (j=(((j=pp[0]&0x2020)>>12)|(j>>5))&0x03)	? palt[j] : bkCol; pSWrBuf+=BUFFER_MODE;
				*pSWrBuf = (j=(((j=pp[0]&0x1010)>>11)|(j>>4))&0x03)	? palt[j] : bkCol; pSWrBuf+=BUFFER_MODE;
				*pSWrBuf = (j=(((j=pp[0]&0x0808)>>10)|(j>>3))&0x03)	? palt[j] : bkCol; pSWrBuf+=BUFFER_MODE;
				*pSWrBuf = (j=(((j=pp[0]&0x0404)>>9)|(j>>2))&0x03)	? palt[j] : bkCol; pSWrBuf+=BUFFER_MODE;
				*pSWrBuf = (j=(((j=pp[0]&0x0202)>>8)|(j>>1))&0x03)	? palt[j] : bkCol; pSWrBuf+=BUFFER_MODE;
				*pSWrBuf = (j=(((j=pp[0]&0x0101)>>7)|(j))&0x03)		? palt[j] : bkCol; pSWrBuf+=BUFFER_MODE;
			}
		}
	}

	for (i=0;i<0x100;i++) ZBuf[i] = 0;

	if(DSPCTL&0x02)
	{
		if((DSPCTL&0x30) == 0x20)
		{
            for(i=0, pW=WBuf+8;i<224;i++)	*pW++=1;
			if((Line>=SCR2WT)&&(Line<=SCR2WB))
				for(i=SCR2WL, pW=WBuf+8+i;i<=SCR2WR;i++)	*pW++=0;
		}
		else if((DSPCTL&0x30) == 0x30)
		{
            for(i=0, pW=WBuf+8;i<224;i++)	*pW++=0;
			if((Line>=SCR2WT)&&(Line<=SCR2WB))
				for(i=SCR2WL, pW=WBuf+8+i;i<=SCR2WR;i++)	*pW++=1;
		}
		else
			for(i=0, pW=WBuf+8;i<0x100;i++)		*pW++=0;

		OffsetX=SCR2X&0x07;
		pSWrBuf=pSBuf-(OffsetX*BUFFER_MODE);
		i=Line+SCR2Y;
		OffsetY=(i&0x07);

		pbTMap=Scr2TMap+((i&0xF8)<<3);
		TMapX=(SCR2X&0xF8)>>2;
		TMapXEnd=((SCR2X+224+7)>>2) &0xFFE;

		pW=WBuf+8-OffsetX;
		pZ=ZBuf+8-OffsetX;
        
		for(;TMapX<TMapXEnd;)
		{
			TMap=*(pbTMap+(TMapX++&0x3F));
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;

			pbTData = IRAM+0x2000;
			pbTData+= (TMap&MAP_TILE)<<4;
			pbTData+= (TMap&MAP_VREV) ? ((7-OffsetY)<<1) : (OffsetY<<1);

			uint16 *pp = (uint16*) &pbTData[0];
			uint16 * palt = Palette[(TMap&MAP_PAL)>>9];
            if(TMap&MAP_HREV) {
				if (!pW[0]&&(j=(((j=pp[0]&0x0101)>>7)|(j))&0x03))		{ pZ[0]=1; pSWrBuf[0*BUFFER_MODE]=palt[j]; }
				if (!pW[1]&&(j=(((j=pp[0]&0x0202)>>8)|(j>>1))&0x03))	{ pZ[1]=1; pSWrBuf[1*BUFFER_MODE]=palt[j]; }
				if (!pW[2]&&(j=(((j=pp[0]&0x0404)>>9)|(j>>2))&0x03))	{ pZ[2]=1; pSWrBuf[2*BUFFER_MODE]=palt[j]; }
				if (!pW[3]&&(j=(((j=pp[0]&0x0808)>>10)|(j>>3))&0x03))	{ pZ[3]=1; pSWrBuf[3*BUFFER_MODE]=palt[j]; }
				if (!pW[4]&&(j=(((j=pp[0]&0x1010)>>11)|(j>>4))&0x03))	{ pZ[4]=1; pSWrBuf[4*BUFFER_MODE]=palt[j]; }
				if (!pW[5]&&(j=(((j=pp[0]&0x2020)>>12)|(j>>5))&0x03))	{ pZ[5]=1; pSWrBuf[5*BUFFER_MODE]=palt[j]; }
				if (!pW[6]&&(j=(((j=pp[0]&0x4040)>>13)|(j>>6))&0x03))	{ pZ[6]=1; pSWrBuf[6*BUFFER_MODE]=palt[j]; }
				if (!pW[7]&&(j=(((j=pp[0]&0x8080)>>14)|(j>>7))&0x03))	{ pZ[7]=1; pSWrBuf[7*BUFFER_MODE]=palt[j]; }
			} else {
				if (!pW[0]&&(j=(((j=pp[0]&0x8080)>>14)|(j>>7))&0x03))	{ pZ[0]=1; pSWrBuf[0*BUFFER_MODE]=palt[j]; }
				if (!pW[1]&&(j=(((j=pp[0]&0x4040)>>13)|(j>>6))&0x03))	{ pZ[1]=1; pSWrBuf[1*BUFFER_MODE]=palt[j]; }
				if (!pW[2]&&(j=(((j=pp[0]&0x2020)>>12)|(j>>5))&0x03))	{ pZ[2]=1; pSWrBuf[2*BUFFER_MODE]=palt[j]; }
				if (!pW[3]&&(j=(((j=pp[0]&0x1010)>>11)|(j>>4))&0x03))	{ pZ[3]=1; pSWrBuf[3*BUFFER_MODE]=palt[j]; }
				if (!pW[4]&&(j=(((j=pp[0]&0x0808)>>10)|(j>>3))&0x03))	{ pZ[4]=1; pSWrBuf[4*BUFFER_MODE]=palt[j]; }
				if (!pW[5]&&(j=(((j=pp[0]&0x0404)>>9)|(j>>2))&0x03))	{ pZ[5]=1; pSWrBuf[5*BUFFER_MODE]=palt[j]; }
				if (!pW[6]&&(j=(((j=pp[0]&0x0202)>>8)|(j>>1))&0x03))	{ pZ[6]=1; pSWrBuf[6*BUFFER_MODE]=palt[j]; }
				if (!pW[7]&&(j=(((j=pp[0]&0x0101)>>7)|(j))&0x03))		{ pZ[7]=1; pSWrBuf[7*BUFFER_MODE]=palt[j]; }
			}

			pW += 8;
			pZ += 8;
			pSWrBuf += (8*BUFFER_MODE);

		}
	}

	if(DSPCTL&0x04)  // Sprite On/Off
	{
		if(DSPCTL&0x08)	// Sprite Window On/Off
        {
            for(i=0, pW=WBuf+8;i<224;i++)					*pW++=1;
			if((Line>=SPRWT)&&(Line<=SPRWB))
				for(i=SPRWL, pW=WBuf+8+i;i<=SPRWR;i++)		*pW++=0;
        }

		if(DSPCTL&0x08) // sprite window on
		{
        
			for(pbTMap=SprETMap;pbTMap>=SprTTMap;pbTMap-=4)
			{
				TMap=pbTMap[0];	TMap|=pbTMap[1]<<8;
				if(pbTMap[2]>0xF8) j=pbTMap[2]-0x100; else j=pbTMap[2];
				if(pbTMap[3]>0xF8) k=pbTMap[3]-0x100; else k=pbTMap[3];
				if(Line<j) continue;
				if(Line>=j+8) continue;
				if(224<=k) continue;
				i=k; pSWrBuf=pSBuf+(i*BUFFER_MODE);
            	pbTData=IRAM+0x4000;
            	pbTData+=(TMap&SPR_TILE)<<5;
				if(TMap&SPR_VREV) pbTData+=(7-Line+j)<<2; else pbTData+=(Line-j)<<2;

				pW=WBuf+8+k;
				pZ=ZBuf+k+8;
				uint16 *palt = Palette[((TMap&SPR_PAL)>>9)+8];
				uint16 *pp = (uint16*) &pbTData[0];

				if(TMap&SPR_CLIP) {
					if (TMap&SPR_LAYR){
						if (TMap&SPR_HREV) {
							if ((*pW++)&&(j=((((j=pp[0]&0x0101)>>7)|(j))&0x03)))		pSWrBuf[0*BUFFER_MODE]=palt[j];
							if ((*pW++)&&(j=((((j=pp[0]&0x0202)>>8)|(j>>1))&0x03)))		pSWrBuf[1*BUFFER_MODE]=palt[j];
							if ((*pW++)&&(j=((((j=pp[0]&0x0404)>>9)|(j>>2))&0x03)))		pSWrBuf[2*BUFFER_MODE]=palt[j];
							if ((*pW++)&&(j=((((j=pp[0]&0x0808)>>10)|(j>>3))&0x03)))	pSWrBuf[3*BUFFER_MODE]=palt[j];
							if ((*pW++)&&(j=((((j=pp[0]&0x1010)>>11)|(j>>4))&0x03)))	pSWrBuf[4*BUFFER_MODE]=palt[j];
							if ((*pW++)&&(j=((((j=pp[0]&0x2020)>>12)|(j>>5))&0x03)))	pSWrBuf[5*BUFFER_MODE]=palt[j];
							if ((*pW++)&&(j=((((j=pp[0]&0x4040)>>13)|(j>>6))&0x03)))	pSWrBuf[6*BUFFER_MODE]=palt[j];
							if ((*pW++)&&(j=((((j=pp[0]&0x8080)>>14)|(j>>7))&0x03)))	pSWrBuf[7*BUFFER_MODE]=palt[j];
						} else {
							if ((*pW++)&&(j=((((j=pp[0]&0x8080)>>14)|(j>>7))&0x03)))	pSWrBuf[0*BUFFER_MODE]=palt[j];
							if ((*pW++)&&(j=((((j=pp[0]&0x4040)>>13)|(j>>6))&0x03)))	pSWrBuf[1*BUFFER_MODE]=palt[j];
							if ((*pW++)&&(j=((((j=pp[0]&0x2020)>>12)|(j>>5))&0x03)))	pSWrBuf[2*BUFFER_MODE]=palt[j];
							if ((*pW++)&&(j=((((j=pp[0]&0x1010)>>11)|(j>>4))&0x03)))	pSWrBuf[3*BUFFER_MODE]=palt[j];
							if ((*pW++)&&(j=((((j=pp[0]&0x0808)>>10)|(j>>3))&0x03)))	pSWrBuf[4*BUFFER_MODE]=palt[j];
							if ((*pW++)&&(j=((((j=pp[0]&0x0404)>>9)|(j>>2))&0x03)))		pSWrBuf[5*BUFFER_MODE]=palt[j];
							if ((*pW++)&&(j=((((j=pp[0]&0x0202)>>8)|(j>>1))&0x03)))		pSWrBuf[6*BUFFER_MODE]=palt[j];
							if ((*pW++)&&(j=((((j=pp[0]&0x0101)>>7)|(j))&0x03)))		pSWrBuf[7*BUFFER_MODE]=palt[j];
						}
						pZ+=8;
					} else {
						if (TMap&SPR_HREV) {
							if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x0101)>>7)|(j))&0x03)))			pSWrBuf[0*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x0202)>>8)|(j>>1))&0x03)))		pSWrBuf[1*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x0404)>>9)|(j>>2))&0x03)))		pSWrBuf[2*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x0808)>>10)|(j>>3))&0x03)))		pSWrBuf[3*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x1010)>>11)|(j>>4))&0x03)))		pSWrBuf[4*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x2020)>>12)|(j>>5))&0x03)))		pSWrBuf[5*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x4040)>>13)|(j>>6))&0x03)))		pSWrBuf[6*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x8080)>>14)|(j>>7))&0x03)))		pSWrBuf[7*BUFFER_MODE]=palt[j];
						} else {
							if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x8080)>>14)|(j>>7))&0x03)))		pSWrBuf[0*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x4040)>>13)|(j>>6))&0x03)))		pSWrBuf[1*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x2020)>>12)|(j>>5))&0x03)))		pSWrBuf[2*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x1010)>>11)|(j>>4))&0x03)))		pSWrBuf[3*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x0808)>>10)|(j>>3))&0x03)))		pSWrBuf[4*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x0404)>>9)|(j>>2))&0x03)))		pSWrBuf[5*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x0202)>>8)|(j>>1))&0x03)))		pSWrBuf[6*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(*pW++)&&(j=((((j=pp[0]&0x0101)>>7)|(j))&0x03)))			pSWrBuf[7*BUFFER_MODE]=palt[j];
						}
					}
				} else {
					if (TMap&SPR_LAYR){
						if (TMap&SPR_HREV) {
							if ((!*pW++)&&(j=((((j=pp[0]&0x0101)>>7)|(j))&0x03)))		pSWrBuf[0*BUFFER_MODE]=palt[j];
							if ((!*pW++)&&(j=((((j=pp[0]&0x0202)>>8)|(j>>1))&0x03)))	pSWrBuf[1*BUFFER_MODE]=palt[j];
							if ((!*pW++)&&(j=((((j=pp[0]&0x0404)>>9)|(j>>2))&0x03)))	pSWrBuf[2*BUFFER_MODE]=palt[j];
							if ((!*pW++)&&(j=((((j=pp[0]&0x0808)>>10)|(j>>3))&0x03)))	pSWrBuf[3*BUFFER_MODE]=palt[j];
							if ((!*pW++)&&(j=((((j=pp[0]&0x1010)>>11)|(j>>4))&0x03)))	pSWrBuf[4*BUFFER_MODE]=palt[j];
							if ((!*pW++)&&(j=((((j=pp[0]&0x2020)>>12)|(j>>5))&0x03)))	pSWrBuf[5*BUFFER_MODE]=palt[j];
							if ((!*pW++)&&(j=((((j=pp[0]&0x4040)>>13)|(j>>6))&0x03)))	pSWrBuf[6*BUFFER_MODE]=palt[j];
							if ((!*pW++)&&(j=((((j=pp[0]&0x8080)>>14)|(j>>7))&0x03)))	pSWrBuf[7*BUFFER_MODE]=palt[j];
						} else {
							if ((!*pW++)&&(j=((((j=pp[0]&0x8080)>>14)|(j>>7))&0x03)))	pSWrBuf[0*BUFFER_MODE]=palt[j];
							if ((!*pW++)&&(j=((((j=pp[0]&0x4040)>>13)|(j>>6))&0x03)))	pSWrBuf[1*BUFFER_MODE]=palt[j];
							if ((!*pW++)&&(j=((((j=pp[0]&0x2020)>>12)|(j>>5))&0x03)))	pSWrBuf[2*BUFFER_MODE]=palt[j];
							if ((!*pW++)&&(j=((((j=pp[0]&0x1010)>>11)|(j>>4))&0x03)))	pSWrBuf[3*BUFFER_MODE]=palt[j];
							if ((!*pW++)&&(j=((((j=pp[0]&0x0808)>>10)|(j>>3))&0x03)))	pSWrBuf[4*BUFFER_MODE]=palt[j];
							if ((!*pW++)&&(j=((((j=pp[0]&0x0404)>>9)|(j>>2))&0x03)))	pSWrBuf[5*BUFFER_MODE]=palt[j];
							if ((!*pW++)&&(j=((((j=pp[0]&0x0202)>>8)|(j>>1))&0x03)))	pSWrBuf[6*BUFFER_MODE]=palt[j];
							if ((!*pW++)&&(j=((((j=pp[0]&0x0101)>>7)|(j))&0x03)))		pSWrBuf[7*BUFFER_MODE]=palt[j];
						}
						pZ+=8;
				} else {
						if (TMap&SPR_HREV) {
							if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x0101)>>7)|(j))&0x03)))			pSWrBuf[0*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x0202)>>8)|(j>>1))&0x03)))		pSWrBuf[1*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x0404)>>9)|(j>>2))&0x03)))		pSWrBuf[2*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x0808)>>10)|(j>>3))&0x03)))		pSWrBuf[3*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x1010)>>11)|(j>>4))&0x03)))		pSWrBuf[4*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x2020)>>12)|(j>>5))&0x03)))		pSWrBuf[5*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x4040)>>13)|(j>>6))&0x03)))		pSWrBuf[6*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x8080)>>14)|(j>>7))&0x03)))		pSWrBuf[7*BUFFER_MODE]=palt[j];
						} else {
							if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x8080)>>14)|(j>>7))&0x03)))		pSWrBuf[0*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x4040)>>13)|(j>>6))&0x03)))		pSWrBuf[1*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x2020)>>12)|(j>>5))&0x03)))		pSWrBuf[2*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x1010)>>11)|(j>>4))&0x03)))		pSWrBuf[3*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x0808)>>10)|(j>>3))&0x03)))		pSWrBuf[4*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x0404)>>9)|(j>>2))&0x03)))		pSWrBuf[5*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x0202)>>8)|(j>>1))&0x03)))		pSWrBuf[6*BUFFER_MODE]=palt[j];
							if ((!*pZ++)&&(!*pW++)&&(j=((((j=pp[0]&0x0101)>>7)|(j))&0x03)))			pSWrBuf[7*BUFFER_MODE]=palt[j];
						}
					}
				}
			}
		}
		else
		{
			for(pbTMap=SprETMap;pbTMap>=SprTTMap;pbTMap-=4) // render sprite
			{
				TMap=pbTMap[0];	TMap|=pbTMap[1]<<8;					
				if(pbTMap[2]>0xF8) j=pbTMap[2]-0x100; else j=pbTMap[2];
				if(pbTMap[3]>0xF8) k=pbTMap[3]-0x100; else k=pbTMap[3];
				if(Line<j) continue;
				if(Line>=j+8) continue;
				if(224<=k) continue;
				i=k; pSWrBuf=pSBuf+(i*BUFFER_MODE);
				pbTData=IRAM+0x4000;
				pbTData+=(TMap&SPR_TILE)<<5;
				if(TMap&SPR_VREV) pbTData+=(7-Line+j)<<2; else pbTData+=(Line-j)<<2;

				pW=WBuf+8+k;
				pZ=ZBuf+k+8;
				uint16 *palt = Palette[((TMap&SPR_PAL)>>9)+8];
				uint16 *pp = (uint16*) &pbTData[0];
				if (TMap&SPR_LAYR) {
					if (TMap&SPR_HREV) {
						if (j=((((j=pp[0]&0x0101)>>7)|(j))&0x03))		pSWrBuf[0*BUFFER_MODE]=palt[j];
						if (j=((((j=pp[0]&0x0202)>>8)|(j>>1))&0x03))	pSWrBuf[1*BUFFER_MODE]=palt[j];
						if (j=((((j=pp[0]&0x0404)>>9)|(j>>2))&0x03))	pSWrBuf[2*BUFFER_MODE]=palt[j];
						if (j=((((j=pp[0]&0x0808)>>10)|(j>>3))&0x03))	pSWrBuf[3*BUFFER_MODE]=palt[j];
						if (j=((((j=pp[0]&0x1010)>>11)|(j>>4))&0x03))	pSWrBuf[4*BUFFER_MODE]=palt[j];
						if (j=((((j=pp[0]&0x2020)>>12)|(j>>5))&0x03))	pSWrBuf[5*BUFFER_MODE]=palt[j];
						if (j=((((j=pp[0]&0x4040)>>13)|(j>>6))&0x03))	pSWrBuf[6*BUFFER_MODE]=palt[j];
						if (j=((((j=pp[0]&0x8080)>>14)|(j>>7))&0x03))	pSWrBuf[7*BUFFER_MODE]=palt[j];
					} else {
						if (j=((((j=pp[0]&0x8080)>>14)|(j>>7))&0x03))	pSWrBuf[0*BUFFER_MODE]=palt[j];
						if (j=((((j=pp[0]&0x4040)>>13)|(j>>6))&0x03))	pSWrBuf[1*BUFFER_MODE]=palt[j];
						if (j=((((j=pp[0]&0x2020)>>12)|(j>>5))&0x03))	pSWrBuf[2*BUFFER_MODE]=palt[j];
						if (j=((((j=pp[0]&0x1010)>>11)|(j>>4))&0x03))	pSWrBuf[3*BUFFER_MODE]=palt[j];
						if (j=((((j=pp[0]&0x0808)>>10)|(j>>3))&0x03))	pSWrBuf[4*BUFFER_MODE]=palt[j];
						if (j=((((j=pp[0]&0x0404)>>9)|(j>>2))&0x03))	pSWrBuf[5*BUFFER_MODE]=palt[j];
						if (j=((((j=pp[0]&0x0202)>>8)|(j>>1))&0x03))	pSWrBuf[6*BUFFER_MODE]=palt[j];
						if (j=((((j=pp[0]&0x0101)>>7)|(j))&0x03))		pSWrBuf[7*BUFFER_MODE]=palt[j];
					}
					pZ+=8;
				} else {
					if (TMap&SPR_HREV) {
						if ((!*pZ++)&&(j=((((j=pp[0]&0x0101)>>7)|(j))&0x03)))			pSWrBuf[0*BUFFER_MODE]=palt[j];
						if ((!*pZ++)&&(j=((((j=pp[0]&0x0202)>>8)|(j>>1))&0x03)))		pSWrBuf[1*BUFFER_MODE]=palt[j];
						if ((!*pZ++)&&(j=((((j=pp[0]&0x0404)>>9)|(j>>2))&0x03)))		pSWrBuf[2*BUFFER_MODE]=palt[j];
						if ((!*pZ++)&&(j=((((j=pp[0]&0x0808)>>10)|(j>>3))&0x03)))		pSWrBuf[3*BUFFER_MODE]=palt[j];
						if ((!*pZ++)&&(j=((((j=pp[0]&0x1010)>>11)|(j>>4))&0x03)))		pSWrBuf[4*BUFFER_MODE]=palt[j];
						if ((!*pZ++)&&(j=((((j=pp[0]&0x2020)>>12)|(j>>5))&0x03)))		pSWrBuf[5*BUFFER_MODE]=palt[j];
						if ((!*pZ++)&&(j=((((j=pp[0]&0x4040)>>13)|(j>>6))&0x03)))		pSWrBuf[6*BUFFER_MODE]=palt[j];
						if ((!*pZ++)&&(j=((((j=pp[0]&0x8080)>>14)|(j>>7))&0x03)))		pSWrBuf[7*BUFFER_MODE]=palt[j];
					} else {
						if ((!*pZ++)&&(j=((((j=pp[0]&0x8080)>>14)|(j>>7))&0x03)))		pSWrBuf[0*BUFFER_MODE]=palt[j];
						if ((!*pZ++)&&(j=((((j=pp[0]&0x4040)>>13)|(j>>6))&0x03)))		pSWrBuf[1*BUFFER_MODE]=palt[j];
						if ((!*pZ++)&&(j=((((j=pp[0]&0x2020)>>12)|(j>>5))&0x03)))		pSWrBuf[2*BUFFER_MODE]=palt[j];
						if ((!*pZ++)&&(j=((((j=pp[0]&0x1010)>>11)|(j>>4))&0x03)))		pSWrBuf[3*BUFFER_MODE]=palt[j];
						if ((!*pZ++)&&(j=((((j=pp[0]&0x0808)>>10)|(j>>3))&0x03)))		pSWrBuf[4*BUFFER_MODE]=palt[j];
						if ((!*pZ++)&&(j=((((j=pp[0]&0x0404)>>9)|(j>>2))&0x03)))		pSWrBuf[5*BUFFER_MODE]=palt[j];
						if ((!*pZ++)&&(j=((((j=pp[0]&0x0202)>>8)|(j>>1))&0x03)))		pSWrBuf[6*BUFFER_MODE]=palt[j];
						if ((!*pZ++)&&(j=((((j=pp[0]&0x0101)>>7)|(j))&0x03)))			pSWrBuf[7*BUFFER_MODE]=palt[j];
					}
				}
			}
		}
	}
}					

void  RefreshLineOld(int Line, void* buffer)
{
	uint16	*pSBuf;			// 서페스하″파의 (0, Y) 좌표호˚인터(8艱″실 포함하지 않고)
	uint16	*pSWrBuf16;

	int *pZ;				// ZBuf의 라이트호˚인터
	int ZBuf[0x100];		// Z絿스크
	int *pW;				// WBuf의 라이트호˚인터
	int WBuf[0x100];		// 윈트″우마스크

	int OffsetX;			// X좌표 오프셋(0∼7)
	int OffsetY;			// Y좌표 오프셋(0∼E)

	byte *pbTMap;			// 독 붐비어 타이르맛후˚의 호˚인터
	int TMap;				// 타이르맛후˚
	int TMapX;				// 타이르맛후˚의 메쉬 X값
	int TMapXEnd;			// 타이르맛후˚의 하″크크″라운트″영역 종료의 메쉬 X값

	byte *pbTData;			// 독 붐비어 타이르테″타의 호˚인터
	int TData;				// 타이르테″타

    int i, j, k, index[8];

	uint16 *pbPal;			// 하˚렛호˚인터
	uint16 defpal = 0;

	pSBuf=(uint16 *) buffer+8;
	pSWrBuf16=pSBuf;

	if(LCDSLP&0x01)
	{
		if(COLCTL&0xE0) // Any color mode
		{
			pbPal=&Palette[(BORDER&0xF0)>>4][BORDER&0x0F];
		}
		else // mono mode
		{
			pbPal=&Palette[16][BORDER&0x07];
		}
	}
	else
	{
		pbPal=&defpal;
	}
	for(i=0;i<224;i++)
	{
		*pSWrBuf16=*pbPal; pSWrBuf16 += BUFFER_MODE;
	}

	if(!(LCDSLP&0x01)) return;

 	if(DSPCTL&0x01) // BG Layer on
	{
		OffsetX=SCR1X&0x07;
		pSWrBuf16=pSBuf-(OffsetX * BUFFER_MODE);
		i=Line+SCR1Y;
		OffsetY=(i&0x07);

		pbTMap=Scr1TMap+((i&0xF8)<<3);
		TMapX=(SCR1X&0xF8)>>2;
		TMapXEnd=((SCR1X+224+7)>>2) &0xFFE;

		for(;TMapX<TMapXEnd;)
		{
			TMap=*(pbTMap+(TMapX++&0x3F));
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;

			if(COLCTL&0x40) // Color Mode
			{
            	if(TMap&MAP_BANK)
            	{
            		pbTData=IRAM+0x8000;
            	}
            	else
            	{
            		pbTData=IRAM+0x4000;
            	}
				pbTData+=(TMap&MAP_TILE)<<5;
				if(TMap&MAP_VREV)
       			{
            		pbTData+=(7-OffsetY)<<2;
       			}
				else
				{
					pbTData+=OffsetY<<2;
				}
			}
			else	// Mono Mode
			{
            	pbTData=IRAM+0x2000;
            	pbTData+=(TMap&MAP_TILE)<<4;
				if(TMap&MAP_VREV)
       			{
            		pbTData+=(7-OffsetY)<<1;
       			}
				else
				{
					pbTData+=OffsetY<<1;
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
				if(COLCTL&0x40)					// 16 Color (unpacked)
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
				else							// 4 Color (unpacked)
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

            if(TMap&MAP_HREV)
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

			if((!index[0])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf16+=BUFFER_MODE;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[0]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += BUFFER_MODE;
			}
			if((!index[1])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))  pSWrBuf16 += BUFFER_MODE;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[1]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += BUFFER_MODE; 
			}
			if((!index[2])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))  pSWrBuf16 += BUFFER_MODE;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[2]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += BUFFER_MODE; 
			}
			if((!index[3])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))  pSWrBuf16 += BUFFER_MODE;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[3]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += BUFFER_MODE; 
			}
			if((!index[4])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))  pSWrBuf16 += BUFFER_MODE;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[4]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += BUFFER_MODE; 
			}
			if((!index[5])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))  pSWrBuf16 += BUFFER_MODE;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[5]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += BUFFER_MODE; 
			}
			if((!index[6])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))  pSWrBuf16 += BUFFER_MODE;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[6]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += BUFFER_MODE; 
			}
			if((!index[7])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))  pSWrBuf16 += BUFFER_MODE;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[7]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += BUFFER_MODE; 
			}
		}
	}

	GPMEMSET(&ZBuf, 0, sizeof(ZBuf));

	if(DSPCTL&0x02)			// FG Layer on
	{
		if((DSPCTL&0x30) ==0x20) // FG Layer displayed only inside window
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
		else if((DSPCTL&0x30) ==0x30) // FG Layer displayed only outside window
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
		else  // no FG window
		{
			for(i=0, pW=WBuf+8;i<0x100;i++)
			{
				*pW++=0;
			}
		}

		OffsetX=SCR2X&0x07;						// X좌표 오프셋을 설정
		pSWrBuf16=pSBuf-(OffsetX * BUFFER_MODE);		// 서페스하″파의 기입호˚인터를 X됫후셋트
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

			if(COLCTL&0x40)
			{
            	if(TMap&MAP_BANK)
            	{
            		pbTData=IRAM+0x8000;
            	}
            	else
            	{
            		pbTData=IRAM+0x4000;
            	}
				pbTData+=(TMap&MAP_TILE)<<5;
				if(TMap&MAP_VREV)
       			{
            		pbTData+=(7-OffsetY)<<2;// 타이르테″타의 호˚인터를 설정
       			}
				else
				{
					pbTData+=OffsetY<<2;// 타이르테″타의 호˚인터를 설정
				}
			}
			else
			{
            	pbTData=IRAM+0x2000;
            	pbTData+=(TMap&MAP_TILE)<<4;
				if(TMap&MAP_VREV)
       			{
            		pbTData+=(7-OffsetY)<<1;// 타이르테″타의 호˚인터를 설정
       			}
				else
				{
					pbTData+=OffsetY<<1;// 타이르테″타의 호˚인터를 설정
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

            if(TMap&MAP_HREV)
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

			if(((!index[0])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW))  pSWrBuf16 += BUFFER_MODE;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[0]];
				*pSWrBuf16=*pbPal++;  pSWrBuf16 += BUFFER_MODE; 
				*pZ=1;
			}
			pW++;pZ++;
			if(((!index[1])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW))  pSWrBuf16 += BUFFER_MODE;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[1]];
				*pSWrBuf16=*pbPal++;  pSWrBuf16 += BUFFER_MODE; 
				*pZ=1;
			}
			pW++;pZ++;
			if(((!index[2])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW))  pSWrBuf16 += BUFFER_MODE;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[2]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += BUFFER_MODE; 
				*pZ=1;
			}
			pW++;pZ++;
			if(((!index[3])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW))  pSWrBuf16 += BUFFER_MODE;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[3]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += BUFFER_MODE; 
				*pZ=1;
			}
			pW++;pZ++;
			if(((!index[4])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW))  pSWrBuf16 += BUFFER_MODE;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[4]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += BUFFER_MODE; 
				*pZ=1;
			}
			pW++;pZ++;
			if(((!index[5])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW))  pSWrBuf16 += BUFFER_MODE;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[5]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += BUFFER_MODE; 
				*pZ=1;
			}
			pW++;pZ++;
			if(((!index[6])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW))  pSWrBuf16 += BUFFER_MODE;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[6]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += BUFFER_MODE; 
				*pZ=1;
			}
			pW++;pZ++;
			if(((!index[7])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW))  pSWrBuf16 += BUFFER_MODE;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[7]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += BUFFER_MODE; 
				*pZ=1;
			}
			pW++;pZ++;
		}
	}

	if(DSPCTL&0x04) // sprites on
	{
		if(DSPCTL&0x08) //sprite window on
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

		for(pbTMap=SprETMap;pbTMap>=SprTTMap;pbTMap-=4)
		{
			TMap=pbTMap[0];
			TMap|=pbTMap[1]<<8;

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
			pSWrBuf16=pSBuf+(i * BUFFER_MODE);

			if(COLCTL&0x40)		// 16 color
			{
            	pbTData=IRAM+0x4000;
            	pbTData+=(TMap&SPR_TILE)<<5;
				if(TMap&SPR_VREV)
       			{
            		pbTData+=(7-Line+j)<<2;
       			}
				else
				{
					pbTData+=(Line-j)<<2;
				}
			}
			else			// 4 color
			{
            	pbTData=IRAM+0x2000;
            	pbTData+=(TMap&SPR_TILE)<<4;
				if(TMap&SPR_VREV)
       			{
            		pbTData+=(7-Line+j)<<1;
       			}
				else
				{
					pbTData+=(Line-j)<<1;
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
            	if(DSPCTL&0x08)			// sprite window on
            	{
            		if(TMap&SPR_CLIP)
            		{
                		if(!*pW)
                		{
                			 pSWrBuf16 += BUFFER_MODE;
							continue;
                		}
            		}
            		else
					{
						if(*pW)
						{
							 pSWrBuf16 += BUFFER_MODE;
                   			continue;
						}
            		}
                }

				if((!index[i])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))
                {
                	 pSWrBuf16 += BUFFER_MODE;
                    continue;
                }

				if((*pZ)&&(!(TMap&SPR_LAYR)))
                {
                	 pSWrBuf16 += BUFFER_MODE;
                    continue;
                }

                pbPal=&Palette[((TMap&SPR_PAL)>>9)+8][index[i]];
#ifdef DRAW_DEBUG
				if(!(COLCTL&0x80))
                {
					pbPal=&Palette[((TMap&SPR_PAL)>>9)+8][index[i]+12];
                }
#endif
				*pSWrBuf16=*pbPal++; pSWrBuf16 += BUFFER_MODE; 
			}
		}
	}
}

