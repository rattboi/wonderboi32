

void cpu_speed(int,int,int);    // CPU Speed control (3 int's, clock, bus, dividor)
/****************************************************************

    Setup CPU Speed - Calls to CPUSPEED.S

****************************************************************/

int cpuSpeeds[6] = {132, 156, 166, 180, 200, 220};

void setCpuSpeed(int freq) {

       // To extend use: cpu_speed(CLK_SPEED, DIV_FACTOR, CLK_MODE);

	   if (freq == 220 ) cpu_speed(220000000, 0x2f010, 3 );
	
	   if (freq == 200 ) cpu_speed(200000000, 0x2a010, 3 );
	
	   if (freq == 180 ) cpu_speed(180000000, 0x16000, 3 );
	
       if (freq == 166 ) cpu_speed(165000000, 0x2f001, 3 );                       // 40 Bus

       if (freq == 156 ) cpu_speed(156000000, 0x2c001, 3 );                       // 36 Bus

       if (freq == 133 ) cpu_speed(133500000, (81<<12) | (2<<4) | 1, 2);    // 66 Bus

       if (freq == 132 ) cpu_speed(132000000, 0x3a011, 3 );                       // 33 Bus

       if (freq == 120 ) cpu_speed(120000000, 0x24001, 2 );                      

       if (freq == 100 ) cpu_speed(102000000, (43<<12) | (1<<4) | 1, 2);

       if (freq == 66 ) cpu_speed( 67500000, (37<<12) | (0<<4) | 2, 2);

       if (freq == 40 ) cpu_speed( 40000000, 0x48013, 1 );                        // Default

       if (freq == 33 ) cpu_speed( 33750000, (37<<12) | (0<<4) | 3, 2);     // Ultra slow
}
