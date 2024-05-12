## :computer: Project 8 bits floating point addition and subtraction functions in C
### :triangular_ruler: Structure:
  - __consists of 1 sign bit, 3 exponent bits and 4 bits of mantissa.__
    - Defined with 2-digit precision. (2^4=16)
    - exp==000, frac!=0000 인 __denormalized__ 의 positive 범위
        ->[ 0.015625 (0 000 0001(2)) ~ 0.234375 (0 000 1111(2)) ]
        exp!=000, frac!=0000 인 __normalized__ 의 positive 범위
        ->[ 0.25 (0 001 0000(2)) ~ 15.5 (0 110 1111(2)) ]
  - The buffer size of the integer and decimal parts is set to 32 bits.
  - __Denormalized Values, Special values, rounding, etc. are all implemented based on IEEE754.__
#### :pushpin: Denormalized Values
  - Since the difference between DeN's MAX and NoR's MIN is small, values ​​between 0 and 0.25 were defined and calculated as DeN.
      (Denormalized scientific notation + setting exp to -2)
  - In DeN's mantissa, even if 5 bits are rounded, the difference is too small and is truncated because it is defined with 2-digit precision, so no rounding was performed.
#### :pushpin: Normalized Values
  - Rounding: If mantissa == '11111', rounding was not performed. Difficult to handle carry.
  - From 16, where the exponent is 4 and '111', it was defined as InF, __∞__
---
### :open_file_folder: About Files:
  - _FP_ files are drafts. It was an assignment for a computer architecture class, but I was disappointed that I couldn't implement it properly because it overlapped with the exam period:dizzy_face:, so I personally wrote the code again.
  - The only two files you should be modifying are _converter.c_ ~~(if needed)~~ and _main.c_
  - _converter.c_ only has floating point conversion functions.
  - _main.c_ also includes the functions of ALU. However, it has not been implemented yet. _I don't know when it will be completed_:joy:
  
---
### :bulb: Ideas : FP8 ALU
  - InF도 sign에 따라 나눠서 ∞-∞ NaN 표현할수있게하기
  - 덧셈과 뺄셈을 나누지않고 덧셈을 수행하는 함수하나로 구현하고싶은데 이에대해 생각하기
  - 입력으로 들어온 두 수의 부호에따라 어떻게 처리할것인지도 생각하기
  - 연산결과를 출력할때 object와 FP8_binary도 함께 출력하기 이떄 NaN은 exp=111, frac!=0000

