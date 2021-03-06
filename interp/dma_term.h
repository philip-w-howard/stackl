#define DMA_T_STATUS    0xF000000
#define DMA_T_CMD       0xF000004
#define DMA_T_ADDR      0xF000008
#define DMA_T_SIZE      0xF00000C

#define DMA_T_STATUS_READ_BUSY   0x00000001
#define DMA_T_STATUS_READ_DONE   0x00000002
#define DMA_T_STATUS_READ_ERROR  0x00000004
#define DMA_T_STATUS_WRITE_BUSY  0x00000011
#define DMA_T_STATUS_WRITE_DONE  0x00000012
#define DMA_T_STATUS_WRITE_ERROR 0x00000014
#define DMA_T_STATUS_ATTN        0x80000000

#define DMA_T_CMD_INT_ENA       0x00000001
#define DMA_T_CMD_START_READ    0x00000002
#define DMA_T_CMD_START_WRITE   0x00000004

#define DMA_T_VECTOR            4

int DMA_T_Init();
