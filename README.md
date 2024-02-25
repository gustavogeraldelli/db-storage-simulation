# About
This project consists of two simulations demonstrating how a database stores data using indexing and B-trees techniques. These implementations were developed as part of academic assignments for the Organization and Retrieval of Information course at college, where the study focuses on how data is organized and manipulated in secondary storage.

## T01: Indexing
In the first assignment, indexing was employed to optimize data retrieval operations. Indexing works by creating a separate data structure, which maps keys to the corresponding locations of records in the database. This allows for faster access to specific records, as the database system can directly access the desired records through the index without needing to scan the entire dataset.

## T02: B-trees
In the second assignment, B-trees were chosen as the primary data structure for efficient storage and retrieval. B-trees are self-balancing tree data structures that maintain sorted data and ensure balanced tree structures. They facilitate fast insertion, deletion, and search operations by recursively traversing the tree structure. This enables efficient storage management and enhances data access performance, especially in scenarios where data needs to be stored and accessed on disk.