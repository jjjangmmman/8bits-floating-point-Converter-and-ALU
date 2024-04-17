#define _CRT_SECURE_NO_WARNINGS
#define SIGN_BITS 1
#define EXP_BITS 3
#define FRAC_BITS 4
#define BIAS 3
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//형식지정자에 알맞는 값을 입력받지않은 경우 or 연산자 입력이 잘못된 경우 프로그램 종료
int check_right_scan(int scanItems, char fop) {
	if (scanItems != 3 || (fop != '+' && fop != '-')) {
		printf("입력 오류, 프로그램 종료\n");
		return 1;
	}
	return 0;
}
//SWUFP8구조 SIGN_BITS=1, EXP_BITS=3, FRAC_BITS=4, BIAS=3
void float_to_SWUFP8(float fnum) {
	//부분별로 변수 생성
	uint8_t sign, exp, frac;

	// 부호 계산
	sign = fnum >= 0 ? 0 : 1;
	fnum = fnum >= 0 ? fnum : -fnum;

	//floating-point number 범위 [exp=1~6(10진수),frac=anything]
	if (fnum >= 0.25 && fnum <= 15.5) {
		//정수부분
		if (fnum >= 1) {
			char integer_part[5];//어짜피 최대가 15 = 1111(2) 널 문자 고려하여 5
			_itoa((int)fnum, integer_part, 2);
			printf("정수 : %s\n", integer_part);
		}
		//소수부분
		char decimal_part[] = { "00000" };//4bit + 반올림하려고 1bit더 저장 + 널 문자 고려하여 6
		float decimal = fnum - (int)fnum;
		for (int i = 0; i < sizeof(decimal_part) - 1; i++) {
			decimal *= 2;
			if (decimal >= 1) {
				decimal_part[i] = '1';
				decimal -= 1;
			}
		}
		printf("소수(5bit) : %s\n", decimal_part);
		//"11111"이 아닌경우만 반올림 처리
		if (strcmp(decimal_part, "11111") != 0) {
			if (decimal_part[4] == '1') {
				//2진문자열(binary[])을 10진수(rounded_frac_10)로 변환
				int binary[FRAC_BITS];
				int rounded_frac_10 = 0;
				for (int i = 0; i < FRAC_BITS; i++) {
					binary[i] = decimal_part[i] - '0';
				}
				int position = 0;
				for (int i = FRAC_BITS - 1; i >= 0; i--) {
					if (binary[i] == 1)
						rounded_frac_10 += (1 << position);
					position++;
				}
				rounded_frac_10++;//반올림하니까 1더함
				//다시 10진수를 2진 문자열로 변환
				char temp[FRAC_BITS + 1];
				_itoa(rounded_frac_10, temp, 2);
				printf("반올림후(4bit) : %s\n", temp);
				printf("temp길이 : %d\n", strlen(temp));
				char final_decimal_part[] = { "0000" };
				for (int i = 4 - strlen(temp); i < 4; i++) {
					final_decimal_part[i] = temp[i - (4 - strlen(temp))];
				}
				printf("최종소수(4bit) : %s\n", final_decimal_part);
			}
		}
	}
}

int main(void) {
	char fop;
	float fnum1, fnum2;
	int scanItems;

	scanItems = scanf("%c %f %f", &fop, &fnum1, &fnum2);

	// 입력 오류 확인
	if (check_right_scan(scanItems, fop) == 1) return 1;// 입력 오류가 있을 경우 프로그램 종료
	// 올바른 입력이 있을 경우 계속 프로그램 실행

	/* translate floating number to SWUFP8 */

	return 0;
}