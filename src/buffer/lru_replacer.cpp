//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.cpp
//
// Identification: src/buffer/lru_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_replacer.h"

namespace bustub {

LRUReplacer::LRUReplacer(size_t num_pages) {
  m_head = (lru_linked*)malloc(sizeof(lru_linked));
  m_tail = (lru_linked*)malloc(sizeof(lru_linked));
  m_tail->front = m_head;
  m_head->tail = m_tail;
  m_lru_max_size = num_pages;
  m_lru_size = 0;
}

LRUReplacer::~LRUReplacer() = default;

bool LRUReplacer::Victim(frame_id_t *frame_id) {
  //lru已经没有可被victim
  if(m_tail->front == m_head) {
    return false;
  }else {
    lru_linked* node = m_tail->front;
    *frame_id = node->frame_id;
    m_tail->front = node->front;
    node->front->tail = m_tail;
    m_lru_map[node->frame_id] = nullptr;
    delete node;
    m_lru_size--;
    node = nullptr;
    return true;
  }
}

// 被bufferpool拿去用了 从lrureplace中移除
void LRUReplacer::Pin(frame_id_t frame_id) {
  if(m_lru_map[frame_id] == nullptr) {
    return ;
  }
  m_lru_mutex.lock();
  lru_linked* node = m_lru_map[frame_id];
  node->front->tail = node->tail;
  node->tail->front = node->front;
  delete node;
  node = nullptr;
  m_lru_map[frame_id] = nullptr;
  m_lru_size--;
  m_lru_mutex.unlock();
  return ;
}

// 没有被bufferpool用了 从bufferpool拿到lrureplace
void LRUReplacer::Unpin(frame_id_t frame_id) {
  // 如果存在
  if(m_lru_map[frame_id] != nullptr || m_lru_size >= m_lru_max_size) {
    return ;
  }
  m_lru_mutex.lock();
  lru_linked* node = (lru_linked*)malloc(sizeof(lru_linked));
  node->frame_id = frame_id;
  m_head->tail->front = node;
  node->front = m_head;
  node->tail = m_head->tail;
  m_head->tail = node;
  m_lru_size++;
  m_lru_map[frame_id] = node;
  m_lru_mutex.unlock();
  return ;
}

size_t LRUReplacer::Size() { return m_lru_size; }

}  // namespace bustub