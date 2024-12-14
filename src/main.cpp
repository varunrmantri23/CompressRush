#include <iostream>
#include <iomanip>
#include "algorithms/huffman.hpp"

int main() {
    try {
        HuffmanCompressor compressor;
        std::string input = "hello";
        
        // Compress
        CompressedData compressed = compressor.compress(input);
        
        // Calculate sizes
        size_t originalSize = input.size();
        size_t compressedSize = compressed.data.size();
        double ratio = (double)compressedSize / originalSize * 100;
        
        // Print statistics
        std::cout << "Compression Statistics:\n";
        std::cout << "---------------------\n";
        std::cout << "Original size: " << originalSize << " bytes\n";
        std::cout << "Compressed size: " << compressedSize << " bytes\n";
        std::cout << "Compression ratio: " << std::fixed << std::setprecision(2) 
                  << ratio << "%\n";
        std::cout << "Valid bits: " << compressed.validBits << "\n";
        std::cout << "Frequency table size: " << compressed.freqTable.size() << "\n\n";
        
        // Decompress and verify
        std::string decompressed = compressor.decompress(compressed);
        std::cout << "Original text: " << input << "\n";
        std::cout << "Decompressed: " << decompressed << "\n";
        std::cout << "Verification: " << (input == decompressed ? "SUCCESS" : "FAILED") << "\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}