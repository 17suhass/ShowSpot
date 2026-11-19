# 🎟️ Movie and Local Theatre Ticket Booking System
### *Empowering Commercial Cinema & Regional Performing Arts via Classical C99 Data Structures*

---

## 📖 Table of Contents
1. [Project Overview](#-project-overview)
2. [Problem Statement](#-problem-statement)
3. [Project Objectives](#-project-objectives)
4. [Key Features](#-key-features)
   - [Buyer Portal](#1-buyer-portal)
   - [Artist Portal](#2-artist-portal)
   - [Nodal Officer Portal](#3-nodal-officer-portal)
   - [Auditorium Seating & Tiered Pricing](#4-auditorium-seating--tiered-pricing)
   - [Multi-Seat Booking Flow](#5-multi-seat-booking-flow)
   - [Cancellation & Undo Operations](#6-cancellation--undo-operations)
   - [Waiting List & Auto-Reallocation](#7-waiting-list--auto-reallocation)
   - [Search & System Analytics](#8-search--system-analytics)
5. [Data Structures & Algorithms Implementation](#-data-structures--algorithms-implementation)
   - [1. Singly Linked List](#1-singly-linked-list)
   - [2. Two-Dimensional Array (10 × 12 Seating Matrix)](#2-two-dimensional-array-10--12-seating-matrix)
   - [3. Stack (LIFO) for Cancellation History & Undo](#3-stack-lifo-for-cancellation-history--undo)
   - [4. Queue (FIFO) for Waiting-List Management](#4-queue-fifo-for-waiting-list-management)
   - [5. Searching Techniques (Linear & Binary Search)](#5-searching-techniques-linear--binary-search)
   - [6. Algorithmic Complexity Matrix](#6-algorithmic-complexity-matrix)
6. [File Handling & Data Persistence](#-file-handling--data-persistence)
7. [System Architecture & Interfaces](#-system-architecture--interfaces)
   - [Dual-Interface Model](#dual-interface-model)
   - [Embedded C HTTP Server & REST API](#embedded-c-http-server--rest-api)
8. [Technologies Used](#-technologies-used)
9. [Project Directory Structure](#-project-directory-structure)
10. [Compilation & Execution Guide](#-compilation--execution-guide)
    - [Prerequisites](#prerequisites)
    - [Running the Web Application (BookMyShow-Style)](#option-a-web-application-bookmyshow-style)
    - [Running the Terminal CLI Application](#option-b-terminal-cli-application)
11. [Academic Context & DSA Learnings](#-academic-context--dsa-learnings)
12. [License & Acknowledgments](#-license--acknowledgments)

---

## 🌟 Project Overview

The **Movie and Local Theatre Ticket Booking System** is an end-to-end ticketing, reservation, and discovery platform developed in **pure C (C99 Standard)**. 

While modern commercial aggregators concentrate exclusively on high-budget multiplex cinema, regional performing arts—such as Marathi classical theatre (*Natsamrat*), Baul and Sufi folk music, Rajasthani folk dance (*Ghoomar & Kalbeliya*), and Sanskrit mythological dramas (*Shakuntala*)—frequently suffer from a lack of digital visibility, automated seating arrangements, and reliable reservation channels.

This project unifies **Commercial Movies** and **Regional Local Theatre / Performing Arts** on a single platform. It offers two fully synchronized interfaces:
1. **Interactive Console CLI**: An educational, menu-driven terminal interface with input validation.
2. **Modern Web UI**: A responsive, BookMyShow-inspired web experience with authentic event artwork, dynamic carousel browsing, interactive seat maps, multi-seat checkouts, and digital boarding passes, served directly by an embedded C HTTP server.

---

## ⚠️ Problem Statement

Traditional entertainment and cultural event booking approaches present significant operational challenges:

1. **Fragmented & Inaccessible Platforms**: Multiplexes possess sophisticated booking software, whereas local theatre groups, folk troupes, and drama halls rely on unorganized physical queues, paper stubs, or phone reservations.
2. **Manual & Error-Prone Seat Tracking**: Tracking VIP versus Regular seating manually leads to double-booking, seat hoarding, and revenue leakage.
3. **Lack of Transparent Cancellation Handling**: When a customer cancels a ticket, physical ticket systems rarely provide immediate refunds or structured mechanisms to revert accidental cancellations.
4. **Unfair Waiting-List Allocation**: When a popular show sells out, manual walk-ins or arbitrary allocations replace fair, first-come-first-served queues.
5. **Abstraction Over Core Fundamentals**: Modern enterprise frameworks hide memory layouts, pointers, and algorithmic trade-offs behind layers of abstractions. Demonstrating these mechanics in a low-level language like C proves how enterprise ticketing engines actually operate under the hood.

---

## 🎯 Project Objectives

- **Promote Regional Theatre & Performing Arts**: Empower grassroots dramatists, folk musicians, and classical artists by granting them equal platform presence alongside commercial blockbusters.
- **Support Local Artists**: Provide an Artist Portal where cultural troupes can register events, specify custom seating tariffs, and track approval status.
- **Unified Entertainment Access**: Enable buyers to discover films, plays, musicals, and dance shows through a single intuitive catalog.
- **Apply Classical Data Structures**: Implement fundamental computer science data structures—**Singly Linked Lists**, **2D Matrices**, **LIFO Stacks**, and **FIFO Queues**—without relying on external libraries or frameworks.
- **Guarantee Robust Persistence & Recovery**: Implement structured binary file handling (`.dat` files) with validation and fail-safe recovery mechanisms.

---

## 🚀 Key Features

### 1. Buyer Portal
- **Catalog Browsing**: Browse commercial blockbusters, regional theatre, live concerts, dance shows, and cultural galas.
- **Category Filtering**: Instant categorization across Movies, Drama & Theatre, Music & Concerts, Folk Dance, and Cultural Events.
- **Multi-Seat Reservation**: Select any quantity between 1 and 10 tickets in a single checkout.
- **Ticket Verification & Boarding Pass**: Review active bookings and generate digital ticket passes showing assigned seat codes, dates, and prices.

### 2. Artist Portal
- **Event Submission**: Local artists and theatre groups register events by specifying title, genre, venue, date/time, and pricing tiers.
- **Pending Status**: Newly registered events enter a "Pending Approval" state (`isApproved = 0`), preventing unverified listings from appearing in the buyer catalog.
- **Track Listings**: Artists can view their submitted shows and track whether they have been approved or rejected.

### 3. Nodal Officer Portal
- **Administrative Governance**: Dedicated administrative access to review submitted artist events.
- **Approval / Rejection Workflow**: Inspect event details, artist credentials, and venue safety before approving (`isApproved = 1`) or rejecting events.
- **System Audit & Revenue Overview**: View real-time platform statistics including total events, active bookings, gross platform revenue, and waiting-list occupancy.

### 4. Auditorium Seating & Tiered Pricing
- **10 × 12 Seating Matrix**: Each auditorium provides exactly 120 seats structured into 10 rows (Rows A through J) and 12 columns (Columns 1 through 12).
- **Two-Tier Pricing**:
  - **VIP Tiers (Rows A & B)**: Premium front-row comfort with dedicated VIP pricing.
  - **Regular Tiers (Rows C through J)**: Standard auditorium seating with affordable regular pricing.
- **Live Seat Availability Status**: Visual feedback showing available (`O`), booked (`X`), and currently selected seats.

### 5. Multi-Seat Booking Flow
```
[Select Quantity] ➔ [Select Seats on 10×12 Matrix] ➔ [Review VIP/Regular Breakdown] ➔ [Generate Boarding Pass]
```
1. **Quantity Prompt**: Buyer specifies desired ticket count (e.g., 3 seats).
2. **Live Counter**: The interface tracks selected seats (e.g., "2 of 3 selected") and prevents proceeding until the exact requested count is chosen.
3. **Deselect & Swap**: Buyers can freely deselect any seat and pick another before confirmation.
4. **Atomic Checkout**: Backend validates all requested seats simultaneously. If even one seat was taken in a race condition, the entire transaction is safely rolled back.

### 6. Cancellation & Undo Operations
- **Single-Seat & Multi-Seat Cancellation**: Buyers can cancel any confirmed ticket by its unique `bookingId`.
- **LIFO Undo Stack**: The most recent cancellation is saved on an undo stack. If a customer cancels by accident, the Nodal Officer or Buyer can trigger **Undo**, restoring the seat and reactivating the booking record immediately.

### 7. Waiting List & Auto-Reallocation
- **Automatic Sold-Out Detection**: When an auditorium reaches full capacity (120/120 booked, `available_seats == 0`), the booking button transitions to **Join Waiting List**.
- **Fair FIFO Queue**: Buyers are enqueued in strict order of arrival.
- **Automatic Allocation on Cancellation**: Whenever a booked seat is cancelled, the system automatically dequeues the attendee at the head of the waiting list, re-books the freed seat under their name, and marks it occupied—eliminating vacant seats without human intervention.

### 8. Search & System Analytics
- **Multi-Field Search**: Search events and bookings by Title, Artist/Troupe, Venue, or Category.
- **Pointer-Array Binary Search**: Fast logarithmic lookup by Event ID or Booking ID.
- **Revenue Analytics**: Live calculation of total collections across VIP and Regular ticket sales.

---

## 🧠 Data Structures & Algorithms Implementation

This project implements computer science fundamentals directly in C. Below is a detailed technical analysis of each data structure and algorithm utilized in the codebase.

```
┌────────────────────────────────────────────────────────────────────────┐
│                        CORE DATA STRUCTURES MAP                        │
├─────────────────────────┬────────────────────────┬─────────────────────┤
│ Data Structure          │ Code Implementation    │ Architectural Role  │
├─────────────────────────┼────────────────────────┼─────────────────────┤
│ Singly Linked List      │ struct Event *head     │ Dynamic Event Store │
│ Singly Linked List      │ struct Booking *bHead  │ Active Booking Log  │
│ 2D Array Matrix (10×12) │ char seats[10][12]     │ Auditorium Seat Map │
│ Stack (LIFO)            │ struct Cancellation *  │ Cancellation & Undo │
│ Queue (FIFO)            │ struct WaitlistNode *  │ Waiting List Queue  │
│ Pointer Binary Search   │ struct Event **arr     │ O(log n) ID Lookup  │
└─────────────────────────┴────────────────────────┴─────────────────────┘
```

---

### 1. Singly Linked List
- **Source Files**: [`events.c`](file:///d:/Movie_Booking/events.c), [`bookings.c`](file:///d:/Movie_Booking/bookings.c)
- **C Definition**:
  ```c
  struct Event {
      int id;
      char title[64];
      int category;
      char artist[64];
      char venue[64];
      char time[32];
      float regularPrice;
      float vipPrice;
      int isApproved;
      char seats[10][12];
      struct Event *next;
  };

  struct Booking {
      int bookingId;
      char buyerName[64];
      int eventId;
      char eventTitle[64];
      char categoryName[32];
      char rowChar;
      int col;
      char seatType[16];
      float price;
      char timestamp[32];
      struct Booking *next;
  };
  ```
- **Why Linked Lists?**
  - Event catalogs and customer booking registries grow dynamically at runtime. Using fixed-size arrays would either waste memory or risk buffer overflow when events or bookings exceed initial bounds.
  - Inserting a new event or appending a new booking operates in $O(1)$ time by adjusting the head or tail pointers.
  - Node deletion during cancellation operates in $O(n)$ without shifting subsequent elements in memory.

---

### 2. Two-Dimensional Array (10 × 12 Seating Matrix)
- **Source Files**: [`events.c`](file:///d:/Movie_Booking/events.c), [`server.c`](file:///d:/Movie_Booking/server.c)
- **C Definition**:
  ```c
  char seats[10][12]; // Within struct Event
  ```
- **Auditorium Layout Structure**:
  - **Rows (0 to 9)**: Represented as characters `'A'` through `'J'`.
    - **Rows 0 & 1 (`A` and `B`)**: **VIP Seats** (24 seats total).
    - **Rows 2 to 9 (`C` through `J`)**: **Regular Seats** (96 seats total).
  - **Columns (0 to 11)**: Represented as seat numbers `1` through `12`.
  - **State Encoding**:
    - `'O'`: Open / Available for booking.
    - `'X'`: Booked / Occupied.
- **Why a 2D Array?**
  - An auditorium is inherently a spatial, two-dimensional grid. A 2D array provides natural row-and-column indexing.
  - Checking availability or modifying seat status is an immediate $O(1)$ memory operation:
    ```c
    int r = rowChar - 'A';
    int c = col - 1;
    if (ptr->seats[r][c] == 'O') {
        ptr->seats[r][c] = 'X'; // Seat successfully locked
    }
    ```

---

### 3. Stack (LIFO) for Cancellation History & Undo
- **Source File**: [`stack.c`](file:///d:/Movie_Booking/stack.c)
- **C Definition**:
  ```c
  struct Cancellation {
      int bookingId;
      int eventId;
      char rowChar;
      int col;
      char buyerName[64];
      float refundAmount;
      char timestamp[32];
      struct Cancellation *next;
  };

  static struct Cancellation *cancelTop = NULL;
  ```
- **Mechanics**:
  - **Push Operation (`pushCancellation`)**: Whenever a ticket is cancelled, a new record containing the seat coordinate (`rowChar`, `col`), buyer identity, and event ID is pushed onto the top of `cancelTop` ($O(1)$).
  - **Pop Operation (`popCancellation`)**: When the user requests an **Undo Cancellation**, the most recent cancellation node is popped from the top of the stack ($O(1)$).
  - **Restoration**: The system navigates to the corresponding event's seat matrix, restores the seat from `'O'` to `'X'`, reinstates the booking into `bookingHead`, and decreases the available seat count.
- **Why a Stack?**
  - "Undo" follows the **Last-In, First-Out (LIFO)** principle. The last action performed must be the first action reverted. A stack structure guarantees optimal $O(1)$ push and pop operations without searching.

---

### 4. Queue (FIFO) for Waiting-List Management
- **Source File**: [`queue.c`](file:///d:/Movie_Booking/queue.c)
- **C Definition**:
  ```c
  struct WaitlistNode {
      int eventId;
      char buyerName[64];
      char buyerPhone[16];
      int priority;
      char timestamp[32];
      struct WaitlistNode *next;
  };

  static struct WaitlistNode *waitlistHead = NULL;
  static struct WaitlistNode *waitlistTail = NULL;
  ```
- **Mechanics**:
  - **Enqueue Operation (`enqueueWaitlist`)**: When an event is sold out (`available_seats == 0`), new attendees are added to the back of the queue via `waitlistTail` in $O(1)$ time.
  - **Dequeue Operation (`dequeueWaitlist`)**: When any customer cancels a seat for that event, the attendee at `waitlistHead` is dequeued in $O(1)$ time.
  - **Auto-Allocation Engine**:
    ```
    [Ticket Cancelled] 
            │
            ▼
    [Check Waitlist Queue for eventId] 
            │
      ┌─────┴────────────────┐
      ▼                      ▼
    [Queue Non-Empty]      [Queue Empty]
      │                      │
      ▼                      ▼
    Dequeue Waitlist Head   Mark Seat 'O'
    Book Seat for Attendee  (Available for Public)
    Mark Seat 'X'
    ```
- **Why a Queue?**
  - Waiting lists require strict fairness based on **First-In, First-Out (FIFO)** order. The person waiting the longest must always be allocated the next available seat. Maintaining explicit `head` and `tail` pointers ensures both enqueue and dequeue execute in $O(1)$ time.

---

### 5. Searching Techniques (Linear & Binary Search)

#### A. Multi-Criteria Linear Search — $O(n)$
- Iterates sequentially through the singly linked list.
- Implemented in `searchEvents()` and `searchBookings()`.
- Supports case-insensitive partial substring matching (`strstr` with lower-cased copies) across titles, artist names, venues, and categories.

#### B. Pointer-Array Binary Search — $O(\log n)$
- **The Challenge**: Standard binary search requires $O(1)$ random access by index (`arr[mid]`), which standard singly linked lists cannot provide.
- **The Solution**: 
  1. Measure linked list size $n$.
  2. Dynamically allocate an array of node pointers:
     ```c
     struct Event **arr = malloc(n * sizeof(struct Event *));
     ```
  3. Traverse the linked list once to populate `arr[i] = curr`.
  4. Ensure or sort the pointer array by ID using an in-place sort.
  5. Execute standard binary search over the pointer array:
     ```c
     int low = 0, high = count - 1;
     while (low <= high) {
         int mid = low + (high - low) / 2;
         if (arr[mid]->id == targetId) return arr[mid];
         else if (arr[mid]->id < targetId) low = mid + 1;
         else high = mid - 1;
     }
     ```
  6. Free the temporary pointer array `arr`.
- Implemented in `binarySearchEvents()` ([`events.c`](file:///d:/Movie_Booking/events.c#L137)) and `binarySearchBookings()` ([`bookings.c`](file:///d:/Movie_Booking/bookings.c#L112)).

---

### 6. Algorithmic Complexity Matrix

| Operation | Data Structure / Algorithm | Time Complexity | Space Complexity |
|---|---|:---:|:---:|
| **Event / Booking Insertion** | Singly Linked List (`addEvent`, `bookTicket`) | $O(1)$ | $O(1)$ |
| **Seat Lookup / Matrix Toggle** | 2D Array (`seats[10][12]`) | $O(1)$ | $O(1)$ |
| **Cancel Ticket (Record Push)** | LIFO Stack (`pushCancellation`) | $O(1)$ | $O(1)$ |
| **Undo Cancellation (Pop)** | LIFO Stack (`popCancellation`) | $O(1)$ | $O(1)$ |
| **Join Waitlist (Enqueue)** | FIFO Queue (`enqueueWaitlist`) | $O(1)$ | $O(1)$ |
| **Promote Waitlist (Dequeue)** | FIFO Queue (`dequeueWaitlist`) | $O(1)$ | $O(1)$ |
| **Search by Text / Category** | Linear Search over Linked List | $O(n)$ | $O(1)$ |
| **Search by Unique ID** | Pointer-Array Binary Search | $O(\log n)$ | $O(n)$ (temp) |
| **Atomic Multi-Seat Validation** | Pre-check array of $k$ seat coordinates | $O(k)$ | $O(k)$ |

---

## 💾 File Handling & Data Persistence

The system implements low-level, binary file input/output (`fopen`, `fread`, `fwrite`) using custom serialization routines to persist state across sessions:

| File Name | Persisted Data Structure | Record Format Details |
|---|---|---|
| `events.dat` | Singly Linked List of Events | Serializes event metadata, pricing, approval flags, and the raw $10 \times 12$ character seat matrix buffer. |
| `bookings.dat` | Singly Linked List of Bookings | Stores individual customer bookings, assigned seat numbers, payment amounts, and timestamps. |
| `cancellations.dat` | LIFO Stack of Cancellations | Stores cancellation nodes to retain undo capability even after terminating the application. |
| `waitlists.dat` | FIFO Queue of Waiting Attendees | Stores waiting lists for sold-out events in chronological queue order. |
| `backup/` | Seed Data Fallback | Clean baseline datasets for instant recovery in case of data corruption. |

### Data Integrity & Safety Guard (`isDataSafeToSave`)
To prevent accidental data corruption or overwriting valid databases with empty lists:
- Upon startup, `loadAllData()` verifies that every `.dat` file is readable and consistent.
- If any data file is corrupted or partially missing, the global flag `isDataSafeToSave` is set to `0`, locking write operations and printing a diagnostic error to safeguard customer records.
- If all data files are absent (e.g., fresh clone), the system automatically initializes default sample shows and seeds fresh `.dat` stores.

---

## 🏛️ System Architecture & Interfaces

### Dual-Interface Model

The architecture decouples core business logic and in-memory data structures from user presentation, providing two interfaces:

```
                  ┌─────────────────────────────────────────┐
                  │    CORE C99 DATA STRUCTURES ENGINE     │
                  │  Events • Seats • Bookings • Undo Stack │
                  └──────────────┬──────────────────┬───────┘
                                 │                  │
                Direct Function  │                  │ Winsock Loopback
                    Invocations  │                  │ HTTP / REST API
                                 ▼                  ▼
                    ┌──────────────────┐   ┌──────────────────┐
                    │   Terminal CLI   │   │ Embedded HTTP    │
                    │  (main.c /       │   │ Server (server.c)│
                    │   portals.c)     │   └────────┬─────────┘
                    └──────────────────┘            │
                                                    │ Serves HTML/CSS/JS
                                                    ▼
                                           ┌──────────────────┐
                                           │  BookMyShow Web  │
                                           │  UI (localhost)  │
                                           └──────────────────┘
```

### Embedded C HTTP Server & REST API
Built with native Windows Sockets (`ws2_32.lib`), [`server.c`](file:///d:/Movie_Booking/server.c) implements an embedded web server that parses HTTP/1.1 requests, dynamic query parameters, and JSON payloads.

#### REST API Specification
- `GET /api/events` — Retrieve all approved events (optional `?filter=<category_id>`).
- `GET /api/seats?eventId=<id>` — Fetch the 120-seat matrix for a specific event.
- `GET /api/bookings` — Fetch all confirmed customer reservations.
- `POST /api/book` — Atomically book 1 to 10 seats (`{"eventId": 2, "seats": [{"row":"C","col":1}, ...]}`).
- `POST /api/cancel` — Cancel a booking by ID (triggers automatic waitlist allocation).
- `POST /api/undo` — Pop the last cancellation from the stack and restore the booking.
- `GET /api/waitlist?eventId=<id>` — View the current queue for a sold-out show.
- `POST /api/waitlist` — Join the waiting queue for a sold-out show.
- `GET /api/search?type=<title|artist|venue>&value=<query>` — Multi-criteria search.
- `GET /api/overview` — Administrative metrics (total events, bookings, revenue).

---

## 🛠️ Technologies Used

- **Primary Programming Language**: C (C99 Standard)
- **Compiler Support**: GCC / MinGW-w64 (Windows), Clang, MSVC
- **Networking Library**: Windows Sockets 2 (`ws2_32.lib`)
- **Frontend Stack**:
  - **HTML5**: Semantic single-page layout structure
  - **CSS3**: Custom BookMyShow design system, CSS Grid, Flexbox, responsive breakpoints
  - **Vanilla JavaScript (ES6+)**: Asynchronous `fetch` calls, client state management, modal controllers
- **Version Control**: Git & GitHub

---

## 📁 Project Directory Structure

```text
Movie_Booking/
├── main.c                 # Entry point for the Terminal CLI application
├── server.c               # Embedded Winsock HTTP REST API & static web server
├── project.h              # Common header definitions, structs, constants & prototypes
├── events.c               # Event linked list, 10×12 seat matrices, pointer binary search
├── bookings.c             # Booking linked list, multi-seat atomic transactions
├── stack.c                # LIFO cancellation stack and undo restoration engine
├── queue.c                # FIFO waitlist queue and automatic seat promotion
├── portals.c              # CLI interface logic for Buyer, Artist, and Nodal Officer
├── events.dat             # Binary persistence: Events and seat layout maps
├── bookings.dat           # Binary persistence: Active customer reservations
├── cancellations.dat      # Binary persistence: Cancellation history (Stack)
├── waitlists.dat          # Binary persistence: Chronological waitlists (Queue)
├── backup/                # Seed backup binary datasets for fail-safe initialization
│   ├── events.dat
│   ├── bookings.dat
│   ├── cancellations.dat
│   └── waitlists.dat
├── frontend/              # BookMyShow-inspired web application client
│   ├── index.html         # Single-page web portal structure
│   ├── style.css          # Design system, red accents, responsive media queries
│   └── app.js             # Client state management, 4-step booking flow, modals
├── docs/
│   └── screenshots/
│       └── homepage.png   # Full-page high-resolution discovery UI screenshot
├── .gitignore             # Excludes executables (*.exe), object files (*.o), and IDE files
└── README.md              # Comprehensive project documentation
```

---

## 💻 Compilation & Execution Guide

### Prerequisites
- **Operating System**: Windows 10/11 (or Linux/macOS for CLI mode)
- **Compiler**: GCC via MinGW-w64 (Recommended)
- **Web Browser**: Google Chrome, Mozilla Firefox, or Microsoft Edge

---

### Option A: Web Application (BookMyShow-Style)

1. **Open PowerShell or Terminal** in the project directory:
   ```powershell
   cd d:\Movie_Booking
   ```

2. **Compile the Embedded Server**:
   ```powershell
   gcc -std=c99 -Wall -Wextra -pedantic server.c events.c bookings.c stack.c queue.c portals.c -lws2_32 -o server.exe
   ```

3. **Run the Server Executable**:
   ```powershell
   .\server.exe
   ```

4. **Launch the Web Portal**:
   Open your browser and navigate to:
   ```text
   http://localhost:8080/
   ```
   *You can now browse shows, select seats on the interactive matrix, book multiple tickets, and test cancellations or waiting lists directly from your browser!*

---

### Option B: Terminal CLI Application

1. **Compile the Console Program**:
   ```powershell
   gcc -std=c99 -Wall -Wextra -pedantic main.c events.c bookings.c stack.c queue.c portals.c -o movie_booking.exe
   ```

2. **Run the Console Executable**:
   ```powershell
   .\movie_booking.exe
   ```

3. **Navigate the Main Menu**:
   ```text
   ==================================================
       MOVIE & LOCAL THEATRE TICKET BOOKING SYSTEM   
      Empowering Commercial & Regional Entertainment 
   ==================================================
   Select Role:
   1. Buyer (Discover Shows & Book Tickets)
   2. Artist (Register & Manage Regional Events)
   3. Nodal Officer (System Oversight & Reports)
   4. Exit Program
   --------------------------------------------------
   Enter choice (1-4):
   ```

---

## 🎓 Academic Context & DSA Learnings

This project was built as a capstone laboratory submission for a university **Data Structures and Algorithms (DSA)** curriculum. 

### Key Pedagogical Takeaways
1. **Practical Application of Abstract Data Types (ADTs)**: Shows how theoretical ADTs (Stacks, Queues, Linked Lists) solve concrete, real-world problems (Undos, Waiting Lists, Inventories).
2. **Pointer Arithmetic & Memory Management**: Hands-on allocation (`malloc`/`free`), pointer traversal (`->next`), and avoiding dangling pointers or memory leaks.
3. **Overcoming Linked List Limitations**: Demonstrates how to bridge the gap between linked lists and binary search using dynamically allocated pointer arrays.
4. **Concurrency & Atomicity Considerations**: Implementation of all-or-nothing seat selection prevents race conditions and partial bookings.
5. **System-Level Network Programming**: Demonstrates how an HTTP server operates over TCP sockets at the byte level without heavy third-party web frameworks.

---

## 📄 License & Acknowledgments

- **License**: MIT Open Source License. Free to use, adapt, and extend for academic and educational purposes.
- **Acknowledgments**:
  - Developed with inspiration from commercial platforms like **BookMyShow** and classical Indian performing arts institutions.
  - Image assets curated from verified open sources (The Movie Database [TMDb], Amazon Media Services, and Unsplash).
