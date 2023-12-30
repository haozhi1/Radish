
#include <iostream>
#include "connection.h"
#include "logger.h"

namespace radish::network {

namespace {
    
bool HandleOneRequest(Connection& conn) {
    // TODO: check if there is a valid request
    // TODO: protocol parsing
    if(!conn.AtState(ConnState::kRead)) return false;
    if(conn.ReadBufferEmpty()) return false;

    std::vector<char> req_msg = conn.GetReadBuffer();
    if(req_msg.size() > 1) {
        conn.WriteBufferAppend({"Dummy response\n"});
    }
    conn.EraseReadBuffer(req_msg.size());
    
    // ...

    conn.TransitionTo(ConnState::kWrite);
    HandleClientResponse(conn);
    return conn.ReadBufferEmpty();
}

} //namespace


bool Connection::TryRead() {
    if(!AtState(ConnState::kRead)) return false;
    if(ReadBufferFull()) return false;
    int maxb = kMaxBufferSize - rbuff_size_;
    int recv = socket_->Read(rbuff_, maxb, rbuff_size_);
    if(recv < 0) {
        if(errno != EAGAIN) TransitionTo(ConnState::kEnd);
        return false;
    }
    if(recv == 0) return false;
    rbuff_size_ += recv;
    return ReadBufferFull();
}

void Connection::EraseReadBuffer(int firstn) {
    int to_erase {std::min(firstn, kMaxBufferSize)};
    auto it_start = rbuff_.begin() + to_erase;
    auto it_end = rbuff_.begin() + ReadBufferSize();
    if(it_start == it_end) {
        rbuff_size_ = 0;
    } else {
        rbuff_size_ = std::distance(it_start, it_end);
        std::rotate(rbuff_.begin(), it_start, it_end);
    }
}

bool Connection::TryWrite() {
    if(!AtState(ConnState::kWrite)) return false;
    if(WriteBufferEmpty()) return false;
    int maxb = WriteBufferSize();
    int sent = socket_->Write(wbuff_, maxb);
    if(sent < 0) {
        if(errno != EAGAIN) TransitionTo(ConnState::kEnd);
        return false;
    }
    if(sent == 0) return false;
    auto it_start = wbuff_.begin() + sent;
    auto it_end = wbuff_.begin() + WriteBufferSize();
    if(it_start == it_end) {
        wbuff_size_ = 0;
    } else {
        wbuff_size_ = std::distance(it_start, it_end);
        std::rotate(wbuff_.begin(), it_start, it_end);
    }
    return WriteBufferEmpty();
}

void Connection::WriteBufferAppend(const std::string& msg) {
    int maxb = kMaxBufferSize - WriteBufferSize();
    if(msg.size() > maxb) return;
    wbuff_.insert(wbuff_.begin() + wbuff_size_, msg.begin(), msg.end());
    wbuff_size_ += msg.size();
}

void HandleClientConnection(Connection& conn) {
    if(conn.AtState(ConnState::kRead)) {
        HandleClientQuery(conn);
    } else if(conn.AtState(ConnState::kWrite)) {
        HandleClientResponse(conn);
    } else {
        LOG(WARNING) << "handling connection with invalid state";
    }
}

void HandleClientQuery(Connection& conn) {
    while(conn.TryRead());
    while(HandleOneRequest(conn));
}

void HandleClientResponse(Connection& conn) {
    while(conn.TryWrite());
    conn.TransitionTo(ConnState::kRead);
}

} // namespace radish::network