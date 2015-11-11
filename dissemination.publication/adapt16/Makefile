ROOT := ../..
HERE := .
THERE:= ${ROOT}/script/SGEMM_NT

LATEX  := pdflatex
BIBTEX := bibtex

RM     := /bin/rm -rf
CP     := /bin/cp
PATCH  := patch

PAPER  := adapt16

FILES := $(addprefix ${HERE}/, ${PAPER}.tex \
    SGEMM_NT-explore-f-n_tmp.pdf \
	SGEMM_NT-explore-f-n_tmp.tex \
	SGEMM_NT-explore-f-n-energy-gpu-mem_tmp.pdf \
	SGEMM_NT-explore-f-n-energy-gpu-mem_tmp.tex \
	SGEMM_NT-explore-f-n-match_tmp.tex)

all: ${FILES}
	${LATEX} ${PAPER} && ${BIBTEX} ${PAPER} && ${LATEX} ${PAPER} && ${LATEX} ${PAPER}

# SGEMM_NT-explore-f-n
${HERE}/SGEMM_NT-explore-f-n_tmp.pdf: ${THERE}/SGEMM_NT-explore-f-n_tmp.pdf
	${CP} $< $@
${HERE}/SGEMM_NT-explore-f-n_tmp.tex: ${THERE}/SGEMM_NT-explore-f-n_tmp.tex ${HERE}/SGEMM_NT-explore-f-n_tmp.tex.patch
	${CP} $< $@
	${PATCH} $@ $@.patch

# SGEMM_NT-explore-f-n-energy-gpu-mem
${HERE}/SGEMM_NT-explore-f-n-energy-gpu-mem_tmp.pdf: ${THERE}/SGEMM_NT-explore-f-n-energy-gpu-mem_tmp.pdf
	${CP} $< $@
${HERE}/SGEMM_NT-explore-f-n-energy-gpu-mem_tmp.tex: ${THERE}/SGEMM_NT-explore-f-n-energy-gpu-mem_tmp.tex
	${CP} $< $@
	${PATCH} $@ $@.patch

# SGEMM_NT-explore-f-n-match
${HERE}/SGEMM_NT-explore-f-n-match_tmp.tex: ${THERE}/SGEMM_NT-explore-f-n-match_tmp.tex
	${CP} $< $@
	${PATCH} $@ $@.patch

nobib: ${FILES}
	${LATEX} ${PAPER}

clean:
	${RM} $(addprefix ${PAPER}, .pdf .log .aux .blg .bbl .out)
	${RM} $(addprefix ${HERE}/SGEMM_NT-explore-f-n_tmp, .tex .pdf)
	${RM} $(addprefix ${HERE}/SGEMM_NT-explore-f-n-energy-gpu-mem_tmp, .tex .pdf)
	${RM} $(addprefix ${HERE}/SGEMM_NT-explore-f-n-match_tmp, .tex)
	${RM} *.orig *.rej
