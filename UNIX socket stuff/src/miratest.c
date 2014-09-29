
#include <stdio.h>
#include "includes/miracl.h"
#include <time.h>
#include <malloc.h>
int main()
{

    miracl *mip;
	#ifndef MR_NOFULLWIDTH   
    mip=mirsys(50,0);
	#else
    mip=mirsys(50,MAXBASE);
	#endif
	time_t seed;

	big prime, primeTwo, Alice, Bob, primeAlice, primeBob, key, Alicekey, Bobkey;
	
	
	prime = mirvar(0);
	primeTwo = mirvar(0);
	Alice = mirvar(0);
	Bob = mirvar(0);
	primeAlice = mirvar(0);
	primeBob = mirvar(0);
	Bobkey = mirvar(0);
	Alicekey = mirvar(0);
	
	time(&seed);
	irand((unsigned long)seed);
	
	bigbits(512, prime);
	nxprime(prime, prime);
	//bigbits(512,  primeTwo);
	nxprime(prime, primeTwo);


	
	bigbits(160, Alice);//Alice's Offline Calc
	powmod(primeTwo, Alice, prime, primeAlice);

	bigbits(160, Bob); //Bobs Offline Calc
	powmod(primeTwo, Bob, prime, primeBob);


	powmod(primeBob, Alice, prime, Alicekey); //Alice calculates key
	powmod(primeAlice, Bob, prime, Bobkey);
	
	//mip->IOBASE=2;
	printf("Bob's Key = \0");
	cotnum(Bobkey, stdout);
	printf("\nAlice's Key = \0");
	cotnum(Alicekey, stdout);
	printf("\n");
	
	char ptr[100];


	big returnd;
	returnd = mirvar(0);
	int len =big_to_bytes(100, Alicekey, ptr, 1);
	bytes_to_big(len, ptr, returnd);
	printf("\nReturned = %d\n", len);
	printf("\nRebuilt = ");
	cotnum(returnd, stdout);

	printf("\n");

	printf("Bob's key is size: %zu\nAlice's key is size: %zu\n", sizeof(Bobkey), sizeof(Alicekey));
	
	return 0;
}
