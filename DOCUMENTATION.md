# TSN Leader Election Algorithms - Technical Documentation

## Table of Contents
1. [Algorithm Details](#algorithm-details)
2. [Implementation Architecture](#implementation-architecture)
3. [Message Protocols](#message-protocols)
4. [Performance Analysis](#performance-analysis)
5. [Integration with TSN](#integration-with-tsn)

---

## Algorithm Details

### 1. Arbitrary Network Election (Flooding-Based)

#### Algorithm Description
This algorithm implements leader election for general network topologies using a flooding approach.

**Pseudocode:**
```
program election_arbitrary_network {for each process i}
    define r: integer {round number}
    define L: process id {identifies the leader}
    
    initially:
        r = 0
        L(i) = i
    
    do r < D →
        send L(i) to each j ∈ N(i)
        receive L(j) from each j ∈ N(i)
        L(i) := max{L(i) ∪ L(j): j ∈ N(i)}
        r := r + 1
    od
end
```

#### Key Properties
- **Deterministic**: Always terminates after exactly D rounds
- **Correct**: Guarantees unique leader (node with maximum ID)
- **Network Requirement**: Any connected topology
- **Knowledge Required**: Network diameter D

#### Implementation Details

**State Variables:**
- `L`: Current leader candidate (maximum ID seen)
- `round`: Current round number (0 to D)
- `receivedL`: Map of L values received from neighbors
- `isLeader`: Boolean flag set when election completes

**Message Flow:**
1. Each round, node broadcasts its L value to all neighbors
2. Node waits to receive L from all neighbors
3. Node updates L to maximum of received values
4. After D rounds, node with L == nodeId is the leader

**Correctness Proof:**
- After round k, L(i) contains the maximum ID within distance k
- After D rounds (diameter), all nodes have seen all IDs
- All nodes converge to the same maximum ID
- Node with max ID recognizes itself as leader

### 2. Anonymous Network Election (Randomized)

#### Algorithm Description
This algorithm achieves leader election without using process IDs, relying on randomization for symmetry breaking.

**Pseudocode:**
```
program election_anonymous {for each active process i in every round}
    define b: bit
    define state ∈ {active, passive}
    define S = {i: b(i) = 1}
    
    initially:
        every process is active
        ∀i: b(i) = 0
        S = ∅
    
    loop:
        b(i) := random{0,1}
        send b(i) to every active process
        receive b(j) from every active process
        
        if (|S| = 1) ∧ (i ∈ S) → i is the leader; break
        [] (1 < |S| < n) ∧ (i ∈ S) → i moves to next round
        [] (1 < |S| < n) ∧ (i ∉ S) → state := passive; quit
        [] (|S| = 0) ∨ (|S| = n) → repeat {no progress}
    end loop
end
```

#### Key Properties
- **Probabilistic**: Expected termination in O(log n) rounds
- **Anonymous**: Does not rely on process IDs
- **Network Requirement**: Fully connected (complete graph)
- **Knowledge Required**: None (fully distributed)

#### Implementation Details

**State Variables:**
- `state`: ACTIVE, PASSIVE, or LEADER
- `bit`: Random bit chosen in current round (0 or 1)
- `receivedBits`: Map of bits received from neighbors
- `activeNodes`: Set of currently active nodes

**State Transitions:**
```
ACTIVE → LEADER:    when |S| = 1 and I chose 1
ACTIVE → PASSIVE:   when 1 < |S| < n and I chose 0
ACTIVE → ACTIVE:    when 1 < |S| < n and I chose 1
                    or when all chose same bit (no progress)
```

**Probabilistic Analysis:**
- Probability that exactly one node chooses 1: $P = \binom{n}{1} \cdot \frac{1}{2^n} = \frac{n}{2^n}$
- Expected nodes advancing: $E[\text{next round}] = \frac{n}{2}$
- Expected rounds: $E[R] = O(\log n)$
- Probability of no progress (all same): $P_{\text{no progress}} = \frac{2}{2^n} \to 0$

---

## Implementation Architecture

### Class Hierarchy

```
TSNNode (base class)
    ├── ArbitraryElection
    └── AnonymousElection
```

### TSNNode Base Class

**Responsibilities:**
- Neighbor discovery
- Message broadcasting
- Statistics collection
- Gate management

**Key Methods:**
```cpp
void initialize()           // Setup node parameters
void discoverNeighbors()    // Find connected neighbors
void broadcastToNeighbors() // Send to all neighbors
int getNeighborGateIndex()  // Map neighbor ID to gate
```

### ArbitraryElection Class

**Extends:** TSNNode

**Additional State:**
- Leader candidate tracking
- Round synchronization
- Message collection per round

**Key Methods:**
```cpp
void startRound()        // Begin new round, broadcast L
void processRound()      // Update L, schedule next round
void completeElection()  // Finalize leader selection
```

### AnonymousElection Class

**Extends:** TSNNode

**Additional State:**
- Active/Passive/Leader state machine
- Bit selection and collection
- Active node tracking

**Key Methods:**
```cpp
void startRound()      // Choose random bit, broadcast
void processRound()    // Count S, make state transition
void becomeLeader()    // Transition to leader state
void becomePassive()   // Transition to passive state
```

---

## Message Protocols

### LeaderMsg (Arbitrary Election)

**Fields:**
- `senderId`: ID of sending node
- `leaderValue`: Current L value
- `roundNum`: Round number for synchronization

**Purpose:** Exchange leader candidate values

**Flow:**
```
Node i (L=5, round=1) → LeaderMsg{sender=i, leader=5, round=1} → Neighbor j
```

### BitMsg (Anonymous Election)

**Fields:**
- `senderId`: ID of sending node (for tracking only)
- `bitValue`: Random bit (0 or 1)
- `roundNum`: Round number
- `isActive`: Whether sender is still active

**Purpose:** Exchange random bits for symmetry breaking

**Flow:**
```
Active Node i → BitMsg{sender=i, bit=1, round=2, active=true} → All neighbors
```

### LeaderAnnouncement

**Fields:**
- `leaderId`: ID of elected leader

**Purpose:** Broadcast final leader to all nodes

---

## Performance Analysis

### Arbitrary Network Election

#### Time Complexity
- **Rounds:** Exactly D (network diameter)
- **Per Round:** O(1) with synchronization
- **Total Time:** O(D)

#### Message Complexity
- **Per Node per Round:** deg(i) messages sent
- **Total Messages:** $\sum_{i \in V} D \cdot deg(i) = D \cdot 2|E|$
- **Asymptotic:** $O(D \cdot \Delta)$ where Δ is max degree

#### Space Complexity
- **Per Node:** O(|neighbors|) for storing received values
- **Total:** O(|E|)

#### Performance by Topology

| Topology | Diameter D | Max Degree Δ | Messages |
|----------|------------|--------------|----------|
| Ring (n nodes) | n/2 | 2 | O(n²) |
| Star (n nodes) | 2 | n-1 | O(n) |
| Mesh (√n × √n) | 2√n | 4 | O(n√n) |
| Complete (n nodes) | 1 | n-1 | O(n²) |

### Anonymous Network Election

#### Time Complexity
- **Expected Rounds:** O(log n)
- **With high probability:** O(log n + log log n)
- **Worst case:** Unbounded (probabilistic)

#### Message Complexity
- **Per Round:** n² messages (fully connected)
- **Expected Total:** O(n² log n)

#### Convergence Analysis

Expected number of rounds for different network sizes:

| Nodes (n) | Expected Rounds | Expected Messages |
|-----------|----------------|-------------------|
| 4 | 2.0 | 32 |
| 8 | 3.0 | 192 |
| 16 | 4.0 | 1,024 |
| 32 | 5.0 | 5,120 |
| 64 | 6.0 | 24,576 |

---

## Integration with TSN

### IEEE 802.1AS Grand Master Election

TSN networks use IEEE 802.1AS for time synchronization, which includes a Best Master Clock Algorithm (BMCA). Our leader election algorithms can complement or replace BMCA in specific scenarios.

#### Use Cases

**1. Initial Grand Master Selection**
- Network boots, all nodes eligible
- Use arbitrary election if topology is known
- Use anonymous election if topology is fully connected

**2. Grand Master Failure Recovery**
- Current GM fails or loses connectivity
- Trigger new election among remaining nodes
- Fast convergence critical for TSN timing

**3. Priority-Based Selection**
- Node IDs represent clock priority
- Higher ID = better clock source
- Arbitrary election naturally selects highest priority

#### Integration Points

```cpp
class TSNGrandMaster : public ArbitraryElection {
    // Add IEEE 802.1AS specific features
    ClockQuality clockQuality;
    Priority1 priority1;
    Priority2 priority2;
    
    // Override ID comparison to use full BMCA
    virtual int compareNodes(int id1, int id2) {
        // Compare using IEEE 802.1AS rules
        // priority1, clockClass, clockAccuracy, etc.
    }
};
```

#### Timing Requirements

TSN has strict timing requirements:

- **Sync Interval:** 125µs - 1s (typically 1ms)
- **Convergence Time:** < 500ms after GM change
- **Stability:** Avoid frequent re-elections

Our algorithms provide:
- **Arbitrary Election:** Deterministic D-round convergence
- **Anonymous Election:** Expected O(log n) rounds

For n=16 nodes, D=4:
- At 100ms per round → 400ms convergence ✓
- At 50ms per round → 200ms convergence ✓

### Fault Tolerance

#### GM Failure Detection
```cpp
void detectGMFailure() {
    if (missedSyncMessages > THRESHOLD) {
        // GM failed, trigger re-election
        triggerElection();
    }
}
```

#### Partition Tolerance
- Each partition independently elects a leader
- On partition heal, re-run election
- Arbitrary algorithm handles arbitrary partitions

---

## Configuration Best Practices

### Choosing an Algorithm

**Use Arbitrary Election when:**
- Network topology is not fully connected
- Deterministic convergence is required
- Network diameter is known or can be estimated
- ID-based priority is acceptable

**Use Anonymous Election when:**
- Network is fully connected
- Nodes are truly anonymous (no IDs)
- Randomized algorithms are acceptable
- Expected convergence is sufficient

### Parameter Tuning

#### Round Delay
- **Too small:** Network congestion, message loss
- **Too large:** Slow convergence
- **Recommended:** 50-200ms for realistic networks

#### Start Delay
- **Purpose:** Stagger node initialization
- **Recommended:** uniform(0, 0.5s)

#### Diameter Estimation
For arbitrary election, if D is unknown:
- **Conservative:** Use n-1 (max possible)
- **Estimate:** Based on topology type
- **Dynamic:** Implement diameter discovery

---

## Testing and Validation

### Unit Tests
Test individual components:
- Neighbor discovery
- Message handling
- State transitions
- Leader announcement

### Integration Tests
Test complete algorithms:
- Small networks (n < 10)
- Various topologies
- Failure scenarios

### Performance Tests
Measure and validate:
- Convergence time vs. theory
- Message counts vs. predictions
- Scalability (n = 10, 50, 100)

### Correctness Properties

**Safety:**
- At most one leader elected
- All nodes agree on leader

**Liveness:**
- Election eventually terminates
- A leader is eventually elected

**Verification:**
```bash
# Run multiple times, check statistics
for i in {1..100}; do
    ./tsnelection -c RingNetwork --seed-set=$i
done
python analyze_results.py
# Verify: exactly 1 leader per run
```

---

## Future Extensions

### Possible Enhancements

1. **Dynamic Topology**
   - Handle node joins/leaves
   - Incremental re-election

2. **Message Optimization**
   - Aggregate messages
   - Reduce redundant broadcasts

3. **INET Integration**
   - Use INET's StandardHost
   - Integrate with Ethernet/TSN modules

4. **Real Clock Sources**
   - Interface with system clock
   - Implement PTP (Precision Time Protocol)

5. **Fault Injection**
   - Simulate node failures
   - Test recovery mechanisms

---

## References

### Academic Papers
- Distributed Systems: An Algorithmic Approach (2nd Edition)
- "A log(n) Distributed Algorithm for Leader Election"
- "Election in Computer Networks" - Garcia-Molina

### Standards
- IEEE 802.1AS-2020: Timing and Synchronization
- IEEE 802.1Qbv: Time-Aware Shaper
- IEEE 1588: Precision Time Protocol

### OMNeT++ Resources
- OMNeT++ Manual: https://omnetpp.org/documentation
- INET Framework: https://inet.omnetpp.org/
- TicToc Tutorial: https://docs.omnetpp.org/tutorials/tictoc/

---

## Appendix

### Complete Parameter Reference

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| nodeId | int | - | Unique node identifier |
| numNodes | int | - | Total nodes in network |
| diameter | int | - | Network diameter (ArbitraryElection) |
| startDelay | double | 0.1s | Initial delay before starting |
| roundDelay | double | 0.5s | Delay between rounds |
| connectivity | double | 0.3 | Connection probability (RandomNetwork) |
| gridSize | int | 3 | Grid dimensions (MeshNetwork) |

### Statistics Reference

| Statistic | Type | Meaning |
|-----------|------|---------|
| leaderElected | scalar | ID of elected leader |
| isLeader | scalar | 1 if node is leader, 0 otherwise |
| totalMessagesSent | scalar | Total messages sent by node |
| totalRounds | scalar | Number of rounds completed |
| finalState | scalar | Final state (0=active, 1=passive, 2=leader) |

---

**Document Version:** 1.0  
**Last Updated:** December 2025  
**Authors:** TSN Leader Election Project Team
