전기정보공학부
2020-18305
김산

추후 구현을 위해 main.cpp, utils.cpp, utils.h로 분리하였으며
컴파일은 Makefile을 통해 실행합니다. 컴파일 옵션은 아래와 같습니다.
구현 편의를 위해 괄호는 심볼이 아니지만 해시 테이블에 저장하였습니다.

---------------------------------------------------

CC=g++
CFLAGS=--std=c++14 
TARGET=main

$(TARGET):
	$(CC) $(CFLAGS) -o $@ main.cpp utils.cpp

clean:
	del $(TARGET).exe

---------------------------------------------------
#   S c h e m e _ i n t e r p r e t e r  
 