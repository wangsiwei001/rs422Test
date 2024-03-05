
#ifndef _MAIN_TEST_
#define _MAIN_TEST_

//UINT16 crc16_ccitt_false(UINT8 *data, UINT16 length);
void SBufferProcess(unsigned char buffer[1024], unsigned int length);

void KABufferProcess(const unsigned char * buffer,unsigned int length);

void * taskSchedulingfun(void *pVoid);

void * taskSchedulingfun2(void *pVoid);
#endif