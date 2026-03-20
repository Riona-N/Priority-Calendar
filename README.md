
# 📅 Priority Calendar (DSA Project)

## 📌 Overview

Priority Calendar is a **console-based scheduling application** designed to efficiently manage events based on date and time. It ensures that all events are stored and displayed in **chronological order**, with fast insertion, deletion, and search operations.

This project demonstrates the practical application of advanced **Data Structures and Algorithms (DSA)** concepts in solving a real-world problem like calendar management. 

---

## 🚀 Features

* ➕ Add events with date and time
* 📋 View all events in sorted order
* 📅 Display events for a specific month
* ⏭️ Find the next upcoming event
* ⚠️ Conflict detection for overlapping events
* ✅ Input validation for date and time
* 💻 Simple and user-friendly console interface

---

## 🧠 Data Structures Implemented

This project integrates multiple data structures:

### 1. 🌳 Treap (Randomized Binary Search Tree)

* Core data structure used to store **dates**
* Maintains:

  * **BST property** → sorted by date
  * **Heap property** → based on random priority
* Ensures **average time complexity: O(log n)**
* Uses:

  * Split and Merge operations
  * Recursive traversal

---

### 2. 🔗 Singly Linked List

* Used to store **events within each date**
* Maintains events in **sorted order by time**
* Enables:

  * Easy insertion
  * Conflict detection
  * Sequential traversal

---

### 3. 🧩 Structures (Structs)

* `Evt` → stores:

  * Time
  * Event title
  * Pointer to next event
* `Node` → stores:

  * Date
  * Priority
  * Left & right child pointers
  * Head pointer to event list

---

### 4. 🔁 Recursion

* Used for:

  * In-order traversal (sorted output)
  * Memory deallocation

---

### 5. ⚙️ Auxiliary Concepts

* Dynamic memory allocation (`malloc`, `free`)
* Input validation functions
* Date encoding/decoding logic
* Lower bound search for upcoming events

---

## 🛠️ Tech Stack

* Language: **C**
* Compiler: **GCC**
* Libraries Used:

  * `<stdio.h>`
  * `<stdlib.h>`
  * `<string.h>`
  * `<time.h>`
  * `<ctype.h>`
---
## ▶️ How to Run

1. Compile the program:

```bash
gcc main.c -o calendar
```
2. Run the executable:

```bash
./calendar
```
---

## 📸 Sample Functionalities

* Add Event → Stores event in correct position
* Show All → Displays sorted events
* Show Month → Filters events using range queries
* Next Upcoming → Finds nearest future event

---

## 🎯 Learning Outcomes

* Practical implementation of **Treap**
* Integration of **multiple data structures**
* Understanding **time complexity optimization**
* Hands-on experience with:

  * Pointers
  * Recursion
  * Memory management
* Improved debugging and modular design skills
---

## 🔮 Future Enhancements

* File storage (persistent data)
* GUI-based interface
* Event editing and deletion
* Notifications/reminders
