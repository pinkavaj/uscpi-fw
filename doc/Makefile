SRC=scpi

all:
	txt2tags -t tex ${SRC}.txt
	latex --style=babel ${SRC}.tex

clean:
	rm ${SRC}.aux ${SRC}.dvi ${SRC}.log ${SRC}.out ${SRC}.tex

