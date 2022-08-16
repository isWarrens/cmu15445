//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_bucket_page.cpp
//
// Identification: src/storage/page/hash_table_bucket_page.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/page/hash_table_bucket_page.h"
#include "common/logger.h"
#include "common/util/hash_util.h"
#include "storage/index/generic_key.h"
#include "storage/index/hash_comparator.h"
#include "storage/table/tmp_tuple.h"

namespace bustub {

template <typename KeyType, typename ValueType, typename KeyComparator>
uint32_t HASH_TABLE_BUCKET_TYPE::GetFirstNoRead() {
  for(uint32_t i = 0; i < BUCKET_ARRAY_SIZE;i++) {
    if(!IsReadable(i)) {
      return i;
    }
  }
  return BUCKET_ARRAY_SIZE;
}



template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::GetValue(KeyType key, KeyComparator cmp, std::vector<ValueType> *result) -> bool {
  for(uint32_t i = 0 ; i < BUCKET_ARRAY_SIZE; i++) {
    if(IsReadable(i)) {
      if (cmp(array_[i].first,key) == 0) {
        result->push_back(array_[i].second);
      }
    }
  }
  return !result->empty();
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::Insert(KeyType key, ValueType value, KeyComparator cmp) -> bool {
  std::vector<ValueType> result;
  GetValue(key,cmp,&result);
  uint32_t insert_pos = GetFirstNoRead();
  LOG_INFO("ss %u", insert_pos);
  // key没有重复
  if(result.empty()) {
    array_[insert_pos].first = key;
    array_[insert_pos].second = value;
    SetReadable(insert_pos);
    SetOccupied(insert_pos);
    LOG_INFO("22");
    return true;
  }
  for(auto c : result) {
    if(c == value) {
      return false;
    }
  }
  array_[insert_pos].first = key;
  array_[insert_pos].second = value;
  SetReadable(insert_pos);
  SetOccupied(insert_pos);
  return true;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::Remove(KeyType key, ValueType value, KeyComparator cmp) -> bool {
  for(uint32_t i = 0; i < BUCKET_ARRAY_SIZE; i++) {
    if(IsReadable(i)) {
      if(cmp(array_[i].first,key) == 0) {
        if(value == array_[i].second) {
          RemoveAt(i);
          return true;
        }
      }
    }
  }
  return false;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::KeyAt(uint32_t bucket_idx) const -> KeyType {
  return array_[bucket_idx].first;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::ValueAt(uint32_t bucket_idx) const -> ValueType {
  return array_[bucket_idx].second;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BUCKET_TYPE::RemoveAt(uint32_t bucket_idx) {
  // 形式删除
  SetNoReadable(bucket_idx);
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::IsOccupied(uint32_t bucket_idx) const -> bool {
  uint32_t index = bucket_idx / 8;
  uint32_t bits_pos = bucket_idx % 8;
  uint32_t mask = 0x1 << bits_pos;
  return static_cast<bool>((mask & occupied_[index]) != 0);
}

template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BUCKET_TYPE::SetOccupied(uint32_t bucket_idx) {
  uint32_t index = bucket_idx / 8;
  uint32_t bits_pos = bucket_idx % 8;
  uint32_t mask = 0x1 << bits_pos;
  occupied_[index] |= mask;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::IsReadable(uint32_t bucket_idx) const -> bool {
  uint32_t index = bucket_idx / 8;
  uint32_t bits_pos = bucket_idx % 8;
  uint32_t mask = 0x1 << bits_pos ;
  return static_cast<bool>((mask & readable_[index]) != 0);
}

template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BUCKET_TYPE::SetReadable(uint32_t bucket_idx) {
  uint32_t index = bucket_idx / 8;
  uint32_t bits_pos = bucket_idx % 8;
  uint32_t mask = 0x1 << bits_pos;
  readable_[index] |= mask;
  LOG_INFO("%u %u %u %u",index,bits_pos,mask,readable_[index]);
}
template <typename KeyType, typename ValueType, typename KeyComparator>
void HashTableBucketPage<KeyType, ValueType, KeyComparator>::SetNoReadable(uint32_t bucket_idx) {
  uint32_t index = bucket_idx / 8;
  uint32_t bits_pos = bucket_idx % 8;
  uint32_t mask = 0x1 << bits_pos;
  readable_[index] -= mask;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::IsFull() -> bool {
  for(uint32_t i = 0; i < BUCKET_ARRAY_SIZE; i++) {
    if(!IsOccupied(i)) {
      return false;
    }
  }
  return true;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::NumReadable() -> uint32_t {
  uint32_t res = 0;
  for(uint32_t i = 0; i < BUCKET_ARRAY_SIZE; i++) {
    if(IsReadable(i)) {
      res++;
    }
  }
  return 0;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::IsEmpty() -> bool {
  for(uint32_t i = 0; i < BUCKET_ARRAY_SIZE; i++) {
    if(IsReadable(i)) {
      return false;
    }
  }
  return true;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BUCKET_TYPE::PrintBucket() {
  uint32_t size = 0;
  uint32_t taken = 0;
  uint32_t free = 0;
  for (size_t bucket_idx = 0; bucket_idx < BUCKET_ARRAY_SIZE; bucket_idx++) {
    if (!IsOccupied(bucket_idx)) {
      break;
    }

    size++;

    if (IsReadable(bucket_idx)) {
      taken++;
    } else {
      free++;
    }
  }

  LOG_INFO("Bucket Capacity: %lu, Size: %u, Taken: %u, Free: %u", BUCKET_ARRAY_SIZE, size, taken, free);
}

// DO NOT REMOVE ANYTHING BELOW THIS LINE
template class HashTableBucketPage<int, int, IntComparator>;

template class HashTableBucketPage<GenericKey<4>, RID, GenericComparator<4>>;
template class HashTableBucketPage<GenericKey<8>, RID, GenericComparator<8>>;
template class HashTableBucketPage<GenericKey<16>, RID, GenericComparator<16>>;
template class HashTableBucketPage<GenericKey<32>, RID, GenericComparator<32>>;
template class HashTableBucketPage<GenericKey<64>, RID, GenericComparator<64>>;

// template class HashTableBucketPage<hash_t, TmpTuple, HashComparator>;

}  // namespace bustub
