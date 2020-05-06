//
// Created by heeve on 27.03.20.
//

#include <unordered_map>
#include <set>
#include <queue>
#include "../headers/ahocorasick.h"
#include <string>
#include <iostream>


namespace aho_corasick {


    class Node {

        typedef Node *node_ptr;

    private:

        char value;
        std::set<std::string> retVals;
        std::unordered_map<char, node_ptr> children;

        node_ptr fail;

    public:

        Node(char init_val)
                : value(init_val), fail(nullptr), retVals(), children() {
        };

        ~Node() {
            if (!children.empty()) {
                for (auto &itr: children) {
                    delete (itr.second);
                }
            }
        }

        node_ptr getFail() {
            return fail;
        }

        std::unordered_map<char, node_ptr> *getChildren() {
            return &children;
        }


        std::set<std::string> *getRetVals() {
            return &retVals;
        }

        void addReturnValue(const std::string &added) {
            retVals.insert(added);
        }

        node_ptr addChild(const char &letter) {
            auto added = childAt(letter);
            if (added == nullptr) {
                added = new Node(letter);
                children[letter] = added;

            }
            return added;


        }

        void setFail(node_ptr failNode) {
            fail = failNode;
            for (auto &s: *failNode->getRetVals()) {
                addReturnValue(s);
            }
        }

        node_ptr childAt(const char &letter) {
            auto itr = (children).find(letter);
            if (itr != children.end()) {
                return children.at(letter);
            }
            return nullptr;
        }

        node_ptr nextNode(const char &letter) {
            node_ptr node = childAt(letter);
            node_ptr failNode = this;

            while (node == nullptr && failNode->value != '&') {
                failNode = failNode->getFail();
                node = failNode->childAt(letter);
            }
            return node;
        }

    };

    class AhoCorasick {
        typedef Node *node_ptr;

        node_ptr start = new Node('&');
        std::vector<std::string> markers;
        unsigned long longestMarker = 0;
    public:

        AhoCorasick() {
            start->setFail(start);
        }

        explicit AhoCorasick(std::vector<std::string> &markersInit) :
                markers(markersInit) {
            start->setFail(start);

            setUpTrie();
        }

        ~AhoCorasick() {
            delete (start);
        }

        void addMarker(std::string marker) {

            if (marker.empty())
                return;

            node_ptr node = start;
            for (const auto &ch: marker) {
                node = node->addChild(ch);
            }
            node->addReturnValue(marker);


        }

        void setUpTrie() {
            // Add nodes to the trie
            node_ptr node;
            for (auto &marker: markers) {
                longestMarker = std::max(longestMarker, marker.size());
                if (marker.empty())
                    continue;
                node = start;
                for (const auto &ch: marker) {
                    node = node->addChild(ch);
                }
                node->addReturnValue(marker);
            }
            addFails();
        }

        void addFails() {

            // Add start as fail to each child node of start
            std::queue<node_ptr> q;
            std::unordered_map<char, node_ptr> *children = start->getChildren();
            for (auto &itr : *children) {
                itr.second->setFail(start);
                q.push(itr.second);
            }

            node_ptr parent;
            node_ptr fail;
            node_ptr childFail;

            // Add fails to each other node
            while (!q.empty()) {
                parent = q.front();
                children = parent->getChildren();

                for (auto &itr: *children) {
                    q.push(itr.second);
                    fail = parent;

                    do {
                        fail = fail->getFail();
                        childFail = fail->childAt(itr.first);
                    } while (childFail == nullptr && fail != start);

                    if (childFail == nullptr) {
                        childFail = start;
                    }

                    itr.second->setFail(childFail);
                }
                q.pop();

            }


        }
        void
        matchWords(std::string text, int startInx, int endInx, std::unordered_map<std::string, std::set<int>> &matched) {

            Node *node = start;
            Node *child;
            for (int i = startInx; i < endInx; ++i) {
                child = node->childAt(text[i]);
                if (child != nullptr) {
                    node = child;
                    for (auto &returns: *node->getRetVals()) {
                        matched[returns].insert(i - returns.size() + 1);
                    }
                } else {
                    if (node != start) {

                        node = node->getFail();
                        --i;
                    }
                }
            }
        }
        void
        matchWords(std::string text, int startInx, int endInx,
                   std::unordered_map<std::string, std::set<int>> &matched) {

            node_ptr child = start;
            for (int i = startInx; i < endInx; ++i) {
                child = child->nextNode(text[i]);
                if (child == nullptr) {
                    child = start;
                    continue;
                }
                // ??? Don't know if it's a good practice
                for (auto &returns: *child->getRetVals()) {
                    matched[returns].insert(i - returns.size() + 1);
                }
            }
        }


        node_ptr getStart() {
            return start;
        }

        int getMaxMarker() {
            return longestMarker;
        }

    };
};

