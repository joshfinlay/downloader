AM_CFLAGS = --pedantic -Wall -std=c99 -O2 ${libcurl_CFLAGS} -g
AM_LDFLAGS =
AM_LIBS = -lcurl -lreadline -lncurses -ltermcap -lm

bin_PROGRAMS = downloader
downloader_SOURCES = downloader.c url_parser.c progressbar.c
downloader_LDADD = ${libcurl_LIBS} -lreadline -lncurses -lm
