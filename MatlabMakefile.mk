MEX=/home/player999/storage/program_files/Matlab/bin/mex

all: filter_ml.mexa64 mixer_ml.mexa64

%.mexa64: %.c
	$(MEX) -L$(PWD) -lcutedsp $<

clean:
	rm -f filter_ml.mexa64
	rm -f mixer_ml.mexa64