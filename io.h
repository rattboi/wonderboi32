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

#ifndef __IO_H__
#define __IO_H__

extern	uint8	*ws_ioRam;

/*
extern	uint8	ws_key_start;
extern	uint8	ws_key_left;
extern	uint8	ws_key_right;
extern	uint8	ws_key_up;
extern	uint8	ws_key_down;
extern	uint8	ws_key_button_1;
extern	uint8	ws_key_button_2;
*/

#define WS_KEY_H_UP		0
#define WS_KEY_H_DOWN	1
#define WS_KEY_H_LEFT	2
#define WS_KEY_H_RIGHT	3

#define WS_KEY_V_UP		4
#define WS_KEY_V_DOWN	5
#define WS_KEY_V_LEFT	6
#define WS_KEY_V_RIGHT	7

#define WS_KEY_1		8
#define WS_KEY_2		9
#define WS_KEY_START	10

extern  uint8   ws_keys[11];

void ws_io_init(void);
void ws_io_reset(void);
void ws_io_flipControls(void);
void ws_io_done(void);

#endif
