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

#ifndef __MEMORY_H__
#define __MEMORY_H__

extern uint8	*ws_staticRam;
extern uint8	*internalRam;
extern uint8	*externalEeprom;
extern uint32	ws_sram_dirty;

void	ws_memory_init(uint8 *rom, uint32 romSize);
void	ws_memory_reset(void);
uint8	*memory_getRom(void);
uint32	memory_getRomSize(void);
uint16	memory_getRomCrc(void);
void	ws_memory_done(void);

#endif
