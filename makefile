main: bst.cpp
	g++ -O2 -std=gnu++11 -pedantic bst.cpp -o bst
clean:
	rm -rf bst
cleanDebugFiles:
	rm -rf bst.dSYM
cleanAll: clean cleanDebugFiles
