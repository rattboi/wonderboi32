//////////////////////////////////////////////////// 
// CPU FREQUENCE FOR GP32 // 
//////////////////////////////////////////////////// 
// By ThunderZ // 
//////////////////////////////////////////////////// 
// Thanks to all GP32 community for make this // 
// table possible. // 
//////////////////////////////////////////////////// 
// WARNING, you use it at your how risk !!! // 
//////////////////////////////////////////////////// 


#include <gpstdlib.h>

#include "clock.h"


// pour overclock 
void SetClockSpeed(int nClockSpeed) 
{ 
  // clkdvn MCLK HCLK PCLK 
  // 0 1 1 1 
  // 1 1 1 1/2 
  // 2 1 1/2 1/2 
  // 3 1 1/2 1/4 
  
  switch (nClockSpeed) 
  { 
    case CLOCK16: 
      // 16.5
      GpClockSpeedChange (16500000, 0x71142, 0); 
      break; 
    case CLOCK33: 
      // 33
      GpClockSpeedChange (33000000, 0x24003, 0); 
      break; 
    case CLOCK40: 
      // 40, 1
      GpClockSpeedChange (40000000, 0x48013, 1); 
      break; 
    case CLOCK67: 
      // 67.5, 2 
      GpClockSpeedChange(67500000, 0x25021, 2); 
      break; 
    case CLOCK80: 
      // 80, 1 
      GpClockSpeedChange(80000000, 0x48012, 1); 
      break; 
    case CLOCK102: 
      // 102, 2 
      GpClockSpeedChange(102000000, 0x1a020, 2); 
      break; 
    case CLOCK132: 
      // 132, 3 
      GpClockSpeedChange(132000000, 0x3a011, 3); 
      break; 
  } 
} 
