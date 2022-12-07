#include <iostream>
#include <string>
#include <vector>

#define SIZE_OF_ABC 27
#define INF INT_MAX

struct SuffixTree {
    struct Node {
        // Количество символов от корня до самой вершины
        int offset;
        // индекс первого символа ребра к которому идет вершина
        int start;
        Node *parent;
        // Массив указателей на вершины
        std::vector<Node *> link;
        Node *sufLink;

        Node(int offset, int start, Node *parent) {
            this->offset = offset;
            this->start = start;
            this->parent = parent;
            link = std::vector<Node *>(SIZE_OF_ABC);
            sufLink = nullptr;
        }
    };

private:
    // Корень
    Node *root;
    // Текущая вершина
    Node *curNode;
    // curSubstring = [L; R)
    // Индекс левого края текущей подстроки
    int L;
    // Индекс правого края текущей подстроки
    int R;
    // Массив номеров букв, полученный в результате разбиения начальной строки
    // (с учетом символа, который ни разу не встречался в строке)
    std::vector<int> s;

    Node *noSuffixLink = nullptr;

    // Отчищает память, занятой вершиной deletedNode суффиксного дерева.
    void freeTree(Node *deletedNode) {
        for (int i = 0; i < SIZE_OF_ABC; i++) {
            if (deletedNode->link[i] != nullptr)
                freeTree(deletedNode->link[i]);
        }

        delete deletedNode;
    }

    // Создает суффиксное дерево по строке записанной в переменную s.
    void creatTree() {
        // Двигаем индекс R в случае перехода по символу
        // Двигаем индекс L в случае перехода по суффиксной ссылке
        while (R < s.size()) {
            addSymbol();
            R++;
        }
    }

    void addSymbol() {
        while (L <= R) {
            bool isTransitionBySymbol = goDown(R);
            if (noSuffixLink != nullptr) {
                noSuffixLink->sufLink = curNode->parent;
                noSuffixLink = nullptr;
            }
            // Проверяем, произошел ли переход по символу
            if (isTransitionBySymbol) {
                curNode = curNode->parent;
                if (curNode->sufLink == nullptr) {
                    noSuffixLink = curNode;
                    curNode = curNode->parent;
                }
                curNode = curNode->sufLink;
                L++;
                // Производим спуск, пока не окажемся в позиции соответствующей текущей подстроке
                while (curNode->offset < R - L) {
                    goDown(L + curNode->offset);
                }
            } else {
                break;
            }
        }
    }

    // Возвращает значение истинно, если не произошел переход по символу, иначе - ложь.
    // Функция спускается по дереву по букве с индексом right.
    bool goDown(int right) {
        // symbol - символ по индексу right
        int symbol = s[right];
        // Смотрим, где находимся
        if (curNode->offset > right - L) { // Мы на ребре
            // находим количество символов от начала этого ребра до позиции соответствующей текущей подстроке
            int edgeOffset = right - L - curNode->parent->offset;
            int symbolOnEdge = s[curNode->start + edgeOffset];
            if (symbolOnEdge != symbol) {
                // Разбиваем ребро
                auto *newNode = new Node(right - L, curNode->start, curNode->parent);
                curNode->parent->link[s[curNode->start]] = newNode;
                curNode->parent = newNode;
                curNode->start += edgeOffset;
                newNode->link[symbolOnEdge] = curNode;
                curNode = newNode;
            }
        }
        if (curNode->offset == right - L) { // Мы в вершине
            if (curNode->link[symbol] == nullptr) { // нет перехода по букве, создаем новой лист
                auto newLeaf = new Node(INF, right, curNode);
                curNode->link[symbol] = newLeaf;
                curNode = newLeaf;
                return true;
            } else { // Переходим по символу
                curNode = curNode->link[symbol];
            }
        }
        return false;
    }

public:
    SuffixTree(std::string str) {
        this->s = std::vector<int>(str.size() + 1);
        for (int i = 0; i < str.length(); i++) {
            this->s[i] = str[i] - 'a' + 1;
        }
        L = 0;
        R = 0;
        root = new Node(0, -1, nullptr);
        curNode = root;
        root->sufLink = root;

        creatTree();
    }

    ~SuffixTree() {
        freeTree(root);
    }

    void makeCurNodeToRoot() {
        curNode = root;
    }

    Node *ptrRoot() {
        return root;
    }

    // Возвращает true, если следующий символ от позиции cur_offset равен c, иначе - false.
    bool checkNextSymbol(char c, int &cur_offset) {
        if (cur_offset == curNode->offset) {
            if (curNode->link[c - 'a' + 1]) {
                curNode = curNode->link[c - 'a' + 1];
                cur_offset++;
                return true;
            }

            return false;
        } else {
            if (s[curNode->start + cur_offset - curNode->parent->offset] == c - 'a' + 1) {
                cur_offset++;
                return true;
            }
            return false;
        }
    }

    // Возвращает true, если строка substr присутствует в суффиксном дереве, иначе - false.
    bool findSubstr(std::string &substr) {
        curNode = root;
        int cur_offset = 0;
        for (const auto &c: substr) {
            if (!checkNextSymbol(c, cur_offset))
                return false;
        }
        return true;
    }

    // Возвращает количество листьев исходящих из вершины node.
    long long getCountOfLeaf(Node *node) {
        if (node->offset == INF)
            return 1;
        long long count = 0;
        for (int i = 0; i < SIZE_OF_ABC; i++) {
            if (node->link[i] != nullptr) {
                count += getCountOfLeaf(node->link[i]);
            }
        }
        return count;
    }

    // Возвращает количество вхождений строки str в суффиксное дерево.
    long long countOfSubstrEntry(std::string &str) {
        if (!findSubstr(str))
            return 0;

        return getCountOfLeaf(curNode);
    }
};

