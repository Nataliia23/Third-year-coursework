all: AESRand.s AESRand 

clean:
	rm AESRand.s AESRand AESRand.o UnitTest

AESRand.s: AESRand.cpp
	g++ -maes -S AESRand.cpp -o AESRand.s

AESRand.o: AESRand.cpp
	g++ -g -c -maes AESRand.cpp -o AESRand.o

UnitTest: UnitTest.cpp AESRand.o
	g++ -g -maes UnitTest.cpp AESRand.o -o UnitTest
