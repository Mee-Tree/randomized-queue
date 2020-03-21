#include "randomized_queue.h"
#include "subset.h"

void subset(unsigned long k, std::istream & in, std::ostream & out)
{
    std::string line;
    while (k > 0 && std::getline(in, line)) {
        out << line << std::endl;
        --k;
    }
}
