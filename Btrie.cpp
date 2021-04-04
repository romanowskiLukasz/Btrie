#include <iostream> 
#include <string>
#include <algorithm>

using namespace std;
const int alphabetSize = 26;

// trie node 
class TrieNode
{
private:
	// posiada tablice potencjalych potomkow (kazda litera moze nastepowac), tab do trienodow
	TrieNode* children[alphabetSize];
	char data;
	// flaga, czy dana litera konczy slowo
	bool is_leaf;
public:
	TrieNode();

	char getData() { return (this == nullptr) ? '\0' : this->data; }     //puste słowo
	bool getFlag() { return this->is_leaf; }
	TrieNode* getChildren(int index) { return this->children[index]; }

	void setData(char l) { this->data = l; }
	bool setFlag(bool state) { return this->is_leaf = state; }
	void setChildren(int index, TrieNode* p) { this->children[index] = p; }
	bool emptyChildren();					//zwraca true jesli node nie ma dzieci
	bool multipleChildren();				//zwraca true jesli node ma wiecej niz 1 dziecko (branching)
};


TrieNode::TrieNode()
{
	is_leaf = false;
	data = '\0';

	for (int i = 0; i < alphabetSize; i++)
		children[i] = nullptr;
}

bool TrieNode::emptyChildren()
{
	for (int i = 0; i < alphabetSize; i++)
		if (this->children[i] != NULL)
			return false;
	return true;
}

bool TrieNode::multipleChildren()
{
	int childrenNumber = 0;
	for (int i = 0; i < alphabetSize; i++)
		if (this->children[i] != NULL)
			childrenNumber++;
	if (childrenNumber > 1)
		return true;
	else
		return false;
}

class TRIETree
{
private:
	TrieNode* root;
public:
	TRIETree() { root = new TrieNode; };


	//zwraca wskażnik do root
	TrieNode* getRoot() { return this->root; }

	//zwraca prawdę gdy drzewo jest puste
	bool empty() { return (root == nullptr) ? true : false; }

	//wstawia węzeł z wartością x
	void insert(string key);

	/*zwraca node z ostatnia litera slowa (jesli istnieje)*/
	TrieNode* get_leaf(string key);

	/*sprawdza czy slowo wystepuje w drzewie*/
	bool search_check(string key);

	//usuwa podane slowo z drzewa
	void del_word(string key);

	//wypisuje drzewo	
	void print_trie(TrieNode* root);

	//sprawdza czy podslowo podanego slowa jest czescia innego slowa i zwraca index rozgalezienia	
	int check_divergance(string key);

	//zwraca najdluzsze wspolne podslowo podanego slowa	
	string find_longest_prefix(string key);

	//zwraca prawde jesli node z ostatnia litera konczy slowo (jesli slowo znajduje sie w drzewie)
	bool is_leaf_node(string key);

	//zmienia podane slowo na slowo obslugiwane przez drzewo	
	string checkKey(string key);

	//rekursywnie usuwa drzewo rozpoczynajac od node	
	void free_trienode(TrieNode* node);
};

TrieNode* TRIETree::get_leaf(string key)
{
	TrieNode* tmp = root;
	for (int i = 0; i < key.size(); i++)
	{
		int pos = (int)key[i] - 'a';  // a w ASCII 97, aby był na 0 miejscu w tab 
		if (tmp->getChildren(pos))
		{
			tmp = tmp->getChildren(pos);
		}
	}
	return tmp;
}

bool TRIETree::is_leaf_node(string key)
{
	TrieNode* tmp = get_leaf(key);
	return tmp->getFlag();
}

void TRIETree::free_trienode(TrieNode* node) {
	for (int i = 0; i < alphabetSize; i++) {
		if (node->getChildren(i) != NULL) {
			free_trienode(node->getChildren(i));
		}
		else {
			continue;
		}
	}
	delete node;
}

string TRIETree::checkKey(string key)
{
	for (int i = 0; i < key.length(); i++)
	{
		int ASCIInum = (int)key[i];
		if (ASCIInum >= 65 and ASCIInum <= 90) //znak jest wielka litera i nalezy zmienic na mala
		{
			key[i] = ::tolower(key[i]);
		}
		else if (ASCIInum < 65 or (ASCIInum > 90 and ASCIInum < 97) or ASCIInum>122) //znak jest liczba lub znakiem specjalnym
		{
			key.erase(i, 1);
			i--;
		}
	}
	return key;
}

void TRIETree::insert(string key)
{
	key = this->checkKey(key);
	TrieNode* p = root;

	for (int i = 0; i < key.length(); i++)	// dla kazdej kolejnej litery klucza
	{
		int index = key[i] - 'a';			// szukamy jej indeksu w tablicy
		if (!p->getChildren(index))			// jezeli jeszcze nie ma jej w tablicy
		{
			TrieNode* tmp = new TrieNode;	// tworzymy node tej litery
			tmp->setData(key[i]);			// i dodajemy do tablicy
			p->setChildren(index, tmp);
		}

		p = p->getChildren(index);			// przechodzimy do nastepnej litery
	}

	p->setFlag(true);
}

bool TRIETree::search_check(string key)
{
	TrieNode* p = root;

	for (int i = 0; i < key.length(); i++)
	{
		int index = key[i] - 'a';
		if (!p->getChildren(index))
			return false;

		p = p->getChildren(index);
	}

	return (p != NULL && p->getFlag());
}

void TRIETree::del_word(string key)
{
	key = this->checkKey(key); //podmieniamy podany KEY na czytelny dla programu 

	if (!root)		// jezeli drzewo jest puste to stop
		return;
	if (key.empty()) // jezeli podany klucz jest pusty to stop
		return;
	if (!is_leaf_node(key))	// jezeli w drzewie wystapi ciag znakow, ale nie jest okreslony jako slowo to stop
		return;

	TrieNode* temp = root;
	string longest_prefix = find_longest_prefix(key);	// szukamy najdluzszego prefiksu, który mozliwie nie jest usuwanym slowem 

	int i;	// indeks pozycji na galezi w ktorej bedziemy usuwac

	for (i = 0; i < longest_prefix.length(); i++) {
		int position = (int)longest_prefix[i] - 'a';
		if (temp->getChildren(position) != nullptr) {
			temp = temp->getChildren(position); // przechodzimy do najdalszego node w danym prefiksie
		}
		else {
			return; // jak nie ma takiego node, stop
		}
	}


	int len = key.length();

	if (i == len)							// jesli prefiks jest usuwanym slowem to znaczy ze jest on czescia dluzszego slowa
		temp->setFlag(false);				// zdejmujemy tylko flage konca slowa
	else {									// w innym usuwamy znaki od konca prefiksu do konca podanego slowa
		int position = (int)key[i] - 'a';
		if (temp->getChildren(position)) {

			TrieNode* rm_node = temp->getChildren(position);
			temp->setChildren(position, nullptr);
			free_trienode(rm_node);
		}
	}
	return;
}

int TRIETree::check_divergance(string key)
{
	TrieNode* temp = root;
	if (key.empty()) return 0;

	int last_index = 0;							// indeks potencjalnego rozgalezienia w drzewie na przeszukiwanej galezi
	for (int i = 0; i < key.length() + 1; i++)	// przechodzimy przez kazda litere klucza
	{
		bool help = false;						// pomocnicza flaga 
		if (i == key.length())	help = true;	// ustawiana kiedy wszystkie litery zostana sprawdzone
		int position = key[i] - 'a';
		if (help or temp->getChildren(position))
		{
			for (int j = 0; j < alphabetSize; j++)
			{
				if (j != position and temp->getChildren(j)) // dla danej litery szukamy potomkow innych niz w podanym kluczu
				{
					last_index = i + 1;						// jezeli istnieja to zmieniamy indeks rozgalezienia
					break;
				}  // sprawdzamy czy potomek danej litery (bedacy czescia klucza) jest koncem innego slowa
				if (i < key.length() - 1 and j == position and temp->getChildren(j)->getFlag())
				{
					last_index = i + 2; // jesli jest to zmieniamy indeks rozgalezienia, bedziemy usuwac slowo od tego miejsca
					break;
				}
			}
			temp = temp->getChildren(position);
		}
	}
	return last_index;
}

string TRIETree::find_longest_prefix(string key)
{
	if (key.empty()) return "";

	string longest_prefix;
	for (int i = 0; i < key.length(); i++)
	{
		longest_prefix += key[i];
	}
	int branch_indx = check_divergance(longest_prefix) - 1; // otrzymujemy indeks rozgalezienia
	if (branch_indx >= 0)
	{
		int prefix_length = longest_prefix.length();
		for (int i = branch_indx; i < prefix_length; i++)
		{
			longest_prefix.pop_back();						// usuwamy czesc slowa po rozgalezieniu
		}
	}
	return longest_prefix;
}

void TRIETree::print_trie(TrieNode* node) {
	if (this->empty() == true) {
		std::cout << "Puste drzewo";
	}
	else if (node == nullptr) return;
	else {
		TrieNode* tmp = node;
		if (tmp->getData() != '\0') cout << tmp->getData();
		if (tmp->getFlag()) cout << "|";
		if (tmp->emptyChildren()) cout << "*";
		for (int i = 0; i < alphabetSize; i++) {

			if (node == root and tmp->getChildren(i) != NULL)
				cout << "---";

			if (node != root and tmp->multipleChildren())	// wypisuje branche od slow
				if (tmp->getChildren(i) != NULL)
					cout << " ->";

			print_trie(tmp->getChildren(i));				// rekursywnie wypisuje potomkow

			if (node == root and tmp->getChildren(i) != NULL) // endl po zakonczonej galezi idacej od roota
				cout << endl;
		}
	}
}

int main()
{
	TRIETree drzewko;
	cout << "Tworzymy drzewo Trie i dodajemy do niego slowa: word, wordcaps, capslock, maximum, marek\n";
	drzewko.insert("word");
	drzewko.insert("wordcaps");
	drzewko.insert("capslock");
	drzewko.insert("maximum");
	drzewko.insert("marek");

	cout << "Wyswietlamy drzewo, przyjmujac oznaczenia\n";
	cout << " ---\t- galaz idaca od roota\n |\t- koniec slowa\n ->\t- poczatek rozgalezienia\n *\t- koniec galezi\n\n";
	drzewko.print_trie(drzewko.getRoot());

	cout << endl;
	cout << "Dodajemy do drzewa slowa 'caps' i 'cap'\n";
	drzewko.insert("caps");
	drzewko.insert("cap");
	cout << endl;
	drzewko.print_trie(drzewko.getRoot());

	cout << endl;
	cout << "Dodajemy slowo 'can'\n";
	drzewko.insert("can");
	cout << endl;
	drzewko.print_trie(drzewko.getRoot());
	cout << endl;

	cout << "Dodajemy do drzewa slowa 'ONE' i 'CAPPA'\n";
	drzewko.insert("ONE");
	drzewko.insert("CAPPA");
	cout << endl;
	drzewko.print_trie(drzewko.getRoot());
	cout << endl;

	cout << "Sprawdzamy czy slowo 'capslock' jest w drzewie (t/f):";
	cout << drzewko.search_check("capslock");
	cout << endl;
	cout << endl;

	cout << "Usuwamy z drzewa slowo 'wordcaps'\n";
	drzewko.del_word("wordcaps");
	cout << endl;
	drzewko.print_trie(drzewko.getRoot());
	cout << endl;

	cout << "Usuwamy z drzewa slowa 'cap' i 'caps'\n";
	drzewko.del_word("caps");
	drzewko.del_word("cap");
	cout << endl;
	drzewko.print_trie(drzewko.getRoot());
	cout << endl;

	cout << "Usuwamy z drzewa slowo 'capslock'\n";
	drzewko.del_word("capslock");
	cout << endl;
	drzewko.print_trie(drzewko.getRoot());
	cout << endl;

	cout << "Usuwamy z drzewa slowo 'one'\n";
	drzewko.del_word("one");
	cout << endl;
	drzewko.print_trie(drzewko.getRoot());
	cout << endl;

	cout << "Sprawdzamy czy slowo 'capslock' jest w drzewie (t/f):";
	cout << drzewko.search_check("capslock");
	cout << endl;
	cout << "Sprawdzamy czy slowo 'word' jest w drzewie (t/f):";
	cout << drzewko.search_check("word");
	cout << endl;
	cout << "Sprawdzamy czy slowo 'one' jest w drzewie (t/f):";
	cout << drzewko.search_check("one");
	cout << endl;
	cout << endl;


	return 0;
}