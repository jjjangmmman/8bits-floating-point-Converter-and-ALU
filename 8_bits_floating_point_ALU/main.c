#define _CRT_SECURE_NO_WARNINGS
#define BIAS 3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*입력 값이 지정된 형식에 맞지 않거나 or 연산자 입력이 잘못된 경우 프로그램 종료*/
int check_right_scan(int scan_items, char op) {
	if (scan_items != 3 || (op != '+' && op != '-')) {
		printf("Please enter it in the correct format.\n");
		return 1;
	}
	return 0;
}

void FP8(float fnum, char* object, char* FP8_fnum) {
	/*STEP1 : decimal to binary 2.5를 2진법으로 바꾸면 10.1*/
	/*부호*/
	char sign = (fnum >= 0) ? '0' : '1';
	fnum = (fnum >= 0) ? fnum : -fnum;

	/*정수*/
	int integer = (int)fnum;
	char* bi_integer = (char*)malloc(33 * sizeof(char));
	if (bi_integer == NULL) exit(1);

	if (integer != 0) {
		int index = 0;
		while (integer > 0) {
			bi_integer[index++] = (integer % 2) + '0';
			integer /= 2;
		}
		bi_integer[index] = '\0';
		/* 정수 부분 역순으로 뒤집기 */
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
	else strcpy(bi_integer, "0");

	/*실수*/
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
		else {
			bi_fraction[index++] = '0';
		}
	}
	for (; index < 32; index++) {
		bi_fraction[index] = '0';
	}
	bi_fraction[32] = '\0';

	/*--------------------------------------------*/
	/*STEP2 : 정규화된 과학적 표기법*/

	/*지수부*/
	int exponent = 0;
	float value = fnum;
	if (value == 0) exponent = -3;
	while (value >= 2.0) {
		value /= 2.0;
		exponent++;
	}
	while (value < 1.0 && value != 0.0) {
		value *= 2.0;
		exponent--;
	}
	
	/*가수부*/
	char* mantissa = (char*)malloc(6 * sizeof(char));
	if (mantissa == NULL) exit(1);
	
	char* find_one = (char*)malloc(65 * sizeof(char));
	if (find_one == NULL) exit(1);
	sprintf(find_one, "%s%s",bi_integer, bi_fraction);

	char* p_one = strchr(find_one, '1');
	if (p_one != NULL) {
		strncpy(mantissa, p_one + 1, 5);
		mantissa[5] = '\0';
	}
	else strcpy(mantissa, "00000");

	/*가수부 반올림*/
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

	/*specialized number구분*/
	if (exponent < -3) exponent = -3;
	if (exponent > 4) exponent = 4;
	exponent += BIAS;

	char str_exponent[4];
	if (exponent == 0) {
		strcpy(str_exponent, "000");
		if (strcmp(mantissa, "0000") == 0) strcpy(object, "ZeR");
		else strcpy(object, "DeN");
	}
	else if (exponent == 7) {
		strcpy(str_exponent, "111");
		if (strcmp(mantissa, "0000") == 0) strcpy(object, "InF");
		else strcpy(object, "NaN");
	}
	else {
		strcpy(object, "NoR");
		_itoa(exponent, str_exponent, 2);
	}
	sprintf(FP8_fnum, "%c%s%s", sign, str_exponent, mantissa);
}


/*메인 함수*/
int main(void) {
	char op;
	float fnum1, fnum2;
	int scan_items;
	char object[4];
	char FP8_fnum[9];

	/*read operator  and 2 float numbers*/
	printf("read operator '+' or '-' and 2 float numbers :");
	scan_items = scanf("%c %f %f", &op, &fnum1, &fnum2);
	if (check_right_scan(scan_items, op) == 1) return 1;

	/*translate floating number to FP8*/
	FP8(fnum1, &object, &FP8_fnum);
	printf("유형 : %s, 2진수 표현 : %s", object, FP8_fnum);

	return 0;
}
