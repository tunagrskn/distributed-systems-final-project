TSN Leader Election - Project Architecture Diagrams
===================================================

## Algorithm Flow Diagrams

### 1. Arbitrary Network Election (Flooding-Based)

```
Initial State:
┌────────┐  ┌────────┐  ┌────────┐  ┌────────┐
│ Node 0 │  │ Node 1 │  │ Node 2 │  │ Node 3 │
│  L=0   │──│  L=1   │──│  L=2   │──│  L=3   │
└────────┘  └────────┘  └────────┘  └────────┘

Round 1: Exchange L values
┌────────┐  ┌────────┐  ┌────────┐  ┌────────┐
│ Node 0 │→1│ Node 1 │→2│ Node 2 │→3│ Node 3 │
│  L=1   │←0│  L=2   │←1│  L=3   │←2│  L=3   │
└────────┘  └────────┘  └────────┘  └────────┘

Round 2: Continue propagating max
┌────────┐  ┌────────┐  ┌────────┐  ┌────────┐
│ Node 0 │→2│ Node 1 │→3│ Node 2 │→3│ Node 3 │
│  L=2   │←1│  L=3   │←2│  L=3   │←3│  L=3   │
└────────┘  └────────┘  └────────┘  └────────┘

Final State (after D rounds):
┌────────┐  ┌────────┐  ┌────────┐  ┌────────┐
│ Node 0 │  │ Node 1 │  │ Node 2 │  │ Node 3 │
│  L=3   │  │  L=3   │  │  L=3   │  │★ L=3   │
└────────┘  └────────┘  └────────┘  └────────┘
                                    ★ = LEADER
```

### 2. Anonymous Network Election (Randomized)

```
Initial State: All nodes ACTIVE
     ┌───┐
     │ 0 │
   ┌─┼───┼─┐
 ┌─┘ │ 1 │ └─┐
 │ 2 └───┘ 3 │
 └─┐  ┌──┐ ┌─┘
   └──┤ 4├─┘
      └──┘
All connected (fully connected graph)

Round 1: Choose random bits
Node 0: bit=1 (ACTIVE)
Node 1: bit=0 (PASSIVE)
Node 2: bit=1 (ACTIVE)
Node 3: bit=1 (ACTIVE)
Node 4: bit=0 (PASSIVE)

|S| = 3, so nodes with bit=1 continue
     ┌───┐
     │ 0 │ ACTIVE
     └───┘
       │
   ┌───┼───┐
 ┌─┘   │   └─┐
 │ 2   │   3 │ ACTIVE
 └─┐   │   ┌─┘
   │  (1)  │  PASSIVE
   │  (4)  │  PASSIVE
   └───────┘

Round 2: Continue with 3 active nodes
Node 0: bit=1 (ACTIVE)
Node 2: bit=0 (PASSIVE)
Node 3: bit=0 (PASSIVE)

|S| = 1, Node 0 is LEADER!
     ┌───┐
     │★0 │ LEADER ★
     └───┘
```

## Class Hierarchy

```
┌─────────────────┐
│    TSNNode      │
│   (base class)  │
└────────┬────────┘
         │
    ┌────┴─────────────────┐
    │                      │
┌───┴──────────────┐  ┌───┴──────────────┐
│ArbitraryElection │  │AnonymousElection │
│                  │  │                  │
│ - L: int         │  │ - state: enum    │
│ - round: int     │  │ - bit: int       │
│ - diameter: int  │  │ - activeNodes    │
│                  │  │                  │
│ + startRound()   │  │ + startRound()   │
│ + processRound() │  │ + processRound() │
│                  │  │ + becomeLeader() │
│                  │  │ + becomePassive()│
└──────────────────┘  └──────────────────┘
```

## Message Flow

### Arbitrary Election - Ring Topology

```
Time →

Node 0: [L=0]──→[L=1]──→[L=2]──→[L=3]──→[L=3] ✓
           ↓      ↓      ↓      ↓      ↓
Node 1:    [L=1]──→[L=2]──→[L=3]──→[L=3]──→[L=3] ✓
              ↓      ↓      ↓      ↓      ↓
Node 2:       [L=2]──→[L=3]──→[L=3]──→[L=3]──→[L=3] ✓
                 ↓      ↓      ↓      ↓      ↓
Node 3:          [L=3]──→[L=3]──→[L=3]──→[L=3]──→[L=3] ★

        Round:    0      1      2      3      4
```

### Anonymous Election - State Transitions

```
         ┌─────────┐
    ┌────│  ACTIVE │────┐
    │    └─────────┘    │
    │         │         │
    │    bit=0│    bit=1│
    │    1<|S|<n   1<|S|<n
    │         │         │
    ↓         ↓         │
┌─────────┐         ┌──┘
│ PASSIVE │         │
└─────────┘    |S|=1,bit=1
                    │
                    ↓
               ┌─────────┐
               │  LEADER │
               └─────────┘
```

## Network Topologies

### Ring Network (8 nodes)
```
        0
       ╱ ╲
      7   1
     │     │
     6     2
      ╲   ╱
        5─4─3

Diameter: 4
Max Degree: 2
Messages: O(n²)
```

### Star Network (7 nodes)
```
    1   2   3
     ╲  │  ╱
      ╲ │ ╱
        0
      ╱ │ ╲
     ╱  │  ╲
    4   5   6

Diameter: 2
Max Degree: 6
Messages: O(n)
```

### Mesh Network (3×3)
```
0───1───2
│   │   │
3───4───5
│   │   │
6───7───8

Diameter: 4
Max Degree: 4
Messages: O(n√n)
```

### Fully Connected (6 nodes)
```
All nodes connected to all others:

  0─────1
  │╲   ╱│
  │ ╲ ╱ │
  │  ╳  │
  │ ╱ ╲ │
  │╱   ╲│
  2─────3
  │╲   ╱│
  │ ╲ ╱ │
  │  ╳  │
  │ ╱ ╲ │
  │╱   ╲│
  4─────5

Diameter: 1
Max Degree: 5
Messages: O(n²logn)
```

## Performance Comparison

```
Time Complexity:
─────────────────
Arbitrary:  ████████ (D rounds, deterministic)
Anonymous:  ████ (log n rounds, expected)

Message Complexity (n=10):
────────────────────────────
Ring (Arbitrary):     ████████ (80 messages)
Star (Arbitrary):     ██████ (60 messages)
Mesh (Arbitrary):     ███████████ (120 messages)
Full (Anonymous):     ████████████████ (300+ messages)

Legend: Each █ represents relative complexity
```

## State Machine - Anonymous Election

```
╔═══════════════════════════════════════╗
║        Anonymous Election FSM          ║
╚═══════════════════════════════════════╝

    ┌─────────────────────────────────┐
    │        INITIALIZE               │
    │  state = ACTIVE                 │
    │  activeNodes = all nodes        │
    └──────────────┬──────────────────┘
                   │
                   ↓
    ┌──────────────────────────────────┐
    │      START ROUND                 │
    │  Choose random bit (0 or 1)      │
    │  Broadcast to all neighbors      │
    └──────────────┬───────────────────┘
                   │
                   ↓
    ┌──────────────────────────────────┐
    │    RECEIVE & COUNT               │
    │  Collect bits from all nodes     │
    │  Compute |S| = count(bit=1)      │
    └──────────────┬───────────────────┘
                   │
          ┌────────┴────────┐
          │                 │
    |S|=1 │           1<|S|<n│
    I=1   │                 │
          ↓                 ↓
    ┌─────────┐      ┌──────────────┐
    │ LEADER  │      │  bit=1?      │
    └─────────┘      └─────┬────┬───┘
                           │    │
                        YES│    │NO
                           │    │
                     ┌─────↓    ↓─────┐
                     │ ACTIVE   PASSIVE│
                     │ (repeat) (quit) │
                     └──────────────────┘
```

## TSN Integration Architecture

```
╔══════════════════════════════════════════════════╗
║         TSN Grand Master Selection               ║
╚══════════════════════════════════════════════════╝

┌─────────────────────────────────────────────────┐
│          Application Layer                      │
│  ┌────────────────────────────────────────┐    │
│  │   TSN Grand Master Clock Service       │    │
│  └──────────────┬─────────────────────────┘    │
└─────────────────┼──────────────────────────────┘
                  │
┌─────────────────┼──────────────────────────────┐
│  Leader Election│Layer                         │
│  ┌──────────────▼──────────┐  ┌──────────────┐ │
│  │  ArbitraryElection      │  │ Anonymous    │ │
│  │  (Known Topology)       │  │ Election     │ │
│  └─────────────────────────┘  └──────────────┘ │
└─────────────────┼──────────────────────────────┘
                  │
┌─────────────────┼──────────────────────────────┐
│  Network Layer  │                              │
│  ┌──────────────▼──────────────┐               │
│  │  INET Framework (Optional)  │               │
│  │  - EthernetInterface        │               │
│  │  - TSN Traffic Shaping      │               │
│  └─────────────────────────────┘               │
└──────────────────────────────────────────────────┘
```

## Simulation Workflow

```
┌───────────────────────────────────────────────────┐
│  1. INITIALIZATION                                │
│     - Load network topology (NED file)            │
│     - Create nodes and connections                │
│     - Assign node IDs                             │
└─────────────────┬─────────────────────────────────┘
                  ↓
┌─────────────────────────────────────────────────┐
│  2. NEIGHBOR DISCOVERY                          │
│     - Each node queries connected gates         │
│     - Build neighbor list                       │
└─────────────────┬───────────────────────────────┘
                  ↓
┌─────────────────────────────────────────────────┐
│  3. ELECTION ALGORITHM                          │
│     ┌─────────────────────────────────────┐    │
│     │  For each round:                    │    │
│     │    - Exchange messages              │    │
│     │    - Update state                   │    │
│     │    - Check termination condition    │    │
│     └─────────────────────────────────────┘    │
└─────────────────┬───────────────────────────────┘
                  ↓
┌─────────────────────────────────────────────────┐
│  4. LEADER ANNOUNCEMENT                         │
│     - Elected leader identified                 │
│     - Broadcast to all nodes                    │
│     - Visual indication (gold color)            │
└─────────────────┬───────────────────────────────┘
                  ↓
┌─────────────────────────────────────────────────┐
│  5. STATISTICS COLLECTION                       │
│     - Record final leader                       │
│     - Count messages sent                       │
│     - Measure convergence time                  │
│     - Save to .sca and .vec files              │
└─────────────────────────────────────────────────┘
```

---
Note: These are ASCII art diagrams for documentation.
For high-quality diagrams, consider using:
- draw.io / diagrams.net
- PlantUML
- GraphViz
- OMNeT++ built-in visualization tools
