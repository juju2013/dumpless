#include <mhash.h>
#include <stdio.h>
#include <stdlib.h>

#include <tcutil.h>
#include <tchdb.h>
#include <stdbool.h>

#define MAXSIZE 2048

typedef struct {
    unsigned long long offset;
    unsigned long size;
    unsigned long long inuse;
} INUSE;

void usage(char* prog) {
	printf("usage: %s blockusage.tch_to_dump\n", prog);
	exit(1);
}

int main(int argc, char * argv[]) {
	if (argc < 2) usage(argv[0]);

  int i;
  MHASH td;
	unsigned char *hash;
	const unsigned char *blksize="LESSFS_BLOCKSIZE";
	unsigned char	kvalue[MAXSIZE+3];
	unsigned char	tmp[MAXSIZE+3];

	td=mhash_init(MHASH_TIGER192);
	if (td == MHASH_FAILED) {
		printf("unable to initialize TIGER192 hash\n");
		exit(1);
	}
	mhash(td, blksize, strlen(blksize));
	hash=mhash_end(td);
/*
	printf("Hash:");
	for (i=0; i<mhash_get_block_size(MHASH_TIGER192); i++) {
		printf("%.2x", hash[i]);
	}
*/
	INUSE* data;
	int retsize=searchKey(argv[1], hash, kvalue);
	if (24==retsize) {
		data = (INUSE*) kvalue;
		printf("%s\tOK:offset=%llu,size=%lu,inuse=%llu\n", argv[1], data->offset, data->size, data->inuse);
	} else {
		printf("%s\tKO:%u bytes readed, expect 24\n", argv[1], retsize);
		if (retsize) {
			memset(tmp, 0, sizeof(tmp));
			for (i=0; i<(retsize>64?64:retsize); i++) {
				sprintf(&tmp[i*2], "%.2x", kvalue[i]);
			}
			printf("\tdump:%s\n", tmp);
		}
	}
	exit(0);
}

int searchKey(char* fTch, char* key, char* kvalue) {
	TCHDB* hdb;
	unsigned int ecode, size;
	char* value;

	ecode = 0; size=0;
	hdb=tchdbnew();
	if (!tchdbopen(hdb, fTch, HDBOREADER)) {
		ecode=tchdbecode(hdb);
		printf("open error: %s\n",tchdberrmsg(ecode));
		goto fin;
	}

	value=tchdbget(hdb, key, 24, &size);
	if (value) {
		memcpy(kvalue, value, 24>size?size:24);
		free(value);
	} else {
		ecode=tchdbecode(hdb);
		printf("get error: %s\n",tchdberrmsg(ecode));
	}

	tchdbclose(hdb);

	fin:
	return size;
}

