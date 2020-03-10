/*
Duncan Spani
Huffman Coding Project
bugs: adds single '.' to end of rebuilt file.
*/
#include "pch.h"
#include "HuffmanTree.h"

using namespace std;

inline bool HuffmanTree::getBit(unsigned char byte, int position) const 
{ 
	return (byte  & BITMASK[position]);
}

// Usage
// mybyte = setBit(mybyte,4);
// This sets the bit in position 4
inline unsigned char HuffmanTree::setBit(unsigned char byte, int position) const 
{
	return  byte | BITMASK[position];
}

void HuffmanTree::printBits(char binary, std::ostream & out) const
{
	for (auto i = 0; i < sizeof(char) * 8; i++)
	{
		if (getBit(binary, i)) {
			out << 1;
		}
		else {
			out << 0;
		}
	}
}

void HuffmanTree::printBinary(vector<char> bytes, std::ostream & out) const 
{		
	for (char byte : bytes) 
	{
		printBits(byte, out);
		out << "-";		
	}
	
}

string HuffmanTree::getCode(char letter)
{	
	string code = "";
	if (root != nullptr)
	{
		getCodes(root, code, letter);
		codeLookup[letter] = code;
	}
	return code;
}

void HuffmanTree::makeEmpty(BinaryNode *& t) 
{	
	if (t != nullptr)
	{
		makeEmpty(t->left);
		makeEmpty(t->right);
		t = nullptr;
		delete t;

	}
}

void HuffmanTree::printTree(BinaryNode *node, std::ostream & out) const 
{
	if (node != nullptr)
	{
		printTree(node->left, out);
		if (node->left == nullptr && node->right == nullptr)
		{
			out << "Frequency: " << node->frequency << "     Code: " << node->element << endl;
		}
		printTree(node->right, out);
	}
}

void HuffmanTree::printCodes(BinaryNode *node, std::ostream & out, string code) const 
{	
	if (node != nullptr)
	{
		printCodes(node->left, out, code + "0");
		if (node->left == nullptr && node->right == nullptr)
		{
			if (node->element[0] == '\a')
			{
				out << "EOF: ";
			}
			out << node->element << "\t" << right << code << endl;
		}
		printCodes(node->right, out, code + "1");
	}
	

}


void HuffmanTree::saveTree(BinaryNode * current, string code) 
{	
	//used iterative tree saving using unordered map
}

void HuffmanTree::saveTree(std::ostream & compressedFileStream)
{
	cout << endl << endl << "Saving Tree..." << endl;
	for (auto pair : codeLookup)
	{
		compressedFileStream << pair.first << '|' << pair.second << '|';
	}
	compressedFileStream << '|';
	
}


void HuffmanTree::rebuildTree(BinaryNode * node, string element, string route) 
{
	if (root == nullptr)
	{
		root = new BinaryNode(element);
		rebuildTree(root, element, route);
	}
	else
	{
		if (node->left == nullptr && route[0] == '0')//left doesnt exist
		{
			node->left = new BinaryNode(element);
			route.erase(0, 1);
			rebuildTree(node->left, element, route);
		}
		else if (node->left != nullptr && route[0] == '0')//left exists
		{
			route.erase(0, 1);
			rebuildTree(node->left, element, route);
			node->element += element;
		}
		else if (node->right == nullptr && route[0] == '1')//right doesnt exist
		{
			node->right = new BinaryNode(element);
			route.erase(0, 1);
			
			rebuildTree(node->right, element, route);
		}
		else if (node->right != nullptr && route[0] == '1')//right exists
		{
			route.erase(0, 1);
			rebuildTree(node->right, element, route);
			node->element += element;
		}
		
	}
}

void HuffmanTree::rebuildTree(ifstream & compressedFile) 
{		
	makeEmpty();
	string element = "x";//clears and overrides later
	string route;
	char letter = 0;
	char code = 0;
	char delim = '|';

	while (element != "")
	{
		route.clear();
		element.clear();
		compressedFile >> noskipws >> letter;
		while (letter != delim)
		{
			element = element + letter;
			compressedFile >> noskipws >> letter;
			compressedFile >> noskipws >> code;
			while (code != delim)
			{
				route = route + code;
				compressedFile >> noskipws >> code;
			}
			rebuildTree(root, element, route);
		}		
	}
}

HuffmanTree::BinaryNode * HuffmanTree::buildTree(string frequencyText) 
{ 
	priority_queue<HuffmanTree::BinaryNode *, vector<HuffmanTree::BinaryNode *>, compareBinaryNodes > nodes;
	
	int frequency[255];
	for (int i = 0; i < 255; i++)//setting up array with letter i and frequency array
	{
		frequency[i] = 0;
	}
	for (unsigned int i = 0; i < frequencyText.size(); i++)
	{
		frequency[frequencyText[i]]++;
	}

	string EOFChar = "";
	EOFChar += EOFCharacter;
	nodes.push(new BinaryNode(EOFChar, 1));

	for (int i = 0; i < 255; i++)//setting up each node
	{
		if (frequency[i] > 0)
		{
			string element;
			element = i;
			BinaryNode * node = new BinaryNode(element, frequency[i]);
			nodes.push(node);
		}
	}
	

	while (nodes.size() > 1)//building tree
	{
		BinaryNode * left = nodes.top();
		nodes.pop();
		BinaryNode * right = nodes.top();
		nodes.pop();
		BinaryNode * node = new BinaryNode((left->element + right->element), (left->frequency + right->frequency), left, right);
		nodes.push(node);
	}

	return nodes.top();
}

void HuffmanTree::getCodes(BinaryNode * node, string &code, char letter)
{
	if (node->element.size() > 0)//if node has elements
	{
		if (contains(node, letter))//if node contains letter
		{
			if (node->left != nullptr && contains(node->left, letter))//check if left has letter/nullptr
			{
				code = code + "0";
				getCodes(node->left, code, letter);
				
			}
			else if (node->right != nullptr && contains(node->right, letter))//check if right has letter/nullptr
			{
				code = code + "1";
				getCodes(node->right, code, letter);
			}
		}
	}
}

HuffmanTree::HuffmanTree(string frequencyText) 
{
	root = buildTree(frequencyText);
	//saveTree(root, string());   // build the lookupTable for codes...can write later
}

HuffmanTree::HuffmanTree(ifstream & frequencyStream) { 	

	std::string frequencyText( (std::istreambuf_iterator<char>(frequencyStream)), std::istreambuf_iterator<char>() );
	frequencyStream.close();
	if (frequencyText.size() > 0) {
		root = buildTree(frequencyText);
		//saveTree(root, string());   // build the lookupTable for codes
	}
}

HuffmanTree::~HuffmanTree() 
{	
	if (root != nullptr)
	{
		makeEmpty();
	}
}

void HuffmanTree::printCodes(std::ostream & out) const 
{ 
	string code;
	printCodes(root, out, code);
}

void HuffmanTree::printTree(std::ostream & out) const 
{	
	printTree(root, out);
}

void HuffmanTree::makeEmpty() 
{	
	makeEmpty(root);
	// calls recursive function	
}

string HuffmanTree::decode(vector<string> encodedBytes) {
	string decoded;
	//you said i could skip test 2
	return decoded;
}

string HuffmanTree::decode(BinaryNode * t, char bit, ifstream & binaryFile)
{
	/*while (t->left != nullptr && t->right != nullptr)
	{
		binaryFile >> bit;
		if (t->left != nullptr && bit == 0)
		{
			decode(t->left, bit, binaryFile);
		}
		else
		{
			decode(t->right, bit, binaryFile);
		}
	}*/
	return t->element;
}

string HuffmanTree::decode(vector<string> bytes, BinaryNode * node)
{
	cout << "Decoding..." << endl;
	/*
	iterate through tree based on bytes[iterator][i]
	*/
	BinaryNode * temp = node;
	string byte = "";
	string retVal;
	int iterator = 0;
	while (bytes.size() > 0)
	{		
		if (byte.size() < 2 && iterator < bytes.size())
		{
			byte += bytes[iterator];
			iterator++;
		}
		if (temp->element[0] == EOFCharacter)
		{
			return retVal;
		}
		//check for short codes and long codes
		for (int i = 0; temp->left != nullptr && temp->right != nullptr; i++)
		{
			if (byte.size() < 2 && iterator < bytes.size())
			{
				byte = byte + bytes[iterator];
				iterator++;
			}
			if (byte[0] == '0')
			{
				temp = temp->left;
				byte.erase(0, 1);
			}
			else
			{
				temp = temp->right;
				byte.erase(0, 1);
			}
		}
		if (temp->element[0] == EOFCharacter)
		{
			return retVal;
		}
		retVal += temp->element;
		temp = node;
		if (iterator >= bytes.size())
		{
			return retVal;
		}
	}
	return retVal;
	/*map<string, string>::iterator find;
	find = decoding.find(bytes[iterator]);
	if (find != decoding.end())
	{
		return find->second;
	}*/
}

vector<string> HuffmanTree::encode(string stringToEncode)
{
	
	stringToEncode.push_back(EOFCharacter);
	vector<string> encoded;

	for (unsigned int i = 0; i < stringToEncode.size(); i++)
	{
		encoded.push_back(getCode(stringToEncode[i]));
	}
	
	return encoded;
}

void HuffmanTree::uncompressFile(string binaryFile, string rebuiltTextFile) 
{
	ofstream textFile;
	ifstream binary;
	textFile.open(rebuiltTextFile);
	binary.open(binaryFile, ios::in | ios::binary);
	rebuildTree(binary);

	unsigned char bit = NULL;
	vector<string> bytes;
	string byte;

	while (!binary.eof())
	{
		binary >> bit;
		for (int i = 0; i < 8; i++)
		{
			if (getBit(bit, i))
			{
				byte += '1';
			}
			else
			{
				byte += '0';
			}
		}
		bytes.push_back(byte);
		byte = "";
	}
	textFile << decode(bytes, root);
	

	textFile.close();
	binary.close();
}

void HuffmanTree::compressFile(string compressToFileName, string uncompressedFileName, bool buildNewTree) 
{	
	ofstream binaryFile; 
	binaryFile.open(compressToFileName, ios::out | ios::binary);
	ifstream textFile;
	textFile.open(uncompressedFileName);

	vector<char> encodedFile;
	char letter = EOFCharacter;
	string temp;
	int count = 0;
	char encodeChar = NULL;
	//encodedFile.push_back(EOFCharacter);
	while (!textFile.eof())
	{
		textFile >> noskipws >> letter;
		temp = getCode(letter);
		for (char code : temp)
		{
			if (code == '1')
			{
				encodeChar = setBit(encodeChar, count);
			}
			else
			{
				//do nothing, already zero
			}
			count++;
			if (count == 8)
			{
				//printBits(encodeChar); cout << endl;
				count = 0;
				encodedFile.push_back(encodeChar);
				encodeChar = NULL;
			}
		}
	}
	string EOFCode = getCode(EOFCharacter);
	
	for (char code : EOFCode)
	{
		if (code == '1')
		{
			encodeChar = setBit(encodeChar, count);
		}
		else
		{
			//do nothing, already zero
		}
		count++;
		if (count == 8)
		{
			//printBits(encodeChar); cout << endl;
			count = 0;
			encodedFile.push_back(encodeChar);
			encodeChar = NULL;
		}
	}
	saveTree(binaryFile);
	
	for (unsigned int i = 0; i < encodedFile.size(); i++)
	{
		binaryFile << encodedFile[i];
	}
	binaryFile.close(); 
	textFile.close();
}

bool HuffmanTree::contains(BinaryNode * node, char letter)
{
	for (unsigned int i = 0; i < node->element.size(); i++)
	{
		if (node->element[i] == letter)
		{
			return true;
		}
	}
	return false;
}


/*if (node->left == nullptr && node->right == nullptr)
	{
		letter = node->element;
	}
	else
	{
		if (bytes[0] == '1')
		{
			bytes.erase(0, 1);
			decodeRecursive(node->right, bytes, letter);
		}
		else
		{
			bytes.erase(0, 1);
			decodeRecursive(node->left, bytes, letter);
		}
	}*/

	/*string rebuilt;
		string letter;
		while (bytes[0])
		{
			letter.clear();
			decodeRecursive(node, bytes, letter);
			rebuilt = rebuilt + letter;
		}
		return rebuilt;*/