PAPER  := adapt16

LATEX  := pdflatex
BIBTEX := bibtex

RM     := /bin/rm -rf

FILES := ${PAPER}.tex

all: ${FILES}
	${LATEX} ${PAPER} && ${BIBTEX} ${PAPER} && ${LATEX} ${PAPER} && ${LATEX} ${PAPER}

nobib: ${FILES}
	${LATEX} ${PAPER}

clean: 
	$(RM) $(addprefix ${PAPER}, .pdf .log .aux .blg .bbl .out)
