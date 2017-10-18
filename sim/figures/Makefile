MAPLE=/Applications/Maple\ 2017/maple

TEXs:=synthetic-01hMTBF-waste-cielo.tex synthetic-040gbs-waste-cielo.tex prosp.tex
DAT:=1h-ckpt-cielo.dat daly-ckpt-cielo.dat model.dat prosp.dat

all: $(TEXs)

synthetic-01hMTBF-waste-cielo.tex: synthetic-MTBFhMTBF-waste-cielo.egnp 1h-ckpt-cielo.dat daly-ckpt-cielo.dat model.dat
	gnuplot -e "MTBF=1" synthetic-MTBFhMTBF-waste-cielo.egnp

synthetic-040gbs-waste-cielo.tex: synthetic-BWgbs-waste-cielo.egnp 1h-ckpt-cielo.dat daly-ckpt-cielo.dat model.dat
	gnuplot -e "BW=40" synthetic-BWgbs-waste-cielo.egnp

prosp.tex: prosp.dat prosp.gnp
	gnuplot prosp.gnp

prosp.dat: prosp.csv prosp-csv2dat.py
	python prosp-csv2dat.py

1h-ckpt-cielo.dat: 1h-ckpt-cielo.csv 1h-ckpt-csv2dat.py
	python 1h-ckpt-csv2dat.py

daly-ckpt-cielo.dat: daly-ckpt-cielo.csv daly-ckpt-csv2dat.py
	python daly-ckpt-csv2dat.py

model.dat: model.mpl
	$(MAPLE) model.mpl

clean:
	rm -f $(TEXs) $(DAT)

