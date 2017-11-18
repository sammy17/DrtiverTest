#include "drivers/xfeature.h"

#include "include/xparameters.h"
#include <chrono>
#include <string.h>
#include <fstream>
#include <iostream>

//V4L2 Includes

#include <stdio.h>
#include <stdlib.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/v4l2-common.h>
#include <linux/v4l2-controls.h>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <string>
// #include <termios.h>

#include "axidma.h"
#include "frame1.h"


/***************** Macros (Inline Functions) Definitions *********************/

#define TX_BASE_ADDR 0x01000000
#define DDR_RANGE 0x01000000
#define RX_BASE_ADDR 0x02000000

#define AXILITES_BASEADDR 0x43C00000
#define CRTL_BUS_BASEADDR 0x43C10000
#define AXILITE_RANGE 0xFFFF

#define WIDTH 320
#define HEIGHT 240
#define BUF_SIZE(W,H) W*H
#define N BUF_SIZE(WIDTH,HEIGHT)

using namespace std;

/***************** Global Variables *********************/

uint8_t * TX_BASE_PTR = (uint8_t *) (TX_BASE_ADDR);
uint8_t * RX_BASE_PTR = (uint8_t *) (RX_BASE_ADDR);

//XBacksub backsub;
XFeature feature;
uint32_t * src;
uint8_t * dst;
int fdIP;

uint8_t * ybuffer = new uint8_t[N];


#define M_AXI_BOUNDING 0x21000000
#define M_AXI_FEATUREH 0x29000000

unsigned char *m_dma_buffer_TX = (unsigned char*) TX_BASE_ADDR;
unsigned char *m_dma_buffer_RX = (unsigned char*) RX_BASE_ADDR;

uint16_t * m_axi_bound = (uint16_t *) M_AXI_BOUNDING;
uint16_t * m_axi_feature = (uint16_t *) M_AXI_FEATUREH;

// void backsub_crtl_print(XBacksub * backsub_ptr){
//     printf("Backsub control reg : %X\n",backsub_ptr->Crtl_bus_BaseAddress);
//     printf("Backsub control reg value: %X\n",(unsigned int)(backsub_ptr->Crtl_bus_BaseAddress));
// }



int feature_init(XFeature * InstancePtr){
    InstancePtr->Axilites_BaseAddress = (u32)mmap(NULL, AXILITE_RANGE, PROT_READ|PROT_WRITE, MAP_SHARED, fdIP, XPAR_XFEATURE_0_S_AXI_AXILITES_BASEADDR);
    InstancePtr->Crtl_bus_BaseAddress = (u32)mmap(NULL, AXILITE_RANGE, PROT_READ|PROT_WRITE, MAP_SHARED, fdIP, XPAR_XFEATURE_0_S_AXI_CRTL_BUS_BASEADDR);
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}

void feature_rel(XFeature * InstancePtr){
    munmap((void*)InstancePtr->Axilites_BaseAddress, AXILITE_RANGE);
    munmap((void*)InstancePtr->Crtl_bus_BaseAddress, AXILITE_RANGE);
}


void print_config() {
//     printf("Is Ready = %d \n", XBacksub_IsReady(&backsub));
// //    printf("Frame in = %X \n", XBacksub_Get_frame_in(&backsub));
// //    printf("Frame out = %X \n", XBacksub_Get_frame_out(&backsub));
//     printf("Init = %d \n", XBacksub_Get_init(&backsub));
}


int main(int argc, char *argv[]) {

    // Initializing IP Core Starts here .........................
    fdIP = open ("/dev/mem", O_RDWR);
    if (fdIP < 1) {
        perror(argv[0]);
        return -1;
    }

    uint16_t * m_axi_bound = (uint16_t *) mmap(NULL, 80,PROT_READ|PROT_WRITE, MAP_SHARED, fdIP, M_AXI_BOUNDING);
    uint16_t * m_axi_feature = (uint16_t *) mmap(NULL, 5120*2,PROT_READ|PROT_WRITE, MAP_SHARED, fdIP,M_AXI_FEATUREH);


    src = (uint32_t*)mmap(NULL, DDR_RANGE,PROT_READ|PROT_WRITE, MAP_SHARED, fdIP, TX_BASE_ADDR); 
    dst = (uint8_t*)mmap(NULL, DDR_RANGE,PROT_EXEC|PROT_READ|PROT_WRITE, MAP_SHARED, fdIP, RX_BASE_ADDR); 

    if(feature_init(&feature)==0) {
        printf("IP Core Initialized\n");
    }

    memcpy(src,frame1,sizeof(unsigned int)*76800/2);
    

    // Initialize DMA 0
    unsigned int * dma_0_base = dma_init_2ch(fdIP,XPAR_AXI_DMA_0_BASEADDR,TX_BASE_ADDR,RX_BASE_ADDR);
    unsigned int * dma_1_base = dma_init_1ch(fdIP,XPAR_AXI_DMA_1_BASEADDR,TX_BASE_ADDR);
   
    /***************************** Begin looping here *********************/
    auto begin = std::chrono::high_resolution_clock::now();
    bool isFirst = true;
    for (int it=0;it<1;it++){
       
        printf("t1\n");
        printf("t2\n");

        // if (isFirst){
        //     backsub_config(true);
        //     isFirst = false;
        // }
        // else{
        //     backsub_config(false);
        // }
        // print_config();
        XFeature_Set_bounding(&feature,(u32)M_AXI_BOUNDING);
        XFeature_Set_featureh(&feature,(u32)M_AXI_FEATUREH);

        memset(m_axi_bound,0,sizeof(uint16_t)*40);

        m_axi_bound[0] = 24;
        m_axi_bound[1] = 24;
        m_axi_bound[2] = 128;
        m_axi_bound[3] = 128;
        printf("t4\n");

        d
     //    XBacksub_Start(&backsub);

     //    // backsub_crtl_print(&backsub);

     //    printf("DMA0 done\n");

     //    while(!XBacksub_IsDone(&backsub));
	    // printf("backsub finished\n");
        dma_transmit_data(dma_1_base,sizeof(unsigned int) * 76800/2);

        ma_config_print(dma_1_base);
        
        //dma_s2mm_sync(dma_1_base);
        XFeature_Start(&feature);
        printf("started\n");
        dma_mm2s_sync(dma_1_base);
        while(!XFeature_IsDone(&feature));


    }

    auto end = std::chrono::high_resolution_clock::now();
    /***************************** End looping here *********************/

    //Release IP Core
    //backsub_rel(&backsub);

    feature_rel(&feature);
    axilite_release(dma_1_base);

    munmap((void*)src, DDR_RANGE);
    munmap((void*)dst, DDR_RANGE);

    close(fdIP);
     
    printf("Elapsed time : %lld us\n",std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count());

    printf("Device unmapped\n");

    return 0;
}

