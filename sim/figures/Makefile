CSV:=$(wildcard *.csv)
DAT:=$(CSV:.csv=.dat)

GNPs:=$(wildcard *.gnp)
PDFs:=$(GNPs:.gnp=.pdf)\
	synthetic-01hMTBF-waste-celio.pdf \
	synthetic-02hMTBF-waste-celio.pdf \
	synthetic-04hMTBF-waste-celio.pdf \
	synthetic-08hMTBF-waste-celio.pdf \
	synthetic-16hMTBF-waste-celio.pdf \
	synthetic-24hMTBF-waste-celio.pdf \
	synthetic-040gbs-waste-celio.pdf \
	synthetic-050gbs-waste-celio.pdf \
	synthetic-060gbs-waste-celio.pdf \
	synthetic-070gbs-waste-celio.pdf \
        synthetic-080gbs-waste-celio.pdf \
        synthetic-090gbs-waste-celio.pdf \
        synthetic-100gbs-waste-celio.pdf \
        synthetic-110gbs-waste-celio.pdf \
        synthetic-120gbs-waste-celio.pdf \
        synthetic-130gbs-waste-celio.pdf \
        synthetic-140gbs-waste-celio.pdf \
        synthetic-150gbs-waste-celio.pdf \
        synthetic-160gbs-waste-celio.pdf

all: $(PDFs)

synthetic-01hMTBF-waste-celio.pdf: synthetic-MTBFhMTBF-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "MTBF=1" synthetic-MTBFhMTBF-waste-celio.egnp
synthetic-02hMTBF-waste-celio.pdf: synthetic-MTBFhMTBF-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "MTBF=2" synthetic-MTBFhMTBF-waste-celio.egnp
synthetic-04hMTBF-waste-celio.pdf: synthetic-MTBFhMTBF-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "MTBF=4" synthetic-MTBFhMTBF-waste-celio.egnp
synthetic-08hMTBF-waste-celio.pdf: synthetic-MTBFhMTBF-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "MTBF=8" synthetic-MTBFhMTBF-waste-celio.egnp
synthetic-16hMTBF-waste-celio.pdf: synthetic-MTBFhMTBF-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "MTBF=16" synthetic-MTBFhMTBF-waste-celio.egnp
synthetic-24hMTBF-waste-celio.pdf: synthetic-MTBFhMTBF-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "MTBF=24" synthetic-MTBFhMTBF-waste-celio.egnp

synthetic-040gbs-waste-celio.pdf: synthetic-BWgbs-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "BW=40" synthetic-BWgbs-waste-celio.egnp
synthetic-050gbs-waste-celio.pdf: synthetic-BWgbs-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "BW=50" synthetic-BWgbs-waste-celio.egnp
synthetic-060gbs-waste-celio.pdf: synthetic-BWgbs-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "BW=60" synthetic-BWgbs-waste-celio.egnp
synthetic-070gbs-waste-celio.pdf: synthetic-BWgbs-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "BW=70" synthetic-BWgbs-waste-celio.egnp
synthetic-080gbs-waste-celio.pdf: synthetic-BWgbs-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "BW=80" synthetic-BWgbs-waste-celio.egnp
synthetic-090gbs-waste-celio.pdf: synthetic-BWgbs-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "BW=90" synthetic-BWgbs-waste-celio.egnp
synthetic-100gbs-waste-celio.pdf: synthetic-BWgbs-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "BW=100" synthetic-BWgbs-waste-celio.egnp
synthetic-110gbs-waste-celio.pdf: synthetic-BWgbs-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "BW=110" synthetic-BWgbs-waste-celio.egnp
synthetic-120gbs-waste-celio.pdf: synthetic-BWgbs-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "BW=120" synthetic-BWgbs-waste-celio.egnp
synthetic-130gbs-waste-celio.pdf: synthetic-BWgbs-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "BW=130" synthetic-BWgbs-waste-celio.egnp
synthetic-140gbs-waste-celio.pdf: synthetic-BWgbs-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "BW=140" synthetic-BWgbs-waste-celio.egnp
synthetic-150gbs-waste-celio.pdf: synthetic-BWgbs-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "BW=150" synthetic-BWgbs-waste-celio.egnp
synthetic-160gbs-waste-celio.pdf: synthetic-BWgbs-waste-celio.egnp 1h-ckpt-celio.dat daly-ckpt-celio.dat
	gnuplot -e "BW=160" synthetic-BWgbs-waste-celio.egnp

%.pdf: %.gnp $(DAT)
	gnuplot $<

$(DAT): $(CSV) csv2dat.py
	python csv2dat.py

clean:
	rm -f $(PDFs) $(DAT)

