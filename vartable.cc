#include <iostream>

#include "vartable.h"


VarTable::VarTable(){
	for (int i=0; i<HTSIZE; i++)
		hashtable[i] = nullptr;
}

int VarTable::genhash(std::string name){
	int hash = 0;
	for (int i=0; i<5 && i<name.length(); i++)
		hash += name[i];
	hash = hash % HTSIZE;
	return hash;
}

void VarTable::addvar(std::string name, Node& value){
	int hash = genhash(name);
	// Copy
	TableEntry* tableentry = new TableEntry(name, value);
	// Add to hashtable
	if (hashtable[hash] != nullptr)
		tableentry->next = hashtable[hash];
	hashtable[hash] = tableentry;
	std::cout << "Added variable " << name << std::endl;
}

bool VarTable::delvar(std::string name){
	int hash = genhash(name);
	TableEntry* te = hashtable[hash];
	TableEntry* prev = nullptr;
	bool found = false;
	while (found == false && te != nullptr){
		Node* vnode = &te->value;
		if (te->name == name){
			found = true;
			prev->next = te->next;
			std::cout << "Deleted variable " << name << std::endl;
			delete te;
		}
		else {
			prev = te;
			te = te->next;
		}
	}
	return found;
}

Node& VarTable::getvar(std::string name){
	int hash = genhash(name);
	TableEntry* te = hashtable[hash];
	Node* vnode = nullptr;
	bool found = false;
	while (found == false && te != nullptr){
		vnode = &te->value;
		if (te->name == name){
			found = true;
		}
		else {
			te = te->next;
		}
	}
	if (vnode == nullptr){
		std::cout << "Variable " << name << " is not defined" << std::endl;
		exit(-1);
	}
	return *vnode;
}
