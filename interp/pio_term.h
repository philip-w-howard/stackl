#define PIO_T_RDR       0xE000000
#define PIO_T_XDR       0xE000000
#define PIO_T_IER       0xE000001
#define PIO_T_IIR       0xE000002

#define PIO_T_IE_RECV            0x01
#define PIO_T_IE_XMIT            0x02
#define PIO_T_IID_INT            0x01
#define PIO_T_IID_XMIT           0x02
#define PIO_T_IID_RECV           0x04

int PIO_T_Init();
int PIO_T_Finish();
