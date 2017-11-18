#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/mman.h>

#define MM2S_CONTROL_REGISTER 0x00
#define MM2S_STATUS_REGISTER 0x04
#define MM2S_START_ADDRESS 0x18
#define MM2S_LENGTH 0x28

#define S2MM_CONTROL_REGISTER 0x30
#define S2MM_STATUS_REGISTER 0x34
#define S2MM_DESTINATION_ADDRESS 0x48
#define S2MM_LENGTH 0x58

#define AXILITE_RANGE 0xFFFF

unsigned int dma_set(unsigned int* dma_virtual_address, int offset, unsigned int value);
unsigned int dma_get(unsigned int* dma_virtual_address, int offset);
int dma_mm2s_sync(unsigned int* dma_virtual_address);
int dma_s2mm_sync(unsigned int* dma_virtual_address);
void dma_s2mm_status(unsigned int* dma_virtual_address);
void dma_mm2s_status(unsigned int* dma_virtual_address);
void memdump(void* virtual_address, int byte_count);

unsigned int* dma_init_2ch(int fd, unsigned int base_addr,unsigned int src, unsigned int dst);
unsigned int* dma_init_1ch(int fd, unsigned int base_addr,unsigned int src);
void axilite_release(unsigned int * virtual_address);
void dma_receive_data(unsigned int * virtual_address, unsigned int byte_count);
void dma_transmit_data(unsigned int * virtual_address, unsigned int byte_count);
void dma_config_print(unsigned int * virtual_address);

#ifdef __cplusplus
}
#endif
