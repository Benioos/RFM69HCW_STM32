/*
 * AES.c
 *
 *  Created on: May 20, 2026
 *      Author: benja
 */

#include "AES.h"

static uint8_t xtime(uint8_t x)
{
    return (x << 1) ^ ((x >> 7) * 0x1B);
}

/*
 * To Encode AES 128 Bits
 */

void Text_16Byte_ToState(state_t state, const char text[16])
{
    uint8_t row;
    uint8_t col;
    uint8_t index = 0;

    for(col = 0; col < 4; col++)
    {
        for(row = 0; row < 4; row++)
        {
            state[row][col] = (uint8_t)text[index];
            index++;
        }
    }
}

void AddRoundKey(state_t state, const uint8_t Cipher_Key[16])
{
    uint8_t row;
    uint8_t col;

    for(col = 0; col < 4; col++)
    {
        for(row = 0; row < 4; row++)
        {
            state[row][col] ^= Cipher_Key[row + 4 * col];
        }
    }
}

void SubBytes(state_t state)
{
    uint8_t row;
    uint8_t col;

    for(col = 0; col < 4; col++)
    {
        for(row = 0; row < 4; row++)
        {
            state[row][col] =
                sbox[state[row][col]];
        }
    }
}

void ShiftRows(state_t state)
{
    uint8_t temp;

    // row 1
    temp = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = temp;

    // row 2
    temp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = temp;

    temp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = temp;

    // row 3
    temp = state[3][3];
    state[3][3] = state[3][2];
    state[3][2] = state[3][1];
    state[3][1] = state[3][0];
    state[3][0] = temp;
}



void MixColumns(state_t state)
{
    uint8_t i;
    uint8_t a0, a1, a2, a3;
    uint8_t t;

    for(i = 0; i < 4; i++)
    {
        a0 = state[0][i];
        a1 = state[1][i];
        a2 = state[2][i];
        a3 = state[3][i];

        t = a0 ^ a1 ^ a2 ^ a3;

        uint8_t tmp = a0;

        state[0][i] ^= t ^ xtime(a0 ^ a1);
        state[1][i] ^= t ^ xtime(a1 ^ a2);
        state[2][i] ^= t ^ xtime(a2 ^ a3);
        state[3][i] ^= t ^ xtime(a3 ^ tmp);
    }
}


// Génération des RoundKey
static const uint8_t Rcon[11] = {
0x00,
0x01,0x02,0x04,0x08,
0x10,0x20,0x40,0x80,
0x1B,0x36
};

void KeyExpansion(uint8_t* RoundKey, const uint8_t* Key)
{
    uint32_t i;
    uint8_t temp[4];
    uint8_t k;

    for(i = 0; i < 16; i++)
    {
        RoundKey[i] = Key[i];
    }

    for(i = 4; i < 44; i++)
    {
        for(k = 0; k < 4; k++)
        {
            temp[k] = RoundKey[(i - 1) * 4 + k];
        }

        if(i % 4 == 0)
        {
            uint8_t t = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = t;

            temp[0] = sbox[temp[0]];
            temp[1] = sbox[temp[1]];
            temp[2] = sbox[temp[2]];
            temp[3] = sbox[temp[3]];

            temp[0] ^= Rcon[i / 4];
        }

        for(k = 0; k < 4; k++)
        {
            RoundKey[i * 4 + k] =
                RoundKey[(i - 4) * 4 + k] ^ temp[k];
        }
    }
}

void AES128_Encrypt(state_t state, const uint8_t RoundKey[176])
{
    AddRoundKey(state, RoundKey);

    for(uint32_t round = 1; round < 10; round++)
    {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(state, RoundKey + (round * 16));
    }

    // Final Round
    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(state, RoundKey + 160);
}


void AES128_Encrypt_Buffer256(uint8_t buffer[256], const uint8_t RoundKey[176])
{
    for (uint16_t i = 0; i < 256; i += 16)
    {
        state_t state;
        uint8_t index = 0;

        for (uint8_t col = 0; col < 4; col++) {
            for (uint8_t row = 0; row < 4; row++) {
                state[row][col] = buffer[i + index];
                index++;
            }
        }

        AES128_Encrypt(state, RoundKey);

        index = 0;
        for (uint8_t col = 0; col < 4; col++) {
            for (uint8_t row = 0; row < 4; row++) {
                buffer[i + index] = state[row][col];
                index++;
            }
        }
    }
}


/*
 * To Decode AES 128 Bits
 */
void InvSubBytes(state_t state)
{
    uint8_t r,c;

    for(r = 0; r < 4; r++)
    {
        for(c = 0; c < 4; c++)
        {
            state[r][c] = inv_sbox[state[r][c]];
        }
    }
}

void InvShiftRows(state_t state)
{
    uint8_t temp;

    // row 1 shift right 1
    temp = state[1][3];
    state[1][3] = state[1][2];
    state[1][2] = state[1][1];
    state[1][1] = state[1][0];
    state[1][0] = temp;

    // row 2 shift right 2
    temp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = temp;

    temp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = temp;

    // row 3 shift right 3
    temp = state[3][0];
    state[3][0] = state[3][1];
    state[3][1] = state[3][2];
    state[3][2] = state[3][3];
    state[3][3] = temp;
}


void InvMixColumns(state_t state)
{
    uint8_t i;
    uint8_t a, b, c, d;
    uint8_t t, t9;

    for(i = 0; i < 4; i++)
    {
        a = state[0][i];
        b = state[1][i];
        c = state[2][i];
        d = state[3][i];

        t = a ^ b ^ c ^ d;

        t9 = xtime(xtime(xtime(t))) ^ t;

        state[0][i] = t9 ^ xtime(xtime(a ^ c)) ^ xtime(a ^ b) ^ a;
        state[1][i] = t9 ^ xtime(xtime(b ^ d)) ^ xtime(b ^ c) ^ b;
        state[2][i] = t9 ^ xtime(xtime(a ^ c)) ^ xtime(c ^ d) ^ c;
        state[3][i] = t9 ^ xtime(xtime(b ^ d)) ^ xtime(a ^ d) ^ d;
    }
}

void AES128_Decrypt(state_t state, const uint8_t RoundKey[176])
{
    int32_t round;

    AddRoundKey(state, RoundKey + 160);
    InvShiftRows(state);
    InvSubBytes(state);

    for(round = 9; round > 0; round--)
    {
        AddRoundKey(state, RoundKey + (round * 16));
        InvMixColumns(state);
        InvShiftRows(state);
        InvSubBytes(state);

    }

    AddRoundKey(state, RoundKey);
}

void AES128_Decrypt_Buffer256(uint8_t buffer[256], const uint8_t RoundKey[176])
{
    for (uint16_t i = 0; i < 256; i += 16)
    {
        state_t state;
        uint8_t index = 0;

        for (uint8_t col = 0; col < 4; col++) {
            for (uint8_t row = 0; row < 4; row++) {
                state[row][col] = buffer[i + index];
                index++;
            }
        }

        AES128_Decrypt(state, RoundKey);

        index = 0;
        for (uint8_t col = 0; col < 4; col++) {
            for (uint8_t row = 0; row < 4; row++) {
                buffer[i + index] = state[row][col];
                index++;
            }
        }
    }
}
