#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cbm.h>

// #pragma static-locals (1); // speed up method execution with static locals

#define poke(addr,val)     (*(unsigned char*) (addr) = (val))
#define pokew(addr,val)    (*(unsigned*) (addr) = (val))
#define peek(addr)         (*(unsigned char*) (addr))
#define peekw(addr)        (*(unsigned*) (addr))


#define sprptr2 0x0340
#define sprptr3 0x0380
#define sprptr4 0x0300
#define sprptr5 0x02C0
#define charset 0x3800
#define screen 0x0400

// consts
const unsigned char balloon[3][63] = 
{
    {
        0,62,0,
        0,255,128,
        1,255,192,
        3,255,224,
        7,231,240,
        7,217,240,
        7,223,240,
        7,217,240,
        3,231,224,
        3,255,224,
        1,255,192,
        1,255,192,
        1,255,192,
        0,190,128,
        0,156,128,
        0,73,0,
        0,65,0,
        0,62,0,
        0,62,0,
        0,62,0,
        0,28,0
    },
    { 
        0,127,0,
        1,255,192,
        3,255,224,
        3,231,224,
        7,217,240,
        7,223,240,
        7,217,240,
        3,231,224,
        3,255,224,
        3,255,224,
        2,255,160,
        1,127,64,
        1,62,64,
        0,156,128,
        0,156,128,
        0,73,0,
        0,73,0,
        0,62,0,
        0,62,0,
        0,62,0,
        0,28,0 
    },
    {
        0,127,128,
        0,255,192,
        1,255,224,
        1,255,224,
        1,247,224,
        1,233,240,
        0,239,224,
        1,233,192,
        0,247,128,
        1,255,0,
        0,255,128,
        0,127,192,
        0,63,192,
        0,63,128,
        0,127,0,
        0,73,0,
        0,34,0,
        0,62,0,
        0,62,0,
        0,62,0,
        0,28,0
    }
};

const unsigned char balloonColor[63] = {
    0,0,0,
    0,0,0,
    0,21,0,
    0,21,0,
    0,21,0,
    0,21,0,
    0,26,0,
    0,26,0,
    0,22,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0
};

const char wall = 160;
const char star = 161;
const char brick = 162;

const unsigned char wallchar[8] = { 0,170,0,40,20,0,170,0 };

const unsigned char starchar[3][8] = {
    { 0,0,24,52,60,24,0,0 },
    { 8,24,255,118,60,60,102,66 },
    { 0,8,24,122,60,24,20,0 }
};

const unsigned char brickchar[4][8] = {
    { 0,60,214,122,94,234,60,0 },
    { 0,60,214,106,86,107,60,0 },
    { 0,60,99,74,82,71,60,0 },
    { 0,60,103,74,82,230,60,0 }
};

const unsigned char space[16] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    12,12,12,8,8,0,24,24
};

const unsigned char separators[32] = {
    0,0,0,0,0,0,24,16,
    0,0,0,60,0,0,0,0,
    0,0,0,0,0,0,24,0,
    24,8,0,0,0,0,0,0
};

const unsigned char numbers[80] = {
    28,34,38,42,50,50,28,0,
    8,8,8,8,12,12,12,0,
    28,2,2,28,48,48,30,0,
    28,2,2,12,6,6,60,0,
    12,20,36,36,62,12,12,0,
    28,32,32,28,6,6,60,0,
    24,32,32,60,50,50,28,0,
    60,2,4,8,8,24,24,0,
    28,50,50,28,50,50,28,0,
    28,50,50,30,6,12,24,0
};

const unsigned char letters[208] = {
    24,36,66,66,126,98,98,0,
    60,66,66,124,98,98,124,0,
    60,64,64,64,96,96,60,0,
    120,68,66,66,98,100,120,0,
    126,64,64,120,96,96,126,0,
    126,64,64,120,96,96,96,0,
    60,64,64,70,98,98,60,0,
    66,66,66,126,98,98,98,0,
    8,8,8,24,24,24,24,0,
    8,8,8,24,24,88,120,0,
    72,72,80,96,120,100,100,0,
    64,64,64,64,96,96,120,0,
    65,99,85,73,73,97,97,0,
    66,98,82,82,74,102,98,0,
    60,66,66,66,98,98,60,0,
    60,66,66,66,124,96,96,0,
    60,66,66,66,66,60,14,0,
    124,66,66,68,120,100,98,0,
    60,64,64,60,6,6,124,0,
    124,16,16,16,16,24,24,0,
    66,66,66,98,98,98,60,0,
    66,66,66,98,98,36,24,0,
    65,65,65,73,93,117,99,0,
    66,66,36,24,36,98,98,0,
    66,66,66,60,16,24,24,0,
    60,4,8,16,48,48,60,0
};

const unsigned char colors[4] = {
    COLOR_WHITE,
    COLOR_GRAY3,
    COLOR_GRAY2,
    COLOR_GRAY1
};

const signed int maxinertia = 50;

// vars
static unsigned char i = 0;
static unsigned int j = 0;

static unsigned int height = 10000u;
static int inertia = 1;
static unsigned char ycoord = 0;

static unsigned char scrollpos = 7;
static unsigned int counter = 0;

static unsigned char level = 1;

static unsigned long score = 0;
static unsigned long highscore = 99;

static unsigned char spr = 0;
static unsigned char oldspr = 0;

static unsigned char buffer[8];

static unsigned char starframe = 0;
static unsigned char brickframe = 0;

static unsigned int loc = 0;

// methods
static void cputsxy(char x, char y, char* value) {
    loc = screen + x + y * 40;
    memcpy((void *)(loc), value, (unsigned)strlen(value));
}

static void scrollleftasm() {
    // soft scroll memory
    __asm__("lda $0429");
    __asm__("sta $0428");
    __asm__("lda $042A");
    __asm__("sta $0429");
    __asm__("lda $042B");
    __asm__("sta $042A");
    __asm__("lda $042C");
    __asm__("sta $042B");
    __asm__("lda $042D");
    __asm__("sta $042C");
    __asm__("lda $042E");
    __asm__("sta $042D");
    __asm__("lda $042F");
    __asm__("sta $042E");
    __asm__("lda $0430");
    __asm__("sta $042F");
    __asm__("lda $0431");
    __asm__("sta $0430");
    __asm__("lda $0432");
    __asm__("sta $0431");
    __asm__("lda $0433");
    __asm__("sta $0432");
    __asm__("lda $0434");
    __asm__("sta $0433");
    __asm__("lda $0435");
    __asm__("sta $0434");
    __asm__("lda $0436");
    __asm__("sta $0435");
    __asm__("lda $0437");
    __asm__("sta $0436");
    __asm__("lda $0438");
    __asm__("sta $0437");
    __asm__("lda $0439");
    __asm__("sta $0438");
    __asm__("lda $043A");
    __asm__("sta $0439");
    __asm__("lda $043B");
    __asm__("sta $043A");
    __asm__("lda $043C");
    __asm__("sta $043B");
    __asm__("lda $043D");
    __asm__("sta $043C");
    __asm__("lda $043E");
    __asm__("sta $043D");
    __asm__("lda $043F");
    __asm__("sta $043E");
    __asm__("lda $0440");
    __asm__("sta $043F");
    __asm__("lda $0441");
    __asm__("sta $0440");
    __asm__("lda $0442");
    __asm__("sta $0441");
    __asm__("lda $0443");
    __asm__("sta $0442");
    __asm__("lda $0444");
    __asm__("sta $0443");
    __asm__("lda $0445");
    __asm__("sta $0444");
    __asm__("lda $0446");
    __asm__("sta $0445");
    __asm__("lda $0447");
    __asm__("sta $0446");
    __asm__("lda $0448");
    __asm__("sta $0447");
    __asm__("lda $0449");
    __asm__("sta $0448");
    __asm__("lda $044A");
    __asm__("sta $0449");
    __asm__("lda $044B");
    __asm__("sta $044A");
    __asm__("lda $044C");
    __asm__("sta $044B");
    __asm__("lda $044D");
    __asm__("sta $044C");
    __asm__("lda $044E");
    __asm__("sta $044D");
    __asm__("lda $044F");
    __asm__("sta $044E");
    __asm__("lda $0451");
    __asm__("sta $0450");
    __asm__("lda $0452");
    __asm__("sta $0451");
    __asm__("lda $0453");
    __asm__("sta $0452");
    __asm__("lda $0454");
    __asm__("sta $0453");
    __asm__("lda $0455");
    __asm__("sta $0454");
    __asm__("lda $0456");
    __asm__("sta $0455");
    __asm__("lda $0457");
    __asm__("sta $0456");
    __asm__("lda $0458");
    __asm__("sta $0457");
    __asm__("lda $0459");
    __asm__("sta $0458");
    __asm__("lda $045A");
    __asm__("sta $0459");
    __asm__("lda $045B");
    __asm__("sta $045A");
    __asm__("lda $045C");
    __asm__("sta $045B");
    __asm__("lda $045D");
    __asm__("sta $045C");
    __asm__("lda $045E");
    __asm__("sta $045D");
    __asm__("lda $045F");
    __asm__("sta $045E");
    __asm__("lda $0460");
    __asm__("sta $045F");
    __asm__("lda $0461");
    __asm__("sta $0460");
    __asm__("lda $0462");
    __asm__("sta $0461");
    __asm__("lda $0463");
    __asm__("sta $0462");
    __asm__("lda $0464");
    __asm__("sta $0463");
    __asm__("lda $0465");
    __asm__("sta $0464");
    __asm__("lda $0466");
    __asm__("sta $0465");
    __asm__("lda $0467");
    __asm__("sta $0466");
    __asm__("lda $0468");
    __asm__("sta $0467");
    __asm__("lda $0469");
    __asm__("sta $0468");
    __asm__("lda $046A");
    __asm__("sta $0469");
    __asm__("lda $046B");
    __asm__("sta $046A");
    __asm__("lda $046C");
    __asm__("sta $046B");
    __asm__("lda $046D");
    __asm__("sta $046C");
    __asm__("lda $046E");
    __asm__("sta $046D");
    __asm__("lda $046F");
    __asm__("sta $046E");
    __asm__("lda $0470");
    __asm__("sta $046F");
    __asm__("lda $0471");
    __asm__("sta $0470");
    __asm__("lda $0472");
    __asm__("sta $0471");
    __asm__("lda $0473");
    __asm__("sta $0472");
    __asm__("lda $0474");
    __asm__("sta $0473");
    __asm__("lda $0475");
    __asm__("sta $0474");
    __asm__("lda $0476");
    __asm__("sta $0475");
    __asm__("lda $0477");
    __asm__("sta $0476");
    __asm__("lda $0479");
    __asm__("sta $0478");
    __asm__("lda $047A");
    __asm__("sta $0479");
    __asm__("lda $047B");
    __asm__("sta $047A");
    __asm__("lda $047C");
    __asm__("sta $047B");
    __asm__("lda $047D");
    __asm__("sta $047C");
    __asm__("lda $047E");
    __asm__("sta $047D");
    __asm__("lda $047F");
    __asm__("sta $047E");
    __asm__("lda $0480");
    __asm__("sta $047F");
    __asm__("lda $0481");
    __asm__("sta $0480");
    __asm__("lda $0482");
    __asm__("sta $0481");
    __asm__("lda $0483");
    __asm__("sta $0482");
    __asm__("lda $0484");
    __asm__("sta $0483");
    __asm__("lda $0485");
    __asm__("sta $0484");
    __asm__("lda $0486");
    __asm__("sta $0485");
    __asm__("lda $0487");
    __asm__("sta $0486");
    __asm__("lda $0488");
    __asm__("sta $0487");
    __asm__("lda $0489");
    __asm__("sta $0488");
    __asm__("lda $048A");
    __asm__("sta $0489");
    __asm__("lda $048B");
    __asm__("sta $048A");
    __asm__("lda $048C");
    __asm__("sta $048B");
    __asm__("lda $048D");
    __asm__("sta $048C");
    __asm__("lda $048E");
    __asm__("sta $048D");
    __asm__("lda $048F");
    __asm__("sta $048E");
    __asm__("lda $0490");
    __asm__("sta $048F");
    __asm__("lda $0491");
    __asm__("sta $0490");
    __asm__("lda $0492");
    __asm__("sta $0491");
    __asm__("lda $0493");
    __asm__("sta $0492");
    __asm__("lda $0494");
    __asm__("sta $0493");
    __asm__("lda $0495");
    __asm__("sta $0494");
    __asm__("lda $0496");
    __asm__("sta $0495");
    __asm__("lda $0497");
    __asm__("sta $0496");
    __asm__("lda $0498");
    __asm__("sta $0497");
    __asm__("lda $0499");
    __asm__("sta $0498");
    __asm__("lda $049A");
    __asm__("sta $0499");
    __asm__("lda $049B");
    __asm__("sta $049A");
    __asm__("lda $049C");
    __asm__("sta $049B");
    __asm__("lda $049D");
    __asm__("sta $049C");
    __asm__("lda $049E");
    __asm__("sta $049D");
    __asm__("lda $049F");
    __asm__("sta $049E");
    __asm__("lda $04A1");
    __asm__("sta $04A0");
    __asm__("lda $04A2");
    __asm__("sta $04A1");
    __asm__("lda $04A3");
    __asm__("sta $04A2");
    __asm__("lda $04A4");
    __asm__("sta $04A3");
    __asm__("lda $04A5");
    __asm__("sta $04A4");
    __asm__("lda $04A6");
    __asm__("sta $04A5");
    __asm__("lda $04A7");
    __asm__("sta $04A6");
    __asm__("lda $04A8");
    __asm__("sta $04A7");
    __asm__("lda $04A9");
    __asm__("sta $04A8");
    __asm__("lda $04AA");
    __asm__("sta $04A9");
    __asm__("lda $04AB");
    __asm__("sta $04AA");
    __asm__("lda $04AC");
    __asm__("sta $04AB");
    __asm__("lda $04AD");
    __asm__("sta $04AC");
    __asm__("lda $04AE");
    __asm__("sta $04AD");
    __asm__("lda $04AF");
    __asm__("sta $04AE");
    __asm__("lda $04B0");
    __asm__("sta $04AF");
    __asm__("lda $04B1");
    __asm__("sta $04B0");
    __asm__("lda $04B2");
    __asm__("sta $04B1");
    __asm__("lda $04B3");
    __asm__("sta $04B2");
    __asm__("lda $04B4");
    __asm__("sta $04B3");
    __asm__("lda $04B5");
    __asm__("sta $04B4");
    __asm__("lda $04B6");
    __asm__("sta $04B5");
    __asm__("lda $04B7");
    __asm__("sta $04B6");
    __asm__("lda $04B8");
    __asm__("sta $04B7");
    __asm__("lda $04B9");
    __asm__("sta $04B8");
    __asm__("lda $04BA");
    __asm__("sta $04B9");
    __asm__("lda $04BB");
    __asm__("sta $04BA");
    __asm__("lda $04BC");
    __asm__("sta $04BB");
    __asm__("lda $04BD");
    __asm__("sta $04BC");
    __asm__("lda $04BE");
    __asm__("sta $04BD");
    __asm__("lda $04BF");
    __asm__("sta $04BE");
    __asm__("lda $04C0");
    __asm__("sta $04BF");
    __asm__("lda $04C1");
    __asm__("sta $04C0");
    __asm__("lda $04C2");
    __asm__("sta $04C1");
    __asm__("lda $04C3");
    __asm__("sta $04C2");
    __asm__("lda $04C4");
    __asm__("sta $04C3");
    __asm__("lda $04C5");
    __asm__("sta $04C4");
    __asm__("lda $04C6");
    __asm__("sta $04C5");
    __asm__("lda $04C7");
    __asm__("sta $04C6");
    __asm__("lda $04C9");
    __asm__("sta $04C8");
    __asm__("lda $04CA");
    __asm__("sta $04C9");
    __asm__("lda $04CB");
    __asm__("sta $04CA");
    __asm__("lda $04CC");
    __asm__("sta $04CB");
    __asm__("lda $04CD");
    __asm__("sta $04CC");
    __asm__("lda $04CE");
    __asm__("sta $04CD");
    __asm__("lda $04CF");
    __asm__("sta $04CE");
    __asm__("lda $04D0");
    __asm__("sta $04CF");
    __asm__("lda $04D1");
    __asm__("sta $04D0");
    __asm__("lda $04D2");
    __asm__("sta $04D1");
    __asm__("lda $04D3");
    __asm__("sta $04D2");
    __asm__("lda $04D4");
    __asm__("sta $04D3");
    __asm__("lda $04D5");
    __asm__("sta $04D4");
    __asm__("lda $04D6");
    __asm__("sta $04D5");
    __asm__("lda $04D7");
    __asm__("sta $04D6");
    __asm__("lda $04D8");
    __asm__("sta $04D7");
    __asm__("lda $04D9");
    __asm__("sta $04D8");
    __asm__("lda $04DA");
    __asm__("sta $04D9");
    __asm__("lda $04DB");
    __asm__("sta $04DA");
    __asm__("lda $04DC");
    __asm__("sta $04DB");
    __asm__("lda $04DD");
    __asm__("sta $04DC");
    __asm__("lda $04DE");
    __asm__("sta $04DD");
    __asm__("lda $04DF");
    __asm__("sta $04DE");
    __asm__("lda $04E0");
    __asm__("sta $04DF");
    __asm__("lda $04E1");
    __asm__("sta $04E0");
    __asm__("lda $04E2");
    __asm__("sta $04E1");
    __asm__("lda $04E3");
    __asm__("sta $04E2");
    __asm__("lda $04E4");
    __asm__("sta $04E3");
    __asm__("lda $04E5");
    __asm__("sta $04E4");
    __asm__("lda $04E6");
    __asm__("sta $04E5");
    __asm__("lda $04E7");
    __asm__("sta $04E6");
    __asm__("lda $04E8");
    __asm__("sta $04E7");
    __asm__("lda $04E9");
    __asm__("sta $04E8");
    __asm__("lda $04EA");
    __asm__("sta $04E9");
    __asm__("lda $04EB");
    __asm__("sta $04EA");
    __asm__("lda $04EC");
    __asm__("sta $04EB");
    __asm__("lda $04ED");
    __asm__("sta $04EC");
    __asm__("lda $04EE");
    __asm__("sta $04ED");
    __asm__("lda $04EF");
    __asm__("sta $04EE");
    __asm__("lda $04F1");
    __asm__("sta $04F0");
    __asm__("lda $04F2");
    __asm__("sta $04F1");
    __asm__("lda $04F3");
    __asm__("sta $04F2");
    __asm__("lda $04F4");
    __asm__("sta $04F3");
    __asm__("lda $04F5");
    __asm__("sta $04F4");
    __asm__("lda $04F6");
    __asm__("sta $04F5");
    __asm__("lda $04F7");
    __asm__("sta $04F6");
    __asm__("lda $04F8");
    __asm__("sta $04F7");
    __asm__("lda $04F9");
    __asm__("sta $04F8");
    __asm__("lda $04FA");
    __asm__("sta $04F9");
    __asm__("lda $04FB");
    __asm__("sta $04FA");
    __asm__("lda $04FC");
    __asm__("sta $04FB");
    __asm__("lda $04FD");
    __asm__("sta $04FC");
    __asm__("lda $04FE");
    __asm__("sta $04FD");
    __asm__("lda $04FF");
    __asm__("sta $04FE");
    __asm__("lda $0500");
    __asm__("sta $04FF");
    __asm__("lda $0501");
    __asm__("sta $0500");
    __asm__("lda $0502");
    __asm__("sta $0501");
    __asm__("lda $0503");
    __asm__("sta $0502");
    __asm__("lda $0504");
    __asm__("sta $0503");
    __asm__("lda $0505");
    __asm__("sta $0504");
    __asm__("lda $0506");
    __asm__("sta $0505");
    __asm__("lda $0507");
    __asm__("sta $0506");
    __asm__("lda $0508");
    __asm__("sta $0507");
    __asm__("lda $0509");
    __asm__("sta $0508");
    __asm__("lda $050A");
    __asm__("sta $0509");
    __asm__("lda $050B");
    __asm__("sta $050A");
    __asm__("lda $050C");
    __asm__("sta $050B");
    __asm__("lda $050D");
    __asm__("sta $050C");
    __asm__("lda $050E");
    __asm__("sta $050D");
    __asm__("lda $050F");
    __asm__("sta $050E");
    __asm__("lda $0510");
    __asm__("sta $050F");
    __asm__("lda $0511");
    __asm__("sta $0510");
    __asm__("lda $0512");
    __asm__("sta $0511");
    __asm__("lda $0513");
    __asm__("sta $0512");
    __asm__("lda $0514");
    __asm__("sta $0513");
    __asm__("lda $0515");
    __asm__("sta $0514");
    __asm__("lda $0516");
    __asm__("sta $0515");
    __asm__("lda $0517");
    __asm__("sta $0516");
    __asm__("lda $0519");
    __asm__("sta $0518");
    __asm__("lda $051A");
    __asm__("sta $0519");
    __asm__("lda $051B");
    __asm__("sta $051A");
    __asm__("lda $051C");
    __asm__("sta $051B");
    __asm__("lda $051D");
    __asm__("sta $051C");
    __asm__("lda $051E");
    __asm__("sta $051D");
    __asm__("lda $051F");
    __asm__("sta $051E");
    __asm__("lda $0520");
    __asm__("sta $051F");
    __asm__("lda $0521");
    __asm__("sta $0520");
    __asm__("lda $0522");
    __asm__("sta $0521");
    __asm__("lda $0523");
    __asm__("sta $0522");
    __asm__("lda $0524");
    __asm__("sta $0523");
    __asm__("lda $0525");
    __asm__("sta $0524");
    __asm__("lda $0526");
    __asm__("sta $0525");
    __asm__("lda $0527");
    __asm__("sta $0526");
    __asm__("lda $0528");
    __asm__("sta $0527");
    __asm__("lda $0529");
    __asm__("sta $0528");
    __asm__("lda $052A");
    __asm__("sta $0529");
    __asm__("lda $052B");
    __asm__("sta $052A");
    __asm__("lda $052C");
    __asm__("sta $052B");
    __asm__("lda $052D");
    __asm__("sta $052C");
    __asm__("lda $052E");
    __asm__("sta $052D");
    __asm__("lda $052F");
    __asm__("sta $052E");
    __asm__("lda $0530");
    __asm__("sta $052F");
    __asm__("lda $0531");
    __asm__("sta $0530");
    __asm__("lda $0532");
    __asm__("sta $0531");
    __asm__("lda $0533");
    __asm__("sta $0532");
    __asm__("lda $0534");
    __asm__("sta $0533");
    __asm__("lda $0535");
    __asm__("sta $0534");
    __asm__("lda $0536");
    __asm__("sta $0535");
    __asm__("lda $0537");
    __asm__("sta $0536");
    __asm__("lda $0538");
    __asm__("sta $0537");
    __asm__("lda $0539");
    __asm__("sta $0538");
    __asm__("lda $053A");
    __asm__("sta $0539");
    __asm__("lda $053B");
    __asm__("sta $053A");
    __asm__("lda $053C");
    __asm__("sta $053B");
    __asm__("lda $053D");
    __asm__("sta $053C");
    __asm__("lda $053E");
    __asm__("sta $053D");
    __asm__("lda $053F");
    __asm__("sta $053E");
    __asm__("lda $0541");
    __asm__("sta $0540");
    __asm__("lda $0542");
    __asm__("sta $0541");
    __asm__("lda $0543");
    __asm__("sta $0542");
    __asm__("lda $0544");
    __asm__("sta $0543");
    __asm__("lda $0545");
    __asm__("sta $0544");
    __asm__("lda $0546");
    __asm__("sta $0545");
    __asm__("lda $0547");
    __asm__("sta $0546");
    __asm__("lda $0548");
    __asm__("sta $0547");
    __asm__("lda $0549");
    __asm__("sta $0548");
    __asm__("lda $054A");
    __asm__("sta $0549");
    __asm__("lda $054B");
    __asm__("sta $054A");
    __asm__("lda $054C");
    __asm__("sta $054B");
    __asm__("lda $054D");
    __asm__("sta $054C");
    __asm__("lda $054E");
    __asm__("sta $054D");
    __asm__("lda $054F");
    __asm__("sta $054E");
    __asm__("lda $0550");
    __asm__("sta $054F");
    __asm__("lda $0551");
    __asm__("sta $0550");
    __asm__("lda $0552");
    __asm__("sta $0551");
    __asm__("lda $0553");
    __asm__("sta $0552");
    __asm__("lda $0554");
    __asm__("sta $0553");
    __asm__("lda $0555");
    __asm__("sta $0554");
    __asm__("lda $0556");
    __asm__("sta $0555");
    __asm__("lda $0557");
    __asm__("sta $0556");
    __asm__("lda $0558");
    __asm__("sta $0557");
    __asm__("lda $0559");
    __asm__("sta $0558");
    __asm__("lda $055A");
    __asm__("sta $0559");
    __asm__("lda $055B");
    __asm__("sta $055A");
    __asm__("lda $055C");
    __asm__("sta $055B");
    __asm__("lda $055D");
    __asm__("sta $055C");
    __asm__("lda $055E");
    __asm__("sta $055D");
    __asm__("lda $055F");
    __asm__("sta $055E");
    __asm__("lda $0560");
    __asm__("sta $055F");
    __asm__("lda $0561");
    __asm__("sta $0560");
    __asm__("lda $0562");
    __asm__("sta $0561");
    __asm__("lda $0563");
    __asm__("sta $0562");
    __asm__("lda $0564");
    __asm__("sta $0563");
    __asm__("lda $0565");
    __asm__("sta $0564");
    __asm__("lda $0566");
    __asm__("sta $0565");
    __asm__("lda $0567");
    __asm__("sta $0566");
    __asm__("lda $0569");
    __asm__("sta $0568");
    __asm__("lda $056A");
    __asm__("sta $0569");
    __asm__("lda $056B");
    __asm__("sta $056A");
    __asm__("lda $056C");
    __asm__("sta $056B");
    __asm__("lda $056D");
    __asm__("sta $056C");
    __asm__("lda $056E");
    __asm__("sta $056D");
    __asm__("lda $056F");
    __asm__("sta $056E");
    __asm__("lda $0570");
    __asm__("sta $056F");
    __asm__("lda $0571");
    __asm__("sta $0570");
    __asm__("lda $0572");
    __asm__("sta $0571");
    __asm__("lda $0573");
    __asm__("sta $0572");
    __asm__("lda $0574");
    __asm__("sta $0573");
    __asm__("lda $0575");
    __asm__("sta $0574");
    __asm__("lda $0576");
    __asm__("sta $0575");
    __asm__("lda $0577");
    __asm__("sta $0576");
    __asm__("lda $0578");
    __asm__("sta $0577");
    __asm__("lda $0579");
    __asm__("sta $0578");
    __asm__("lda $057A");
    __asm__("sta $0579");
    __asm__("lda $057B");
    __asm__("sta $057A");
    __asm__("lda $057C");
    __asm__("sta $057B");
    __asm__("lda $057D");
    __asm__("sta $057C");
    __asm__("lda $057E");
    __asm__("sta $057D");
    __asm__("lda $057F");
    __asm__("sta $057E");
    __asm__("lda $0580");
    __asm__("sta $057F");
    __asm__("lda $0581");
    __asm__("sta $0580");
    __asm__("lda $0582");
    __asm__("sta $0581");
    __asm__("lda $0583");
    __asm__("sta $0582");
    __asm__("lda $0584");
    __asm__("sta $0583");
    __asm__("lda $0585");
    __asm__("sta $0584");
    __asm__("lda $0586");
    __asm__("sta $0585");
    __asm__("lda $0587");
    __asm__("sta $0586");
    __asm__("lda $0588");
    __asm__("sta $0587");
    __asm__("lda $0589");
    __asm__("sta $0588");
    __asm__("lda $058A");
    __asm__("sta $0589");
    __asm__("lda $058B");
    __asm__("sta $058A");
    __asm__("lda $058C");
    __asm__("sta $058B");
    __asm__("lda $058D");
    __asm__("sta $058C");
    __asm__("lda $058E");
    __asm__("sta $058D");
    __asm__("lda $058F");
    __asm__("sta $058E");
    __asm__("lda $0591");
    __asm__("sta $0590");
    __asm__("lda $0592");
    __asm__("sta $0591");
    __asm__("lda $0593");
    __asm__("sta $0592");
    __asm__("lda $0594");
    __asm__("sta $0593");
    __asm__("lda $0595");
    __asm__("sta $0594");
    __asm__("lda $0596");
    __asm__("sta $0595");
    __asm__("lda $0597");
    __asm__("sta $0596");
    __asm__("lda $0598");
    __asm__("sta $0597");
    __asm__("lda $0599");
    __asm__("sta $0598");
    __asm__("lda $059A");
    __asm__("sta $0599");
    __asm__("lda $059B");
    __asm__("sta $059A");
    __asm__("lda $059C");
    __asm__("sta $059B");
    __asm__("lda $059D");
    __asm__("sta $059C");
    __asm__("lda $059E");
    __asm__("sta $059D");
    __asm__("lda $059F");
    __asm__("sta $059E");
    __asm__("lda $05A0");
    __asm__("sta $059F");
    __asm__("lda $05A1");
    __asm__("sta $05A0");
    __asm__("lda $05A2");
    __asm__("sta $05A1");
    __asm__("lda $05A3");
    __asm__("sta $05A2");
    __asm__("lda $05A4");
    __asm__("sta $05A3");
    __asm__("lda $05A5");
    __asm__("sta $05A4");
    __asm__("lda $05A6");
    __asm__("sta $05A5");
    __asm__("lda $05A7");
    __asm__("sta $05A6");
    __asm__("lda $05A8");
    __asm__("sta $05A7");
    __asm__("lda $05A9");
    __asm__("sta $05A8");
    __asm__("lda $05AA");
    __asm__("sta $05A9");
    __asm__("lda $05AB");
    __asm__("sta $05AA");
    __asm__("lda $05AC");
    __asm__("sta $05AB");
    __asm__("lda $05AD");
    __asm__("sta $05AC");
    __asm__("lda $05AE");
    __asm__("sta $05AD");
    __asm__("lda $05AF");
    __asm__("sta $05AE");
    __asm__("lda $05B0");
    __asm__("sta $05AF");
    __asm__("lda $05B1");
    __asm__("sta $05B0");
    __asm__("lda $05B2");
    __asm__("sta $05B1");
    __asm__("lda $05B3");
    __asm__("sta $05B2");
    __asm__("lda $05B4");
    __asm__("sta $05B3");
    __asm__("lda $05B5");
    __asm__("sta $05B4");
    __asm__("lda $05B6");
    __asm__("sta $05B5");
    __asm__("lda $05B7");
    __asm__("sta $05B6");
    __asm__("lda $05B9");
    __asm__("sta $05B8");
    __asm__("lda $05BA");
    __asm__("sta $05B9");
    __asm__("lda $05BB");
    __asm__("sta $05BA");
    __asm__("lda $05BC");
    __asm__("sta $05BB");
    __asm__("lda $05BD");
    __asm__("sta $05BC");
    __asm__("lda $05BE");
    __asm__("sta $05BD");
    __asm__("lda $05BF");
    __asm__("sta $05BE");
    __asm__("lda $05C0");
    __asm__("sta $05BF");
    __asm__("lda $05C1");
    __asm__("sta $05C0");
    __asm__("lda $05C2");
    __asm__("sta $05C1");
    __asm__("lda $05C3");
    __asm__("sta $05C2");
    __asm__("lda $05C4");
    __asm__("sta $05C3");
    __asm__("lda $05C5");
    __asm__("sta $05C4");
    __asm__("lda $05C6");
    __asm__("sta $05C5");
    __asm__("lda $05C7");
    __asm__("sta $05C6");
    __asm__("lda $05C8");
    __asm__("sta $05C7");
    __asm__("lda $05C9");
    __asm__("sta $05C8");
    __asm__("lda $05CA");
    __asm__("sta $05C9");
    __asm__("lda $05CB");
    __asm__("sta $05CA");
    __asm__("lda $05CC");
    __asm__("sta $05CB");
    __asm__("lda $05CD");
    __asm__("sta $05CC");
    __asm__("lda $05CE");
    __asm__("sta $05CD");
    __asm__("lda $05CF");
    __asm__("sta $05CE");
    __asm__("lda $05D0");
    __asm__("sta $05CF");
    __asm__("lda $05D1");
    __asm__("sta $05D0");
    __asm__("lda $05D2");
    __asm__("sta $05D1");
    __asm__("lda $05D3");
    __asm__("sta $05D2");
    __asm__("lda $05D4");
    __asm__("sta $05D3");
    __asm__("lda $05D5");
    __asm__("sta $05D4");
    __asm__("lda $05D6");
    __asm__("sta $05D5");
    __asm__("lda $05D7");
    __asm__("sta $05D6");
    __asm__("lda $05D8");
    __asm__("sta $05D7");
    __asm__("lda $05D9");
    __asm__("sta $05D8");
    __asm__("lda $05DA");
    __asm__("sta $05D9");
    __asm__("lda $05DB");
    __asm__("sta $05DA");
    __asm__("lda $05DC");
    __asm__("sta $05DB");
    __asm__("lda $05DD");
    __asm__("sta $05DC");
    __asm__("lda $05DE");
    __asm__("sta $05DD");
    __asm__("lda $05DF");
    __asm__("sta $05DE");
    __asm__("lda $05E1");
    __asm__("sta $05E0");
    __asm__("lda $05E2");
    __asm__("sta $05E1");
    __asm__("lda $05E3");
    __asm__("sta $05E2");
    __asm__("lda $05E4");
    __asm__("sta $05E3");
    __asm__("lda $05E5");
    __asm__("sta $05E4");
    __asm__("lda $05E6");
    __asm__("sta $05E5");
    __asm__("lda $05E7");
    __asm__("sta $05E6");
    __asm__("lda $05E8");
    __asm__("sta $05E7");
    __asm__("lda $05E9");
    __asm__("sta $05E8");
    __asm__("lda $05EA");
    __asm__("sta $05E9");
    __asm__("lda $05EB");
    __asm__("sta $05EA");
    __asm__("lda $05EC");
    __asm__("sta $05EB");
    __asm__("lda $05ED");
    __asm__("sta $05EC");
    __asm__("lda $05EE");
    __asm__("sta $05ED");
    __asm__("lda $05EF");
    __asm__("sta $05EE");
    __asm__("lda $05F0");
    __asm__("sta $05EF");
    __asm__("lda $05F1");
    __asm__("sta $05F0");
    __asm__("lda $05F2");
    __asm__("sta $05F1");
    __asm__("lda $05F3");
    __asm__("sta $05F2");
    __asm__("lda $05F4");
    __asm__("sta $05F3");
    __asm__("lda $05F5");
    __asm__("sta $05F4");
    __asm__("lda $05F6");
    __asm__("sta $05F5");
    __asm__("lda $05F7");
    __asm__("sta $05F6");
    __asm__("lda $05F8");
    __asm__("sta $05F7");
    __asm__("lda $05F9");
    __asm__("sta $05F8");
    __asm__("lda $05FA");
    __asm__("sta $05F9");
    __asm__("lda $05FB");
    __asm__("sta $05FA");
    __asm__("lda $05FC");
    __asm__("sta $05FB");
    __asm__("lda $05FD");
    __asm__("sta $05FC");
    __asm__("lda $05FE");
    __asm__("sta $05FD");
    __asm__("lda $05FF");
    __asm__("sta $05FE");
    __asm__("lda $0600");
    __asm__("sta $05FF");
    __asm__("lda $0601");
    __asm__("sta $0600");
    __asm__("lda $0602");
    __asm__("sta $0601");
    __asm__("lda $0603");
    __asm__("sta $0602");
    __asm__("lda $0604");
    __asm__("sta $0603");
    __asm__("lda $0605");
    __asm__("sta $0604");
    __asm__("lda $0606");
    __asm__("sta $0605");
    __asm__("lda $0607");
    __asm__("sta $0606");
    __asm__("lda $0609");
    __asm__("sta $0608");
    __asm__("lda $060A");
    __asm__("sta $0609");
    __asm__("lda $060B");
    __asm__("sta $060A");
    __asm__("lda $060C");
    __asm__("sta $060B");
    __asm__("lda $060D");
    __asm__("sta $060C");
    __asm__("lda $060E");
    __asm__("sta $060D");
    __asm__("lda $060F");
    __asm__("sta $060E");
    __asm__("lda $0610");
    __asm__("sta $060F");
    __asm__("lda $0611");
    __asm__("sta $0610");
    __asm__("lda $0612");
    __asm__("sta $0611");
    __asm__("lda $0613");
    __asm__("sta $0612");
    __asm__("lda $0614");
    __asm__("sta $0613");
    __asm__("lda $0615");
    __asm__("sta $0614");
    __asm__("lda $0616");
    __asm__("sta $0615");
    __asm__("lda $0617");
    __asm__("sta $0616");
    __asm__("lda $0618");
    __asm__("sta $0617");
    __asm__("lda $0619");
    __asm__("sta $0618");
    __asm__("lda $061A");
    __asm__("sta $0619");
    __asm__("lda $061B");
    __asm__("sta $061A");
    __asm__("lda $061C");
    __asm__("sta $061B");
    __asm__("lda $061D");
    __asm__("sta $061C");
    __asm__("lda $061E");
    __asm__("sta $061D");
    __asm__("lda $061F");
    __asm__("sta $061E");
    __asm__("lda $0620");
    __asm__("sta $061F");
    __asm__("lda $0621");
    __asm__("sta $0620");
    __asm__("lda $0622");
    __asm__("sta $0621");
    __asm__("lda $0623");
    __asm__("sta $0622");
    __asm__("lda $0624");
    __asm__("sta $0623");
    __asm__("lda $0625");
    __asm__("sta $0624");
    __asm__("lda $0626");
    __asm__("sta $0625");
    __asm__("lda $0627");
    __asm__("sta $0626");
    __asm__("lda $0628");
    __asm__("sta $0627");
    __asm__("lda $0629");
    __asm__("sta $0628");
    __asm__("lda $062A");
    __asm__("sta $0629");
    __asm__("lda $062B");
    __asm__("sta $062A");
    __asm__("lda $062C");
    __asm__("sta $062B");
    __asm__("lda $062D");
    __asm__("sta $062C");
    __asm__("lda $062E");
    __asm__("sta $062D");
    __asm__("lda $062F");
    __asm__("sta $062E");
    __asm__("lda $0631");
    __asm__("sta $0630");
    __asm__("lda $0632");
    __asm__("sta $0631");
    __asm__("lda $0633");
    __asm__("sta $0632");
    __asm__("lda $0634");
    __asm__("sta $0633");
    __asm__("lda $0635");
    __asm__("sta $0634");
    __asm__("lda $0636");
    __asm__("sta $0635");
    __asm__("lda $0637");
    __asm__("sta $0636");
    __asm__("lda $0638");
    __asm__("sta $0637");
    __asm__("lda $0639");
    __asm__("sta $0638");
    __asm__("lda $063A");
    __asm__("sta $0639");
    __asm__("lda $063B");
    __asm__("sta $063A");
    __asm__("lda $063C");
    __asm__("sta $063B");
    __asm__("lda $063D");
    __asm__("sta $063C");
    __asm__("lda $063E");
    __asm__("sta $063D");
    __asm__("lda $063F");
    __asm__("sta $063E");
    __asm__("lda $0640");
    __asm__("sta $063F");
    __asm__("lda $0641");
    __asm__("sta $0640");
    __asm__("lda $0642");
    __asm__("sta $0641");
    __asm__("lda $0643");
    __asm__("sta $0642");
    __asm__("lda $0644");
    __asm__("sta $0643");
    __asm__("lda $0645");
    __asm__("sta $0644");
    __asm__("lda $0646");
    __asm__("sta $0645");
    __asm__("lda $0647");
    __asm__("sta $0646");
    __asm__("lda $0648");
    __asm__("sta $0647");
    __asm__("lda $0649");
    __asm__("sta $0648");
    __asm__("lda $064A");
    __asm__("sta $0649");
    __asm__("lda $064B");
    __asm__("sta $064A");
    __asm__("lda $064C");
    __asm__("sta $064B");
    __asm__("lda $064D");
    __asm__("sta $064C");
    __asm__("lda $064E");
    __asm__("sta $064D");
    __asm__("lda $064F");
    __asm__("sta $064E");
    __asm__("lda $0650");
    __asm__("sta $064F");
    __asm__("lda $0651");
    __asm__("sta $0650");
    __asm__("lda $0652");
    __asm__("sta $0651");
    __asm__("lda $0653");
    __asm__("sta $0652");
    __asm__("lda $0654");
    __asm__("sta $0653");
    __asm__("lda $0655");
    __asm__("sta $0654");
    __asm__("lda $0656");
    __asm__("sta $0655");
    __asm__("lda $0657");
    __asm__("sta $0656");
    __asm__("lda $0659");
    __asm__("sta $0658");
    __asm__("lda $065A");
    __asm__("sta $0659");
    __asm__("lda $065B");
    __asm__("sta $065A");
    __asm__("lda $065C");
    __asm__("sta $065B");
    __asm__("lda $065D");
    __asm__("sta $065C");
    __asm__("lda $065E");
    __asm__("sta $065D");
    __asm__("lda $065F");
    __asm__("sta $065E");
    __asm__("lda $0660");
    __asm__("sta $065F");
    __asm__("lda $0661");
    __asm__("sta $0660");
    __asm__("lda $0662");
    __asm__("sta $0661");
    __asm__("lda $0663");
    __asm__("sta $0662");
    __asm__("lda $0664");
    __asm__("sta $0663");
    __asm__("lda $0665");
    __asm__("sta $0664");
    __asm__("lda $0666");
    __asm__("sta $0665");
    __asm__("lda $0667");
    __asm__("sta $0666");
    __asm__("lda $0668");
    __asm__("sta $0667");
    __asm__("lda $0669");
    __asm__("sta $0668");
    __asm__("lda $066A");
    __asm__("sta $0669");
    __asm__("lda $066B");
    __asm__("sta $066A");
    __asm__("lda $066C");
    __asm__("sta $066B");
    __asm__("lda $066D");
    __asm__("sta $066C");
    __asm__("lda $066E");
    __asm__("sta $066D");
    __asm__("lda $066F");
    __asm__("sta $066E");
    __asm__("lda $0670");
    __asm__("sta $066F");
    __asm__("lda $0671");
    __asm__("sta $0670");
    __asm__("lda $0672");
    __asm__("sta $0671");
    __asm__("lda $0673");
    __asm__("sta $0672");
    __asm__("lda $0674");
    __asm__("sta $0673");
    __asm__("lda $0675");
    __asm__("sta $0674");
    __asm__("lda $0676");
    __asm__("sta $0675");
    __asm__("lda $0677");
    __asm__("sta $0676");
    __asm__("lda $0678");
    __asm__("sta $0677");
    __asm__("lda $0679");
    __asm__("sta $0678");
    __asm__("lda $067A");
    __asm__("sta $0679");
    __asm__("lda $067B");
    __asm__("sta $067A");
    __asm__("lda $067C");
    __asm__("sta $067B");
    __asm__("lda $067D");
    __asm__("sta $067C");
    __asm__("lda $067E");
    __asm__("sta $067D");
    __asm__("lda $067F");
    __asm__("sta $067E");
    __asm__("lda $0681");
    __asm__("sta $0680");
    __asm__("lda $0682");
    __asm__("sta $0681");
    __asm__("lda $0683");
    __asm__("sta $0682");
    __asm__("lda $0684");
    __asm__("sta $0683");
    __asm__("lda $0685");
    __asm__("sta $0684");
    __asm__("lda $0686");
    __asm__("sta $0685");
    __asm__("lda $0687");
    __asm__("sta $0686");
    __asm__("lda $0688");
    __asm__("sta $0687");
    __asm__("lda $0689");
    __asm__("sta $0688");
    __asm__("lda $068A");
    __asm__("sta $0689");
    __asm__("lda $068B");
    __asm__("sta $068A");
    __asm__("lda $068C");
    __asm__("sta $068B");
    __asm__("lda $068D");
    __asm__("sta $068C");
    __asm__("lda $068E");
    __asm__("sta $068D");
    __asm__("lda $068F");
    __asm__("sta $068E");
    __asm__("lda $0690");
    __asm__("sta $068F");
    __asm__("lda $0691");
    __asm__("sta $0690");
    __asm__("lda $0692");
    __asm__("sta $0691");
    __asm__("lda $0693");
    __asm__("sta $0692");
    __asm__("lda $0694");
    __asm__("sta $0693");
    __asm__("lda $0695");
    __asm__("sta $0694");
    __asm__("lda $0696");
    __asm__("sta $0695");
    __asm__("lda $0697");
    __asm__("sta $0696");
    __asm__("lda $0698");
    __asm__("sta $0697");
    __asm__("lda $0699");
    __asm__("sta $0698");
    __asm__("lda $069A");
    __asm__("sta $0699");
    __asm__("lda $069B");
    __asm__("sta $069A");
    __asm__("lda $069C");
    __asm__("sta $069B");
    __asm__("lda $069D");
    __asm__("sta $069C");
    __asm__("lda $069E");
    __asm__("sta $069D");
    __asm__("lda $069F");
    __asm__("sta $069E");
    __asm__("lda $06A0");
    __asm__("sta $069F");
    __asm__("lda $06A1");
    __asm__("sta $06A0");
    __asm__("lda $06A2");
    __asm__("sta $06A1");
    __asm__("lda $06A3");
    __asm__("sta $06A2");
    __asm__("lda $06A4");
    __asm__("sta $06A3");
    __asm__("lda $06A5");
    __asm__("sta $06A4");
    __asm__("lda $06A6");
    __asm__("sta $06A5");
    __asm__("lda $06A7");
    __asm__("sta $06A6");
    __asm__("lda $06A9");
    __asm__("sta $06A8");
    __asm__("lda $06AA");
    __asm__("sta $06A9");
    __asm__("lda $06AB");
    __asm__("sta $06AA");
    __asm__("lda $06AC");
    __asm__("sta $06AB");
    __asm__("lda $06AD");
    __asm__("sta $06AC");
    __asm__("lda $06AE");
    __asm__("sta $06AD");
    __asm__("lda $06AF");
    __asm__("sta $06AE");
    __asm__("lda $06B0");
    __asm__("sta $06AF");
    __asm__("lda $06B1");
    __asm__("sta $06B0");
    __asm__("lda $06B2");
    __asm__("sta $06B1");
    __asm__("lda $06B3");
    __asm__("sta $06B2");
    __asm__("lda $06B4");
    __asm__("sta $06B3");
    __asm__("lda $06B5");
    __asm__("sta $06B4");
    __asm__("lda $06B6");
    __asm__("sta $06B5");
    __asm__("lda $06B7");
    __asm__("sta $06B6");
    __asm__("lda $06B8");
    __asm__("sta $06B7");
    __asm__("lda $06B9");
    __asm__("sta $06B8");
    __asm__("lda $06BA");
    __asm__("sta $06B9");
    __asm__("lda $06BB");
    __asm__("sta $06BA");
    __asm__("lda $06BC");
    __asm__("sta $06BB");
    __asm__("lda $06BD");
    __asm__("sta $06BC");
    __asm__("lda $06BE");
    __asm__("sta $06BD");
    __asm__("lda $06BF");
    __asm__("sta $06BE");
    __asm__("lda $06C0");
    __asm__("sta $06BF");
    __asm__("lda $06C1");
    __asm__("sta $06C0");
    __asm__("lda $06C2");
    __asm__("sta $06C1");
    __asm__("lda $06C3");
    __asm__("sta $06C2");
    __asm__("lda $06C4");
    __asm__("sta $06C3");
    __asm__("lda $06C5");
    __asm__("sta $06C4");
    __asm__("lda $06C6");
    __asm__("sta $06C5");
    __asm__("lda $06C7");
    __asm__("sta $06C6");
    __asm__("lda $06C8");
    __asm__("sta $06C7");
    __asm__("lda $06C9");
    __asm__("sta $06C8");
    __asm__("lda $06CA");
    __asm__("sta $06C9");
    __asm__("lda $06CB");
    __asm__("sta $06CA");
    __asm__("lda $06CC");
    __asm__("sta $06CB");
    __asm__("lda $06CD");
    __asm__("sta $06CC");
    __asm__("lda $06CE");
    __asm__("sta $06CD");
    __asm__("lda $06CF");
    __asm__("sta $06CE");
    __asm__("lda $06D1");
    __asm__("sta $06D0");
    __asm__("lda $06D2");
    __asm__("sta $06D1");
    __asm__("lda $06D3");
    __asm__("sta $06D2");
    __asm__("lda $06D4");
    __asm__("sta $06D3");
    __asm__("lda $06D5");
    __asm__("sta $06D4");
    __asm__("lda $06D6");
    __asm__("sta $06D5");
    __asm__("lda $06D7");
    __asm__("sta $06D6");
    __asm__("lda $06D8");
    __asm__("sta $06D7");
    __asm__("lda $06D9");
    __asm__("sta $06D8");
    __asm__("lda $06DA");
    __asm__("sta $06D9");
    __asm__("lda $06DB");
    __asm__("sta $06DA");
    __asm__("lda $06DC");
    __asm__("sta $06DB");
    __asm__("lda $06DD");
    __asm__("sta $06DC");
    __asm__("lda $06DE");
    __asm__("sta $06DD");
    __asm__("lda $06DF");
    __asm__("sta $06DE");
    __asm__("lda $06E0");
    __asm__("sta $06DF");
    __asm__("lda $06E1");
    __asm__("sta $06E0");
    __asm__("lda $06E2");
    __asm__("sta $06E1");
    __asm__("lda $06E3");
    __asm__("sta $06E2");
    __asm__("lda $06E4");
    __asm__("sta $06E3");
    __asm__("lda $06E5");
    __asm__("sta $06E4");
    __asm__("lda $06E6");
    __asm__("sta $06E5");
    __asm__("lda $06E7");
    __asm__("sta $06E6");
    __asm__("lda $06E8");
    __asm__("sta $06E7");
    __asm__("lda $06E9");
    __asm__("sta $06E8");
    __asm__("lda $06EA");
    __asm__("sta $06E9");
    __asm__("lda $06EB");
    __asm__("sta $06EA");
    __asm__("lda $06EC");
    __asm__("sta $06EB");
    __asm__("lda $06ED");
    __asm__("sta $06EC");
    __asm__("lda $06EE");
    __asm__("sta $06ED");
    __asm__("lda $06EF");
    __asm__("sta $06EE");
    __asm__("lda $06F0");
    __asm__("sta $06EF");
    __asm__("lda $06F1");
    __asm__("sta $06F0");
    __asm__("lda $06F2");
    __asm__("sta $06F1");
    __asm__("lda $06F3");
    __asm__("sta $06F2");
    __asm__("lda $06F4");
    __asm__("sta $06F3");
    __asm__("lda $06F5");
    __asm__("sta $06F4");
    __asm__("lda $06F6");
    __asm__("sta $06F5");
    __asm__("lda $06F7");
    __asm__("sta $06F6");
    __asm__("lda $06F9");
    __asm__("sta $06F8");
    __asm__("lda $06FA");
    __asm__("sta $06F9");
    __asm__("lda $06FB");
    __asm__("sta $06FA");
    __asm__("lda $06FC");
    __asm__("sta $06FB");
    __asm__("lda $06FD");
    __asm__("sta $06FC");
    __asm__("lda $06FE");
    __asm__("sta $06FD");
    __asm__("lda $06FF");
    __asm__("sta $06FE");
    __asm__("lda $0700");
    __asm__("sta $06FF");
    __asm__("lda $0701");
    __asm__("sta $0700");
    __asm__("lda $0702");
    __asm__("sta $0701");
    __asm__("lda $0703");
    __asm__("sta $0702");
    __asm__("lda $0704");
    __asm__("sta $0703");
    __asm__("lda $0705");
    __asm__("sta $0704");
    __asm__("lda $0706");
    __asm__("sta $0705");
    __asm__("lda $0707");
    __asm__("sta $0706");
    __asm__("lda $0708");
    __asm__("sta $0707");
    __asm__("lda $0709");
    __asm__("sta $0708");
    __asm__("lda $070A");
    __asm__("sta $0709");
    __asm__("lda $070B");
    __asm__("sta $070A");
    __asm__("lda $070C");
    __asm__("sta $070B");
    __asm__("lda $070D");
    __asm__("sta $070C");
    __asm__("lda $070E");
    __asm__("sta $070D");
    __asm__("lda $070F");
    __asm__("sta $070E");
    __asm__("lda $0710");
    __asm__("sta $070F");
    __asm__("lda $0711");
    __asm__("sta $0710");
    __asm__("lda $0712");
    __asm__("sta $0711");
    __asm__("lda $0713");
    __asm__("sta $0712");
    __asm__("lda $0714");
    __asm__("sta $0713");
    __asm__("lda $0715");
    __asm__("sta $0714");
    __asm__("lda $0716");
    __asm__("sta $0715");
    __asm__("lda $0717");
    __asm__("sta $0716");
    __asm__("lda $0718");
    __asm__("sta $0717");
    __asm__("lda $0719");
    __asm__("sta $0718");
    __asm__("lda $071A");
    __asm__("sta $0719");
    __asm__("lda $071B");
    __asm__("sta $071A");
    __asm__("lda $071C");
    __asm__("sta $071B");
    __asm__("lda $071D");
    __asm__("sta $071C");
    __asm__("lda $071E");
    __asm__("sta $071D");
    __asm__("lda $071F");
    __asm__("sta $071E");
    __asm__("lda $0721");
    __asm__("sta $0720");
    __asm__("lda $0722");
    __asm__("sta $0721");
    __asm__("lda $0723");
    __asm__("sta $0722");
    __asm__("lda $0724");
    __asm__("sta $0723");
    __asm__("lda $0725");
    __asm__("sta $0724");
    __asm__("lda $0726");
    __asm__("sta $0725");
    __asm__("lda $0727");
    __asm__("sta $0726");
    __asm__("lda $0728");
    __asm__("sta $0727");
    __asm__("lda $0729");
    __asm__("sta $0728");
    __asm__("lda $072A");
    __asm__("sta $0729");
    __asm__("lda $072B");
    __asm__("sta $072A");
    __asm__("lda $072C");
    __asm__("sta $072B");
    __asm__("lda $072D");
    __asm__("sta $072C");
    __asm__("lda $072E");
    __asm__("sta $072D");
    __asm__("lda $072F");
    __asm__("sta $072E");
    __asm__("lda $0730");
    __asm__("sta $072F");
    __asm__("lda $0731");
    __asm__("sta $0730");
    __asm__("lda $0732");
    __asm__("sta $0731");
    __asm__("lda $0733");
    __asm__("sta $0732");
    __asm__("lda $0734");
    __asm__("sta $0733");
    __asm__("lda $0735");
    __asm__("sta $0734");
    __asm__("lda $0736");
    __asm__("sta $0735");
    __asm__("lda $0737");
    __asm__("sta $0736");
    __asm__("lda $0738");
    __asm__("sta $0737");
    __asm__("lda $0739");
    __asm__("sta $0738");
    __asm__("lda $073A");
    __asm__("sta $0739");
    __asm__("lda $073B");
    __asm__("sta $073A");
    __asm__("lda $073C");
    __asm__("sta $073B");
    __asm__("lda $073D");
    __asm__("sta $073C");
    __asm__("lda $073E");
    __asm__("sta $073D");
    __asm__("lda $073F");
    __asm__("sta $073E");
    __asm__("lda $0740");
    __asm__("sta $073F");
    __asm__("lda $0741");
    __asm__("sta $0740");
    __asm__("lda $0742");
    __asm__("sta $0741");
    __asm__("lda $0743");
    __asm__("sta $0742");
    __asm__("lda $0744");
    __asm__("sta $0743");
    __asm__("lda $0745");
    __asm__("sta $0744");
    __asm__("lda $0746");
    __asm__("sta $0745");
    __asm__("lda $0747");
    __asm__("sta $0746");
    __asm__("lda $0749");
    __asm__("sta $0748");
    __asm__("lda $074A");
    __asm__("sta $0749");
    __asm__("lda $074B");
    __asm__("sta $074A");
    __asm__("lda $074C");
    __asm__("sta $074B");
    __asm__("lda $074D");
    __asm__("sta $074C");
    __asm__("lda $074E");
    __asm__("sta $074D");
    __asm__("lda $074F");
    __asm__("sta $074E");
    __asm__("lda $0750");
    __asm__("sta $074F");
    __asm__("lda $0751");
    __asm__("sta $0750");
    __asm__("lda $0752");
    __asm__("sta $0751");
    __asm__("lda $0753");
    __asm__("sta $0752");
    __asm__("lda $0754");
    __asm__("sta $0753");
    __asm__("lda $0755");
    __asm__("sta $0754");
    __asm__("lda $0756");
    __asm__("sta $0755");
    __asm__("lda $0757");
    __asm__("sta $0756");
    __asm__("lda $0758");
    __asm__("sta $0757");
    __asm__("lda $0759");
    __asm__("sta $0758");
    __asm__("lda $075A");
    __asm__("sta $0759");
    __asm__("lda $075B");
    __asm__("sta $075A");
    __asm__("lda $075C");
    __asm__("sta $075B");
    __asm__("lda $075D");
    __asm__("sta $075C");
    __asm__("lda $075E");
    __asm__("sta $075D");
    __asm__("lda $075F");
    __asm__("sta $075E");
    __asm__("lda $0760");
    __asm__("sta $075F");
    __asm__("lda $0761");
    __asm__("sta $0760");
    __asm__("lda $0762");
    __asm__("sta $0761");
    __asm__("lda $0763");
    __asm__("sta $0762");
    __asm__("lda $0764");
    __asm__("sta $0763");
    __asm__("lda $0765");
    __asm__("sta $0764");
    __asm__("lda $0766");
    __asm__("sta $0765");
    __asm__("lda $0767");
    __asm__("sta $0766");
    __asm__("lda $0768");
    __asm__("sta $0767");
    __asm__("lda $0769");
    __asm__("sta $0768");
    __asm__("lda $076A");
    __asm__("sta $0769");
    __asm__("lda $076B");
    __asm__("sta $076A");
    __asm__("lda $076C");
    __asm__("sta $076B");
    __asm__("lda $076D");
    __asm__("sta $076C");
    __asm__("lda $076E");
    __asm__("sta $076D");
    __asm__("lda $076F");
    __asm__("sta $076E");
    __asm__("lda $0771");
    __asm__("sta $0770");
    __asm__("lda $0772");
    __asm__("sta $0771");
    __asm__("lda $0773");
    __asm__("sta $0772");
    __asm__("lda $0774");
    __asm__("sta $0773");
    __asm__("lda $0775");
    __asm__("sta $0774");
    __asm__("lda $0776");
    __asm__("sta $0775");
    __asm__("lda $0777");
    __asm__("sta $0776");
    __asm__("lda $0778");
    __asm__("sta $0777");
    __asm__("lda $0779");
    __asm__("sta $0778");
    __asm__("lda $077A");
    __asm__("sta $0779");
    __asm__("lda $077B");
    __asm__("sta $077A");
    __asm__("lda $077C");
    __asm__("sta $077B");
    __asm__("lda $077D");
    __asm__("sta $077C");
    __asm__("lda $077E");
    __asm__("sta $077D");
    __asm__("lda $077F");
    __asm__("sta $077E");
    __asm__("lda $0780");
    __asm__("sta $077F");
    __asm__("lda $0781");
    __asm__("sta $0780");
    __asm__("lda $0782");
    __asm__("sta $0781");
    __asm__("lda $0783");
    __asm__("sta $0782");
    __asm__("lda $0784");
    __asm__("sta $0783");
    __asm__("lda $0785");
    __asm__("sta $0784");
    __asm__("lda $0786");
    __asm__("sta $0785");
    __asm__("lda $0787");
    __asm__("sta $0786");
    __asm__("lda $0788");
    __asm__("sta $0787");
    __asm__("lda $0789");
    __asm__("sta $0788");
    __asm__("lda $078A");
    __asm__("sta $0789");
    __asm__("lda $078B");
    __asm__("sta $078A");
    __asm__("lda $078C");
    __asm__("sta $078B");
    __asm__("lda $078D");
    __asm__("sta $078C");
    __asm__("lda $078E");
    __asm__("sta $078D");
    __asm__("lda $078F");
    __asm__("sta $078E");
    __asm__("lda $0790");
    __asm__("sta $078F");
    __asm__("lda $0791");
    __asm__("sta $0790");
    __asm__("lda $0792");
    __asm__("sta $0791");
    __asm__("lda $0793");
    __asm__("sta $0792");
    __asm__("lda $0794");
    __asm__("sta $0793");
    __asm__("lda $0795");
    __asm__("sta $0794");
    __asm__("lda $0796");
    __asm__("sta $0795");
    __asm__("lda $0797");
    __asm__("sta $0796");
    __asm__("lda $0799");
    __asm__("sta $0798");
    __asm__("lda $079A");
    __asm__("sta $0799");
    __asm__("lda $079B");
    __asm__("sta $079A");
    __asm__("lda $079C");
    __asm__("sta $079B");
    __asm__("lda $079D");
    __asm__("sta $079C");
    __asm__("lda $079E");
    __asm__("sta $079D");
    __asm__("lda $079F");
    __asm__("sta $079E");
    __asm__("lda $07A0");
    __asm__("sta $079F");
    __asm__("lda $07A1");
    __asm__("sta $07A0");
    __asm__("lda $07A2");
    __asm__("sta $07A1");
    __asm__("lda $07A3");
    __asm__("sta $07A2");
    __asm__("lda $07A4");
    __asm__("sta $07A3");
    __asm__("lda $07A5");
    __asm__("sta $07A4");
    __asm__("lda $07A6");
    __asm__("sta $07A5");
    __asm__("lda $07A7");
    __asm__("sta $07A6");
    __asm__("lda $07A8");
    __asm__("sta $07A7");
    __asm__("lda $07A9");
    __asm__("sta $07A8");
    __asm__("lda $07AA");
    __asm__("sta $07A9");
    __asm__("lda $07AB");
    __asm__("sta $07AA");
    __asm__("lda $07AC");
    __asm__("sta $07AB");
    __asm__("lda $07AD");
    __asm__("sta $07AC");
    __asm__("lda $07AE");
    __asm__("sta $07AD");
    __asm__("lda $07AF");
    __asm__("sta $07AE");
    __asm__("lda $07B0");
    __asm__("sta $07AF");
    __asm__("lda $07B1");
    __asm__("sta $07B0");
    __asm__("lda $07B2");
    __asm__("sta $07B1");
    __asm__("lda $07B3");
    __asm__("sta $07B2");
    __asm__("lda $07B4");
    __asm__("sta $07B3");
    __asm__("lda $07B5");
    __asm__("sta $07B4");
    __asm__("lda $07B6");
    __asm__("sta $07B5");
    __asm__("lda $07B7");
    __asm__("sta $07B6");
    __asm__("lda $07B8");
    __asm__("sta $07B7");
    __asm__("lda $07B9");
    __asm__("sta $07B8");
    __asm__("lda $07BA");
    __asm__("sta $07B9");
    __asm__("lda $07BB");
    __asm__("sta $07BA");
    __asm__("lda $07BC");
    __asm__("sta $07BB");
    __asm__("lda $07BD");
    __asm__("sta $07BC");
    __asm__("lda $07BE");
    __asm__("sta $07BD");
    __asm__("lda $07BF");
    __asm__("sta $07BE");
    __asm__("rts");
}

static void waitraster() {
    __asm__("lda #38");
    __asm__("cmp $D012");
    __asm__("bne %v", waitraster);
    __asm__("rts");
}

static void falldown() {
    memcpy((void *)sprptr2, &balloon[2], 63);

    poke(54276ul, 129); // Voice #1 control register.
    poke(54277ul, 15); // Voice #1 Attack and Decay length

    poke(54296ul, 3); // Volume and filter modes. Bits #0-#3: Volume

    // poke(0xD017, 12); // double height
    // poke(0xD01D, 12); // double width

    while(height < 19600ul) {
        ycoord = (int)(height / 76);
        poke(0xD005, ycoord); // sprite 2 y - coord
        poke(0xD007, ycoord); // sprite 3 y - coord
        height = height + 10;
    }
    
    // poke(0xD017, 0); // double height
    // poke(0xD01D, 0); // double width

    // turn off balloon falldown sound
    poke(54276ul, 0); // Voice #1 control register.
    poke(54277ul, 0); // Voice #1 Attack and Decay length

    poke(54296ul, 31); // Volume and filter modes. Bits #0-#3: Volume
}

static void clearscreen() {
    // turn off screen
    // poke(0xD011, 11);

    // set vic ram to all blank spaces
    for(j = 1064; j <= 1984; j++) {
        poke(j, 32);
    }

    // draw top, bottom limit
    for(i = 0; i < 40; i++) {
        poke(1024 + i, wall);
        // poke(55296ul + i, COLOR_WHITE);
        poke(1984 + i, wall);
        // poke(56256ul + i, COLOR_WHITE);
    }

    // turn on screen
    // poke(0xD011, 27);
}

void waitforkeypress() {
    // wait for button to be released
    while((peek(0xDC00) & 0x10) != 0x10) {
        __asm__ ("nop");
    }

    // wait for button to be pressed
    while((peek(0xDC00) & 0x10) != 0x00) {
        __asm__ ("nop");
    }
}

void setupSound() {
    poke(54284ul, 8); // attack duration
    poke(54285ul, 198); // sustain level
    poke(54280ul, 255); // frequency voice 2 high byte
    poke(54296ul, 31); // filter mode and main volume control
    poke(54294ul, 40); // filter cutoff frequency high byte

    poke(54295ul, 2); // filter resonance and routing

    // setup balloon falldown sound
    pokew(54272ul, 1000ul); // Voice #1 frequency lo & hi
    poke(54278ul, 0); // Voice #1 Sustain volume and Release length
}

void balloonSoundOn() {
    poke(54283ul, 129); // control register voice 2
}

void balloonSoundOff() {
    poke(54283ul, 136); // control register voice 2
    //poke(54295ul, 0); // filter resonance and routing
}

void gameover() {
    balloonSoundOff();
    falldown();
    if (score > 0) {
        cputsxy (15, 13, "game over!");
        cputsxy (15, 14, "  score   ");
        sprintf(buffer, "%3ld000", score);
        cputsxy (18, 15, buffer);
        cputsxy (15, 16, " hi-score ");
        sprintf(buffer, "%3ld000", highscore);
        cputsxy (18, 17, buffer);
    }

    if (score > highscore) {
        highscore = score;
        cputsxy (15, 17, "new record");
    }

    waitforkeypress();

    clearscreen();
}

static void resetgame() {
    height = 10000u;
    inertia = 1;
    level = 1;
    counter = 1;
    score = 0;
    scrollpos = 7;

    // sprite data copy
    memcpy((void *)sprptr2, &balloon[spr], 63);

    clearscreen();

    sprintf(buffer, "%3ld000 ", highscore);

    cputsxy (15, 2, " hi-score ");
    cputsxy (18, 3, buffer);

    cputsxy (8, 6, "     balloon flight     ");

    cputsxy (8, 15, "     hi balloonier,     ");

    cputsxy (8, 17, " navigate your balloon, ");
    cputsxy (8, 18, "  avoid the stars, and   ");
    cputsxy (8, 19, " don/t touch the walls. ");
    cputsxy (8, 21, " use joystick in port 2 ");
    cputsxy (8, 23, "stefan kostic, jan 2018.");

    poke(0xD004, 172); // sprite 2 x - coord
    poke(0xD006, 172); // sprite 3 x - coord

    ycoord = (int)(height / 76);
    poke(0xD005, ycoord); // sprite 2 y - coord
    poke(0xD007, ycoord); // sprite 3 y - coord

    // acknowledge collision
    poke(0xD019, peek(0xD01F) | 2);

    waitforkeypress();

    clearscreen();

    cputsxy (15, 13, "get ready!");
    cputsxy (15, 14, " player 1 ");
    cputsxy (15, 16, " hi-score ");
    cputsxy (18, 17, buffer);

    waitforkeypress();

    clearscreen();
}

static void game() {
    resetgame();

    while(1)
    {
        counter++;

        // joy 2 button press
        if ((peek(0xDC00) & 0x10) == 0x00) {
            if (inertia > -maxinertia) {
                inertia = inertia - 2;
                spr = 0;
            }
        } else {
            if (inertia < maxinertia) {
                inertia = inertia + 2;
                spr = 1;
            }
        }

        height = height + inertia;
        ycoord = (int)(height / 76);
        poke(0xD005, ycoord); // sprite 2 y - coord
        poke(0xD007, ycoord); // sprite 3 y - coord

        // every 2000th cycle
        if (counter % 2000 == 0) {
            level++;
            counter == 0;
        }

        // every 50th cycle
        if (counter % 50 == 0) {
            // add star
            poke(1062 + (40 * (level + (rand() % (25 - 2 * level)))), star);
            score++;
        }

        // every 13th cycle
        if (counter % 13 == 0) {
            starframe++;
            if (starframe == 3) { starframe = 0; }
            memcpy((void *)(charset + (star * 8)), &starchar[starframe], 8);
        }

        // every 17th cycle
        if (counter % 17 == 0) {
            brickframe++;
            if (brickframe == 4) { brickframe = 0; }
            memcpy((void *)(charset + (brick * 8)), &brickchar[brickframe], 8);
        }

        // every 3rd cycle
        if (counter % 3 == 0) {
            scrollpos--;
            if (scrollpos == 0) {
                scrollleftasm();
                scrollpos = 7;
                goto next;
            }

            if (scrollpos == 1) {
                for(i = 1; i < level; i++) {
                    // additional level walls
                    if (i % 2 == 1) {
                        poke(1063 + (i * 40), brick);
                        poke(2023 - (i * 40), brick);
                    } else {
                        poke(1063 + (i * 40), star);
                        poke(2023 - (i * 40), star);
                    }
                }
            }

            if (scrollpos == 2) {
                if (spr != oldspr) {
                    memcpy((void *)sprptr2, &balloon[spr], 63);
                    oldspr = spr;
                }
            }

            if (scrollpos == 4) {
                if (spr == 0) {
                    balloonSoundOn();
                }

                if (spr == 1) {
                    balloonSoundOff();
                }
            }
next:
            // set scroll pos
            // poke(53270ul, (peek(53270ul) & 248) + scrollpos);
            __asm__("lda $d016");
            __asm__("and #$F8");
            __asm__("clc");
            __asm__("adc %v", scrollpos);
            __asm__("sta $d016");

            waitraster();

            // colision?
            if ((peek(0xD01F) & 4) == 4) {
                return;
            }
        }  
    }
}

void init() {
    // disable basic rom
    poke(1, 54);

    // disable screen
    poke(0xD011, 11);

    // set VIC to use Bank #3 ($C000-$FFFF, 49152-65535)
    // poke(0xDD00, peek(0xDD00) | 3);

    // SID random number generator
    __asm__("lda #$FF"); // maximum frequency value
    __asm__("sta $D40E"); // voice 3 frequency low byte
    __asm__("sta $D40F"); // voice 3 frequency high byte
    __asm__("lda #$80"); // noise waveform, gate bit off
    __asm__("sta $D412"); // voice 3 control register

    // randomize seed
    srand(peek(0xD41B));

    // turn off keyboard buffer
    poke(0x289, 0x00);

    // enable smooth scrolling
    poke(53270ul, peek(53270ul) & 247);

    // sprite enable (sprite 2, 3, 4, 5)
    // poke(0xD015, 60);
    // sprite enable (sprite 2, 3)
    poke(0xD015, 12);

    // sprite pointer
    poke(0x7FA, 13);
    poke(0x7FB, 14);
    poke(0x7FC, 12);
    poke(0x7FD, 12);
    // sprite #2 color
    poke(0xD029, COLOR_WHITE);
    // sprite #3 color
    poke(0xD02A, COLOR_RED);
    // sprite #4 color
    poke(0xD02B, COLOR_WHITE);
    // sprite #5 color
    poke(0xD02C, COLOR_WHITE);

    // score
    poke(0xD008, 35); // sprite 4 x - coord
    poke(0xD00A, 59); // sprite 5 x - coord
    poke(0xD009, 64); // sprite 4 y - coord
    poke(0xD00B, 64); // sprite 5 y - coord

    // copy 3rd sprite data
    memcpy((void *)sprptr3, &balloonColor[spr], 63);

    // sprites additional color 1
    poke(53285ul, COLOR_BLUE);
    // sprites additional color 2
    poke(53286ul, COLOR_GRAY1);
    // set multicolor sprite 3
    poke(53276ul, 8);

    // set char mem location
    poke(53272ul, 31);

    // separators
    memcpy((void *)(charset + 32 * 8), &space, 16ul);
    memcpy((void *)(charset + 44 * 8), &separators, 32ul);

    // numbers
    memcpy((void *)(charset + 48 * 8), &numbers, 80ul);

    // letters
    memcpy((void *)(charset + 65 * 8), &letters, 208ul);

    // gfx
    memcpy((void *)(charset + (wall * 8)), &wallchar, 8);
    memcpy((void *)(charset + (star * 8)), &starchar[1], 8);
    memcpy((void *)(charset + (brick * 8)), &brickchar[0], 8);

    // set screen colors
    poke(53280ul, COLOR_BLACK);
    poke(53281ul, COLOR_BLACK);
    poke(646, COLOR_GRAY1);

    // setup color ram
    for(i = 0; i <= 24; i++) {
        for(j = 0; j <= 39; j++) {
            if (j <= 4) {
                poke(55296ul + i * 40 + j, colors[3]);
            }
            if (j > 4 && j <= 8) {
                poke(55296ul + i * 40 + j, colors[2]);
            }
            if (j > 8 && j <= 12) {
                poke(55296ul + i * 40 + j, colors[1]);
            }
            if (j > 12 && j <= 20) {
                poke(55296ul + i * 40 + j, colors[0]);
            }
            if (j > 20 && j <= 24) {
                poke(55296ul + i * 40 + j, colors[0]);
            }
            if (j > 24 && j <= 28) {
                poke(55296ul + i * 40 + j, colors[1]);
            }
            if (j > 28 && j <= 32) {
                poke(55296ul + i * 40 + j, colors[2]);
            }
            if (j > 32 && j <= 38) {
                poke(55296ul + i * 40 + j, colors[3]);
            }
        }
    }

    // enable screen
    poke(0xD011, 27);

    // sound
    setupSound();
}

void main(void) {
    init();

    while(1) {
        game();
        gameover();
    }
}