#include <avr/pgmspace.h>
#include "lib/pt_RT_table.h"

/* header: 
#include <inttypes.h>

typedef uint16_t fix_2_14;
typedef uint16_t fix_10_6;

fix_2_14 pt_R(fix_10_6 T);
fix_10_6 pt_T(fix_2_14 R);

*/

#define TEMP_STEP 10.000000

static const fix_2_14 PROGMEM temp_to_R[] = {

        0x4000, /* 0.000000 */
        0x427f, /* 10.000000 */
        0x44fc, /* 20.000000 */
        0x4778, /* 30.000000 */
        0x49f1, /* 40.000000 */
        0x4c69, /* 50.000000 */
        0x4edf, /* 60.000000 */
        0x5153, /* 70.000000 */
        0x53c5, /* 80.000000 */
        0x5635, /* 90.000000 */
        0x58a3, /* 100.000000 */
        0x5b10, /* 110.000000 */
        0x5d7a, /* 120.000000 */
        0x5fe3, /* 130.000000 */
        0x6249, /* 140.000000 */
        0x64ae, /* 150.000000 */
        0x6711, /* 160.000000 */
        0x6972, /* 170.000000 */
        0x6bd1, /* 180.000000 */
        0x6e2e, /* 190.000000 */
        0x7089, /* 200.000000 */
        0x72e2, /* 210.000000 */
        0x753a, /* 220.000000 */
        0x778f, /* 230.000000 */
        0x79e3, /* 240.000000 */
        0x7c35, /* 250.000000 */
        0x7e84, /* 260.000000 */
        0x80d2, /* 270.000000 */
        0x831e, /* 280.000000 */
        0x8568, /* 290.000000 */
        0x87b1, /* 300.000000 */
        0x89f7, /* 310.000000 */
        0x8c3b, /* 320.000000 */
        0x8e7e, /* 330.000000 */
        0x90be, /* 340.000000 */
        0x92fd, /* 350.000000 */
        0x953a, /* 360.000000 */
        0x9775, /* 370.000000 */
        0x99ae, /* 380.000000 */
        0x9be5, /* 390.000000 */
        0x9e1a, /* 400.000000 */
        0xa04d, /* 410.000000 */
        0xa27f, /* 420.000000 */
        0xa4ae, /* 430.000000 */
        0xa6dc, /* 440.000000 */
        0xa907, /* 450.000000 */
        0xab31, /* 460.000000 */
        0xad59, /* 470.000000 */
        0xaf7f, /* 480.000000 */
        0xb1a3, /* 490.000000 */
        0xb3c5, /* 500.000000 */
        0xb5e6, /* 510.000000 */
        0xb804, /* 520.000000 */
        0xba20, /* 530.000000 */
        0xbc3b, /* 540.000000 */
        0xbe54, /* 550.000000 */
        0xc06a, /* 560.000000 */
        0xc27f, /* 570.000000 */
        0xc492, /* 580.000000 */
        0xc6a3, /* 590.000000 */
        0xc8b3, /* 600.000000 */
        0xcac0, /* 610.000000 */
        0xcccb, /* 620.000000 */
        0xced5, /* 630.000000 */
        0xd0dc, /* 640.000000 */
        0xd2e2, /* 650.000000 */
        0xd4e6, /* 660.000000 */
        0xd6e7, /* 670.000000 */
        0xd8e7, /* 680.000000 */
        0xdae5, /* 690.000000 */
        0xdce2, /* 700.000000 */
        0xdedc, /* 710.000000 */
        0xe0d4, /* 720.000000 */
        0xe2cb, /* 730.000000 */
        0xe4bf, /* 740.000000 */
        0xe6b2, /* 750.000000 */
        0xe8a3, /* 760.000000 */
        0xea91, /* 770.000000 */
        0xec7e, /* 780.000000 */
        0xee69, /* 790.000000 */
        0xf053, /* 800.000000 */
};

fix_2_14 pt_R(fix_10_6 T)
{
    uint16_t idx = 0;
    fix_2_14 R;

    idx = T / (uint16_t)((1<<6) * TEMP_STEP);
    R = pgm_read_word(&temp_to_R[idx]);

    /* TODO: linearni interpolace */
    return R;
}

fix_10_6 pt_T(fix_2_14 R)
{
    fix_10_6 T;
    fix_2_14 R_1;
    uint16_t idx = sizeof(temp_to_R) / 2;
    uint16_t step = sizeof(temp_to_R) / 4;
    
    while (step) {
        R_1 = pgm_read_word(idx);
        if (R_1 > R)
            idx -= step;
        else
            idx += step;
        step /= 2;
    };
    /* TODO: tohle je jen prvni krok vyhledavani!!! nutno dohledat podle
     * posledni polozky*/
    /* TODO: linearni interpolace */
    T = idx << 6;

    return T;
}

