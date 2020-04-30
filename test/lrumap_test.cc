#include <stdio.h>
#include <iostream>
#include "base/def.h"
#include "base/lru_map.h"

int main(int argc, char *argv[])
{
    LruMap<uint32, uint32>* int_cache = NULL;  
    int_cache = new LruMap<uint32, uint32>(4);
    int_cache->insert(1, 2);
    
    auto it = int_cache->find(1);
    if (it != int_cache->end()) {
        std::cout << " " << it->first << ": " << it->second;
        std::cout << std::endl;
        int_cache->erase(it);
    }
    int_cache->insert(2, 200);
    std::cout << "Is empty?" << int_cache->empty() << std::endl;
    int_cache->erase(it->first);
    delete int_cache;

    return 0;
}
