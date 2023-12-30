#ifndef NETWORK_CONNECTION_H
#define NETWORK_CONNECTION_H

#include <vector>
#include <string>
#include <utility>
#include <iterator>
#include "socket.h"

namespace radish::network {

constexpr int kMaxBufferSize {4096};

/*
 * kRead -> kRead: reading, there are still readable data.
 * kRead -> kWrite: read buffer has enough data to be processed.
 * kRead -> kEnd: error while reading data.
 * kWrite -> kWrite: clearing the write buffer.
 * kWrite -> kRead: write buffer is empty, go back to reading incoming data.
 * kWrite -> kEnd: error while writing data.
 */
enum class ConnState {
    kRead,
    kWrite,
    kEnd,
};

// An abstraction over Socket that provides buffered IO and state management.
class Connection {
    public:
        Connection(std::unique_ptr<Socket> socket): 
            socket_{std::move(socket)},
            rbuff_size_{0},
            rbuff_(kMaxBufferSize),
            wbuff_size_{0},
            wbuff_(kMaxBufferSize) {}
        ~Connection() = default;

        // No copy
        Connection(const Connection& connection) = delete;
        Connection& operator=(const Connection& connection) = delete;
        Connection(Connection&& connection) = default;
        Connection& operator=(Connection&& connection) = default;

        bool AtState(ConnState state) const { return state_ == state; }
        void TransitionTo(ConnState state) { if(!AtState(ConnState::kEnd)) state_ = state; }
        
        bool TryRead();
        bool ReadBufferEmpty() const { return rbuff_size_ == 0; }
        bool ReadBufferFull() const { return rbuff_size_ == kMaxBufferSize; }
        int ReadBufferSize() const { return rbuff_size_; }
        std::vector<char> GetReadBuffer() { return {rbuff_.begin(), rbuff_.begin() + rbuff_size_}; }
        void EraseReadBuffer(int firstn);
        
        bool TryWrite();
        bool WriteBufferEmpty() const { return wbuff_size_ == 0; }
        bool WriteBufferFull() const { return wbuff_size_ == kMaxBufferSize; }
        int WriteBufferSize() const { return wbuff_size_; }
        void WriteBufferAppend(const std::string& msg);
    private:
        std::unique_ptr<Socket> socket_;
        ConnState state_ {ConnState::kRead};
        int rbuff_size_;
        std::vector<char> rbuff_;
        int wbuff_size_;
        std::vector<char> wbuff_;
};

void HandleClientConnection(Connection& conn);

void HandleClientQuery(Connection& conn);

void HandleClientResponse(Connection& conn);
} // namespace radish

#endif // NETWORK_CONNECTION_H