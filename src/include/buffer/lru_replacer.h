//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.h
//
// Identification: src/include/buffer/lru_replacer.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <list>
#include <mutex>  // NOLINT
#include <vector>
#include <map>

#include "buffer/replacer.h"
#include "common/config.h"

namespace bustub {

/**
 * LRUReplacer implements the Least Recently Used replacement policy.
 */
class LRUReplacer : public Replacer {
 public:
  /**
     * Create a new LRUReplacer.
     * @param num_pages the maximum number of pages the LRUReplacer will be required to store
   */
  explicit LRUReplacer(size_t num_pages);

  /**
     * Destroys the LRUReplacer.
   */
  ~LRUReplacer() override;

  bool Victim(frame_id_t *frame_id) override;

  void Pin(frame_id_t frame_id) override;

  void Unpin(frame_id_t frame_id) override;

  size_t Size() override;

 private:

  // 双向链表
  struct lru_linked
  {
    lru_linked *front;
    lru_linked *tail;
    frame_id_t frame_id;
  };

  // 链表头指针
  lru_linked *m_head;
  // 链表尾指针
  lru_linked *m_tail;

  // 根据framid来找到该framid对应双链表指针的map
  std::map<frame_id_t, lru_linked*> m_lru_map;

  // 互斥量
  std::mutex m_lru_mutex;

  // lru size
  size_t m_lru_size;

  // lru max size
  size_t m_lru_max_size;
};

}  // namespace bustub
