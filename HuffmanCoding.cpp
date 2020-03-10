// HuffmanCoding.cpp : Defines the entry point for the console application.
//
//#include "stdafx.h"
#include "pch.h"
#include "HuffmanTree.h"
#include <fstream>
#include <cassert>

using namespace std;

int main(int argc, char* argv[]) {

	assert("Must include input file (*.txt)", argc > 1);
	ifstream infile(argv[1]);
	HuffmanTree tree(infile);
	cout << "Compressing File..." << endl;
	tree.compressFile("binary.bin", argv[1], true);
	cout << "Uncompressing File..." << endl;
	tree.uncompressFile("binary.bin", "rebuilt.txt");
}

