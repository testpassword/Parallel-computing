curl -L https://sourceforge.net/projects/framewave/files/framewave-releases/Framewave%201.3.1/FW_1.3.1_Lin64.tar.gz/download > AMD_Framewave_Lin64.tar.gz
tar xvzf AMD_Framewave_Lin64.tar.gz
mv FW_1.3.1_Lin64 src/
cd src/FW_1.3.1_Lin64/lib
for FILE in *; do
    for EXT in "so.1" "so"; do
        LIB_FILE=$(echo $FILE | awk -F '.' '{print $1}')
        ln -sf $FILE "$LIB_FILE.$EXT"
    done
done
cd ../..
gcc -O3 -Wall -Werror -o lab2.o lab2.c -lm -I FW_1.3.1_Lin64 -c
gcc -O3 -L FW_1.3.1_Lin64/lib lab2.o -o ../lab2 -lfwImage -lfwBase