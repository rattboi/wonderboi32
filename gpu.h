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

#ifndef __GPU_H__
#define __GPU_H__

#define COLOUR_SCHEME_DEFAULT	0
#define COLOUR_SCHEME_AMBER		1
#define COLOUR_SCHEME_GREEN		2

extern	uint8	ws_gpu_scanline;
extern	uint8	ws_gpu_operatingInColor;
extern	uint8	ws_videoMode;
extern	int16	ws_palette[16*4];
extern	int8	ws_paletteColors[8];
extern	int16	wsc_palette[16*16];
extern  uint32  ws_gpu_unknownPort;

extern	uint32	vblank_count;

extern  uint32  ws_gpu_spritebuffer_start;
extern  uint32  ws_gpu_spritebuffer_end;
extern  uint32  ws_gpu_spritetable_buffer[256];

void ws_gpu_init(void);
void ws_gpu_done(void);
void ws_gpu_reset(void);
void ws_gpu_renderScanline(int16 *framebuffer);
void ws_gpu_changeVideoMode(uint8 value);
void ws_gpu_write_byte(DWORD offset, BYTE value);
void ws_gpu_port_write(DWORD port,BYTE value);
BYTE ws_gpu_port_read(BYTE port);
void ws_gpu_set_colour_scheme(int scheme);
void ws_gpu_changeVideoMode(uint8 value);
void ws_gpu_forceColorSystem(void);
void ws_gpu_forceMonoSystem(void);
void ws_gpu_forceReset(void);
void ws_gpu_clearCache(void);

#endif

