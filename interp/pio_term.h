#define PIO_T_STATUS    0xE000000
#define PIO_T_CMD       0xE000004
#define PIO_T_DATA      0xE000008

#define PIO_T_STATUS_READ_BUSY   0x01
#define PIO_T_STATUS_READ_DONE   0x02
#define PIO_T_STATUS_READ_ERROR  0x04
#define PIO_T_STATUS_WRITE_BUSY  0x11
#define PIO_T_STATUS_WRITE_DONE  0x12
#define PIO_T_STATUS_WRITE_ERROR 0x14
#define PIO_T_STATUS_ATTN        0x80

#define PIO_T_CMD_INT_ENA       0x00000001
#define PIO_T_CMD_START_READ    0x00000002

int PIO_T_Init();
int PIO_T_Finish();
