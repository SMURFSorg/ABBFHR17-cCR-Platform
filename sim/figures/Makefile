CSV:=$(wildcard *.csv)
DAT:=$(CSV:.csv=.dat)

GNPs:=$(wildcard *.gnp)
PDFs:=$(GNPs:.gnp=.pdf)

all: $(PDFs)

%.pdf: %.gnp $(DAT)
	gnuplot $<

$(DAT): $(CSV) csv2dat.py
	python csv2dat.py

clean:
	rm -f $(PDFs) $(DAT)

	
