ROOT := ../..
HERE := .
THERE:= ${ROOT}/script/SGEMM_NT

LATEX  := pdflatex
BIBTEX := bibtex
FLATEX := $(addprefix ${HERE}/, flatex)

RM     := /bin/rm -rf
CP     := /bin/cp
MV     := /bin/mv
PATCH  := patch

PAPER  := adapt16

FILES := $(addprefix ${HERE}/, ${PAPER}.tex \
    SGEMM_NT-explore-f-n_tmp.pdf \
	SGEMM_NT-explore-f-n_tmp.tex \
	SGEMM_NT-explore-f-n-energy-gpu-mem_tmp.pdf \
	SGEMM_NT-explore-f-n-energy-gpu-mem_tmp.tex \
	SGEMM_NT-explore-f-n-match_tmp.tex \
    SGEMM_NT-explore-n-lws_tmp.pdf \
	SGEMM_NT-explore-n-lws_tmp.tex \
)

all: ${FILES}
	${LATEX} ${PAPER} && ${BIBTEX} ${PAPER} && ${LATEX} ${PAPER} && ${LATEX} ${PAPER}

${FLATEX}: flatex.c
	${CC} $< -o $@

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

# SGEMM_NT-explore-n-lws
${HERE}/SGEMM_NT-explore-n-lws_tmp.pdf: ${THERE}/SGEMM_NT-explore-n-lws_tmp.pdf
	${CP} $< $@
${HERE}/SGEMM_NT-explore-n-lws_tmp.tex: ${THERE}/SGEMM_NT-explore-n-lws_tmp.tex ${HERE}/SGEMM_NT-explore-n-lws_tmp.tex.patch
	${CP} $< $@
	${PATCH} $@ $@.patch

nobib: ${FILES}
	${LATEX} ${PAPER}

arxiv: ${FLATEX} all
	${FLATEX} ${PAPER}.tex
	${MV} ${PAPER}.flt ${PAPER}.arxiv.tex
	${LATEX} ${PAPER}.arxiv && ${LATEX} ${PAPER}.arxiv

clean:
	${RM} $(addprefix ${PAPER}, .pdf .log .aux .blg .bbl .out .arxiv.tex .arxiv.pdf)
	${RM} $(addprefix ${HERE}/SGEMM_NT-explore-f-n_tmp, .tex .pdf)
	${RM} $(addprefix ${HERE}/SGEMM_NT-explore-f-n-energy-gpu-mem_tmp, .tex .pdf)
	${RM} $(addprefix ${HERE}/SGEMM_NT-explore-f-n-match_tmp, .tex)
	${RM} $(addprefix ${HERE}/SGEMM_NT-explore-n-lws_tmp, .tex .pdf)
	${RM} *.orig *.rej
	${RM} ${FLATEX}
