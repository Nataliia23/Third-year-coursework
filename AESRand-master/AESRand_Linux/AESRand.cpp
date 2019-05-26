#include <iostream>
#include <immintrin.h>
#include <array>
#include <ctime>


__m128i AESRand_init(){
	return _mm_set_epi8(0x2f, 0x2b, 0x2b, 0x15, 0x1f, 0xfd, 0x17, 0x13, 
		0x11, 0x0D, 0x02, 0x07, 0x05, 0x01, 0x0, 0x0);
}

__m128i increment = _mm_set_epi8(0x2f, 0x2b, 0x29, 0x25, 0x1f, 0x1d, 0x17, 0x13, 
		0x11, 0x0D, 0x0B, 0x07, 0x05, 0x03, 0x02, 0x01); 

void AESRand_increment(__m128i& state){
	state += increment; 
}

std::array<__m128i, 2> AESRand_rand(const __m128i state){
	__m128i penultimate = _mm_aesenc_si128(state, increment); 
	return {_mm_aesenc_si128(penultimate, increment), _mm_aesdec_si128(penultimate, increment)};
}

void print128_num(__m128i var)
{
    uint16_t *val = (uint16_t*) &var;
    printf("Numerical: %i %i %i %i %i %i %i %i \n", 
           val[0], val[1], val[2], val[3], val[4], val[5], 
           val[6], val[7]);
}

int main(){
	std::cout << "Running 5-billion iterations (160 Billion-bytes of Random Data)" << std::endl;
	
	//__m128i state = AESRand_init();
	__m128i state = AESRand_init();

	for(long long i=0; i<5; i++){
		AESRand_increment(state); 
		auto rands = AESRand_rand(state); 
		print128_num(rands[1]);
	}

}
