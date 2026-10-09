#pragma once

#include <cstdint>
#include <array>
#include <limits>
#include "allocator.hpp"

namespace systems::lob {

enum class Side : uint8_t { Buy, Sell };

struct alignas(64) Order {
    uint64_t id;
    uint32_t price;
    uint32_t qty;
    Side side;

    Order* prev{nullptr};
    Order* next{nullptr};
};

struct LimitLevel {
    uint32_t price{0};
    uint32_t total_volume{0};
    uint32_t order_count{0};
    Order* head{nullptr};
    Order* tail{nullptr};

    void push_back(Order* order) noexcept {
        order->next = nullptr;
        order->prev = tail;
        if (tail) {
            tail->next = order;
        } else {
            head = order;
        }
        tail = order;
        total_volume += order->qty;
        ++order_count;
    }

    void remove(Order* order) noexcept {
        if (order->prev) order->prev->next = order->next;
        if (order->next) order->next->prev = order->prev;
        if (head == order) head = order->next;
        if (tail == order) tail = order->prev;

        total_volume -= order->qty;
        --order_count;
    }
};

template <size_t MaxPriceTicks = 10000, size_t MaxOrders = 100000>
class LimitOrderBook {
public:
    LimitOrderBook() : allocator_() {}

    Order* add_order(uint64_t id, uint32_t price, uint32_t qty, Side side) noexcept {
        if (price >= MaxPriceTicks) [[unlikely]] return nullptr;

        void* mem = allocator_.allocate();
        if (!mem) [[unlikely]] return nullptr;

        Order* order = new (mem) Order{id, price, qty, side, nullptr, nullptr};

        if (side == Side::Buy) {
            bids_[price].push_back(order);
            if (price > best_bid_) best_bid_ = price;
        } else {
            asks_[price].push_back(order);
            if (price < best_ask_) best_ask_ = price;
        }

        return order;
    }

    void cancel_order(Order* order) noexcept {
        if (!order) [[unlikely]] return;

        if (order->side == Side::Buy) {
            bids_[order->price].remove(order);
            update_best_bid();
        } else {
            asks_[order->price].remove(order);
            update_best_ask();
        }

        order->~Order();
        allocator_.deallocate(order);
    }

    [[nodiscard]] uint32_t best_bid() const noexcept { return best_bid_; }
    [[nodiscard]] uint32_t best_ask() const noexcept { return best_ask_; }

private:
    void update_best_bid() noexcept {
        while (best_bid_ > 0 && bids_[best_bid_].order_count == 0) {
            --best_bid_;
        }
    }

    void update_best_ask() noexcept {
        while (best_ask_ < MaxPriceTicks && asks_[best_ask_].order_count == 0) {
            ++best_ask_;
        }
    }

    std::array<LimitLevel, MaxPriceTicks> bids_{};
    std::array<LimitLevel, MaxPriceTicks> asks_{};
    uint32_t best_bid_{0};
    uint32_t best_ask_{MaxPriceTicks};
    systems::memory::FixedBlockAllocator<sizeof(Order), alignof(Order), MaxOrders> allocator_;
};

} // namespace systems::lob
