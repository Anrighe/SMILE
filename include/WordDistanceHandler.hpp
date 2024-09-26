#include <string>
#include <vector>
#include <algorithm>
#include <map>


class WordDistanceHandler {

private:

    /**
     * This Damerau-Leveshtein word distance implementation follows the pseudocode found in this website https://hyperskill.org/learn/step/18819
     * The distance of two strings a and b, is defined using indices i and j to traverse them, and is calculated with the function d(i,j)
     * which is the minimum of the following values:
     * - 0                      if i == 0 and j == 0
     * - d(i, j-1) + 1          if i > 0
     * - d(i-1, j) + 1          if j > 0
     * - d(i-1, j-1)            if i,j > 0 and a[i] == b[j]
     * - d(i-1, j-1) + 1        if i,j > 0 and a[i] != b[j]
     * - d(i-2, j-2) + 1        if i,j > 0 and a[i] == b[j-1] and a[i-1] == b[j]
     * 
     * @param word1 The first string.
     * @param word2 The second string.
     * @param word1Index The current index in word1.
     * @param word2Index The current index in word2.
     *
     * @returns The distance (integer) between the two words: 0 if they are identical, and greater than 0 if they differ.
     */
    static int calculateDistance(std::string word1, std::string word2, int word1Index, int word2Index) {
        std::vector<int> results;

        if (word1Index == 0) return word2Index;
        if (word2Index == 0) return word1Index;

        if (word1Index > 0) {
            results.push_back(calculateDistance(word1, word2, word1Index-1 , word2Index) + 1);
        }

        if (word2Index > 0) {
            results.push_back(calculateDistance(word1, word2, word1Index , word2Index-1) + 1);
        }

        if (word1Index > 0 && word2Index > 0 && word1[word1Index-1] == word2[word2Index-1]) {
            results.push_back(calculateDistance(word1, word2, word1Index-1 , word2Index-1));
        }

        if (word1Index > 0 && word2Index > 0 && word1[word1Index-1] != word2[word2Index-1]) {
            results.push_back(calculateDistance(word1, word2, word1Index-1 , word2Index-1) + 1);
        }

        if (word1Index > 1 && word2Index > 1 && word1[word1Index-1] == word2[word2Index-2] && word1[word1Index-2] == word2[word2Index-1]) {
            results.push_back(calculateDistance(word1, word2, word1Index-2 , word2Index-2) + 1);
        }

        std::vector<int>::iterator result = std::min_element(results.begin(), results.end());
        return  *result;
    }


public:
    WordDistanceHandler() { }

    /**
     * Wrapper method to the calculateDistance (avoid having to insert the size of string each time).
     * Calculates the word distance between two strings using the Damerau-Leveshtein algorithm
     * @param word1 the first string
     * @param word2 the second string
     * 
     * @returns The distance (integer) between the two words: 0 if they are identical, and greater than 0 if they differ.
     */
    static int calculateWordDistance(std::string word1, std::string word2) {
        int wordDistance = calculateDistance(word1, word2, word1.size(), word2.size());
        return wordDistance;
    }

    static std::map<std::string, int> calculateWordDistance(std::string word1, std::vector<std::string> wordList) {
        std::map<std::string, int> distanceMap;
        for(std::string entry : wordList)
            distanceMap.insert(std::make_pair(entry, calculateWordDistance(word1, entry)));

        return distanceMap;
    }

    /**
     * Prints a map of words and their distances.
     * 
     * @param distanceMap A map where the key is a word (string)
     *        and the value is the word's distance (int).
     */
    static void printDistanceMap(const std::map<std::string, int>& distanceMap) {
        for (const auto& pair : distanceMap) {
            std::cout << "Word: " << pair.first << ", Distance: " << pair.second << std::endl;
        }
    }
};