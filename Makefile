VERSION := -std=c89
CC := gcc
FILES := main.c
FILE_OUT := main
all: $(FILES)
	$(CC) -g $(VERSION) $(FILES) -O0 -o  $(FILE_OUT)
clear: $(FILE_OUT)
	rm $(FILE_OUT)
