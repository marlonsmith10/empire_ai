!#/bin/sh

rm -r ./html
doxygen ./empire_ai_doxyfile
firefox ./html/index.html
