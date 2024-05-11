8 bits floating point addition and subtraction functions in C


Structure:
  consists of 1 sign bit, 3 exponent bits and 4 bits of mantissa.
  
  Special values, rounding, etc. are all implemented based on IEEE754.

DeN에대해서 생각하기
처음부터 범위를 나누는게 나은지? 컴퓨터가 이해하는 방식그대로하는게 나은지? 아ㅏ러니ㅏㅓㄹ미ㅏㅓ라
InF도 sign에 따라 나눠서 ∞-∞ NaN 표현할수있게하기
덧셈기를 구현하는게 맞나? 내가지금 만든건 FP8변환기일뿐임
이걸 발전시키면 덧셈기가 될듯? 일단 자러가

  
