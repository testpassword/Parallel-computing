AMD_FW_DISTRO="AMD_Framewave_Lin64.tar.gz"
curl -L https://sourceforge.net/projects/framewave/files/framewave-releases/Framewave%201.3.1/FW_1.3.1_Lin64.tar.gz/download > $AMD_FW_DISTRO
tar xvzf $AMD_FW_DISTRO
rm $AMD_FW_DISTRO
mv FW_1.3.1_Lin64 src/
cd src/FW_1.3.1_Lin64/lib
for FILE in *; do
    for EXT in "so.1" "so"; do
        LIB_FILE=$(echo $FILE | awk -F '.' '{print $1}')
        ln -sf $FILE "$LIB_FILE.$EXT"
    done
done
cd ../..
gcc -O3 -Wall -Werror -o lab2.o lab2.c -I FW_1.3.1_Lin64 -c
gcc -O3 -L FW_1.3.1_Lin64/lib lab2.o -o ../lab2 -lm -lfwImage -lfwBase -lfwSignal