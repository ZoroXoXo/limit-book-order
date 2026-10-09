#pragma once

#include <cstddef>
#include <cstdint>
#include <new>
#include <utility>

namespace systems::memory {

template <size_t BlockSize, size_t Alignment = 64, size_t Capacity = 100000>
class FixedBlockAllocator {
private:
    struct Node {
        Node* next;
    };

    static constexpr size_t ActualBlockSize = (BlockSize < sizeof(Node)) ? sizeof(Node) : BlockSize;

    alignas(Alignment) uint8_t pool_[ActualBlockSize * Capacity];
    Node* free_list_{nullptr};

public:
    FixedBlockAllocator() noexcept {
        for (size_t i = 0; i < Capacity - 1; ++i) {
            auto* current = reinterpret_cast<Node*>(&pool_[i * ActualBlockSize]);
            auto* next = reinterpret_cast<Node*>(&pool_[(i + 1) * ActualBlockSize]);
            current->next = next;
        }
        reinterpret_cast<Node*>(&pool_[(Capacity - 1) * ActualBlockSize])->next = nullptr;
        free_list_ = reinterpret_cast<Node*>(&pool_[0]);
    }

    ~FixedBlockAllocator() = default;
    FixedBlockAllocator(const FixedBlockAllocator&) = delete;
    FixedBlockAllocator& operator=(const FixedBlockAllocator&) = delete;

    [[nodiscard]] void* allocate() noexcept {
        if (!free_list_) [[unlikely]] return nullptr;
        Node* node = free_list_;
        free_list_ = free_list_->next;
        return static_cast<void*>(node);
    }

    void deallocate(void* ptr) noexcept {
        if (!ptr) [[unlikely]] return;
        auto* node = static_cast<Node*>(ptr);
        node->next = free_list_;
        free_list_ = node;
    }
};

} // namespace systems::memory
