CC:=gcc
CFLAGS:=-I.
OUTDIR:=$(CURDIR)/out

main:
	mkdir -p $(OUTDIR)
	cd src && $(MAKE) OUTDIR:=$(OUTDIR)

clean:
	rm -rf $(OUTDIR)
