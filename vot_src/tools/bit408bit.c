#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

typedef struct
{
    uint32_t addr;
    uint32_t value;
} detector_data_t;

#define DETECTOR_BITS   408
#define DETECTOR_BYTES  (DETECTOR_BITS / 8)

static uint8_t g_detector_data[DETECTOR_BYTES];


static const detector_data_t g_input_detector_data[] =
{
    {0x0000001f, 0x0001390A},
    {0x0000001e, 0x3734153B},
    {0x0000001d, 0x343AFB57},
    {0x0000001c, 0xFDFF1D0C},
    {0x0000001b, 0x0C035F82},
    {0x0000001a, 0xA1700080},
    {0x00000019, 0x826CD90C},
    {0x00000018, 0x8B8416C5},
    {0x00000017, 0xB36DB61B},
    {0x00000016, 0x6C410080},
    {0x00000015, 0x08045133},
    {0x00000014, 0x3EC3A6BD},
    {0x00000013, 0x1C400000},
};

int get_detector_bits(uint16_t high_bit,
                      uint16_t low_bit,
                      uint32_t *value)
{
    uint16_t bit_count;
    uint16_t i;
    uint32_t result = 0;

    if (value == NULL)
        return -1;

    if (high_bit > 407 || low_bit > 407)
        return -1;

    if (high_bit < low_bit)
        return -1;

    bit_count = high_bit - low_bit + 1;

    /*
     * 返回uint32_t，因此最多32bit
     */
    if (bit_count > 32)
        return -2;

    /*
     * 逐bit读取
     *
     * value bit0
     *     <- low_bit
     *
     * value bit1
     *     <- low_bit + 1
     *
     * ...
     *
     * value bitN
     *     <- high_bit
     */
    for (i = 0; i < bit_count; i++)
    {
        uint16_t bit;
        uint16_t word_index;
        uint16_t bit_index;
        uint32_t bit_value;

        bit = low_bit + i;

        /*
         * 407~376 -> word0
         * 375~344 -> word1
         * ...
         */
        word_index = (407 - bit) / 32;

        /*
         * 407 -> bit31
         * 406 -> bit30
         * ...
         * 376 -> bit0
         */
        bit_index = (407 - bit) % 32;

        bit_value =
            (g_input_detector_data[word_index].value >> bit_index) & 0x01;

        result |= bit_value << i;
    }

    *value = result;

    return 0;
}

/*
 * 初始化
 */
void detector_init(void)
{
    memset(g_detector_data, 0, sizeof(g_detector_data));
}


/*
 * 设置单独一个 bit
 *
 * bit:
 *     0 ~ 407
 *
 * value:
 *     0 / 1
 */
int detector_set_bit(uint16_t bit, uint8_t value)
{
    uint16_t byte_index;
    uint8_t bit_index;

    if (bit >= DETECTOR_BITS)
        return -1;

    /*
     * bit407 -> byte[0] bit7
     * bit406 -> byte[0] bit6
     * ...
     * bit400 -> byte[0] bit0
     */
    byte_index = (DETECTOR_BITS - 1 - bit) / 8;
    bit_index  = bit % 8;

    if (value)
    {
        g_detector_data[byte_index] |=
            (uint8_t)(1U << bit_index);
    }
    else
    {
        g_detector_data[byte_index] &=
            (uint8_t)~(1U << bit_index);
    }

    return 0;
}


/*
 * 获取单独一个 bit
 */
int detector_get_bit(uint16_t bit)
{
    uint16_t byte_index;
    uint8_t bit_index;

    if (bit >= DETECTOR_BITS)
        return -1;

    byte_index = (DETECTOR_BITS - 1 - bit) / 8;
    bit_index  = bit % 8;

    return (g_detector_data[byte_index] >> bit_index) & 0x01;
}


/*
 * ============================================================
 * 设置指定 bit 范围的数据
 *
 * high_bit : 高位，例如 407
 * low_bit  : 低位，例如 399
 * value    : 要写入的数据
 *
 * 例如：
 *
 * detector_set_bits(407, 399, 0x123);
 *
 * 407 ~ 399 共 9bit
 *
 * value:
 *
 * 0x123 = 二进制 100100011
 *
 * 对应：
 *
 * bit407 = 1
 * bit406 = 0
 * bit405 = 0
 * bit404 = 1
 * bit403 = 0
 * bit402 = 0
 * bit401 = 0
 * bit400 = 1
 * bit399 = 1
 *
 * ============================================================
 */
int detector_set_bits(uint16_t high_bit,
                      uint16_t low_bit,
                      uint32_t value)
{
    uint16_t bit_count;
    uint16_t i;

    if (high_bit >= DETECTOR_BITS ||
        low_bit >= DETECTOR_BITS)
    {
        return -1;
    }

    if (high_bit < low_bit)
    {
        return -1;
    }

    /*
     * 计算字段长度
     */
    bit_count = high_bit - low_bit + 1;

    /*
     * uint32_t最多支持32bit
     */
    if (bit_count > 32)
    {
        return -1;
    }

    /*
     * 检查 value 是否超出字段范围
     *
     * 例如：
     *
     * 407~399 是9bit
     *
     * 最大值：
     * 0x1FF
     *
     * 如果传入：
     * 0x200
     *
     * 就超出了9bit范围
     */
    if (bit_count < 32)
    {
        uint32_t max_value;

        max_value = (1ULL << bit_count) - 1;

        if (value > max_value)
        {
            return -1;
        }
    }

    /*
     * value 的 bit0
     * 对应 low_bit
     *
     * value 的 bit1
     * 对应 low_bit + 1
     *
     * ...
     *
     * value 的最高bit
     * 对应 high_bit
     */
    for (i = 0; i < bit_count; i++)
    {
        uint8_t bit_value;

        bit_value = (uint8_t)((value >> i) & 0x01);

        detector_set_bit(low_bit + i, bit_value);
    }

    return 0;
}


/*
 * ============================================================
 * 获取指定 bit 范围的数据
 *
 * 例如：
 *
 * detector_get_bits(407, 399)
 *
 * 返回 bit407~399 对应的整数
 * ============================================================
 */
int detector_get_bits(uint16_t high_bit,
                      uint16_t low_bit,
                      uint32_t *value)
{
    uint16_t bit_count;
    uint16_t i;
    uint32_t result = 0;

    if (value == NULL)
        return -1;

    if (high_bit >= DETECTOR_BITS ||
        low_bit >= DETECTOR_BITS)
    {
        return -1;
    }

    if (high_bit < low_bit)
    {
        return -1;
    }

    bit_count = high_bit - low_bit + 1;

    if (bit_count > 32)
    {
        return -1;
    }

    for (i = 0; i < bit_count; i++)
    {
        int bit_value;

        bit_value = detector_get_bit(low_bit + i);

        if (bit_value < 0)
            return -1;

        if (bit_value)
        {
            result |= (uint32_t)1U << i;
        }
    }

    *value = result;

    
    printf("%d:%d = 0x%X\n", high_bit, low_bit, result);
    return 0;
}


/*
 * 打印51字节
 */
void detector_print_bytes(void)
{
    int i;
    uint32_t *value = (uint32_t *)g_detector_data;

    printf("\n========== 51 BYTE ==========\n");

    for (i = 0; i < DETECTOR_BYTES/4; i++)
    {
        printf("data[%02d] = 0x%08X\n",
               i,
               value[i]);
    }

    int remaining_bytes = DETECTOR_BYTES % 4;
    if(remaining_bytes<DETECTOR_BYTES){
    
        for (i=i*4; i < DETECTOR_BYTES; i++)
        {
            printf("data[%02d] = 0x%02X\n",
                i,
                g_detector_data[i]);
        }
    }
}


/*
 * 打印指定范围
 */
void detector_print_bits(uint16_t high_bit,
                         uint16_t low_bit)
{
    int i;

    printf("bit[%03d:%03d] = ",
           high_bit,
           low_bit);

    for (i = high_bit; i >= low_bit; i--)
    {
        printf("%d", detector_get_bit(i));
    }

    printf("\n");
}


/*
 * 获取最终51字节数据
 */
const uint8_t *detector_get_data(void)
{
    return g_detector_data;
}


/*
 * ============================================================
 * TEST
 * ============================================================
 */
int main(void)
{
    uint32_t value;

    detector_init();


    /*
     * ========================================================
     * 1. 设置单bit
     * ========================================================
     */

    // detector_set_bit(407, 1);
    // detector_set_bit(406, 0);


    /*
     * ========================================================
     * 2. 设置407~399，共9bit
     *
     * value = 0x123
     *
     * 0x123 = 100100011
     * ========================================================
     */

    // if (detector_set_bits(407, 399, 0x123) != 0)
    // {
    //     printf("set bits failed\n");
    // }


    /*
     * ========================================================
     * 3. 设置20~15，共6bit
     *
     * 0x35 = 110101
     * ========================================================
     */

    // detector_set_bits(20, 15, 0x35);


    /*
     * ========================================================
     * 4. 设置10~8，共3bit
     *
     * 5 = 101
     * ========================================================
     */

    // detector_set_bits(10, 8, 5);


    /*
     * ========================================================
     * 5. 设置7~0，共8bit
     *
     * 0xA5 = 10100101
     * ========================================================
     */

    // detector_set_bits(7, 0, 0xA5);


    /*
     * ========================================================
     * 读取407~399
     * ========================================================
     */

    // if (detector_get_bits(407, 399, &value) == 0)
    // {
    //     printf("\nbit[407:399] = 0x%X\n", value);
    // }


// M[407:392] 0000_0000_0000_0000
// M[391:376] 0100_1001_0000_1010
// M[375:360] 0011_0111_0011_0100
// M[359:344] 0001_0101_0011_1011
// M[343:336] 0011_0100
// M[328:313] 1001_1101_1010_1011
// M[312:297] 1111_1110_1111_1111
// M[296:281] 1000_1110_1000_0110
// M[280:265] 0000_0110_0000_0001
// M[264:249] 1010_1111_1100_0001
// M[248:233] 1010_1110_1011_1000
// M[232:224] 0000_1111_1
// M[223:211] 1_1110_0001_0000
// M[210:196] 0100_1101_1001_101
// M[189] 0
// M[183] 1
// M[168:153] 0000_1011_0110_0010
// M[152:137] 1101_1001_1011_0110
// M[136:121] 1101_1011_0000_1101
// M[120:105] 1011_0110_0010_0000
// M[104:89] 1000_0000_0100_0000
// M[88:73] 0000_0100_0000_0010
// M[72:57] 0010_1000_1001_1001
// M[56:41] 1001_1111_0110_0001
// M[40:25] 1101_0011_0101_1110
// M[24:11] 10_0011_1000_1001
// M[4:0]


    detector_set_bits(407, 392, 0x0);
    detector_set_bits(391, 376, 0x0);

    detector_set_bits(375, 360, 0x0);
    detector_set_bits(359, 344, 0x0);

    detector_set_bits(343, 336, 0x0);



    /*
     * ========================================================
     * 打印
     * ========================================================
     */

    // detector_print_bits(407, 399);
    // detector_print_bits(20, 15);
    // detector_print_bits(10, 8);
    // detector_print_bits(7, 0);

    // detector_print_bytes();


    detector_init();

    detector_set_bits(407, 392, 0x0000);
    detector_set_bits(391, 376, 0x490A);
    detector_set_bits(375, 360, 0x3734);
    detector_set_bits(359, 344, 0x153B);
    detector_set_bits(343, 336, 0x34);

    /*
    * M[335:329] 未提供
    */  
    //0x343AFB57}, //343-312
    detector_set_bits(335, 329, 0x1D);

    detector_set_bits(328, 313, 0x9DAB);
    detector_set_bits(312, 297, 0xFEFF);
    detector_set_bits(296, 281, 0x8E86);
    detector_set_bits(280, 265, 0x0601);
    detector_set_bits(264, 249, 0xAFC1);
    detector_set_bits(248, 233, 0xAEB8);
    detector_set_bits(232, 224, 0x01F);
    detector_set_bits(223, 211, 0x1E10);
    detector_set_bits(210, 196, 0x26CD);

    /*
    * M[195:190] 未提供
    */
   //0x826CD90C}, //215-184
    detector_set_bits(195, 190, 0x24);

   
    detector_set_bit(189, 0);

    /*
    * M[188:184] 未提供
    */
   //0x826CD90C}, //215-184
    detector_set_bits(188, 184, 0x0C);

    detector_set_bit(183, 1);

    /*
    * M[182:169] 未提供
    */
   //0x8B8416C5}, //183-152
   detector_set_bits(182, 169, 0x05C2);


    detector_set_bits(168, 153, 0x0B62);
    detector_set_bits(152, 137, 0xD9B6);
    detector_set_bits(136, 121, 0xDB0D);
    detector_set_bits(120, 105, 0xB620);
    detector_set_bits(104, 89,  0x8040);
    detector_set_bits(88, 73,   0x0402);
    detector_set_bits(72, 57,   0x2899);
    detector_set_bits(56, 41,   0x9F61);
    detector_set_bits(40, 25,   0xD35E);
    detector_set_bits(24, 11,   0x2389);

    /*
    * M[10:5] 未提供
    */

    //0x1C400000}, //23:-8
    detector_set_bits(10, 5, 0x0);

    detector_set_bits(4, 0, 0x10);


    detector_print_bits(392, 388);
    detector_set_bits(392, 388, 0x1);

    // detector_print_bits(343, 336);
    // detector_print_bits(328, 313);
    // detector_print_bits(312, 297);

    detector_print_bits(335, 329);
    detector_print_bits(195, 190);
    detector_print_bits(188, 184);
    detector_print_bits(182, 169);
    detector_print_bits(10, 5);
    detector_print_bits(202, 196);
    detector_print_bits(392, 388);


    detector_get_bits(335, 329, &value);
    detector_get_bits(195, 190, &value);
    detector_get_bits(188, 184, &value);
    detector_get_bits(182, 169, &value);
    detector_get_bits(10, 5, &value);


    
    detector_print_bytes();



    return 0;
}