//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
// Copyright(C)2000-2001 by david Raingeard
// cinati@caramail.com or david.raingeard@etu.univ-tours.fr
// For more information please read the readme.txt file
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __WS_H__
#define __WS_H__

int 	ws_init(uint8 *rom, uint32 romSize, char *filename);
void	ws_reset(void);
int		ws_executeLine(int16 *framebuffer, int renderLine);
void	ws_patchRom(void);
void	ws_done(void);
int 	ws_rotated(void);
int		ws_saveState(char *statepath);
int		ws_loadState(char *statepath);

#endif
