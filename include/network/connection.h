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
 * kRequest -> kRequest: reading, there are still readable data.
 * kRequest -> kResponse: buffer contains a valid request.
 * kRequest -> kEnd: error while reading requests.
 * kResponse -> kResponse: clearing the write buffer.
 * kResponse -> kRequest: write buffer is empty, go back to reading requests.
 * kResponse -> kEnd: error while writing responses.
 */
enum class ConnState {
    kRequest,
    kResponse,
    kEnd,
};

class Connection {
    public:
        Connection(std::unique_ptr<Socket> socket): 
            socket_{std::move(socket)},
            rbuff_size_{0},
            rbuff_(kMaxBufferSize),
            wbuff_size_{0},
            wbuff_(kMaxBufferSize) {}
        ~Connection() { socket_->Close(); }

        // No copy
        Connection(const Connection& connection) = delete;
        Connection& operator=(const Connection& connection) = delete;
        Connection(Connection&& connection) = default;
        Connection& operator=(Connection&& connection) = default;

        ConnState getState() { return state_; }
        void SetState(ConnState state) { state_ = state; }
        
        bool Read();
        bool ReadBufferEmpty() const { return rbuff_size_ == 0; }
        bool ReadBufferFull() const { return rbuff_size_ == kMaxBufferSize; }
        int ReadBufferSize() const { return rbuff_size_; }
        std::vector<char> GetReadBuffer() { return {rbuff_.begin(), rbuff_.begin() + rbuff_size_}; }
        void EraseReadBuffer(int firstn);
        
        bool Write();
        bool WriteBufferEmpty() const { return wbuff_size_ == 0; }
        bool WriteBufferFull() const { return wbuff_size_ == kMaxBufferSize; }
        int WriteBufferSize() const { return wbuff_size_; }
        void WriteBufferAppend(const std::string& msg);
    private:
        std::unique_ptr<Socket> socket_;
        ConnState state_ {ConnState::kRequest};
        int rbuff_size_;
        std::vector<char> rbuff_;
        int wbuff_size_;
        std::vector<char> wbuff_;
};

void HandleConnection(Connection& conn);

void HandleRequest(Connection& conn);

void HandleResponse(Connection& conn);
} // namespace radish

#endif