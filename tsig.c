// tsig.c ... functions on Tuple Signatures (tsig's)
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include <unistd.h>
#include <string.h>
#include "defs.h"
#include "tsig.h"
#include "reln.h"
#include "hash.h"
#include "bits.h"

// make a tuple signature
Bits codeword(char *attr_value, int m, int k)
{
	int  nbits = 0;   // count of set bits
   	Bits cword = newBits(m);   // assuming m <= 32 bits
   	if ( !strcmp(attr_value, "?") ){
   		//printf("%s\n", attr_value);
   		return cword;
   	}
   	srandom(hash_any(attr_value, strlen(attr_value)));
   	while (nbits < k) {
   		//printf("nbits: %d k: %d\n", nbits, k);
   		int i = random() % m;
   		//printf("i: %d m: %d\n", i, m);
   		if (!bitIsSet(cword, i)){
   			setBit(cword, i);
   			//printf("%hhu\n", cword->bitstring[1]);
	      	nbits++;
	      }
	  }
	  // printf("cw:"); showBits(cword);printf("\n");
	  return cword;  // m-bits with k 1-bits and m-k 0-bits
}

Bits makeTupleSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);

	Bits cw = newBits(tsigBits(r));
	Bits tsig = newBits(tsigBits(r));
	unsetAllBits(tsig);
	unsetAllBits(cw);
	char** tv = tupleVals(r, t);
	for (int i = 0; i < nAttrs(r); i++){
		cw = codeword(tv[i], tsigBits(r), codeBits(r));
		orBits(tsig, cw);
	}
	//freeBits(cw);
	//printf("tisg:"); showBits(tsig);printf("\n");
	return tsig;
}

// find "matching" pages using tuple signatures

void findPagesUsingTupSigs(Query q)
{
	assert(q != NULL);
	//TODO
	PageID pid;
	Bits querySig = makeTupleSig(q->rel, q->qstring);
	//printf("%s\n", querySig->bitstring[0]);
	
	unsetAllBits(q->pages); 
	Bits tsig = newBits(tsigBits(q->rel));
	//printf("nTsigPages(q->rel): %s\n", nTsigPages(q->rel));
	for (int i = 0; i < nTsigPages(q->rel); i++){
		Page queryPage = getPage(tsigFile(q->rel), i);
		//printf("tsigFile: %u\n", tsigFile(q->rel));
		q->nsigpages++;
		for (int j = 0; j < pageNitems(queryPage); j++){
			getBits(queryPage, j, tsig);
			//printf("Here\n");
			//printf("querySig: %hhu tsig:%hhu\n", querySig->bitstring[0], tsig->bitstring[0]);
			if ( isSubset(querySig, tsig) ){
				//printf("Here\n");
				pid = q->nsigs / maxTupsPP(q->rel);
				//printf("q->pages:%hhu pid:%d\n", q->pages->bitstring[0], pid);
				setBit(q->pages, pid);
				//printf("q->pages:%hhu\n", q->pages->bitstring[0]);
			}
			unsetAllBits(tsig);
			q->nsigs++;
		}

	}
	// The printf below is primarily for debugging
	// Remove it before submitting this function
	printf("Matched Pages:"); showBits(q->pages); putchar('\n');
}

