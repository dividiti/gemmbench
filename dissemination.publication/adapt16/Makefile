ROOT := ../..
HERE := .

LATEX  := pdflatex
BIBTEX := bibtex

RM     := /bin/rm -rf
CP     := /bin/cp
PATCH  := patch

PAPER  := adapt16

FILES := $(addprefix ${HERE}/, ${PAPER}.tex SGEMM_NT-explore-f-n_tmp.pdf SGEMM_NT-explore-f-n_tmp.tex)

all: ${FILES}
	${LATEX} ${PAPER} && ${BIBTEX} ${PAPER} && ${LATEX} ${PAPER} && ${LATEX} ${PAPER}

${HERE}/SGEMM_NT-explore-f-n_tmp.pdf: ${ROOT}/script/SGEMM_NT/SGEMM_NT-explore-f-n_tmp.pdf
	${CP} $< $@

${HERE}/SGEMM_NT-explore-f-n_tmp.tex: ${ROOT}/script/SGEMM_NT/SGEMM_NT-explore-f-n_tmp.tex ${HERE}/SGEMM_NT-explore-f-n_tmp.tex.patch
	${CP} $< $@
	${PATCH} $@ $@.patch

nobib: ${FILES}
	${LATEX} ${PAPER}

clean:
	${RM} $(addprefix ${PAPER}, .pdf .log .aux .blg .bbl .out)
	${RM} $(addprefix ${HERE}/SGEMM_NT-explore-f-n_tmp, .tex .pdf)
