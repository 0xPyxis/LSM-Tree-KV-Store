# LSM Tree Key-Value Store (C++)

A simplified **Log-Structured Merge Tree (LSM Tree)** based key-value storage engine written in C++.

This project explores the internal storage architecture used by modern databases such as LevelDB, RocksDB, and Cassandra.

The engine implements durable writes, SSTable storage, compaction, bloom filters, sparse indexes, and block-based disk layouts.

---

# Features

* Write-Ahead Logging (WAL) for crash safety
* Memtable for fast in-memory writes
* Immutable SSTables for persistent storage
* Binary SSTable format
* Block-based SSTable storage
* Block index for efficient disk access
* Bloom filters to skip irrelevant SSTables
* Sparse indexing for faster lookups
* Compaction to merge and clean SSTables
* Crash recovery via WAL replay

Planned improvements:

* Background compaction thread
* Immutable memtables
* Block cache
* Range queries
* Configurable bloom filters

---

# Architecture

## Write Path

PUT(key, value)
↓
Write-Ahead Log (append-only)
↓
Memtable (sorted in-memory structure)
↓
Flush to SSTable when threshold reached

Writes are fast because random writes are converted into **sequential disk writes**.

---

## Read Path

GET(key)
↓
Memtable lookup
↓
Bloom Filter (skip SSTables that cannot contain key)
↓
Block Index lookup
↓
Seek to relevant block
↓
Scan records inside block

This minimizes disk I/O by reading only the necessary block.

---

# Storage Layout

Each SSTable file is organized as:

[data block 0]
[data block 1]
[data block 2]

[block index]

[footer]

Block index maps:

first_key_of_block → block_offset

Typical block size:

4 KB

This aligns with OS page sizes and improves disk performance.

---

# Record Format (Binary)

Each record is stored as:

[key_size][key_bytes][value_size][value_bytes]

Example:

key = "apple"
value = "10"

Stored as:

05 apple 02 10

Binary encoding avoids expensive string parsing and enables faster reads.

---

# Bloom Filters

Each SSTable maintains a bloom filter used to quickly check:

"Key definitely not present"

If the bloom filter rejects a key, the SSTable is skipped without reading from disk.

This dramatically reduces disk I/O for negative lookups.

---

# Compaction

Compaction merges multiple SSTables to:

* Remove deleted entries (tombstones)
* Remove outdated key versions
* Reduce read amplification

Example:

sstable_1:
a → 1

sstable_2:
a → 3

Compaction result:
a → 3

---

# Crash Recovery

Durability is ensured using a **Write-Ahead Log (WAL)**.

Recovery procedure:

1. Load WAL
2. Replay operations
3. Rebuild Memtable

This ensures no committed writes are lost after crashes.

---

# Example Usage

PUT a 1
PUT b 2
PUT c 3

GET a
→ 1

DELETE a

GET a
→ not found

---

# Build

Compile with g++:

```
g++ -std=c++17 main.cpp storage_engine.cpp memtable.cpp wal.cpp sstable.cpp compaction.cpp bloom_filter.cpp -o lsm_store
```

Run:

```
./lsm_store
```

---

# Learning Goals

This project explores the fundamental techniques behind modern storage engines:

* Log Structured Storage
* Disk I/O optimization
* Probabilistic data structures
* Compaction strategies
* Binary storage formats
* Block-based indexing

---

# Future Improvements

Potential extensions:

* Background compaction thread
* Immutable memtables
* Range queries
* LRU block cache
* Multi-level compaction
* Compression for SSTables

---

# References

Concepts implemented in this project are inspired by:

* LevelDB Architecture
* RocksDB Design
* Log-Structured Merge Trees (LSM Trees)
