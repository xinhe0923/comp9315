// bsig.c ... functions on Tuple Signatures (bsig's)
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "bsig.h"
#include "psig.h"

void findPagesUsingBitSlices(Query q)
{
	assert(q != NULL);
	//TODO
// 	Qsig = makePageSig(Query)
// Pages = AllOneBits
// for each i in 0..pm-1 {
//     if (Psig bit[i] is 1) {
//         Slice = get i'th bit slice from bsigFile
//         zero bits in Pages which are zero in Slice
//     }
// }
	PageID bsigPid = -1;
	PageID pbsigPid = -1;
	Bits qsig = makePageSig(q->rel, q->qstring);
    //printf("qisg is:");showBits(qsig);printf("\n");
	setAllBits(q->pages);
	for (int i = 0; i < psigBits(q->rel); ++i)
	{
		// bsigPid = i / maxBsigsPP(q->rel);
		// Page p = getPage(bsigFile(q->rel), bsigPid);
		// if (bsigPid == 0){
		// 	q->nsigpages = 1;
		// 	printf("bsigPid:%d\n", bsigPid);
		// }else if (pbsigPid != bsigPid){
		// 	q->nsigpages++;
		// 	printf("bsigPid:%d\n", bsigPid);
		// }
		Bits slice = newBits(bsigBits(q->rel));
		if ( bitIsSet(qsig, i) )
		{
			bsigPid = i / maxBsigsPP(q->rel);
			//printf("bsigPid:%d\n", bsigPid);
			Page p = getPage(bsigFile(q->rel), bsigPid);
			if ( bsigPid != pbsigPid){
				q->nsigpages++;
				//printf("bsigPid:%d\n", bsigPid);
			}
			pbsigPid = bsigPid;
			getBits(p, i % maxBsigsPP(q->rel), slice);
			q->nsigs++;
			for (int j = 0; j < nPages(q->rel); ++j)
			{
				if( !bitIsSet(slice, j)){
					unsetBit(q->pages, j);
				}
			}
			
		}
		//pbsigPid = bsigPid;
	}

	//setAllBits(q->pages); // remove this
}

