#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <immintrin.h>
#include <array>
#include <ctime>
#include <cmath>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


const int genMatrix[11][16] = {
	{1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1},
	{0,0,1,1, 0,0,1,1, 0,0,1,1, 0,0,1,1},
	{0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1},
	{0,0,0,1, 0,0,0,1, 0,0,0,1, 0,0,0,1},

	{0,0,0,0, 1,1,1,1, 0,0,0,0, 1,1,1,1},
	{0,0,0,0, 0,0,1,1, 0,0,0,0, 0,0,1,1},
	{0,0,0,0, 0,1,0,1, 0,0,0,0, 0,1,0,1},

	{0,0,0,0, 0,0,0,0, 1,1,1,1, 1,1,1,1},
	{0,0,0,0, 0,0,0,0, 0,0,1,1, 0,0,1,1},
	{0,0,0,0, 0,0,0,0, 0,1,0,1, 0,1,0,1},
	{0,0,0,0, 0,0,0,0, 0,0,0,0, 1,1,1,1}
};

__m128i AESRand_init(){
	uint8_t a1 = rand();
	uint8_t a2 = rand();
	uint8_t a3 = rand();
	return _mm_set_epi8(a1, 0x2b, 0x2b, 0x15, 0x1f, 0xfd, a3, 0x13, 
		0x11, 0x0D, 0x02, a2, 0x05, 0x01, 0x0, 0x0);
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

void print128_num(__m128i var) {
    uint16_t *val = (uint16_t*) &var;
    printf("Numerical: %i %i %i %i %i %i %i %i \n", 
           val[0], val[1], val[2], val[3], val[4], val[5], 
           val[6], val[7]);
}

uint16_t RNG_AES(int iter){
	//std::cout << "Running 5-billion iterations (160 Billion-bytes of Random Data)" << std::endl;
	
	//__m128i state = AESRand_init();
	__m128i state = AESRand_init();
	uint16_t *val;

	for(long long i=0; i<iter; i++){
		AESRand_increment(state); 
		auto rands = AESRand_rand(state); 
		//print128_num(rands[0]);
		val = (uint16_t*) &rands[0];
	}
	
	return (val[rand()%8]);
}

void RNG_AES_long(int iter, uint16_t *result){
	//std::cout << "Running 5-billion iterations (160 Billion-bytes of Random Data)" << std::endl;
	
	//__m128i state = AESRand_init();
	__m128i state = AESRand_init();
	uint16_t *val;

	for(long long i=0; i<iter; i++){
		AESRand_increment(state); 
		auto rands = AESRand_rand(state); 
		//print128_num(rands[0]);
		val = (uint16_t*) &rands[0];
		uint16_t *var = (uint16_t*) &val;
		for(int j = 8*i; j < 8*i+8; j++) {
			result[j] = var[j%8];
			std::cout << result[j]<<std::endl;
		}
	}
	
	return;
}

unsigned bcr(int n,int k) {
	if (k>n/2) k=n-k;
	if (k==1)  return n;
	if (k==0)  return 1;
	return bcr(n-1,k)+bcr(n-1,k-1);
}

int find_el(int *per, int x, int p) {
	for(int i = 0; i < p; ++i){
		if (per[i] == x) return 1;
	}
	return 0;
}

int create_rand_set(int *permute, int p, int n) {
	
	for(int i = 0; i < p; ++i) {
		int temp = -1;
		int count = 0;
		while (find_el(permute, temp, p)) {
			temp = RNG_AES(rand()%3)%(n/4);
			//std::cout << temp;
			count++;
			if (count > 10) {
				std::cerr << "Error: Try again" << std::endl;
				return 0;
			}
		}
		permute[i] = temp;
 	}
 	return 1;
}

int set_permutation(int *permute1, int *permute2, int p, int n) {
	int i, stime;
	long ltime;
	int count = 1;

	while (!create_rand_set(permute1, p, n)) {
		ltime = time (NULL);
		std::cout << ltime << std::endl;
		stime = (unsigned int) ltime%1000 + rand()*100%count;
		srand(stime);
		for(int i = 0; i < p; ++i){
			permute1[i] = -1;
		}
		++count;
		if (count>10000) {
			std::cerr << "Error: try again (set_permutation1)" << std::endl;
		}
	};
	count = 1;
	while (!create_rand_set(permute2, p, n)) {
		ltime = time (NULL);
		stime = (unsigned int) ltime%1000 + rand()*100%count;
		srand(stime);
		for(int i = 0; i < p; ++i){
			permute2[i] = -1;
		}
		count++;
		if (count>10000) {
			std::cerr << "Error: try again (set_permutation2)" << std::endl;
		}
	};

	for(int i = p-1; i >= 0; --i) {
		int j = RNG_AES(1)%p;
		int temp = permute1[i];
		permute1[i] = permute1[j];
		permute1[j] = temp;
	}

	for(int i = p-1; i >= 0; --i) {
		int j = RNG_AES(1)%p;
		int temp = permute2[i];
		permute2[i] = permute2[j];
		permute2[j] = temp;
	}

	std::cout << "permutation1:" <<std::endl;
	for(int i = 0; i < p; ++i) {
		std::cout << permute1[i];
	}
	std::cout << std::endl;

	std::cout << "permutation2:" <<std::endl;
	for(int i = 0; i < p; ++i) {
		std::cout << permute2[i];
	}
	std::cout << std::endl;


}

void swap_col(int matrix[11][16],int k, int t) { 
	int i = 0;
	double tmp;
	for (i = 0; i < 11 ; ++i) {
		tmp = matrix[i][k];
		matrix[i][k] = matrix[i][t];
		matrix[i][t] = tmp;
	}
}

void swap_row(int matrix[11][16],int k, int t) { 
	int i = 0;
	double tmp;
	for (i = 0; i < 16 ; ++i) {
		tmp = matrix[k][i];
		matrix[k][i] = matrix[t][i];
		matrix[t][i] = tmp;
	}
}
void swap_row_less(int **matrix, int k, int t) { 
	int i = 0;
	double tmp;
	for (i = 0; i < 16 ; ++i) {
		tmp = matrix[k][i];
		matrix[k][i] = matrix[t][i];
		matrix[t][i] = tmp;
	}
}

void gauss(int a[11][16], int n, int q, int E[16]) {
	int i = 0;
	int j = 0;
	int k, t, max;
	double mask;
	double EPS = 0.00001;
		
	//straight_run
	for (k = 0; k < n; ++k) {//столбцы
		
		for(t = k; (t < n) && (abs(a[t][k]) < EPS); ++t);

		if (t < n) {
			swap_row(a,k,t);
		} else {
			for(t = k; (t < q) && (abs(a[k][t]) < EPS); ++t);
			if (t < q) {
				swap_col(a, k, t);
				int temp = E[k];
				E[k] = E[t];
				E[t] = temp;
			} 
		}

		for(int i = k + 1; i < n; ++i) {
			if (a[i][k]) {
				for(int j = k; j < q; ++j) {
					a[i][j] = (a[i][j] - a[k][j]) & 1;
				}
			}
		}
	}

	for(int k = n - 1; k > 0; --k) {
		for(int i = k - 1; i >= 0; --i) {
			if (a[i][k]) {
				for (int j = k; j < q; ++j) {
					a[i][j] = (a[i][j] - a[k][j]) & 1;
				}
			}
		}
	}
	return;
}

void set_S(int **S, int n, int &flag) {
	int k, i,t, EPS = 0.00001;
	for(int i = 0; i < n; ++i) {
		for(int j = 0; j < 5; ++j) {
			if (i == j) S[i][j] = 1;
			else S[i][j] = RNG_AES(rand()%3)%2;
		}
	}
	for(int i = 0; i < n; ++i) {
		for(int j = 0; j < 5; ++j) {
			std::cout << S[i][j];
		}
		std::cout << std::endl;
	}
	std::cout <<  std::endl;

	flag = 0;

	//straight_run
	for (k = 0; k < n; ++k) {//столбцы
		for(t = k; (t < n) && (abs(S[t][k]) < EPS); ++t);
		if (t < n) {
			swap_row_less(S,k,t);
		} else {
			std::cerr <<"Error: matrix is singular, try again"<< std::endl;
			flag = 1;
			return;
		}
		for(int i = k + 1; i < n; ++i) {
			if (S[i][k]) {
				for(int j = k; j < n; ++j) {
					S[i][j] = (S[i][j] - S[k][j]) & 1;
				}
			}
		}
	}
	return;
}

void get_back_matr(int **S, int **S_tr, int n) {
	for(int i = 0; i < n; ++i) {
		for(int j = 0; j < n; ++j) {
			S_tr[i][j] = S[j][i];
		}
	}
	return;
}

void multiply_matr(int **S, std::vector<int>syndrom, std::vector<int> &syndrom_new, int k, int n) {

	for(int i = 0; i < k; ++i) {
		int temp = 0;
		for(int j = 0; j < n; ++j) {
			temp = (temp + S[i][j]*syndrom[j]) % 2;
		}
		syndrom_new[i] = temp;
	}
	return;

}

void depermute(std::vector<int> &y, int *sig, int n, int front, int rear) {
	int *sig_back = new int[n/4];
	for(int i = 0; i < n/4; ++i) {
		sig_back[sig[i]] = i;
	}
	std::cout << "sig" << std::endl;
	for(int i = 0; i < n/4; ++i) {
		std::cout << sig[i];
	}
	std::cout << std::endl;

	std::cout << "sig_back" << std::endl;
	for(int i = 0; i < n/4; ++i) {
		std::cout << sig_back[i];
	}
	std::cout << std::endl;

	for(int i = front; i < rear; ++i) {
		y[i] = y[sig_back[i]];
	}
	std::cout << "DEP y" << std::endl;
	for(int i = 0; i < n/4; ++i) {
		std::cout << y[i];
	}
	std::cout << std::endl;

}

void rec_decode(std::vector<int> y, int r, int m, int front, int rear, int *sig1, int *sig2, int n) {
	if (r == 0) min_dist_decode(y, 0, m);
	else if (r == m) min_dist_decode(y, r, r);
	else {
		if ((front == 0) and (rear == n/4)) {
			depermute(y, sig1, n, front, rear);
		}
		if ((front == 3*n/4) and (rear == n)) {
			depermute(y, sig2, n, front, rear);
		}
	}

	int mid = (front + rear)/2;
	std::vector<int> y_uv;
	for(int i = mid; i < rear; ++i) {
		y_uv.push_bavk(y[i]);
	}
	int j = front;
	for(int i = mid; i < rear; ++i) {
		y[i] = y[i]*[j];
		++j;
	}
	rec_decode(y, r-1, m-1, mid, rear);
	j = mid;
	for(int i = front; i <mid; ++i) {
		y[i] = (y[i] + y[j]*y_uv[i]);
		++j;
	}
	rec_decode(y, r, m-1, mid, rear);
	j = front;
	for(int i = mid; i <rear; ++i) {
		y[i] = (y[i]*y[j]);
		++j;
	}

	if ((front == 0) and (rear == n/4)) {
		permute(y, sig1, n, front, rear);
	}
	if ((front == 3*n/4) and (rear == n/4)) {
		permute(y, sig2, n, front, rear);
	}
	return;
}

void decode(std::vector<int> s, int *sig1,int *sig2, int n) {
	std::vector<int> y[5];
	for(int i = 0; i < 5; ++i) {
		y[i] = 2*s[i]-1;
	}
	r = 2;
	m = 4;
	rec_decode(y, r, m, 0, 5, sig1, sig2, n);
}

void signing(int **S, int H[5][16], int **Q, int *sig1, int*sig2, int n,
			 std::string file_name="", std::string file_name_out="") {
	uint16_t i[16];
	//std::string file_name = "";

	RNG_AES_long(rand()%4, i);
	for(int j = 0; j < 16; ++j) {
		std::cout << i[j] << ' ';
	}
	std::cout << std::endl;

	pid_t pid;
  	int rv;
 	switch(pid=fork()) {
 		case -1:
			perror("fork"); /* произошла ошибка */
        	exit(1); /*выход из родительского процесса*/
 		case 0:
 			printf(" CHILD: Это процесс-потомок!\n");
          	printf(" CHILD: Мой PID -- %d\n", getpid());
          	printf(" CHILD: PID моего родителя -- %d\n", getppid());
          	printf("CHILD: Введите мой код возврата (как можно меньше):");

          	scanf("%d", &rv);
          	std::cout << "files:" <<std::endl;
          	std::cout << file_name << ' ' << file_name_out << std::endl;
          	std::cout << execl("hash", "hash", file_name.c_str(), file_name_out.c_str(), NULL);
			perror("EXEC:");

          	printf(" CHILD: Выход!\n");
          	exit(rv);
  		default:
          	printf("PARENT: Это процесс-родитель!\n");
          	printf("PARENT: Мой PID -- %d\n", getpid());
          	printf("PARENT: PID моего потомка %d\n",pid);
          	printf("PARENT: Я жду, пока потомок не вызовет exit()...\n");
          	//int status;
          	wait(&rv);
          	printf("PARENT: Код возврата потомка:%d\n",
            WEXITSTATUS(rv));
          	printf("PARENT: Выход!\n");
 	}
 	std::cout << "NEW";
 	FILE *file;
 	switch(pid=fork()) {
 		case -1:
			perror("fork"); /* произошла ошибка */
        	exit(1); /*выход из родительского процесса*/
 		case 0:
 			printf("CHILD: Введите мой код возврата (как можно меньше):");
          	scanf("%d", &rv);
          	file = freopen(file_name_out.c_str(), "a+", stdout);//OUT.txt
			if (!file) {
				std::cerr << "Failed to open file here " << file_name_out << std::endl;//OUT.txt
				return;
			}
			for(int j = 0; j < 16; ++j) {
				fprintf(file, "%04x", i[j]);
			}
          	printf(" CHILD: Выход!\n");
          	exit(rv);
  		default:
          	wait(&rv);
          	printf("PARENT: Выход!\n");
 	}
	//freopen(stdout, "a+", stdout);
	/*FILE *file_hmi = freopen(file_name_out.c_str(), "r", stdin);//OUT.txt
	if (!file) {
		std::cerr << "Failed to open file for syndrom" << file_hmi << std::endl;
		return;
	}
	FILE *file_syn = freopen("SYN.txt", "w", stdout);
	if (!file) {
		std::cerr << "Failed to open file for syndrom" << "SYN.txt" << std::endl;
		return;
	}*/
	//stdout = save;
	std::cout << "READY" << std::endl;
	std::cout << "READY" << std::endl;
	std::cout << "READY" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

	switch(pid=fork()) {
 		case -1:
			perror("fork");
        	exit(1); 
 		case 0:
 			printf(" CHILD: Это процесс-потомок!\n");
          	printf(" CHILD: Мой PID -- %d\n", getpid());
          	printf(" CHILD: PID моего родителя -- %d\n",
            getppid());
          	printf("CHILD: Введите мой код возврата (как можно меньше):");

          	scanf("%d", &rv);
          	std::cout << "files:" <<std::endl;
          	std::cout << file_name_out << ' ' << "SYN.txt" << std::endl;
          	std::cout << execl("hash", "hash", file_name_out.c_str(), "SYN.txt", NULL);
			perror("EXEC:");

          	printf(" CHILD: Выход!\n");
          	exit(rv);
  		default:
          	printf("PARENT: Это процесс-родитель!\n");
          	printf("PARENT: Мой PID -- %d\n", getpid());
          	printf("PARENT: PID моего потомка %d\n",pid);
          	printf("PARENT: Я жду, пока потомок не вызовет exit()...\n");
          	//int status;
          	wait(&rv);
          	printf("PARENT: Код возврата потомка:%d\n",
            WEXITSTATUS(rv));
          	printf("PARENT: Выход!\n");
 	}

 	char syn;
 	uint8_t syn_int;

 	std::ifstream in("SYN.txt");

 	if (in.is_open()) {
 		in >> syn;
 		if (('0' <= syn) && (syn <= '9'))
 			syn_int=syn-'0';
 		if (('a' <= syn) && (syn <= 'f'))
 			syn_int=10+syn-'a';
 	} 

 	in.close();

   // std::cout << "SYN VALUE: " << int(syn_int) << '/' << syn_int << std::endl;

    std::vector<int> syndrom(5);
    std::vector<int> syndrom_s(5);

    for(int j = 0;  j < 5; ++j) {
    	syndrom[j]= syn_int & 1;
    	syn_int >>= 1;
    }

    std::reverse(std::begin(syndrom), std::end(syndrom));

    std::cout << "syndrom" << std::endl;
 	for(int j = 0;  j < 5; ++j) {
    	std::cout << syndrom[j];
    }
    std::cout << std::endl;

	for(int i = 0; i < 5; ++i) {
		for(int j = 0; j < 5; ++j) {
			std::cout << S[i][j];
		}
		std::cout << std::endl;
	}

	int **S_tr = new int*[5];
	for (int count = 0; count < 5; count++)
        S_tr[count] = new int [5]; 

    get_back_matr(S, S_tr, 5);

    std::cout << "S*" <<std::endl;

    for(int i = 0; i < 5; ++i) {
		for(int j = 0; j < 5; ++j) {
			std::cout << S_tr[i][j];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	std::cout << "syndrom" << std::endl;
 	for(int j = 0;  j < 5; ++j) {
    	std::cout << syndrom[j];
    }
    std::cout << std::endl;
    std::cout << std::endl;

    multiply_matr(S_tr, syndrom, syndrom_s, 5, 5);

    std::cout << "s`" <<std::endl;
    for(int j = 0; j < 5; ++j) {
		std::cout << syndrom_s[j];
	}

	decode(syndrom_s, sig1, sig2, n);
}

void set_per_matrix(int **Q, int *permute, int n) {
	
	for(int i = 0; i < n; ++i){
		permute[i] = -1;
	}

	int i, stime;
	long ltime;
	int count = 1;

	for(int i = 0; i < n; ++i) {
		int temp = -1;
		int count = 0;
		while (find_el(permute, temp, n)) {
			temp = rand()%n;
			count++;
			if (count > 1000) {
				std::cerr << "Error: Try again with Q" << std::endl;
				return ;
			}
		}
		permute[i] = temp;
 	}
	

	for(int i = n-1; i >= 0; --i) {
		int j = RNG_AES(1)%n;
		int temp = permute[i];
		permute[i] = permute[j];
		permute[j] = temp;
	}


	for(int i = 0; i < n; ++i) {
		for(int j = 0; j < n; ++j) {
			Q[i][j] = 0;
		}
	}

	for(int i = 0; i < n; ++i) {
		Q[permute[i]][i] = 1;
	}

	return;
}

int main(int argc, const char* argv[]){
	int r, m , p, w, n, k, rnum;
	int E[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	
	r = 2;
	m = 4;
	k = 0; //11
	n = 0; //16

	n = pow(2, m);
	for(int i = 0; i < r+1; ++i) {
		k += bcr(m, i);
	}

	int i, stime;
	long ltime;

	std::cout << "Enter a random numer " << std::endl;
	std::cin >> rnum;
	srand(rnum);
	int iter = rand()%4+1;
	//uint16_t ran_num = RNG_AES(iter);
	
	p = 0;
	int count = 0; 
	while (p<=1) {
		p = RNG_AES(iter)%(n/4+1);
		std::cout << "HEE";
		count++;
		if (count > 10) {
			ltime = time (NULL);
			stime = (unsigned int) ltime%1000;
			srand(stime);
			std::cerr << "ERROR: try again with p" << std::endl;
			count = 0;
			iter = rand()%5;
		}
	}
	std::cout << "iter=" <<iter << ' '<< p << std::endl;
	//return 0;

	int *permute1 = new int[p];
	int *permute2 = new int[p];

	for(int i = 0; i < p; ++i){
		permute1[i] = -1;
		permute2[i] = -1;
	}

	
	set_permutation(permute1, permute2, p, n);

	int *sig1 = new int[n/4];
	int *sig2 = new int[n/4];

	int j = 0;
	for(int i = 0; i < n/4; ++i) {
		if (find_el(permute1, i, p)) {
			sig1[i] = permute1[j];
			++j;
		} else {
			sig1[i] = i;
		}
	}
	j = 0;
	for(int i = 0; i < n/4; ++i) {
		if (find_el(permute2, i, p)) {
			sig2[i] = permute2[j];
			++j;
		} else {
			sig2[i] = i;
		}
	}

	for(int i = 0; i < n/4; ++i) {
		std::cout << sig1[i];
	}
	std::cout <<  std::endl;
	for(int i = 0; i < n/4; ++i) {
		std::cout << sig2[i];
	}
	std::cout <<  std::endl;

	int perMatrix[11][16] = {0};

	for(int step = 0; step < n; step += n/4) {
		for(int j = step; j < step+n/4; ++j) {
			for(int i = 0; i < 4; ++i) {
				perMatrix[i][j] = genMatrix[i][sig1[j%(n/4)]];
			}
		}
	}

	for(int i = 4; i < 10; ++i) {
		for(int j = 0; j < n; ++j) {
			perMatrix[i][j] = genMatrix[i][j];
		}
	}

	for(int i = 10; i < 11; ++i) {
		for(int j = 0; j < 3*n/4; ++j) {
			perMatrix[i][j] = genMatrix[i][j];
		}
		for(int j = 3*n/4; j < n; ++j) {
			perMatrix[i][j] = genMatrix[i][sig2[j%(n/4)]+3*n/4];
		}
	}


	for(int i = 0; i < k; ++i) {
		for(int j = 0; j < n; ++j) {
			std::cout << perMatrix[i][j];
			if (!((j+1)%4)) std::cout << ' ';
		}
		std::cout << std::endl;
		if (!((i+1)%4)) std::cout << std::endl;
	}
	std::cout << std::endl;

	
	gauss(perMatrix, 11, 16, E);

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

	for(int i = 0; i < k; ++i) {
		for(int j = 0; j < n; ++j) {
			std::cout << perMatrix[i][j];
			if (!((j+1)%4)) std::cout << ' ';
		}
		std::cout << std::endl;
		if (!((i+1)%4)) std::cout << std::endl;
	}
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

	int checkMatrix[5][16];

	for(int i = 0; i < 5; ++i) {
		for(int j = 0; j < 11; ++j) {
			checkMatrix[i][j] = perMatrix[j][i+11];
		}
		for(int j = 11; j < 16; ++j) {
			if (i == j-11) checkMatrix[i][j] = 1;
			else checkMatrix[i][j] =0;
		}
	}

	for(int j = 0; j < 5; ++j) {
		for (int i = 0; i < 16; ++i) {
			std::cout << checkMatrix[j][i];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	int **S = new int*[5];
	for (int count = 0; count < 5; count++)
        S[count] = new int [5]; 

    int **Q = new int*[16];
	for (int count = 0; count < 16; count++)
        Q[count] = new int [16]; 

    int *permute = new int[16];

	int flag = 1;
	while(flag)	set_S(S, 5, flag);

	std::cout << std::endl;
	std::cout << "S:" << std::endl;
	for(int j = 0; j < 5; ++j) {
		for (int i = 0; i < 5; ++i) {
			std::cout << S[j][i];
		}
		std::cout << std::endl;
	}

	set_per_matrix(Q, permute, 16);

	std::cout << "permutation:" <<std::endl;
	for(int i = 0; i < n; ++i) {
		std::cout << permute[i] << ' ';
	}
	std::cout << std::endl;

	signing(S, checkMatrix, Q, sig1, sig2, n, argv[1], argv[2]);
};