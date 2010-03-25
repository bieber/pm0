echo -e "Input file: \c "
read inputFile
if [ $# -eq 0 ]; then
	echo "./scanner -q $inputFile | ./parser -q > intermediate_code.txt"
	./scanner -q $inputFile | ./parser -q > intermediate_code.txt
	echo "./vm intermediate_code.txt"
	./vm intermediate_code.txt

# "-s" = run the input file or stream through the scanner only
# "-p" = run the input file or stream through the parser only
# "-v" = run the input file or stream through the virtual machine only
# "-c [outfile]" = generate intermediate code to screen or outfile

elif [ $# -gt 0 ] && [ $# -lt 3 ]; then
	if [ $1 = "-s" ]; then
		if [ $# -eq 2 ]; then
			if [ $2 = "-q" ]; then
				echo "./scanner -q $inputFile"
				./scanner -q $inputFile
			else
				echo "Error. Usage: ./driver.sh [-s/p/v/c] [-q]"
			fi
		else
			echo "./scanner $inputFile"
			./scanner $inputFile
		fi

	elif [ $1 = "-p" ]; then
		if [ $# -eq 2 ]; then
			if [ $2 = "-q" ]; then
				echo "./parser -q $inputFile"
				./parser -q $inputFile
			else
				echo "Error. Usage: ./driver.sh [-s/p/v/c] [-q]"
			fi
		else
			echo "./parser $inputFile"
			./parser $inputFile
		fi

	elif [ $1 = "-v" ]; then
		if [ $# -eq 2 ]; then
			if [ $2 = "-q" ]; then
				echo "./vm -q $inputFile"
				./vm -q $inputFile
			else
				echo "Error. Usage: ./driver.sh [-s/p/v/c] [-q]"
			fi
		else
			echo "./parser $inputFile"
			./parser $inputFile
		fi

	elif [ $1 = "-c" ]; then
		if [ $# -eq 2 ]; then
			if [ $2 = "-q" ]; then
				echo "./scanner -q $inputFile | ./parser -q"
				./scanner -q $inputFile | ./parser -q
			else
				echo "Error. Usage: ./driver.sh [-s/p/v/c] [-q]"
			fi
		else
			echo "./scanner -q $inputFile | ./parser"
			./scanner -q $inputFile | ./tokenprinter
			echo ""
			cat $inputFile
			echo ""
			./scanner -q $inputFile | ./parser
		fi
	fi
else
	echo "Error. Usage: ./driver.sh [-s/p/v/c] [-q]"
fi
