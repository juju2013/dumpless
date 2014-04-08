#! /bin/sh

gcc dumpless.c \
	-static -L/usr/lib -pthread -lmhash -ltokyocabinet -lz -lbz2 -lm \
	-L/usr/lib/x86_64-linux-gnu \
	-o dumpless
