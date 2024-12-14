#include "huffman.hpp"
#include <queue>
#include <iostream>

/*
frequency table for "hello":
h: 1
e: 1
l: 2
o: 1
total: 5 chars = 40 bits originally

tree building steps:
1. start with smallest frequencies:
   h(1), e(1), o(1), l(2)

2. build tree bottom-up:
                (5)
                / \
               /   \
             (3)   l(2)
            /   \
         h(1)   (2)
                /  \
              e(1) o(1)

3. final codes:
h: 00  (left, left)
e: 010 (left, right, left)
l: 1   (right)
o: 011 (left, right, right)

4. encoding "hello":
h: 00
e: 010
l: 1
l: 1
o: 011

initial bits: 01101000 01100101 01101100 01101100 01101111 (40 bits) -> used online string to bits converter
final bits: 0010110011 (10 bits)
saved 30 bits! (40 -> 10)
although we also send the huffman table with it thus we send 10 bits + 8*4 + 9 bits ig which might not be actually compressed in this case but when sentences and examples are having repeated characters, it will be compressed
*/

void HuffmanCompressor::buildFrequencyTable(const std::string& input) {
    frequency.clear();
    for (char c : input) {
        frequency[c]++;
    }
}

std::unique_ptr<HuffmanCompressor::Node> HuffmanCompressor::buildHuffmanTree(
    const std::map<char, int>& freq) {
    std::priority_queue<
        std::unique_ptr<Node>,
        std::vector<std::unique_ptr<Node>>,
        CompareNodes
    > pq;
    
    for (const auto& [ch, f] : freq) {
        pq.push(std::make_unique<Node>(ch, f));
    }

    while (pq.size() > 1) {
        auto left = std::move(const_cast<std::unique_ptr<Node>&>(pq.top())); 
        pq.pop();
        auto right = std::move(const_cast<std::unique_ptr<Node>&>(pq.top()));
        pq.pop();
        
        auto parent = std::make_unique<Node>('\0', left->frequency + right->frequency);
        parent->left = std::move(left);
        parent->right = std::move(right);
        
        pq.push(std::move(parent));
    }
    
    return pq.empty() ? nullptr : std::move(const_cast<std::unique_ptr<Node>&>(pq.top()));
}

std::vector<uint8_t> HuffmanCompressor::packBits(
    const std::vector<bool>& bits, size_t& validBits) {
    std::vector<uint8_t> bytes((bits.size() + 7) / 8, 0);
    validBits = bits.size();
    
    for (size_t i = 0; i < bits.size(); ++i) {
        if (bits[i]) {
            bytes[i/8] |= (1 << (7 - (i % 8)));
        }
    }
    
    return bytes;
}

CompressedData HuffmanCompressor::compress(const std::string& input) {
    if (input.empty()) return CompressedData{};
    
    buildFrequencyTable(input);
    auto root = buildHuffmanTree(frequency);
    
    codes.clear();
    std::vector<bool> code;
    generateCodes(root.get(), code);
    
    std::vector<bool> allBits;
    for (char c : input) {
        const auto& charCode = codes[c];
        allBits.insert(allBits.end(), charCode.begin(), charCode.end());
    }
    
    size_t validBits;
    auto packedData = packBits(allBits, validBits);
    
    return CompressedData{packedData, validBits, frequency};
}


void HuffmanCompressor::generateCodes(Node* root, std::vector<bool>& code) {
    if (!root) return;
    
    if (!root->left && !root->right) {
        codes[root->data] = code;
        return;
    }
    
    // generate code for left subtree (add 0)
    code.push_back(false);
    generateCodes(root->left.get(), code);
    code.pop_back();
    
    // generate code for right subtree (add 1)
    code.push_back(true);
    generateCodes(root->right.get(), code);
    code.pop_back();
}

std::string HuffmanCompressor::decompress(const CompressedData& compressed) {
    if (compressed.data.empty()) return "";
    
    auto root = buildHuffmanTree(compressed.freqTable);
    if (!root) return "";
    
    std::string result;
    Node* current = root.get();
    size_t bitCount = 0;
    
    // Print initial tree
    std::cout << "\nHuffman Tree Structure:\n";
    printTree(root.get(), "", true);
    std::cout << "\nDecoding Process:\n";
    
    for (uint8_t byte : compressed.data) {
        for (int i = 7; i >= 0 && bitCount < compressed.validBits; --i) {
            bool bit = (byte >> i) & 1;
            bitCount++;
            
            // Print path taken
            std::cout << "Bit: " << bit << " -> ";
            current = bit ? current->right.get() : current->left.get();
            
            if (!current->left && !current->right) {
                std::cout << "Found: '" << current->data << "'\n";
                result += current->data;
                current = root.get();
            } else {
                std::cout << "Internal node (freq: " << current->frequency << ")\n";
            }
        }
    }
    
    return result;
}

// Add this helper function to HuffmanCompressor class
void HuffmanCompressor::printTree(Node* root, std::string prefix, bool isLeft) {
    if (!root) return;
    
    std::cout << prefix;
    std::cout << (isLeft ? "├──" : "└──");
    
    if (root->isLeaf()) {
        std::cout << "'" << root->data << "' (" << root->frequency << ")\n";
    } else {
        std::cout << "(" << root->frequency << ")\n";
    }
    
    printTree(root->left.get(), prefix + (isLeft ? "│   " : "    "), true);
    printTree(root->right.get(), prefix + (isLeft ? "│   " : "    "), false);
}