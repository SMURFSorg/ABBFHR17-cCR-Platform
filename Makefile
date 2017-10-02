TFLAGS=-Wmisc -Wrefs -s


ipdps18.pdf: $(wildcard *.tex *.bib figures/*.pdf)
	rubber $(TFLAGS) -d ipdps18
