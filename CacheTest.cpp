#include "CacheTest.h"

int Cache::getCacheMisses() const { return return cacheMisses; }
int Cache::getInvalidations() const { return invalidations; }

future<uint64_t> Cache::read(uint64_t address)
{
    uint64_t blockIndex = getBlockIndex(address);
    uint64_t tag = getTag(address);
    int offset = getOffset(address);

    CacheBlock& block = block[blockIndex];

    if (block.mesiState == MODIFIED || block.mesiState == SHARED || block.mesiState == EXCLUSIVE)
    {
        // Cache hit
        if(block.tag == tag)
        {
            std::cout << "Cache hit en la lectura del bloque " << blockIndex << std::endl;
            uint64_t data = block.data[offset]
            return data;
        }
    }

    //Cache miss
    if(block.mesiState == MODIFIED)
    {
        cout << "Escribiendo bloque modificado en memoria" << endl;
        bus.alwaysWriteOnMemory(blockIndex, this->peId, address, data);
    }

    uint64_t data = bus.enqueueRead(*this, blockIndex, this->peId,address);
    block.valid = true;
    block.tag = tag;
    block.mesiState = SHARED;

    return data;
}

void Cache::write(uint64_t address, uint64_t data)
{
    uint64_t blockIndex = getBlockIndex(address);
    uint64_t tag = getTag(address);
    int offset = getOffset(address);

    CacheBlock& block = blocks[blockIndex];

    if(block.mesiState == MODIFIED || block.mesiState == EXCLUSIVE)
    {
        // Cache hit
        if (block.tag == tag)
        {
            cout << "Cache hit en escritura" << blockIndex << endl;
            block[offset] = data;
            block.mesiState = MODIFIED;
            block.dirty = true;
            return;
        }

        cout << "Cache miss en escrtura" << blockIndex << endl;
        cacheMisses++;

        if (block.mesiState == MODIFIED)
        {
            cout << "Escribiendo en memoria antes de write-back" << endl;
            bus.alwaysWriteOnMemory(blockIndex, peId, address, data);
        }
        
        block.valid = true;
        block.tag = tag;
        block.mesiState = MODIFIED;
        block.data[offset] = data;
        block.dirty = true;
    }
}

void Cache::invalidate(uint64_t address)
{
    uint64_t blockIndex = getBlockIndex(address);
    CacheBlock& block = blocks[blockIndex];

    if (block.mesiState != INVALID && block.tag == getTag(address))
    {
        block.mesiState = INVALID;
        invalidations++;
        cout << "Bloque invalidado en indice: " << blockIndex << endl;
    }
    
}

uint64_t Cache::getBlockIndex(uint64_t address) const
{
    return (address / 32) % blocks.size();;
}

uint64_t Cache::getTag(uint64_t address) const
{
    return address / 32;
}

int Cache::getOffset(uint64_t address) const
{
    return (address % 32) / 8;
}
