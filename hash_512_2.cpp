#include <iostream>
#include <string>
#include <fstream>
#include <netinet/in.h>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <utility>

static uint8_t Pi[256]=
{
	252, 238, 221,  17, 207, 110,  49,  22, 251, 196, 250,
		218,  35, 197,   4,  77, 233, 119, 240, 219, 147,  46,
		153, 186,  23,  54, 241, 187,  20, 205,  95, 193, 249,
		24, 101,  90, 226,  92, 239,  33, 129,  28,  60,  66,
		139,   1, 142,  79,   5, 132,   2, 174, 227, 106, 143,
		160,   6,  11, 237, 152, 127, 212, 211,  31, 235,  52,
		44,  81, 234, 200,  72, 171, 242,  42, 104, 162, 253,
		58, 206, 204, 181, 112,  14,  86,   8,  12, 118,  18,
		191, 114,  19,  71, 156, 183,  93, 135,  21, 161, 150,
		41,  16, 123, 154, 199, 243, 145, 120, 111, 157, 158,
		178, 177,  50, 117,  25,  61, 255,  53, 138, 126, 109,
		84, 198, 128, 195, 189,  13,  87, 223, 245,  36, 169,
		62, 168,  67, 201, 215, 121, 214, 246, 124,  34, 185,
		3, 224,  15, 236, 222, 122, 148, 176, 188, 220, 232,
		40,  80,  78,  51,  10,  74, 167, 151,  96, 115,  30,
		0,  98,  68,  26, 184,  56, 130, 100, 159,  38,  65,
		173,  69,  70, 146,  39,  94,  85,  47, 140, 163, 165,
		125, 105, 213, 149,  59,   7,  88, 179,  64, 134, 172,
		29, 247,  48,  55, 107, 228, 136, 217, 231, 137, 225,
		27, 131,  73,  76,  63, 248, 254, 141,  83, 170, 144,
		202, 216, 133,  97,  32, 113, 103, 164,  45,  43,   9,
		91, 203, 155,  37, 208, 190, 229, 108,  82,  89, 166,
		116, 210, 230, 244, 180, 192, 209, 102, 175, 194,  57,
		75,  99, 182
};

static uint8_t Tau[64] =
{

		0,  8, 16, 24, 32, 40, 48, 56,
		1,  9, 17, 25, 33, 41, 49, 57,
		2, 10, 18, 26, 34, 42, 50, 58,
		3, 11, 19, 27, 35, 43, 51, 59,
		4, 12, 20, 28, 36, 44, 52, 60,
		5, 13, 21, 29, 37, 45, 53, 61,
		6, 14, 22, 30, 38, 46, 54, 62,
		7, 15, 23, 31, 39, 47, 55, 63

};

static uint64_t CONST[12][8] = 
{

	{0xB1085BDA1ECADAE9, 0xEBCB2F81C0657C1F,
	0x2F6A76432E45D016, 0x714EB88D7585C4FC,
	0x4B7CE09192676901, 0xA2422A08A460D315,
	0x05767436CC744D23, 0xDD806559F2A64507},

	{0x6FA3B58AA99D2F1A, 0x4FE39D460F70B5D7,
	0xF3FEEA720A232B98, 0x61D55E0F16B50131,
	0x9AB5176B12D69958, 0x5CB561C2DB0AA7CA,
	0x55DDA21BD7CBCD56, 0xE679047021B19BB7},

	{0xF574DCAC2BCE2FC7, 0x0A39FC286A3D8435,
	0x06F15E5F529C1F8B, 0xF2EA7514B1297B7B,
	0xD3E20FE490359EB1, 0xC1C93A376062DB09,
	0xC2B6F443867ADB31, 0x991E96F50ABA0AB2},

	{0xEF1FDFB3E81566D2, 0xF948E1A05D71E4DD,    
	0x488E857E335C3C7D, 0x9D721CAD685E353F,
	0xA9D72C82ED03D675, 0xD8B71333935203BE,
	0x3453EAA193E837F1, 0x220CBEBC84E3D12E},

	{0x4BEA6BACAD474799, 0x9A3F410C6CA92363,
	0x7F151C1F1686104A, 0x359E35D7800FFFBD,
	0xBFCD1747253AF5A3, 0xDFFF00B723271A16,
	0x7A56A27EA9EA63F5, 0x601758FD7C6CFE57},

	{0xAE4FAEAE1D3AD3D9, 0x6FA4C33B7A3039C0,
	0x2D66C4F95142A46C, 0x187F9AB49AF08EC6,
	0xCFFAA6B71C9AB7B4, 0x0AF21F66C2BEC6B6,
	0xBF71C57236904F35, 0xFA68407A46647D6E},

	{0xF4C70E16EEAAC5EC, 0x51AC86FEBF240954,
	0x399EC6C7E6BF87C9, 0xD3473E33197A93C9,
	0x0992ABC52D822C37, 0x06476983284A0504,
	0x3517454CA23C4AF3, 0x8886564D3A14D493},

	{0x9B1F5B424D93C9A7, 0x03E7AA020C6E4141,
	0x4EB7F8719C36DE1E, 0x89B4443B4DDBC49A,
	0xF4892BCB929B0690, 0x69D18D2BD1A5C42F,
	0x36ACC2355951A8D9, 0xA47F0DD4BF02E71E},

	{0x378F5A541631229B, 0x944C9AD8EC165FDE,
	0x3A7D3A1B25894224, 0x3CD955B7E00D0984,
	0x800A440BDBB2CEB1, 0x7B2B8A9AA6079C54,
	0x0E38DC92CB1F2A60, 0x7261445183235ADB},

	{0xABBEDEA680056F52, 0x382AE548B2E4F3F3,
	0x8941E71CFF8A78DB, 0x1FFFE18A1B336103,
	0x9FE76702AF69334B, 0x7A1E6C303B7652F4,
	0x3698FAD1153BB6C3, 0x74B4C7FB98459CED},

	{0x7BCD9ED0EFC889FB, 0x3002C6CD635AFE94,
	0xD8FA6BBBEBAB0761, 0x2001802114846679,
	0x8A1D71EFEA48B9CA, 0xEFBACD1D7D476E98,
	0xDEA2594AC06FD85D, 0x6BCAA4CD81F32D1B},

	{0x378EE767F11631BA, 0xD21380B00449B17A,
	0xCDA43C32BCDF1D77, 0xF82012D430219F9B,
	0x5D80EF9D1891CC86, 0xE71DA4AA88E12852,
	0xFAF417D5D9B21B99, 0x48BC924AF11BD720},

};

static uint64_t A[64] = 
{

	0x8E20FAA72BA0B470, 0x47107DDD9B505A38, 0xAD08B0E0C3282D1C, 0xD8045870EF14980E, 
	0x6C022C38F90A4C07, 0x3601161CF205268D, 0x1B8E0B0E798C13C8, 0x83478B07B2468764,
	0xA011D380818E8F40, 0x5086E740CE47C920, 0x2843FD2067ADEA10, 0x14AFF010BDD87508,
	0x0AD97808D06CB404, 0x05E23C0468365A02, 0x8C711E02341B2D01, 0x46B60F011A83988E,
	0x90DAB52A387AE76F, 0x486DD4151C3DFDB9, 0x24B86A840E90F0D2, 0x125C354207487869,
	0x092E94218D243CBA, 0x8A174A9EC8121E5D, 0x4585254F64090FA0, 0xACCC9CA9328A8950,
	0x9D4DF05D5F661451, 0xC0A878A0A1330AA6, 0x60543C50DE970553, 0x302A1E286FC58CA7,
	0x18150F14B9EC46DD, 0x0C84890AD27623E0, 0x0642CA05693B9F70, 0x0321658CBA93C138,

	0x86275DF09CE8AAA8, 0x439DA0784E745554, 0xAFC0503C273AA42A, 0xD960281E9D1D5215,
	0xE230140FC0802984, 0x71180A8960409A42, 0xB60C05CA30204D21, 0x5B068C651810A89E,
	0x456C34887A3805B9, 0xAC361A443D1C8CD2, 0x561B0D22900E4669, 0x2B838811480723BA,
	0x9BCF4486248D9F5D, 0xC3E9224312C8C1A0, 0xEFFA11AF0964EE50, 0xF97D86D98A327728,
	0xE4FA2054A80B329C, 0x727D102A548B194E, 0x39B008152ACB8227, 0x9258048415EB419D,
	0x492C024284FBAEC0, 0xAA16012142F35760, 0x550B8E9E21F7A530, 0xA48B474F9EF5DC18,
	0x70A6A56E2440598E, 0x3853DC371220A247, 0x1CA76E95091051AD, 0x0EDD37C48A08A6D8,
	0x07E095624504536C, 0x8D70C431AC02A736, 0xC83862965601DD1B, 0x641C314B2B8EE083

};

#define BLOCK_SIZE 64

void cr_hash(struct hash &object, int flag);

struct hash {
	uint8_t buffer[64];
	uint8_t n[64];
	uint8_t sigma[64];
	uint8_t cur_hash[64];

	size_t buffer_size;
	size_t cur_hash_size;

	hash() {
		std::memset(buffer, 0, sizeof(buffer));
		std::memset(n, 0, sizeof(n));
		std::memset(sigma, 0, sizeof(sigma));
		std::memset(cur_hash, 0, sizeof(cur_hash));
		buffer_size = 0;
		cur_hash_size = 512;
		
	}

	void update(uint8_t *data, size_t size) {
		if (buffer_size) {
			size_t len_free = 64 - buffer_size; //длина пустого буфера
			if (len_free > size) len_free = size; //должна совпадать с size или = 64
			memcpy(buffer + buffer_size, data, len_free); // копируем data в buffer
			data += len_free; //обрезаем data слева, выкидывая то, что уже положили в буфер
			size -= len_free; // размер data изменился - теперь либо 0,либо size = size -len_free = size - 64 + buffer_size 
			
			buffer_size += len_free; //а вот размер буфера увеличился - теперь либо 64, либо меньше

			if (buffer_size == 64) { //да-да, здесь не может быть >64
				int n = buffer_size;
				for (int i  = 0; i < (n+1) / 2; i++) {
 					std::swap(buffer[i], buffer[n - i - 1]);
				}
				
				cr_hash(*this, 0);
				printf("HASH = ");
				buffer_size = 0;
			}
		}

		while(size > 64){

			memcpy(buffer, data, 64);
			buffer_size = 64;
			int n = buffer_size;

			for (int i  = 0; i < (n+1) / 2; i++) {
 				std::swap(buffer[i], buffer[n - i - 1]);
			}

			cr_hash(*this, 0);
			size -= BLOCK_SIZE;
			data += BLOCK_SIZE;
			buffer_size = 0;
		}
		if (size > 0) {
			memset(buffer, 0, sizeof buffer);
			memcpy(buffer, data, size);
			buffer_size = size;
		}
	}
};

void XOR(uint8_t *hash, uint8_t *prev_hash, uint8_t *n) {
	for(int i = 0; i < 64; ++i) {
		hash[i] = prev_hash[i] ^ n[i];
	}
}

uint64_t L(uint64_t &b) {
	uint64_t result = 0;
	for(int i = 0; i < 64; i++) {
		if ((b & ((uint64_t)1 << i)) >> i) {					 //get bi
			result ^= A[64 - i - 1];
		}        
	}
	return result;
}


void LPS(uint8_t *temp) {
	for(int i = 0; i < 64; ++i) { 											  //S
		temp[i] = Pi[temp[i]];
	}

	uint8_t copy[64];
	memcpy(copy, temp, 64);
	for(int i = 0; i < 64; ++i) { 												//P
		temp[i] = copy[Tau[i]];
	}

	for(int i = 0; i < 8; ++i){														//L
		uint64_t step = 0;
		for(int j = 0; j < 8; ++j) {
			step |= (uint64_t(temp[i*8 + j])) << (64 - (j+1)*8);
		}
		step = L(step);
		for(int j = 0; j < 8; ++j) {
			temp[i*8 + j] = uint8_t(step >> (64 - (j+1)*8));
		}
	}

}

void set_K(uint8_t C[12][64], uint8_t KEY[13][64], uint8_t *K) {
	memcpy(KEY[0], K, 64);                //K1 = K
	
	for(int i = 1; i < 13; ++i) {
		uint8_t temp[64];
		XOR(temp, KEY[i-1], C[i-1]);
		LPS(temp);

		memcpy(KEY[i], temp, 64);
	}
}

void set_C(uint8_t C[12][64]) {
	for(int i = 0; i < 12; ++i) {
		for(int j = 0; j < 8; ++j) {
			for(int k = 0; k < 8; ++k) {
				C[i][j*8+k] = (CONST[i][j] << (8*k)) >> (56);
			}
		}
	}
}

void E(uint8_t *hash, uint8_t *buf) {

	uint8_t KEY[13][64];
	uint8_t C[12][64];
	set_C(C);
	set_K(C, KEY, hash);

	uint8_t mess[64];
	memcpy(mess, buf, 64);

	for(int i = 0; i < 12; ++i) {
		XOR(mess, KEY[i], mess);		
		LPS(mess);
	}
	
	XOR(mess, KEY[12], mess);
	memcpy(hash, mess, 64);
}

void ADD_2(uint8_t *x, uint8_t *y) {
	int carry = 0;
    for(int i = 63; i >= 0; --i) {
       x[i] = x[i] + y[i] + carry;
       carry = (x[i] < y[i]);
    }
}

void cr_hash(struct hash &object, int flag) {     //flag = 0 ordinary hash, flag = 1 - hash without n and sigma, flag = 2 - g0
	uint8_t current[64];
	uint8_t prev_hash[64];
	memcpy(prev_hash, object.cur_hash, 64);    //save prev_hash = h
	uint8_t N_0[64];
	memset(N_0, 0, 64);

	if (flag == 2) {
		XOR(object.cur_hash, object.cur_hash, N_0);
	} else {
		XOR(object.cur_hash, object.cur_hash, object.n);
	}
	
	LPS(object.cur_hash);

	E(object.cur_hash, object.buffer); // hash = X[K13]...LPSX[K1](buffer);
	
	XOR(current, object.cur_hash, prev_hash); //current = hash + h
	XOR(current, current, object.buffer); //current = hash + h + m

	memcpy(object.cur_hash, current, 64);

	uint8_t VEC_512[64], M_L[64];
  for(size_t i = 0; i < 64; ++i) {
    VEC_512[i] = 0;
    if (i == 62) VEC_512[i] = 0x2;
  }
  int n = object.buffer_size * 8;

  for(size_t i = 0; i < 64; ++i) {
    M_L[i] = 0;
  }
  for(int i = 0; i < 4; ++i) {
    M_L[63 - i] = (n & ((int)0xff << (8*i))) >> 8*i;
  }

  if (flag == 0) {
  	ADD_2(object.n, VEC_512);
		ADD_2(object.sigma, object.buffer);
  } else if (flag == 1){
  	ADD_2(object.n, M_L);
  	ADD_2(object.sigma, object.buffer);
  }

};
 
void drop_buf(uint8_t *buf){
	for (int i = 0; i < BLOCK_SIZE*8; ++i){
		buf[i] = 0;
	}
	return;
};

void padding(struct hash &object) {
	
	if (object.buffer_size < 64) {
		object.buffer[object.buffer_size] = 0x01;
    	for (size_t i = object.buffer_size + 1; i < 64; i++) {
        	object.buffer[i] = 0;
    	}
	}
	int n = object.buffer_size;
}


void finish(struct hash &object, uint8_t *result_hash, int fl) {

	if (object.buffer_size  || fl) {

		padding(object);

		for (int i  = 0; i < 64 / 2; i++) {
 			std::swap(object.buffer[i], object.buffer[64 - i - 1]);
		}
		cr_hash(object, 1);
		memcpy(object.buffer, object.n, 64);
		cr_hash(object, 2);

	}
	memcpy(object.buffer, object.sigma, 64);
	cr_hash(object, 2);
	memcpy(result_hash, object.cur_hash, 64);
};

int main(int argc, const char* argv[]){
	hash object; 										//init()
	size_t size = 0;
	uint8_t buf[512];
	uint8_t result_hash[64];
	std::string file_name = "";
	std::string file_name2 = "";
	drop_buf(buf);
	int indicate = 0;

	try {
		file_name = argv[1];
	} 
	catch (std::logic_error) {
		std::cerr << "Too few arguments" << std::endl;
		return 0;
	}
	try {
		file_name2 = argv[2];
	} 
	catch (std::logic_error) {
		std::cerr << "Too few arguments" << std::endl;
		return 0;
	}
	std::cout << "files_hash:" <<std::endl;
    std::cout << file_name.c_str() << ' ' << file_name2.c_str() << std::endl;

	FILE *file = freopen(file_name.c_str(), "r", stdin);
	if (!file) {
		std::cerr << "Failed to open in file " << file_name << std::endl;
		return 0;
	}
	
	while (size = (fread(buf, 1, sizeof buf, file))) {
		indicate = 1;
		object.update(buf, size);
	}
	int fl = 0; //для пустого текста - если пустой, то нужно делать finish, если это пустой конец сообщения, то не нужно
	if (!size) {
		printf("EMPTY INPUT\n");
		memset(buf, 0, sizeof buf);
		object.update(buf, size);
		fl = 1;
	}
	finish(object, result_hash, fl);
	std::cout << "FINAL" << std::endl;
	for(int i = 0; i < 64; ++i) {
		printf("%02x", result_hash[i]);

	} 
	std::cout << std::endl;
	FILE *file_out = freopen(file_name2.c_str(), "w", stdout);
	if (!file) {
		std::cerr << "Failed to open out file " << file_name2 << std::endl;
		return 0;
	}

	for(int i = 0; i < 64; ++i) {		
		fprintf(file_out, "%x", result_hash[i]);
	}

	fclose(file_out);
	fclose(file);
}