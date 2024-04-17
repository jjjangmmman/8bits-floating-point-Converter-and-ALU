#define _CRT_SECURE_NO_WARNINGS
#define SIGN_BITS 1
#define EXP_BITS 3
#define FRAC_BITS 4
#define BIAS 3
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//���������ڿ� �˸´� ���� �Է¹������� ��� or ������ �Է��� �߸��� ��� ���α׷� ����
int check_right_scan(int scanItems, char fop) {
	if (scanItems != 3 || (fop != '+' && fop != '-')) {
		printf("�Է� ����, ���α׷� ����\n");
		return 1;
	}
	return 0;
}
//SWUFP8���� SIGN_BITS=1, EXP_BITS=3, FRAC_BITS=4, BIAS=3
void float_to_SWUFP8(float fnum) {
	//�κк��� ���� ����
	uint8_t sign, exp, frac;

	// ��ȣ ���
	sign = fnum >= 0 ? 0 : 1;
	fnum = fnum >= 0 ? fnum : -fnum;

	//floating-point number ���� [exp=1~6(10����),frac=anything]
	if (fnum >= 0.25 && fnum <= 15.5) {
		//�����κ�
		if (fnum >= 1) {
			char integer_part[5];//��¥�� �ִ밡 15 = 1111(2) �� ���� ����Ͽ� 5
			_itoa((int)fnum, integer_part, 2);
			printf("���� : %s\n", integer_part);
		}
		//�Ҽ��κ�
		char decimal_part[] = { "00000" };//4bit + �ݿø��Ϸ��� 1bit�� ���� + �� ���� ����Ͽ� 6
		float decimal = fnum - (int)fnum;
		for (int i = 0; i < sizeof(decimal_part) - 1; i++) {
			decimal *= 2;
			if (decimal >= 1) {
				decimal_part[i] = '1';
				decimal -= 1;
			}
		}
		printf("�Ҽ�(5bit) : %s\n", decimal_part);
		//"11111"�� �ƴѰ�츸 �ݿø� ó��
		if (strcmp(decimal_part, "11111") != 0) {
			if (decimal_part[4] == '1') {
				//2�����ڿ�(binary[])�� 10����(rounded_frac_10)�� ��ȯ
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
				rounded_frac_10++;//�ݿø��ϴϱ� 1����
				//�ٽ� 10������ 2�� ���ڿ��� ��ȯ
				char temp[FRAC_BITS + 1];
				_itoa(rounded_frac_10, temp, 2);
				printf("�ݿø���(4bit) : %s\n", temp);
				printf("temp���� : %d\n", strlen(temp));
				char final_decimal_part[] = { "0000" };
				for (int i = 4 - strlen(temp); i < 4; i++) {
					final_decimal_part[i] = temp[i - (4 - strlen(temp))];
				}
				printf("�����Ҽ�(4bit) : %s\n", final_decimal_part);
			}
		}
	}
}

int main(void) {
	char fop;
	float fnum1, fnum2;
	int scanItems;

	scanItems = scanf("%c %f %f", &fop, &fnum1, &fnum2);

	// �Է� ���� Ȯ��
	if (check_right_scan(scanItems, fop) == 1) return 1;// �Է� ������ ���� ��� ���α׷� ����
	// �ùٸ� �Է��� ���� ��� ��� ���α׷� ����

	/* translate floating number to SWUFP8 */

	return 0;
}