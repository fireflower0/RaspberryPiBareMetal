#define MMIO_BASE       0x3F000000

/* mailbox message buffer */
volatile unsigned int  __attribute__((aligned(16))) mbox[36];

#define VIDEOCORE_MBOX  (MMIO_BASE+0x0000B880)
#define MBOX_READ       ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x00))
#define MBOX_POLL       ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x10))
#define MBOX_SENDER     ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x14))
#define MBOX_STATUS     ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x18))
#define MBOX_CONFIG     ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x1C))
#define MBOX_WRITE      ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x20))

#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000

#define MBOX_REQUEST    0

/* channels */
#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LEDS    4
#define MBOX_CH_BTNS    5
#define MBOX_CH_TOUCH   6
#define MBOX_CH_COUNT   7
#define MBOX_CH_PROP    8

/* tags */
#define MBOX_TAG_SETPOWER       0x28001
#define MBOX_TAG_SETCLKRATE     0x38002
#define MBOX_TAG_LAST           0

int mbox_call(unsigned char ch){
    unsigned int r = (((unsigned int)((unsigned long)&mbox) & ~0xF) | (ch & 0xF));

    /* メールボックスに書き込むまで待つ */
    do{
        asm volatile("nop");
    }while(*MBOX_STATUS & MBOX_FULL);

    /* メッセージのアドレスをチャネル識別子を持つメールボックスに書き込む */
    *MBOX_WRITE = r;

    while(1) {
        /* 応答を待つ */
        do{
            asm volatile("nop");
        }while(*MBOX_STATUS & MBOX_EMPTY);

        /* メッセージに対する応答か判定 */
        if(r == *MBOX_READ){
            /* 有効な成功応答か判定 */
            return mbox[1] == MBOX_RESPONSE;
        }
    }
    return 0;
}

unsigned int width, height, pitch, bpp;
unsigned char *lfb;

void lfb_init(){
    for(int i = 0; i < 36; i++){
        mbox[i] = 0;
    }

    mbox[0] = 112;
    mbox[1] = MBOX_REQUEST;

    mbox[2] = 0x48003;  //set phy wh
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = 1280;     //FrameBufferInfo.width
    mbox[6] = 800;      //FrameBufferInfo.height

    mbox[7] = 0x48004;  //set virt wh
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = 1280;    //FrameBufferInfo.virtual_width
    mbox[11] = 800;     //FrameBufferInfo.virtual_height
    
    mbox[12] = 0x48005; //set depth
    mbox[13] = 4;
    mbox[14] = 4;
    mbox[15] = 16;      //FrameBufferInfo.depth

    mbox[16] = 0x40008; //get pitch
    mbox[17] = 4;
    mbox[18] = 4;
    mbox[19] = 0;       //FrameBufferInfo.pitch

    mbox[20] = 0x40001; //get framebuffer, gets alignment on request
    mbox[21] = 8;
    mbox[22] = 8;
    mbox[23] = 4096;    //FrameBufferInfo.pointer
    mbox[24] = 0;       //FrameBufferInfo.size

    mbox[25] = MBOX_TAG_LAST;

    if(mbox_call(MBOX_CH_PROP) && mbox[15] == 16 && mbox[23] != 0) {
        mbox[23] &= 0x3FFFFFFF;
        width  = mbox[5];
        height = mbox[6];
        bpp    = mbox[15];
        pitch  = mbox[19];
        lfb = (void*)((unsigned long)mbox[23]);
    }
}

static inline void *coord2ptr(int x, int y) {
    return (void *) (lfb + (bpp + 7 >> 3) * x + pitch * y);
}

void hline16(int x, int y, int l, unsigned int c) {
    unsigned short int *p = (unsigned short int *) coord2ptr(x, y);
    if (width < l + x) {
        l = width - x;
    }
    for(int i = 0; i < l; i++) {
        *p++ = c;
    }
}

void vline16(int x, int y, int l, unsigned int c) {
    unsigned short int *p = (unsigned short int *) coord2ptr(x, y);
    if (height < l + y) {
        l = height - y;
    }
    for(int i = 0; i < l; i++) {
        *p = c;
        p += pitch >> 1;
    }
}

int main(void){
    unsigned int i;

    lfb_init();

    for(i = 0; i < height; i += 8) {
        hline16(0, i, width, 0x5050 * i);
    }
    for(i = 0; i < width; i += 8) {
        vline16(i, 0, height, 0xa0a0 * i);
    }

    while(1)
        ;

    return 0;
}
