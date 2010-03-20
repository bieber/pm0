echo -e "Input file: \c "
read inputFile
echo "./scanner -q $inputFile | ./parser > intermediate_code.txt"
./scanner -q $inputFile | ./parser > intermediate_code.txt
echo "./vm intermediate_code.txt"
./vm intermediate_code.txt
