MAPLE=/Applications/Maple\ 2017/maple

TEXs:=synthetic-01hMTBF-waste-cielo.tex synthetic-040gbs-waste-cielo.tex prosp.tex ckpt-slowdown.tex
DAT:=1h-ckpt-cielo.dat daly-ckpt-cielo.dat model.dat prosp.dat ckpt-slowdown.dat

all: $(TEXs)

synthetic-01hMTBF-waste-cielo.tex: synthetic-MTBFhMTBF-waste-cielo.egnp 1h-ckpt-cielo.dat daly-ckpt-cielo.dat model.dat style.gnp
	gnuplot -e "MTBF=1" synthetic-MTBFhMTBF-waste-cielo.egnp

synthetic-040gbs-waste-cielo.tex: synthetic-BWgbs-waste-cielo.egnp 1h-ckpt-cielo.dat daly-ckpt-cielo.dat model.dat style.gnp
	gnuplot -e "BW=40" synthetic-BWgbs-waste-cielo.egnp

prosp.tex: prosp.dat prosp.gnp style.gnp prosp-model.dat
	gnuplot prosp.gnp

prosp.dat: prosp.csv prosp-csv2dat.py
	python prosp-csv2dat.py

ckpt-slowdown.tex: ckpt-slowdown.dat ckpt-slowdown.gnp
	gnuplot ckpt-slowdown.gnp

ckpt-slowdown.dat: ckpt-slowdown.py 1h-ckpt-cielo.dat daly-ckpt-cielo.dat
	python ckpt-slowdown.py

prosp-model.dat: prosp-model.mpl
	$(MAPLE) prosp-model.mpl

1h-ckpt-cielo.dat: 1h-ckpt-cielo.csv 1h-ckpt-csv2dat.py
	python 1h-ckpt-csv2dat.py

daly-ckpt-cielo.dat: daly-ckpt-cielo.csv daly-ckpt-csv2dat.py
	python daly-ckpt-csv2dat.py

model.dat: model.mpl
	$(MAPLE) model.mpl

clean:
	rm -f $(TEXs) $(DAT)

