#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class MinHeap {
    std::vector<int> values;
    void heapifyUp(std::size_t child) {
        while (child != 0) {
            const auto parent = (child - 1) / 2;
            if (values[parent] <= values[child]) return;
            std::swap(values[parent], values[child]);
            child = parent;
        }
    }
    void heapifyDown(std::size_t parent) {
        while (2 * parent + 1 < values.size()) {
            auto child = 2 * parent + 1;
            if (child + 1 < values.size() &&
                values[child + 1] < values[child]) ++child;
            if (values[parent] <= values[child]) return;
            std::swap(values[parent], values[child]);
            parent = child;
        }
    }
public:
    void insert(int value) {
        values.push_back(value);
        heapifyUp(values.size() - 1);
    }
    std::optional<int> peekMin() const {
        if (values.empty()) return std::nullopt;
        return values.front();
    }
    std::optional<int> removeMin() {
        if (values.empty()) return std::nullopt;
        const int minimum = values.front();
        values.front() = values.back();
        values.pop_back();
        if (!values.empty()) heapifyDown(0);
        return minimum;
    }
    bool valid() const {
        for (std::size_t i = 1; i < values.size(); ++i)
            if (values[(i - 1) / 2] > values[i]) return false;
        return true;
    }
    void print() const {
        std::cout << '[';
        for (std::size_t i = 0; i < values.size(); ++i)
            std::cout << (i ? ", " : "") << values[i];
        std::cout << "]\n";
    }
};

struct TreapNode {
    int key, priority;
    TreapNode* left = nullptr;
    TreapNode* right = nullptr;
    TreapNode(int k, int p) : key(k), priority(p) {}
};

class Treap {
    TreapNode* root = nullptr;
    int leftRotations = 0, rightRotations = 0;
    void rotateLeft(TreapNode*& node) {
        TreapNode* promoted = node->right;
        node->right = promoted->left;
        promoted->left = node;
        node = promoted;
        ++leftRotations;
    }
    void rotateRight(TreapNode*& node) {
        TreapNode* promoted = node->left;
        node->left = promoted->right;
        promoted->right = node;
        node = promoted;
        ++rightRotations;
    }
    bool add(TreapNode*& node, int key, int priority) {
        if (!node) {
            node = new TreapNode(key, priority);
            return true;
        }
        if (key == node->key) return false; // Keep the existing priority.
        if (key < node->key) {
            if (!add(node->left, key, priority)) return false;
            if (node->left->priority < node->priority) rotateRight(node);
        } else {
            if (!add(node->right, key, priority)) return false;
            if (node->right->priority < node->priority) rotateLeft(node);
        }
        return true;
    }
    static void clear(TreapNode* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }
    static bool validate(const TreapNode* node,
                         const TreapNode* lower, const TreapNode* upper) {
        if (!node) return true;
        if (lower && node->key <= lower->key) return false;
        if (upper && node->key >= upper->key) return false;
        if (node->left && node->left->priority < node->priority) return false;
        if (node->right && node->right->priority < node->priority) return false;
        return validate(node->left, lower, node) &&
               validate(node->right, node, upper);
    }
    static void collect(const TreapNode* node,
                        std::vector<std::pair<int,int>>& result) {
        if (!node) return;
        collect(node->left, result);
        result.emplace_back(node->key, node->priority);
        collect(node->right, result);
    }
    static void show(const TreapNode* node, const std::string& indent,
                     const std::string& label) {
        if (!node) return;
        std::cout << indent << label << node->key
                  << " (priority " << node->priority << ")\n";
        show(node->left, indent + "  ", "L: ");
        show(node->right, indent + "  ", "R: ");
    }
public:
    Treap() = default;
    ~Treap() { clear(root); }
    Treap(const Treap&) = delete;
    Treap& operator=(const Treap&) = delete;
    bool insert(int key, int priority) { return add(root, key, priority); }
    bool search(int key) const {
        const TreapNode* node = root;
        while (node) {
            if (node->key == key) return true;
            node = key < node->key ? node->left : node->right;
        }
        return false;
    }
    bool valid() const { return validate(root, nullptr, nullptr); }
    std::vector<std::pair<int,int>> inorder() const {
        std::vector<std::pair<int,int>> result;
        collect(root, result);
        return result;
    }
    void print() const { show(root, "", "Root: "); }
    int leftCount() const { return leftRotations; }
    int rightCount() const { return rightRotations; }
};

int checks = 0;
void check(bool passed, const std::string& description) {
    ++checks;
    std::cout << (passed ? "PASS: " : "FAIL: ") << description << '\n';
    if (!passed) throw std::runtime_error(description);
}
int main() {
    std::cout << "MIN-HEAP\n";
    MinHeap heap;
    check(!heap.peekMin() && !heap.removeMin(), "empty operations return no value");
    const std::vector<int> input = {37, 12, 58, -4, 23, 7, 41, 0, 19, 7, 65, 3};
    bool correct = true;
    for (int value : input) {
        heap.insert(value);
        correct &= heap.valid();
    }
    check(correct, "heap order holds after each of 12 insertions");
    std::cout << "After 12 inserts: "; heap.print();
    check(heap.peekMin() == -4, "peekMin is -4");
    check(heap.removeMin() == -4 && heap.valid(), "remove -4 and restore heap order");
    std::cout << "After removing -4: "; heap.print();
    heap.insert(-9);
    check(heap.peekMin() == -9 && heap.valid(), "insert -9 and restore heap order");
    std::cout << "After inserting -9: "; heap.print();
    check(heap.removeMin() == -9 && heap.valid(), "remove -9");
    std::cout << "After removing -9: "; heap.print();
    std::vector<int> removed;
    bool allValid = true;
    while (auto minimum = heap.removeMin()) {
        removed.push_back(*minimum);
        allValid &= heap.valid();
    }
    std::cout << "Drain order:";
    for (int value : removed) std::cout << ' ' << value;
    std::cout << '\n';
    check(removed == std::vector<int>({0,3,7,7,12,19,23,37,41,58,65}) &&
          allValid, "drain matches exact values, including duplicate 7");
    check(!heap.peekMin() && !heap.removeMin(), "heap is empty after draining");
    heap.insert(0);
    check(heap.removeMin() == 0 && !heap.peekMin(), "single zero is a valid value");

    std::cout << "\nTREAP (smaller priorities rise toward the root)\n";
    Treap tree;
    check(tree.valid() && !tree.search(42), "empty treap");
    const std::vector<std::pair<int,int>> pairs = {
        {46,70}, {21,45}, {73,60}, {9,80}, {34,20}, {59,35},
        {88,90}, {16,55}, {28,65}, {41,50}, {64,10}, {81,75}
    };
    bool inserted = true;
    for (const auto& item : pairs) {
        inserted &= tree.insert(item.first, item.second) && tree.valid();
        std::cout << "Insert (" << item.first << ',' << item.second << ")\n";
    }
    check(inserted, "BST and priority order hold after all 12 insertions");
    const std::vector<std::pair<int,int>> expected = {
        {9,80},{16,55},{21,45},{28,65},{34,20},{41,50},
        {46,70},{59,35},{64,10},{73,60},{81,75},{88,90}
    };
    check(tree.inorder() == expected, "inorder preserves every key-priority pair");
    bool foundAll = true;
    for (const auto& item : pairs) foundAll &= tree.search(item.first);
    check(foundAll, "all inserted keys found");
    check(tree.search(41) && !tree.search(42), "41 found; 42 absent");
    check(!tree.insert(34,1) && tree.inorder() == expected,
          "duplicate key rejected without changing priority");
    check(tree.leftCount() > 0 && tree.rightCount() > 0,
          "both rotation directions exercised");
    std::cout << "Rotations: left=" << tree.leftCount()
              << ", right=" << tree.rightCount() << '\n';
    std::cout << "Final structure (L/R labels identify children):\n";
    tree.print();
    std::cout << "Inorder (key,priority):";
    for (const auto& item : tree.inorder())
        std::cout << " (" << item.first << ',' << item.second << ')';
    std::cout << '\n';
    Treap ties;
    ties.insert(2,5); ties.insert(1,5); ties.insert(3,5);
    check(ties.valid() && ties.search(1) && ties.search(3),
          "equal priorities allowed; BST order maintained");
    std::cout << "\nAll " << checks << " checks passed.\n";
}
