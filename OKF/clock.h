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


#ifndef __clock_h__
#define __clock_h__

#ifdef __cplusplus
extern "C" {
#endif


// Per overclock 
#define CLOCK16  0 
#define CLOCK33  1 
#define CLOCK40  2 
#define CLOCK67  3 
#define CLOCK80  4 
#define CLOCK102 5 
#define CLOCK132 6 

// Prototipus
void SetClockSpeed(int nClockSpeed); 


#ifdef __cplusplus
  }
#endif

#endif /*__clock_h__*/
