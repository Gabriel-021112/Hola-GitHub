#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>

/*
 * Unión que permite representar un valor de 128 bits como:
 *
 *   - un registro SIMD (__m128i)
 *   - dos palabras de 64 bits
 *
 * lo = bits 0-63
 * hi = bits 64-127
 */
typedef union {
    __m128i m128;

    struct {
        uint64_t lo;
        uint64_t hi;
    } u64;

} U128;


/*
 * Multiplica dos números de 128 bits.
 *
 * El resultado es un número de 256 bits:
 *
 * result[0] -> bits 0-63
 * result[1] -> bits 64-127
 * result[2] -> bits 128-191
 * result[3] -> bits 192-255
 */
void mul128(U128 a, U128 b, uint64_t result[4])
{
    uint64_t a_lo = a.u64.lo;
    uint64_t a_hi = a.u64.hi;

    uint64_t b_lo = b.u64.lo;
    uint64_t b_hi = b.u64.hi;


    /*
     * Variables para las partes altas de MULX.
     *
     * GCC espera unsigned long long *
     * como tercer argumento de _mulx_u64().
     */
    unsigned long long hi_lo;
    unsigned long long hi_hi;
    unsigned long long carry;
    unsigned long long hi_hi_hi;


    /*
     * Productos parciales:
     *
     * a_lo * b_lo
     * a_lo * b_hi
     * a_hi * b_lo
     * a_hi * b_hi
     */

    uint64_t lo_lo =
        _mulx_u64(
            (unsigned long long)a_lo,
            (unsigned long long)b_lo,
            &hi_lo
        );

    uint64_t mid1 =
        _mulx_u64(
            (unsigned long long)a_lo,
            (unsigned long long)b_hi,
            &hi_hi
        );

    uint64_t mid2 =
        _mulx_u64(
            (unsigned long long)a_hi,
            (unsigned long long)b_lo,
            &carry
        );

    uint64_t hi_hi_lo =
        _mulx_u64(
            (unsigned long long)a_hi,
            (unsigned long long)b_hi,
            &hi_hi_hi
        );


    /*
     * =====================================================
     * PALABRA 1
     * =====================================================
     *
     * Se suma:
     *
     * parte alta de (a_lo * b_lo)
     *
     * +
     *
     * parte baja de (a_lo * b_hi)
     *
     * +
     *
     * parte baja de (a_hi * b_lo)
     *
     */

    uint64_t sum_mid = (uint64_t)hi_lo;

    uint64_t carry_mid = 0;

    uint64_t old = sum_mid;

    sum_mid += mid1;

    if (sum_mid < old)
        carry_mid++;


    old = sum_mid;

    sum_mid += mid2;

    if (sum_mid < old)
        carry_mid++;


    /*
     * =====================================================
     * PALABRA 2
     * =====================================================
     *
     * Se suma:
     *
     * parte alta de (a_lo * b_hi)
     *
     * +
     *
     * parte alta de (a_hi * b_lo)
     *
     * +
     *
     * parte baja de (a_hi * b_hi)
     *
     * +
     *
     * acarreo de la palabra anterior
     */

    uint64_t sum_high = (uint64_t)hi_hi;

    uint64_t carry_high = 0;


    old = sum_high;

    sum_high += (uint64_t)carry;

    if (sum_high < old)
        carry_high++;


    old = sum_high;

    sum_high += hi_hi_lo;

    if (sum_high < old)
        carry_high++;


    old = sum_high;

    sum_high += carry_mid;

    if (sum_high < old)
        carry_high++;


    /*
     * =====================================================
     * RESULTADO
     * =====================================================
     */

    result[0] = lo_lo;
    result[1] = sum_mid;
    result[2] = sum_high;

    result[3] = (uint64_t)hi_hi_hi + carry_high;
}


/*
 * Imprime el resultado de 256 bits
 * desde la parte más significativa
 * hasta la menos significativa.
 */
void print_bytes(uint64_t r[4])
{
    for (int i = 3; i >= 0; --i)
    {
        uint64_t v = r[i];

        for (int j = 7; j >= 0; --j)
        {
            unsigned char byte =
                (unsigned char)((v >> (j * 8)) & 0xFF);

            printf("%02X ", byte);
        }
    }

    printf("\n");
}


int main(void)
{
    /*
     * a =
     *
     * 0x0123456789ABCDEFFEDCBA9876543210
     */

    U128 a = {
        .u64 = {
            .lo = 0xFEDCBA9876543210ULL,
            .hi = 0x0123456789ABCDEFULL
        }
    };


    /*
     * b =
     *
     * 0x0F1E2D3C4B5A69788796A5B4C3D2E1F0
     */

    U128 b = {
        .u64 = {
            .lo = 0x8796A5B4C3D2E1F0ULL,
            .hi = 0x0F1E2D3C4B5A6978ULL
        }
    };


    /*
     * Arreglo para almacenar los 256 bits
     */
    uint64_t result[4];


    /*
     * Multiplicación de 128 × 128 bits
     */
    mul128(a, b, result);


    /*
     * Mostrar resultado
     */
    printf("Resultado (256 bits) en bytes:\n");

    print_bytes(result);


    return 0;
}
