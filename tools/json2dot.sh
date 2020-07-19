#!/bin/bash

jq -r 'to_entries[] | .key as $k | .value[]? | if $k != . then if . | startswith("AGE") then "\($k) [group=\(.)]" else "\(.) -> \($k)" end else "" end' \
    | cat <(sed -n '/^ICON_ADVANCE_/s|^.*ICON_ADVANCE_\([^[:space:]]*\).*FirstFrame[^"]*"\([^"]*\).TGA".*$|  ADVANCE_\1 [shape=box,\nlabel=<<TABLE border="0" cellspacing="0" cellborder="0"><TR><TD><IMG SCALE="TRUE" SRC="\L\2\E.png"/></TD></TR>\n<TR><TD><B>\1</B></TD></TR></TABLE>>] |p' ../ctp2_data/default/gamedata/uniticon.txt ) \
          <(awk '/{/ {a=$1}; $1 ~ /^EnableAdvance$/ {{printf("%s [xlabel=%s]\n", $2, a)}}' ../ctp2_data/default/gamedata/Units.txt ) \
          - \
    | sed '/^<TR><TD>/s/_/ /g' `# needs label= to be in a different line than node-name (\n above)!` \
    | cat <(echo 'digraph G {') \
          <(echo 'imagepath="pic555"') \
          <(echo 'rankdir=LR') \
          <(echo 'node [fontname="Sans serif" fontsize=7.5]') \
          <(echo 'forcelabels=true;') \
	  - \
	  <(echo '}')
