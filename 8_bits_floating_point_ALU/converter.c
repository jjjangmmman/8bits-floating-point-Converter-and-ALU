#define _CRT_SECURE_NO_WARNINGS
#define EXPONENT_BITS 3
#define MANTISSA_BITS 4
#define BIAS 3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void FP8(float fnum, char* object, char* FP8_binary, float* p_FP8_decimal) {
	/*sign*/
	char sign = (fnum >= 0) ? '0' : '1';
	fnum = (fnum >= 0) ? fnum : -fnum;
	/*integer*/
	int integer = (int)fnum;
	char* bi_integer = (char*)malloc(33 * sizeof(char));
	if (bi_integer == NULL) exit(1);

	if (integer == 0) strcpy(bi_integer, "0");
	else {
		int index = 0;
		while (integer > 0) {
			bi_integer[index++] = (integer % 2) + '0';
			integer /= 2;
		}
		bi_integer[index] = '\0';
		/* reverse arr bi_integer*/
		int start = 0;
		int end = index - 1;
		while (start < end) {
			char temp = bi_integer[start];
			bi_integer[start] = bi_integer[end];
			bi_integer[end] = temp;
			start++;
			end--;
		}
	}
	/*fraction*/
	float fraction = fnum - (int)fnum;
	char* bi_fraction = (char*)malloc(33 * sizeof(char));
	if (bi_fraction == NULL) {
		free(bi_integer);
		exit(1);
	}
	int index = 0;
	while (fraction != 0.0 && index < 32) {
		fraction *= 2;
		if (fraction >= 1.0) {
			bi_fraction[index++] = '1';
			fraction -= 1.0;
		}
		else bi_fraction[index++] = '0';
	}
	for (; index < 32; index++) bi_fraction[index] = '0';
	bi_fraction[32] = '\0';
	/*--------------------------------------------*/
	char str_exponent[4];
	char* mantissa = (char*)malloc(6 * sizeof(char));
	if (mantissa == NULL) exit(1);

	float value = fnum;
	if (value == 0) {
		strcpy(object, "ZeR");
		strcpy(str_exponent, "000");
		strcpy(mantissa, "0000");
	}
	else if (value < 0.25) {
		strcpy(object, "DeN");
		strcpy(str_exponent, "000");
		strncpy(mantissa, bi_fraction + 2, 4);
		mantissa[4] = '\0';
		//너무 미세한 차이와 범위라서 따로 반올림 안하겠음 해도 정밀도가 1자리라 잘림ㅋ
		float f_mantissa = 0.0;
		for (int i = 0; i < MANTISSA_BITS; i++) {
			f_mantissa += (mantissa[i] - '0') * pow(2, -i - 1);
		}
		*p_FP8_decimal = (sign == '0') ? f_mantissa * pow(2, -2) : f_mantissa * pow(2, -2) * -1;
	}
	else if (value < 16) {
		strcpy(object, "NoR");
		int exponent = BIAS;
		while (value >= 2.0) {
			value /= 2.0;
			exponent++;
		}
		while (value < 1.0 && value != 0.0) {
			value *= 2.0;
			exponent--;
		}
		char* find_one = (char*)malloc(65 * sizeof(char));
		if (find_one == NULL) exit(1);
		sprintf(find_one, "%s%s", bi_integer, bi_fraction);

		char* p_one = strchr(find_one, '1');
		if (p_one != NULL) {
			strncpy(mantissa, p_one + 1, 5);
			mantissa[5] = '\0';
		}
		/*rounding*/
		//mantissa가 111111...인경우 올림처리하지않고 반올림을 수행하지않겠음
		if (strcmp(mantissa, "11111") != 0 && mantissa[4] == '1') {
			int carry = 1;
			for (int i = 3; i >= 0; i--) {
				if (mantissa[i] == '0' && carry == 1) {
					mantissa[i] = '1';
					carry = 0;
				}
				else if (mantissa[i] == '1' && carry == 1) {
					mantissa[i] = '0';
					carry = 1;
				}
			}
		}
		mantissa[4] = '\0';
		_itoa(exponent, str_exponent, 2);
		int fill_zero = EXPONENT_BITS - strlen(str_exponent);
		if (fill_zero > 0) {
			//앞부분을 0으로 채우기
			char temp_str[10];
			memset(temp_str, '0', fill_zero);
			temp_str[fill_zero] = '\0';
			strcat(temp_str, str_exponent);
			strcpy(str_exponent, temp_str);
		}
		float f_mantissa = 0.0;
		for (int i = 0; i < MANTISSA_BITS; i++) {
			f_mantissa += (mantissa[i] - '0') * pow(2, -i - 1);
		}
		f_mantissa += 1;
		*p_FP8_decimal = (sign == '0') ? f_mantissa * pow(2, exponent - BIAS) : -f_mantissa * pow(2, exponent - BIAS);
	}
	else {
		strcpy(object, "InF");
		strcpy(str_exponent, "111");
		strcpy(mantissa, "0000");
		//NaN은 연산이 수행된 이후에 구분할 수 있으므로 단순히 FP8에서는 구현하지않겠음
	}

	sprintf(FP8_binary, "%c%s%s", sign, str_exponent, mantissa);
	/*--------------------------------------------*/

}
void precision_adjustment(float f) {
	int neg_flag = (f < 0) ? 1 : 0;
	float value = fabsf(f);

	if (value == 0) {
		printf(", FP8_decimal : 0");
	}
	else if (value < 1) {
		// 소수점 이하 둘째 자리까지 출력
		value = roundf(value * 100) / 100;
		printf(", FP8_decimal : %.2f\n", (neg_flag == 1) ? -value : value);
	}
	else if (value < 10) {
		// 소수점 이하 첫째 자리까지 출력
		if (value == (int)value) {
			printf(", FP8_decimal : %d", (neg_flag == 1) ? -(int)value : (int)value);
		}
		else {
			value = roundf(value * 10) / 10;
			printf(", FP8_decimal : %.1f\n", (neg_flag == 1) ? -value : value);
		}
	}
	else {
		// 정수 부분만 출력
		printf(", FP8_decimal : %d", (neg_flag == 1) ? -(int)value : (int)value);
	}
}

/*main*/
int main(void) {
	float fnum;
	char object[4];
	char FP8_binary[9];
	float FP8_decimal = 0;

	/*read float number*/
	printf("Enter the number you want to express in FP8 : ");
	scanf("%f", &fnum);

	/*translate floating number to FP8*/
	FP8(fnum, &object, &FP8_binary, &FP8_decimal);
	printf("NUM TYPE : %s, FP8_binary : %s", object, FP8_binary);
	if (strcmp(object, "ZeR") == 0 || strcmp(object, "DeN") == 0 || strcmp(object, "NoR") == 0) {
		precision_adjustment(FP8_decimal);
	}
	return 0;
}