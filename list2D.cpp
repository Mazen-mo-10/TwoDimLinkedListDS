#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define vi vector<int> 
#define vs vector<string>  
#define vl vector<long long>  
#define nul nullptr 
#define str string
class List2Dim {
private:
    class Node { // by use smart Pinters
    public:
        int data;
        shared_ptr<Node> next, down; 
        weak_ptr<Node> prev, up;  
        explicit Node(int val) : data(val) {}
    };
    using NodePtr = shared_ptr<Node>; 
    NodePtr head = nul;
    int rowsCt = 0;
    class RowProxy { // for [][] use in function in class 2D
        NodePtr Rrow;
    public:
        explicit RowProxy(NodePtr r) : Rrow(r) {}
        int& operator[](int y) {
            NodePtr col = Rrow;
            for (int i = 0; i < y && col; ++i) col = col->next;
            return col->data;
        }
    };
public: 
    List2Dim() = default;
    List2Dim(const List2Dim& other) { // Copy Constructor
        head = nul; rowsCt = 0;
        for (NodePtr row = other.head; row; row = row->down) {
            vector<int> tempRow;
            for (NodePtr col = row; col; col = col->next) {
                tempRow.push_back(col->data);
            }
            addRowArray(tempRow.data(), tempRow.size());
        }
    }
    ~List2Dim() { clear(); }
    bool empty() const { return !head; }
    int  rowCount() const { return rowsCt; } 
    class Iterator { 
        NodePtr row, col;
    public:
        explicit Iterator(NodePtr start) : row(start), col(start) {}
        int& operator*() const { return col->data; }
        Iterator& operator++() {
            if (col && col->next) {
                col = col->next;
            }
            else if (row) {
                row = row->down;
                col = row;
            }
            return *this;
        }
        Iterator operator++(int) { 
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        Iterator& operator--() {
            auto p = col ? col->prev.lock() : nul; 
            if (p) 
                col = p;
            else if (row) { 
                auto u = row->up.lock();
                row = u;  col = row; 
            }
            return *this;
        }
        Iterator operator--(int) {
            Iterator tmp = *this; 
            --(*this);
            return tmp;
        }
        bool operator!=(const Iterator& other) const { return col != other.col; }
        bool operator==(const Iterator& other) const { return col == other.col; }
        bool isRowEnd() const { return col && col->next == nul; }
    };
    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nul); }
    Iterator begin() const { return Iterator(head); }
    Iterator end()   const { return Iterator(nul); }
    void addRowEnd(int value) {
        NodePtr node = make_shared<Node>(value); 
        if (!head) head = node;
        else {
            NodePtr tail = head;
            while (tail->down) tail = tail->down;
            tail->down = node;
            node->up = tail;
        }
        ++rowsCt;
    }
    void addRowBeg(int value) {
        NodePtr node = make_shared<Node>(value);
        if (!head) head = node;
        else {
            node->down = head;
            head->up = node;
            head = node;
        }
        ++rowsCt;
    }
    void addByIndex(int x, int y, int value) {
        if (x < 0 || x >= rowsCt) return;
        NodePtr row = head; 
        for (int i = 0; i < x; ++i) row = row->down; 
        NodePtr node = make_shared<Node>(value); 
        if (y == 0) {
            node->next = row; 
            if (row) row->prev = node;
            if (auto up = row->up.lock()) {
                node->up = up;
                up->down = node;
            }
            if (auto down = row->down) {
                node->down = down;
                down->up = node;
            }
            if (x == 0) head = node;
            else {
                NodePtr upRow =  head;
                for (int i = 0; i < x - 1; ++i) upRow = upRow->down;
                upRow->down = node;
            }
        }
        else {
            NodePtr prev = row;
            for (int i = 0; i < y - 1 && prev->next; ++i) prev = prev->next;
            node->next = prev->next;
            if (prev->next) prev->next->prev = node;
            prev->next = node;
            node->prev = prev;
            NodePtr upRow = row->up.lock();
            for (int i = 0; i < y && upRow; ++i) upRow = upRow->next;
            if (upRow) { node->up = upRow; upRow->down = node; }
            NodePtr downRow = row->down;
            for (int i = 0; i < y && downRow; ++i) downRow = downRow->next;
            if (downRow) { node->down = downRow; downRow->up = node; }
        }
    }
    void removeRow(int x) {
        if (x < 0 || x >= rowsCt || !head) return;
        NodePtr target = head;
        for (int i = 0; i < x; ++i) target = target->down;
        auto up = target->up.lock();
        auto down = target->down;
        if (up) up->down = down;
        else head = down;
        if (down) down->up = up;
        while (target) {
            auto next = target->next;
            target->next = nul; 
            target->prev.reset();
            target->up.reset();
            target->down = nul;
            target = next;
        }
        --rowsCt; 
    }
    void removeCol(int y) {
        if (!head || y < 0) return;
        NodePtr row = head;
        while (row) {
            NodePtr col = row;
            int idx = 0;
            while (col && idx < y) {
                col = col->next;
                ++idx;
            }
            NodePtr nextRow = row->down;
            if (col) {
                auto left = col->prev.lock();
                auto right = col->next;
                if (left)  left->next = right;
                else {
                    if (row == head) head = right;
                    else {
                        auto upRow = row->up.lock();
                        if (upRow) upRow->down = right;
                    }
                }
                if (right) {
                    right->prev = left;
                }
                auto up = col->up.lock();
                auto down = col->down;
                if (up) up->down = down;
                if (down) down->up = up;
                col->next = nul;
                col->down = nul;
                col->prev.reset();
                col->up.reset();
            }
            row = nextRow;
        }
    }
    int getDataIndex(int x, int y) const {
        if (x < 0 || x >= rowsCt || !head) return -1;
        NodePtr row = head; 
        for (int i = 0; i < x; ++i) row = row->down;
        NodePtr col = row; 
        for (int i = 0; i < y && col; ++i) col = col->next;
        return col ? col->data : -1;
    }
    int countRowEle(int x) const {
        if (x < 0 || x >= rowsCt || !head) return -1;
        NodePtr row = head; 
        for (int i = 0; i < x; ++i) row = row->down;
        int cnt = 0;
        while (row) { ++cnt; row = row->next; }
        return cnt;
    }
    void mapFunction(const function<int(int)>& fn) {
        for (auto& val : *this) val = fn(val);
    }
    void clear() {
        head = nul; 
        rowsCt = 0;
    }
    void printAll() const {
        int r = 0;
        for (NodePtr row = head; row; row = row->down, ++r) {
            cout << "Row " << r << ": ";
            for (NodePtr col = row; col; col = col->next) cout << col->data << ' ';
            cout << '\n';
        }
        cout << '\n';
    }
    RowProxy operator[](int x) {
        NodePtr row = head;
        for (int i = 0; i < x && row; ++i) row = row->down;
        return RowProxy(row);
    }
    pair<int, int> operator()(int value) const {
        int rx = 0;
        for (NodePtr row = head; row; row = row->down, ++rx) { 
            int cx = 0;
            for (NodePtr col = row; col; col = col->next, ++cx) { 
                if (col->data == value) return { rx, cx };
            }
        }
        return { -1,-1 };
    }
    List2Dim operator+(const List2Dim& other) const {
        List2Dim res;
        for (auto v : *this) res.addRowEnd(v);
        for (auto v : other) res.addRowEnd(v);
        return res;
    }
    bool operator==(const List2Dim& other) const {
        Iterator it1 = begin(), end1 = end();
        Iterator it2 = other.begin(), end2 = other.end();
        while (it1 != end1 && it2 != end2) {
            if (*it1 != *it2) return false;
            ++it1; ++it2;
        }
        return it1 == end1 && it2 == end2;
    }
    List2Dim& operator=(const List2Dim& other) {
        if (this != &other) {
            clear();
            for (NodePtr row = other.head; row; row = row->down) {
                vector<int> tempRow;
                for (NodePtr col = row; col; col = col->next) {
                    tempRow.push_back(col->data);
                }
                addRowArray(tempRow.data(), tempRow.size());
            }
        }
        return *this;
    }
    friend ostream& operator<<(ostream& os, const List2Dim& list) {
        int r = 0;
        for (auto row = list.head; row; row = row->down, ++r) {
            os << "Row " << r << ": ";
            for (auto col = row; col; col = col->next)
                os << col->data << ' ';
            os << '\n';
        }
        return os;
    }
    void reverseRows() {
        if (!head || rowsCt < 2) return;
        NodePtr cur = head, prev = nul;
        while (cur) {
            NodePtr nxt = cur->down;
            cur->down = prev;
            cur->up = nxt;
            prev = cur;
            cur = nxt;
        }
        head = prev;
    }
    void addRowArray(int* arr, int size) {
        if (!arr || size <= 0) return;
        addRowEnd(arr[0]);
        int r = rowCount() - 1;
        for (int i = 1; i < size; ++i) addByIndex(r, i, arr[i]);
    }
    void addColArray(int* arr, int size) {
        if (!arr || size <= 0) return;
        for (int i = 0; i < size; ++i) addByIndex(i, countRowEle(i), arr[i]);
    }
    void add2DArray(int arr[3][3], int x, int y) {
        for (int i = 0; i < x; ++i) addRowArray(arr[i], y);
    }
};
int doubleValue(int x) { return x * 2; } // for test my function mapFunction
int main() {
    ios::sync_with_stdio(false); 
    cin.tie(nul); 

    // Tests of All Functions 2D Linked List : 
    List2Dim ls; // object 1
    ls.addRowEnd(2); ls.addRowEnd(3); ls.addRowBeg(1);
    ls.addByIndex(0, 1, 10); ls.addByIndex(1, 1, 100); ls.addByIndex(2, 1, 1000); ls.addByIndex(1, 2, 200); ls.addByIndex(0, 0, 22);  
    ls.printAll();
    cout << ls.getDataIndex(1, 1) << endl;
    cout << ls[1][1] << endl;
    cout << "Index of Number 100 in 2D linked list : [" << ls(100).first << "][" << ls(100).second << "]" << endl;
    cout << ls.countRowEle(1) << endl;
    ls.mapFunction(doubleValue); ls.printAll();
    int arr[] = { 11,12,13,14,15 }; 
    ls.addRowArray(arr, 5); ls.printAll();   
    int arr2[] = { 21,22,23 };
    ls.addColArray(arr2, 3); ls.printAll();
    List2Dim d2;  // object 2
    d2.addRowArray(arr, 5); d2.addRowArray(arr, 5); d2.printAll();
    List2Dim res = ls + d2;  res.printAll(); // object 3
    if (d2 == ls)  cout << "Are Equal!!" << endl; 
    else cout << "Are not Equal!!!" << endl; 
    int arr2d[3][3] = { {1,2,3},{4,5,6},{7,8,9} }; 
    ls.add2DArray(arr2d, 3, 3); ls.printAll();
    ls.reverseRows(); ls.printAll();
    ls.removeCol(1); ls.printAll();
    for (List2Dim::Iterator it = ls.begin(); it != ls.end(); ++it) { cout << *it << ' ';if (it.isRowEnd()) cout << '\n'; } // print By Iterator
    cout << ls << endl;
    List2Dim a; // test Copy Constructor
    int arrt[] = { 1, 2, 3 };
    a.addRowArray(arrt, 3);
    List2Dim b = a; 
    a[0][0] = 100;
    b.printAll(); 
    List2Dim c;  // test 2
    c = a; 
    a[0][1] = 200; 
    c.printAll();

    return 0;
}
