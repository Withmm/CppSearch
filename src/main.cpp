#include <iostream>

#include "IRText.h"
#include "utils.h"
#include "IndexRepository.h"

IndexRepository INDEX_REPO;
int main() {
    const IROptions ops;
    build_idx_repo_from("../data/tdt3", ops);
    std::cout << "Generated index successfully!" << std::endl;
    INDEX_REPO.dumpSummaryToDisk();
}