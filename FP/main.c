#define _CRT_SECURE_NO_WARNINGS
#define MAX_BINARY_LENGTH 32 
//#define SIGN_BITS 1
#define EXP_BITS 3
#define FRAC_BITS 4
#define BIAS 3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//입력이 올바른지 확인하는 함수
int check_right_scan(int scanItems, char fop) {
    if (scanItems != 3 || (fop != '+' && fop != '-')) {
        exit(1);
    }
    return 0;
}
/*
translate floating number to SWUFP8
step1 : 10진 실수를 2진수로 변환 -> 부호 | 정수 | 소수]
step2 : 2진수를 정규화된 과학적표기법으로 표현 -> [부호 | 지수부 | 가수부]
step3 : 소수점아래 4번째 반올림
step4 : SIGN_BITS + EXP_BITS + FRAC_BITS = SWUFP8
*/
char* decimalToSWUFP8(float num) {
    char sign;
    int exp;
    char frac[6];

    char* binaryString = (char*)malloc(MAX_BINARY_LENGTH * sizeof(char));
    if (binaryString == NULL) exit(1);

    /* 부호 */
    sign = (num >= 0) ? '0' : '1';
    num = (num >= 0) ? num : -num;

    int integerPart = (int)num;
    float fractionalPart = num - integerPart;

    /* 정수 */
    char integerBinary[MAX_BINARY_LENGTH];
    int index = 0;
    if (integerPart == 0) {
        integerBinary[index++] = '0';
    }
    else {
        while (integerPart != 0) {
            integerBinary[index++] = (integerPart % 2) + '0';
            integerPart /= 2;
        }
    }
    integerBinary[index] = '\0';

    int length = strlen(integerBinary);
    for (int i = 0; i < length; i++) {
        binaryString[length - 1 - i] = integerBinary[i];
    }
    binaryString[length] = '\0';

    /* 실수 */
    strcat(binaryString, ".");
    index = strlen(binaryString);
    for (int i = 0; i < 10; ++i) {
        fractionalPart *= 2;
        int bit = (int)fractionalPart;
        binaryString[index++] = bit + '0';
        fractionalPart -= bit;
    }
    binaryString[index] = '\0';
    /*printf("2진수 변환 후 : %s\n", binaryString);*/

    /* 지수부 */
    char* dotPosition = strchr(binaryString, '.');
    int dotIndex = (int)(dotPosition - binaryString);

    char* find1Position = strchr(binaryString, '1');
    int find1Index = (find1Position != NULL) ? (int)(find1Position - binaryString) : -1;

    exp = (find1Index == -1) ? -3 : ((binaryString[0] == '1') ? dotIndex - 1 : dotIndex - find1Index);

    char bias_exp[4];
    int e = exp + BIAS;
    if (e > 0 && e < 7) {//NoR
        _itoa(e, bias_exp, 2);
        int size = strlen(bias_exp);
        if (size < 3) {
            memmove(bias_exp + (3 - size), bias_exp, size + 1);
            memset(bias_exp, '0', 3 - size);
        }
    }
    else if (e <= 0) {
        // Zero, DeN
        memset(bias_exp, '0', 3); // exp = "000"
    }
    else if (e >= 7) {
        // inf, NaN
        memset(bias_exp, '1', 3); // exp = "111"
    }
    bias_exp[3] = '\0';

    /* 가수부 */
    int length2 = strlen(binaryString);
    int cnt = 0;
    for (int i = find1Index + 1; i < length2; i++) {
        if (binaryString[i] != '.') {
            frac[cnt] = binaryString[i];
            cnt++;
            if (cnt == 5) break;
        }
    }
    frac[cnt] = '\0';

    /* 반올림 */
    //frac = "11111..."인 경우 정수부로 올림하지않고 버림
    if (strcmp(frac, "11111") != 0 && frac[4] == '1') {
        int carry = 1;
        for (int i = 3; i >= 0; --i) {
            if (frac[i] == '0' && carry == 1) {
                frac[i] = '1';
                carry = 0;
            }
            else if (frac[i] == '1' && carry == 1) {
                frac[i] = '0';
            }
        }
    }
    /*printf("sign: %c\n", sign);
    printf("exp: %d,%d,%s\n", exp,e,bias_exp);
    printf("frac: %s\n", frac);*/
    /* final */
    char* final = (char*)malloc(9 * sizeof(char));
    if (final == NULL) exit(1);
    strcpy(final, "");

    if (strcmp(bias_exp, "111") == 0) {
        strcpy(final, "InF");
    }
    else if (strcmp(bias_exp, "000") == 0 && strcmp(frac, "00000") == 0) {
        strcpy(final, "Zero");
    }
    else {
        final[0] = sign;
        strcpy(final + 1, bias_exp);
        strncat(final, frac, 4);
    }

    free(binaryString);
    return final;
}

float SWUFP8ToDec(char* swnum) {
    float result = 0;
    if (strcmp(swnum, "Zero") == 0) {
        return result;
    }
    else {
        char sign = swnum[0];

        int E_dec = 0;
        char exp1[4] = "";
        strncpy(exp1, swnum + 1, 3);
        exp1[3] = '\0';
        for (int i = 2; i >= 0; i--) {
            if (exp1[i] == '1') {
                E_dec += pow(2, 2 - i);
            }
        }

        float F_dec = 1;
        char frac1[5] = "";
        strncpy(frac1, swnum + 4, 4);
        frac1[4] = '\0';
        for (int i = 0; i < 4; i++) {
            if (frac1[i] == '1') {
                F_dec += pow(2, -1 - i);
            }
        }


        if (sign == '0') {
            result = F_dec * (pow(2, E_dec - BIAS));
        }
        else {
            result = -F_dec * (pow(2, E_dec - BIAS));
        }
        //FRAC_BITS 4이므로 10진수로 정밀도 약 소수점아래 1~2자리수
        return result = roundf(result * 100) / 100;
    }
}
int main() {
    FILE* inputFile, * outputFile;
    inputFile = fopen("input.txt", "r");
    if (inputFile == NULL) {
        printf("입력 파일을 열 수 없습니다.\n");
        return 1;
    }

    outputFile = fopen("output.txt", "w");
    if (outputFile == NULL) {
        printf("출력 파일을 생성할 수 없습니다.\n");
        fclose(inputFile);
        return 1;
    }

    char fop;
    float fnum1, fnum2, swuresult;
    int scanItems;

    for (int i = 0; i < 10; i++) {
        scanItems = fscanf(inputFile, "%f %c %f", &fnum1, &fop, &fnum2);
        if (check_right_scan(scanItems, fop) == 1) continue;

        char* swufnum1 = decimalToSWUFP8(fnum1);
        char* swufnum2 = decimalToSWUFP8(fnum2);
        if (strcmp(swufnum1, "InF") == 0 && strcmp(swufnum2, "InF") == 0 && fop == '-') {
            fprintf(outputFile, "결과 : NaN\n");//무한대-무한대 = NaN
            free(swufnum1);
            free(swufnum2);
            continue;
        }
        else if (strcmp(swufnum1, "InF") == 0 || strcmp(swufnum2, "InF") == 0) {
            fprintf(outputFile, "결과 : InF\n"); // 위의 경우를 제외한 무한대가 있는 연산은 결과도 무한대
            free(swufnum1);
            free(swufnum2);
            continue;
        }

        if (fop == '+') {
            swuresult = SWUFP8ToDec(swufnum1) + SWUFP8ToDec(swufnum2);
        }
        else {
            swuresult = SWUFP8ToDec(swufnum1) - SWUFP8ToDec(swufnum2);
        }
        /*
        ---SWUFP8형식의 표현범위---

        exp==000, frac!=0000 인 denormalized의 positive 범위
        ->[ 0.015625 (0 000 0001(2)) ~ 0.234375 (0 000 1111(2)) ]

        exp!=000, frac!=0000 인 normalized의 positive 범위
        ->[ 0.25 (0 001 0000(2)) ~ 15.5 (0 110 1111(2)) ]
        */
        if (swuresult == 0) {
            fprintf(outputFile, "결과 : %.1f, Zero\n", swuresult);
            free(swufnum1);
            free(swufnum2);
            continue;
        }
        float abs_f = fabs(swuresult);
        if (abs_f >= 0.015625 && abs_f <= 0.234375) {
            fprintf(outputFile, "결과 : %.1f, DeN\n", swuresult);
            free(swufnum1);
            free(swufnum2);
            continue;
        }
        else if (abs_f <= 0.25 || abs_f >= 15.5) {
            fprintf(outputFile, "결과 : InF\n");
            free(swufnum1);
            free(swufnum2);
            continue;
        }
        else {
            fprintf(outputFile, "결과 : %.1f, NoR\n", swuresult);
            free(swufnum1);
            free(swufnum2);
            continue;
        }
    }

    fclose(inputFile);
    fclose(outputFile);
    return 0;
}