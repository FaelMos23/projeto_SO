# create directories
mkdir ../../bin
mkdir ../../bin/comm
mkdir ../../bin/extras

# main application binary
gcc ../shell.c  -o ../../bin/shell

# base commmands
gcc ../ls.c     -o ../../bin/comm/ls
gcc ../pwd.c    -o ../../bin/comm/pwd
gcc ../cat.c    -o ../..bin/comm/cat

# extras binaries
gcc ../sum.c    -o ../../bin/extras/sum
gcc ../test.c   -o ../../bin/extras/test
gcc ../pretty.c -o ../../bin/extras/pretty
