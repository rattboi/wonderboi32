////////////////////////////////////////////////////////////////////////////////
// Memory
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////

#include "types.h"

#include "gpstdlib.h"
#include "gpstdio.h"

#include "rom.h"
#include "./nec/nec.h"
#include "io.h"
#include "gpu.h"

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
#define IO_ROM_BANK_BASE_SELECTOR	0xC0


uint8	*ws_rom;
uint8	*ws_staticRam;
uint8	*internalRam;
uint8	*externalEeprom;

uint16  ws_rom_checksum;

uint32	sramAddressMask;
uint32	externalEepromAddressMask;
uint32	romAddressMask;
uint32  romSize;

uint32  ws_sram_dirty = 0;

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
void cpu_writemem20(DWORD addr,BYTE value)
{
	uint32	offset=addr&0xffff;
	uint32	bank=addr>>16;

	// 0 - RAM - 16 KB (WS) / 64 KB (WSC) internal RAM
	if (!bank)
		ws_gpu_write_byte(offset,value);
	else
	// 1 - SRAM (cart) 
	if (bank==1)
	{
		ws_staticRam[offset&sramAddressMask]=value; 
		ws_sram_dirty = 1;
	}

	// other banks are read-only
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
BYTE cpu_readmem20(DWORD addr)
{
	uint32	offset=addr&0xffff;
	uint32	bank=addr>>16;
	int romBank;

	switch (bank)
	{
	case 0:		// 0 - RAM - 16 KB (WS) / 64 KB (WSC) internal RAM
				if (ws_gpu_operatingInColor)
					return(internalRam[offset]);
				else
				if (offset<0x4000)
					return(internalRam[offset]);
				return(0xff);

	case 1:  	// 1 - SRAM (cart) 
				return ws_staticRam[offset&sramAddressMask];
	case 2:
	case 3:		return ws_rom[offset+((ws_ioRam[IO_ROM_BANK_BASE_SELECTOR+bank]&((romSize>>16)-1))<<16)];
	default: 
				romBank=(256-(((ws_ioRam[IO_ROM_BANK_BASE_SELECTOR]&0xf)<<4)|(bank&0xf)));
				return ws_rom[(unsigned)(offset+romSize-(romBank<<16))];
	}
	return(0xff);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
void ws_memory_init(uint8 *rom, uint32 wsRomSize)
{
	ws_romHeaderStruct	*ws_romHeader;
	
	ws_rom=rom;
	romSize=wsRomSize;
	ws_romHeader=ws_rom_getHeader(ws_rom,romSize);

	internalRam=(uint8*)GPMALLOC(0x10000);
	ws_staticRam=(uint8*)GPMALLOC(0x10000); 
	externalEeprom=(uint8*)GPMALLOC(ws_rom_eepromSize(ws_rom,romSize));
	sramAddressMask=ws_rom_sramSize(ws_rom,romSize)-1;
	externalEepromAddressMask=ws_rom_eepromSize(ws_rom,romSize)-1;
	romAddressMask=romSize-1;

	if (ws_romHeader->minimumSupportSystem==WS_SYSTEM_COLOR)
		ws_gpu_operatingInColor=1;

	GPFREE(ws_romHeader);
			
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
void ws_memory_reset(void)
{
	GPMEMSET(internalRam,0,0x10000);
//	GPMEMSET(ws_staticRam,0,0x10000);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
void ws_memory_done(void)
{
//	GPFREE(ws_rom);
	GPFREE(ws_staticRam);
	GPFREE(internalRam);
	GPFREE(externalEeprom);
	ws_rom 			= NULL;
	ws_staticRam 	= NULL;
	internalRam 	= NULL;
	externalEeprom 	= NULL;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
uint8	*memory_getRom(void)
{
	return(ws_rom);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
uint32	memory_getRomSize(void)
{
	return(romSize);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
uint16	memory_getRomCrc(void)
{
	return(ws_rom_checksum);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
void ws_sram_load(char *path)
{
	F_HANDLE F;
	uint32 result;
	ulong sizeread;

	result = GpFileOpen(path,OPEN_R, &F);
	
	if (result != SM_OK)
	{
		memset(ws_staticRam, 0, 0x10000);
		return;
	}

	GpFileRead(F, ws_staticRam, 0x8000, &sizeread);

//	read = fread(ws_staticRam, 1, 0x8000, f);
//	fprintf(log_get(), "read 0x%x (of 0x%x?) bytes of save ram from %s\n", read, ws_rom_sramSize(ws_rom, romSize), path);

	GpFileClose(F);

}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
void ws_sram_save(char *path)
{
	F_HANDLE F;
	uint32 result;
//	size_t wrote;

	result=GpFileCreate(path,ALWAYS_CREATE,&F);

	if (result != SM_OK)
	{
//		fprintf(log_get(), "error opening %s for writing save ram. (%s)\n", path, strerror(errno));
		return;
	}

	GpFileWrite(F,ws_staticRam,0x8000);

	GpFileClose(F);
}

