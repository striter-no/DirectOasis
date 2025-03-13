#pragma once
#include <iostream>
#include <memory>
#include <vector>

namespace utils::node{

    class Node : public std::enable_shared_from_this<Node> {
        std::string notes;
        public:
        
        std::shared_ptr<Node> parent = nullptr;
        std::vector<std::shared_ptr<Node>> children;

        Node(std::string notes = "") : notes(notes) {}

        void addChild(const std::shared_ptr<Node> child) {
            child->parent = shared_from_this();
            children.push_back(child);
        }

        void print(const std::string& prefix = "") const {
            std::cout << prefix << notes << std::endl;
            for (const auto& child : children) {
                child->print(prefix + "\t");
            }
        }

        void getReversed(std::shared_ptr<Node> &out){
            out->addChild(shared_from_this());
            if(parent) parent->getReversed(out);
        }

        std::string getString(std::string prefix = "") const {
            std::string result = prefix + notes + "\n";
            for (const auto& child : children) {
                result += child->getString(prefix + "->");
            }
            return result;
        }
    };

}