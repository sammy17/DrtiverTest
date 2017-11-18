
#include "axidma.h"

unsigned int dma_set(unsigned int* dma_virtual_address, int offset, unsigned int value) {
    dma_virtual_address[offset>>2] = value;
}

unsigned int dma_get(unsigned int* dma_virtual_address, int offset) {
    return dma_virtual_address[offset>>2];
}

int dma_mm2s_sync(unsigned int* dma_virtual_address) {
    unsigned int mm2s_status =  dma_get(dma_virtual_address, MM2S_STATUS_REGISTER);
    while(!(mm2s_status & 1) ){
        dma_s2mm_status(dma_virtual_address);
        dma_mm2s_status(dma_virtual_address);

        mm2s_status =  dma_get(dma_virtual_address, MM2S_STATUS_REGISTER);
    }
}

int dma_s2mm_sync(unsigned int* dma_virtual_address) {
    unsigned int s2mm_status = dma_get(dma_virtual_address, S2MM_STATUS_REGISTER);
    while(!(s2mm_status & 1<<12) || !(s2mm_status & 1<<1)){
        dma_s2mm_status(dma_virtual_address);
        dma_mm2s_status(dma_virtual_address);

        s2mm_status = dma_get(dma_virtual_address, S2MM_STATUS_REGISTER);
    }
}

void dma_s2mm_status(unsigned int* dma_virtual_address) {
    unsigned int status = dma_get(dma_virtual_address, S2MM_STATUS_REGISTER);
    printf("Stream to memory-mapped status (0x%08x@0x%02x):", status, S2MM_STATUS_REGISTER);
    if (status & 0x00000001) printf(" halted"); else printf(" running");
    if (status & 0x00000002) printf(" idle");
    if (status & 0x00000008) printf(" SGIncld");
    if (status & 0x00000010) printf(" DMAIntErr");
    if (status & 0x00000020) printf(" DMASlvErr");
    if (status & 0x00000040) printf(" DMADecErr");
    if (status & 0x00000100) printf(" SGIntErr");
    if (status & 0x00000200) printf(" SGSlvErr");
    if (status & 0x00000400) printf(" SGDecErr");
    if (status & 0x00001000) printf(" IOC_Irq");
    if (status & 0x00002000) printf(" Dly_Irq");
    if (status & 0x00004000) printf(" Err_Irq");
    printf("\n");
}

void dma_mm2s_status(unsigned int* dma_virtual_address) {
    unsigned int status = dma_get(dma_virtual_address, MM2S_STATUS_REGISTER);
    printf("Memory-mapped to stream status (0x%08x@0x%02x):", status, MM2S_STATUS_REGISTER);
    if (status & 0x00000001) printf(" halted"); else printf(" running");
    if (status & 0x00000002) printf(" idle");
    if (status & 0x00000008) printf(" SGIncld");
    if (status & 0x00000010) printf(" DMAIntErr");
    if (status & 0x00000020) printf(" DMASlvErr");
    if (status & 0x00000040) printf(" DMADecErr");
    if (status & 0x00000100) printf(" SGIntErr");
    if (status & 0x00000200) printf(" SGSlvErr");
    if (status & 0x00000400) printf(" SGDecErr");
    if (status & 0x00001000) printf(" IOC_Irq");
    if (status & 0x00002000) printf(" Dly_Irq");
    if (status & 0x00004000) printf(" Err_Irq");
    printf("\n");
}

void memdump(void* virtual_address, int byte_count) {
    char *p = virtual_address;
    int offset;
    for (offset = 0; offset < byte_count; offset++) {
        printf("%02x", p[offset]);
        if (offset % 4 == 3) { printf(" "); }
    }
    printf("\n");
}

unsigned int * dma_init_2ch(int fd, unsigned int base_addr,unsigned int src, unsigned int dst){
	unsigned int * virtual_address = mmap(NULL, AXILITE_RANGE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base_addr);
	printf("Resetting DMA\n");
    dma_set(virtual_address, S2MM_CONTROL_REGISTER, 4);
    dma_set(virtual_address, MM2S_CONTROL_REGISTER, 4);
    dma_s2mm_status(virtual_address);
    dma_mm2s_status(virtual_address);

    dma_set(virtual_address, S2MM_CONTROL_REGISTER, 0x0001);
	dma_set(virtual_address, MM2S_CONTROL_REGISTER, 0x0001);

    printf("Halting DMA\n");
    dma_set(virtual_address, S2MM_CONTROL_REGISTER, 0);
    dma_set(virtual_address, MM2S_CONTROL_REGISTER, 0);
    dma_s2mm_status(virtual_address);
    dma_mm2s_status(virtual_address);

    printf("Writing destination address\n");
    dma_set(virtual_address, S2MM_DESTINATION_ADDRESS, dst); // Write destination address
    dma_s2mm_status(virtual_address);

    printf("Writing source address...\n");
    dma_set(virtual_address, MM2S_START_ADDRESS, src); // Write source address
    dma_mm2s_status(virtual_address);

    return virtual_address;
}
// Only MM2S is enabled
unsigned int * dma_init_1ch(int fd, unsigned int base_addr,unsigned int src){
	unsigned int * virtual_address = mmap(NULL, AXILITE_RANGE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base_addr);
	printf("Resetting DMA\n");
    dma_set(virtual_address, MM2S_CONTROL_REGISTER, 4);

    dma_mm2s_status(virtual_address);

	dma_set(virtual_address, MM2S_CONTROL_REGISTER, 0x0001);

    printf("Halting DMA\n");

    dma_set(virtual_address, MM2S_CONTROL_REGISTER, 0);

    dma_mm2s_status(virtual_address);

    printf("Writing source address...\n");
    dma_set(virtual_address, MM2S_START_ADDRESS, src); // Write source address
    dma_mm2s_status(virtual_address);

    return virtual_address;
}

void axilite_release(unsigned int * virtual_address){
	munmap((void*)virtual_address, AXILITE_RANGE);
}

void dma_receive_data(unsigned int * virtual_address, unsigned int byte_count){
	printf("Writing S2MM transfer length...\n");
    dma_set(virtual_address, S2MM_LENGTH, byte_count);
    dma_s2mm_status(virtual_address);
}

void dma_transmit_data(unsigned int * virtual_address, unsigned int byte_count){
	printf("Writing MM2S transfer length...\n");
    dma_set(virtual_address, MM2S_LENGTH, byte_count);
    dma_mm2s_status(virtual_address);
}

void dma_config_print(unsigned int * virtual_address){
    printf("Printing DMA configuration at %X\n",virtual_address);
    printf("MM2S_CONTROL_REGISTER : %X\n",virtual_address[1]);
    printf("Source Address : %X\n",virtual_address[6]);
    printf("MM2S_LENGTH : %X\n",virtual_address[10]);
    printf("S2MM_CONTROL_REGISTER : %X\n",virtual_address[12]);
    printf("Destination Address : %X\n",virtual_address[18]);
    printf("S2MM_LENGTH : %X\n",virtual_address[22]);
}
