

all : Advance_english.png
all : Advance_english.svg

all : Advance_english.pdf
all : Advance_german.pdf
all : Advance_french.pdf
all : Advance_italian.pdf
all : Advance_spanish.pdf

all : Advance_english.gv
all : Advance_german.gv
all : Advance_french.gv
all : Advance_italian.gv
all : Advance_spanish.gv

%.json : ../ctp2_data/default/gamedata/%.txt
	../tools/txt2json.sh $< > $@

Advance_%.gv_ : Advance.json ../ctp2_data/%/gamedata/gl_str.txt
	../tools/json2dot.sh < $< > $@
	../tools/advBenefits.sh $@
	sed -i 's|<TD>|<TD align="text">|g;s|</TD>|<BR align="left" /></TD>|g' $@
	sed -i 's|<TD align="text"><BR align="left" /></TD>|<TD></TD>|g' $@
	sed -i 's|<TH>|<TD>|g;s|</TH>|</TD>|g' $@
	awk -v FPAT='([^ \t]+)|("[^"]+")' ' !/^#/ && NF>1 {gsub("\"","", $$2); cmd = sprintf("sed -i -r \"/label/s/%s([^_])/%s\\1/g\" $@", $$1, $$2); system(cmd)}' $(word 2,$^)  # 1st look-up run needed before gvpr removes unnecessary double-quotes

Advance_%.gv : Advance_%.gv_ ../ctp2_data/%/gamedata/gl_str.txt
	gvpr -c 'N[$$.degree==0]{delete(root, $$)}' $< `# remove nodes without any edges, e.g. DEFAULT, SUBNEURAL_ADS, MIND_CONTROL ` \
	| tred `# removes edges implied by transitivity ` \
	| gvpr -c -f ../tools/reformat.g > $@ 
	awk -v FPAT='([^ \t]+)|("[^"]+")' ' !/^#/ && NF>1 {gsub("\"","", $$2); cmd = sprintf("sed -i \"/label/s/%s/\\\"%s\\\"/g\" $@", $$1, $$2); system(cmd)}' $(word 2,$^)  # 2nd look-up run needed for cluster label introduced by reformat.g

%.png : %.gv
	dot $< -Gcharset=latin1 -Gsize=8 -Gdpi=100 -Tpng > $@ # https://stackoverflow.com/questions/14784405/how-to-set-the-output-size-in-graphviz-for-the-dot-format

%.svg : %.gv
	dot $< -Gcharset=latin1 -Tsvg \
	| sed 's|xlink:href="|xlink:href="pic555/|' > $@

%.pdf : %.gv
	dot $< -Gcharset=latin1 -Tpdf \
	| sed "s|^[[:space:]]*/CreationDate (.*)$$|/CreationDate (D:20190828000000+00'00')|; \
	     s|^[[:space:]]*/ModDate (.*)$$|/ModDate (D:20190828000000+00'00')|; \
	     /^\/ID \[<.*> <.*>\]$$/d" >  $@ # deleting ID instead of replacing: http://superuser.com/questions/130347/how-do-i-produce-bytewise-consistent-documents-with-pdflatex#130804

