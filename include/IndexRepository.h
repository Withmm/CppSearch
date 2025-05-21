//
// Created by ByteDance on 25-5-21.
//

#ifndef INDEXREPOSITORY_H
#define INDEXREPOSITORY_H
#include <iostream>
#include <ranges>
#include <string>
#include <unordered_map>
#include <utility>
#include <fstream>

#include "IRText.h"

using Path = std::string;

struct Word {
    std::string name;
    std::unordered_map<Path, int> where;
    int total_hit = 0;

    Word() = default;
    explicit Word(std::string  name_) :name(std::move(name_)) {}

    bool operator==(const Word& other) const {
        return name == other.name;
    }

    [[nodiscard]] int doc_frequency() const {
        return static_cast<int>(where.size());
    }
};

struct Posting {
    Path doc_path;
    int position = 0;

    Posting() = default;

    explicit Posting(Path doc_path_, const int position_): doc_path(std::move(doc_path_)), position(position_) {}

    bool operator==(const Posting& other) const {
        return doc_path == other.doc_path && position == other.position;
    }
};

namespace std {
    template <>
    struct hash<Posting> {
        std::size_t operator()(const Posting& p) const noexcept {
            return hash<std::string>()(p.doc_path) ^ (hash<int>()(p.position) << 1);
        }
    };
    template<>
    struct hash<Word> {
        std::size_t operator()(const Word& w) const noexcept {
            return std::hash<std::string>()(w.name);
        }
    };
}

struct IndexRepository {
    // very useful
    std::unordered_map<Word, std::vector<Posting>> word2posting;
    std::unordered_map<Posting, Word> posting2word;
    std::unordered_map<std::string, Word> name2word;

    // maybe useful
    std::vector<IRText> irs;

    void saveItem(const std::string& item, const Posting& posting) {
        if (!name2word.contains(item)) {
            name2word[item] = Word(item);
        }
        Word& word = name2word[item];
        word.where[posting.doc_path] += 1;
        ++word.total_hit;

        word2posting[word].push_back(posting);
        posting2word[posting] = word;
    }

    void dumpSummaryToDisk() const {
        namespace fs = std::filesystem;

        fs::create_directories("../index");

        std::ofstream out("../index/summary.txt");
        if (!out.is_open()) {
            std::cerr << "Failed to open ../index/summary.txt for writing.\n";
            return;
        }

        out << "===== IndexRepository Summary =====\n";
        out << "Total unique terms       : " << name2word.size() << "\n";
        out << "Total postings recorded  : " << posting2word.size() << "\n\n";

        out << "[word2posting] Mappings:\n";
        for (const auto& [word, postings] : word2posting) {
            out << "- " << word.name << " → ";
            for (const auto& p : postings) {
                out << "(" << p.doc_path << ":" << p.position << ") ";
            }
            out << "\n";
        }

        out << "\n[posting2word] Mappings:\n";
        for (const auto& [posting, word] : posting2word) {
            out << "- (" << posting.doc_path << ":" << posting.position << ") → "
                << word.name << "\n";
        }

        out << "====================================\n";

        out.close();
        std::cout << "[✓] Index summary written to index/summary.txt\n";
    }

};
#endif //INDEXREPOSITORY_H
