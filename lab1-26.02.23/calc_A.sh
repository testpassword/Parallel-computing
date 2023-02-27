# usage ./calc_A.sh "Surname Name Patronymic"
PRODUCT=1
for I in $1
    do
        ((PRODUCT*=${#I}))
    done
echo $PRODUCT