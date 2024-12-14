// huffman.cpp
#include "huffman.hpp"
#include <queue>

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
    
    // Generate code for left subtree (add 0)
    code.push_back(false);
    generateCodes(root->left.get(), code);
    code.pop_back();
    
    // Generate code for right subtree (add 1)
    code.push_back(true);
    generateCodes(root->right.get(), code);
    code.pop_back();
}

std::string HuffmanCompressor::decompress(const CompressedData& compressed) {
    if (compressed.data.empty()) return "";
    
    // Rebuild Huffman tree using frequency table
    auto root = buildHuffmanTree(compressed.freqTable);
    if (!root) return "";
    
    std::string result;
    Node* current = root.get();
    size_t bitCount = 0;
    
    // Process each byte
    for (uint8_t byte : compressed.data) {
        // Process each bit in the byte
        for (int i = 7; i >= 0 && bitCount < compressed.validBits; --i) {
            bool bit = (byte >> i) & 1;
            bitCount++;
            
            // Navigate tree
            current = bit ? current->right.get() : current->left.get();
            
            // Found a leaf node (character)
            if (!current->left && !current->right) {
                result += current->data;
                current = root.get();
            }
        }
    }
    
    return result;
}