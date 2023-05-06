OCB_FLAGS = -use-ocamlfind -I src
OCB = ocamlbuild $(OCB_FLAGS)

all: native byte
clean:
	$(OCB) -clean
native:
	$(OCB) chkenv.native
byte:
	$(OCB) chkenv.byte
profile:
	$(OCB) -tag profile chkenv.native
debug:
	$(OCB) -tag debug chkenv.byte
sanity:
	ocamlfind query str unix

.PHONY: all clean ntive byte profile debug sanity
