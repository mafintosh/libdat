dat: dat.c storage.h storage.c
	cc dat.c storage.c -luv -o dat

clean:
	rm dat
