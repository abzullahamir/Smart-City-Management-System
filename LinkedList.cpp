#include "LinkedList.h"

StringNode::StringNode(string val) : data(val), next(nullptr) {}

StringLinkedList::StringLinkedList() : head(nullptr), size(0) {}

StringLinkedList::~StringLinkedList() {
    clear();
}

void StringLinkedList::append(string data) {
    StringNode* newNode = new StringNode(data);
    if (!head) {
        head = newNode;
    }
    else {
        StringNode* temp = head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    size++;
}

void StringLinkedList::prepend(string data) {
    StringNode* newNode = new StringNode(data);
    newNode->next = head;
    head = newNode;
    size++;
}

string StringLinkedList::get(int index) {
    if (index < 0 || index >= size) {
        return "";
    }
    StringNode* temp = head;
    for (int i = 0; i < index; i++) {
        temp = temp->next;
    }
    return temp->data;
}

bool StringLinkedList::contains(string data) {
    StringNode* temp = head;
    while (temp) {
        if (temp->data == data) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

int StringLinkedList::getSize() const {
    return size;
}

bool StringLinkedList::isEmpty() const {
    return head == nullptr;
}

void StringLinkedList::clear() {
    while (head) {
        StringNode* temp = head;
        head = head->next;
        delete temp;
    }
    size = 0;
}

void StringLinkedList::display() {
    StringNode* temp = head;
    while (temp) {
        cout << temp->data;
        if (temp->next) cout << " -> ";
        temp = temp->next;
    }
    cout << endl;
}

StringNode* StringLinkedList::getHead() const {
    return head;
}