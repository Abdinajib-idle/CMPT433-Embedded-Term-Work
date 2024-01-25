OUTFILE = helloEmbedded
OUTDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Werror -Wall -g -std=c11 -D_POSIX_SOURCE=200809L

all: nestedDir app done

help:
	@echo "Build Hello Embedded World for BeagleBone!"
	@echo "Targets include all, app, and clean."
nestedDir:
	mkdir -p $(OUTDIR)
app:
	$(CC_C) $(CFLAGS) hello.c -o $(OUTDIR)/$(OUTFILE)
done:
	@echo "Yay! you just finished your just said hello to Embedded World!"
clean:
	rm -f $(OUTDIR)/$(OUTFILE)