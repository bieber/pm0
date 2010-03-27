if [ $# -eq 1 ]; then
	echo "./scanner -q $1 | ./parser -q > intermediate_code.txt"
	./scanner -q $1 | ./parser -q > intermediate_code.txt
	echo "./vm intermediate_code.txt"
	./vm intermediate_code.txt

# "-s" = run the input file or stream through the scanner only
# "-p" = run the input file or stream through the parser only
# "-v" = run the input file or stream through the virtual machine only
# "-c [outfile]" = generate intermediate code to screen or outfile

elif [ $# -gt 1 ] && [ $# -lt 4 ]; then
	if [ $1 = "-s" ]; then
		if [ $# -eq 3 ]; then
			if [ $2 = "-q" ]; then
				echo "./scanner -q $3"
				./scanner -q $inputFile
			else
				echo "Error. Usage: ./driver.sh [-s/p/v/c] [-q] inputfile"
			fi
		else
			echo "./scanner $2"
			./scanner $2
		fi

	elif [ $1 = "-p" ]; then
		if [ $# -eq 3 ]; then
			if [ $2 = "-q" ]; then
				echo "./parser -q $3"
				./parser -q $3
			else
				echo "Error. Usage: ./driver.sh [-s/p/v/c] [-q] inputfile"
			fi
		else
			echo "./parser $2"
			./parser $2
		fi

	elif [ $1 = "-v" ]; then
		if [ $# -eq 3 ]; then
			if [ $2 = "-q" ]; then
				echo "./vm -q $3"
				./vm -q $3
			else
				echo "Error. Usage: ./driver.sh [-s/p/v/c] [-q] inputfile"
			fi
		else
			echo "./parser $2"
			./parser $2
		fi

	elif [ $1 = "-c" ]; then
		if [ $# -eq 3 ]; then
			if [ $2 = "-q" ]; then
				echo "./scanner -q $3 | ./parser -q"
				./scanner -q $3 > tempfile.txt
				if [ $? -ne 0 ]; then
					echo "Errors present in scanner output."
					./scanner -q $3
				else
					./parser -q < tempfile.txt
				fi
			else
				echo "Error. Usage: ./driver.sh [-s/p/v/c] [-q] inputfile"
			fi
		else
			echo "./scanner -q $2 | ./parser"
			./scanner -q $2 > tempfile.txt
			if [ $? -ne 0 ]; then
				echo "Errors present in the scanner output."
				./scanner -q $2
			else
				./tokenprinter < tempfile.txt
				echo ""
				cat $2
				echo ""
				./scanner -q $2 | ./parser
			fi
		fi
	fi
else
	echo "Error. Usage: ./driver.sh [-s/p/v/c] [-q] inputfile"
fi
