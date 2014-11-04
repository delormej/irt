#
# Wraps xsl parse, pass in the tcx file
#
set in=%1
set out=%in:~,-4%csv
c:\tools\msxsl.exe %1 ..\..\tcx-to-csv.xslt -o %out%
find_force.py %out%
