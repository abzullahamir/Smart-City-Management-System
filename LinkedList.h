#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#pragma once

#include <iostream>
#include <string>
using namespace std;

class StringNode {
public:
	string data;
	StringNode* next;

	StringNode(string val);
};

class StringLinkedList {
private:
	StringNode* head;
	int size;

public:
	StringLinkedList();
	~StringLinkedList();

	void append(string data);
	void prepend(string data);
	string get(int index);
	bool contains(string data);
	int getSize() const;
	bool isEmpty() const;
	void clear();
	void display();
	StringNode* getHead() const;
};

#endif