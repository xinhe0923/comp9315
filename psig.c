// psig.c ... functions on page signatures (psig's)
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include <unistd.h>
#include <string.h>
#include "defs.h"
#include "reln.h"
#include "query.h"
#include "psig.h"
#include "hash.h"

Bits codeword_psig(char *attr_value, int m, int k)
{
	int  nbits = 0;   // count of set bits
   	Bits cword = newBits(m);   // assuming m <= 32 bits
   	if ( !strcmp(attr_value, "?") ){
   		return cword;
   	}
   	srandom(hash_any(attr_value, strlen(attr_value)));
   	while (nbits < k) {
   		 int i = random() % m;
   		 if ( !bitIsSet(cword, i) ) {
   		 	setBit(cword, i);
   		 	nbits++; 
   		 }
   		}
   		return cword;  // m-bits with k 1-bits and m-k 0-bits
}

Bits makePageSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);
	
	Bits psig = newBits(psigBits(r));
	Bits cw = newBits(psigBits(r));
	unsetAllBits(psig);
	unsetAllBits(cw);

	char** tv = tupleVals(r, t);
	for (int i = 0; i < nAttrs(r); i++){
		cw = codeword_psig(tv[i], psigBits(r), codeBits(r));
		orBits(psig, cw);
	}
	//printf("psig in makePageSig:");showBits(psig); printf("\n");
	return psig;
}


void findPagesUsingPageSigs(Query q)
{
	assert(q != NULL);
	//TODO
	PageID pid;
	Bits querySig = makePageSig(q->rel, q->qstring);
	//printf("querySig: ");showBits(querySig);printf("\n");
	unsetAllBits(q->pages); 
	Bits psig = newBits(psigBits(q->rel));

	for (int i = 0; i < nPsigPages(q->rel); i++){
		Page queryPage = getPage(psigFile(q->rel), i);
		q->nsigpages++;
		for (int j = 0; j < pageNitems(queryPage); j++){
			getBits(queryPage, j, psig);
			//printf("psig: ");showBits(psig);printf("\n");
			if (isSubset(querySig, psig)){
				//printf("YES!!!!\n");
				pid = q->nsigs;
				//printf("pid: %d\n", pid);
				setBit(q->pages, pid);
			}
			unsetAllBits(psig);
			q->nsigs++;
		}

	}
    printf("Matched Pages:"); showBits(q->pages); putchar('\n');
}

