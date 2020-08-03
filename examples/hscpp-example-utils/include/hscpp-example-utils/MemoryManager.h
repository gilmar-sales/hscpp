#pragma once

#include <cstdint>
#include <vector>
#include <type_traits>

#include "hscpp/IAllocator.h"
#include "hscpp/Hotswapper.h"
#include "hscpp-example-utils/Ref.h"

// Memory allocator that is meant to map ids to memory addresses. Kept simple to better demonstrate
// the concept.
class MemoryManager : public hscpp::IAllocator
{
public:
    static MemoryManager& Instance();
    
    void SetHotswapper(hscpp::Hotswapper* pSwapper);

    template <typename T>
    static Ref<T> Allocate()
    {
        MemoryManager& instance = MemoryManager::Instance();

        if (instance.m_pSwapper == nullptr)
        {
            size_t size = sizeof(std::aligned_storage<sizeof(T)>::type);
            size_t iBlock = instance.TakeFirstFreeBlock(size);

            T* pT = new (instance.m_Blocks.at(iBlock).pMemory) T;

            Ref<T> ref;
            ref.id = iBlock;

            return ref;
        }
        else
        {
            // If hscpp is enabled, we always want to allocate through the hscpp::Hotswapper, as it
            // will use the latest constructor for a given type.
            hscpp::AllocationInfo info = instance.m_pSwapper->Allocate<T>();

            Ref<T> ref;
            ref.id = info.id;

            return ref;
        }
    }

    template <typename T>
    static void Free(Ref<T> ref)
    {
        MemoryManager& instance = MemoryManager::Instance();

        if (ref.id < instance.m_Blocks.size())
        {
            Block& block = instance.m_Blocks.at(ref.id);
            block.bFree = true;

            T* pObject = reinterpret_cast<T*>(block.pMemory);
            pObject->~T();

            // Zero out memory to cause crashes when dereferencing dangling pointers.
            std::memset(block.pMemory, 0, block.capacity);
        }
    }

    static uint8_t* GetMemory(size_t id);

    //============================================================================
    // hscpp::IAllocator implementation.
    //============================================================================
    hscpp::AllocationInfo Hscpp_Allocate(uint64_t size) override;
    hscpp::AllocationInfo Hscpp_AllocateSwap(uint64_t previousId, uint64_t size) override;
    uint64_t Hscpp_Free(uint8_t* pMemory) override;

private:
    struct Block
    {
        bool bFree = false;
        size_t capacity = 0;
        uint8_t* pMemory = nullptr;
    };

    size_t TakeFirstFreeBlock(size_t size);

    std::vector<Block> m_Blocks;
    hscpp::Hotswapper* m_pSwapper = nullptr;
};