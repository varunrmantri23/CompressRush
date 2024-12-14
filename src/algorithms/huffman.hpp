// huffman.hpp
#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

struct CompressedData {
    std::vector<uint8_t> data;
    size_t validBits;  
    std::map<char, int> freqTable;  
};

// Forward declare CompareNodes
struct CompareNodes;

class HuffmanCompressor {
public:
    CompressedData compress(const std::string& input);
    std::string decompress(const CompressedData& compressed);

private:
    struct Node {
        char data;
        int frequency;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        
        Node(char d, int freq) : data(d), frequency(freq) {}
        bool isLeaf() const { return !left && !right; }
    };


    void printTree(Node* root, std::string prefix, bool isLeft);
    // Proper friend declaration cause issues with Node struct
    friend struct CompareNodes;
    std::map<char, std::vector<bool>> codes;  // stores binary codes for each char
    std::map<char, int> frequency;  // tracks char frequencies
    
    void buildFrequencyTable(const std::string& input);
    std::unique_ptr<Node> buildHuffmanTree(const std::map<char, int>& freq);
    void generateCodes(Node* root, std::vector<bool>& code);
    std::vector<uint8_t> packBits(const std::vector<bool>& bits, size_t& validBits);
};

// sorts nodes by frequency for building optimal tree
struct CompareNodes {
    bool operator()(const std::unique_ptr<HuffmanCompressor::Node>& a, 
                   const std::unique_ptr<HuffmanCompressor::Node>& b) const {
        return a->frequency > b->frequency;
    }
};