/**
 * EXAMPLE: How to extend the TSN Leader Election project
 * 
 * This file demonstrates how to add a new leader election algorithm
 * based on the Chang-Roberts ring algorithm.
 */

#ifndef __CHANGROBERTS_H_
#define __CHANGROBERTS_H_

#include "TSNNode.h"
#include <map>

/**
 * Chang-Roberts Algorithm for Ring Networks
 * 
 * A classic leader election algorithm for unidirectional rings.
 * Each node sends its ID clockwise. When a node receives an ID:
 * - If received_id > my_id: forward it
 * - If received_id < my_id: discard it
 * - If received_id == my_id: I am the leader
 * 
 * Best Case: O(n) messages (when nodes are in descending order)
 * Worst Case: O(n²) messages (when nodes are in ascending order)
 * Average Case: O(n log n) messages
 */
class ChangRoberts : public TSNNode
{
  private:
    int maxIdSeen;        // Maximum ID seen so far
    bool isLeader;        // True if this node is the leader
    bool electionComplete;// True when election is finished
    int messagesSent;     // Count of messages sent
    
    cMessage *startTimer; // Timer to start the election
    
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    
    void startElection();
    void forwardId(int id);
    void becomeLeader();
    
  public:
    ChangRoberts();
    virtual ~ChangRoberts();
};

/**
 * Message type for Chang-Roberts algorithm
 */
class RingElectionMsg : public cMessage
{
  private:
    int candidateId;
    int hopCount;
    
  public:
    RingElectionMsg(const char *name = "RingElectionMsg") : cMessage(name) {
        candidateId = -1;
        hopCount = 0;
    }
    
    void setCandidateId(int id) { candidateId = id; }
    int getCandidateId() const { return candidateId; }
    
    void setHopCount(int count) { hopCount = count; }
    int getHopCount() const { return hopCount; }
    void incrementHopCount() { hopCount++; }
};

#endif

/**
 * IMPLEMENTATION (ChangRoberts.cc)
 */

/*
#include "ChangRoberts.h"

Define_Module(ChangRoberts);

ChangRoberts::ChangRoberts()
{
    maxIdSeen = -1;
    isLeader = false;
    electionComplete = false;
    messagesSent = 0;
    startTimer = nullptr;
}

ChangRoberts::~ChangRoberts()
{
    cancelAndDelete(startTimer);
}

void ChangRoberts::initialize()
{
    TSNNode::initialize();
    
    maxIdSeen = nodeId;
    
    EV << "Node " << nodeId << " initialized (Chang-Roberts)\n";
    
    // Start election after delay
    startTimer = new cMessage("startTimer");
    scheduleAt(simTime() + par("startDelay").doubleValue(), startTimer);
}

void ChangRoberts::handleMessage(cMessage *msg)
{
    if (msg == startTimer) {
        startElection();
    }
    else if (dynamic_cast<RingElectionMsg*>(msg)) {
        RingElectionMsg *rmsg = check_and_cast<RingElectionMsg*>(msg);
        int candidateId = rmsg->getCandidateId();
        
        EV << "Node " << nodeId << " received candidate ID " << candidateId << "\n";
        
        if (candidateId > maxIdSeen) {
            // Forward the larger ID
            maxIdSeen = candidateId;
            forwardId(candidateId);
            EV << "Node " << nodeId << " forwarding ID " << candidateId << "\n";
        }
        else if (candidateId == nodeId && !electionComplete) {
            // My ID came back - I am the leader!
            becomeLeader();
        }
        // else: discard (candidateId < maxIdSeen)
        
        delete msg;
    }
    else {
        delete msg;
    }
}

void ChangRoberts::startElection()
{
    EV << "Node " << nodeId << " starting election\n";
    forwardId(nodeId);
}

void ChangRoberts::forwardId(int id)
{
    // In a ring, forward to the next node (clockwise)
    if (neighbors.size() > 0) {
        int nextNeighbor = *neighbors.begin(); // Get first neighbor
        int gateIndex = getNeighborGateIndex(nextNeighbor);
        
        RingElectionMsg *rmsg = new RingElectionMsg();
        rmsg->setCandidateId(id);
        
        send(rmsg, "port$o", gateIndex);
        messagesSent++;
        emit(messagesSentSignal, messagesSent);
    }
}

void ChangRoberts::becomeLeader()
{
    isLeader = true;
    electionComplete = true;
    
    EV << "Node " << nodeId << " ELECTED AS LEADER (Chang-Roberts)\n";
    
    emit(leaderElectedSignal, nodeId);
    
    bubble("I am the Leader!");
    getDisplayString().setTagArg("i", 1, "gold");
    getDisplayString().setTagArg("i", 2, "40");
}

void ChangRoberts::finish()
{
    recordScalar("isLeader", isLeader ? 1 : 0);
    recordScalar("totalMessagesSent", messagesSent);
    recordScalar("maxIdSeen", maxIdSeen);
    
    EV << "Node " << nodeId << " statistics:\n";
    EV << "  Is Leader: " << (isLeader ? "YES" : "NO") << "\n";
    EV << "  Messages Sent: " << messagesSent << "\n";
    EV << "  Max ID Seen: " << maxIdSeen << "\n";
}
*/

/**
 * NED DEFINITION (Add to TSNElection.ned)
 */

/*
simple ChangRoberts extends TSNNode
{
    parameters:
        @class(ChangRoberts);
        double startDelay @unit(s) = default(0.1s);
}

network ChangRobertsRing
{
    parameters:
        int numNodes = default(8);
        
    submodules:
        node[numNodes]: ChangRoberts {
            parameters:
                nodeId = index;
                @display("p=,,ring");
        }
        
    connections:
        // Unidirectional ring (clockwise)
        for i=0..numNodes-2 {
            node[i].port++ --> {delay = 10ms;} --> node[i+1].port++;
        }
        node[numNodes-1].port++ --> {delay = 10ms;} --> node[0].port++;
}
*/

/**
 * CONFIGURATION (Add to omnetpp.ini)
 */

/*
[Config ChangRobertsRing]
network = ChangRobertsRing
description = "Chang-Roberts algorithm on unidirectional ring"
*.numNodes = 8
repeat = 5
*/

/**
 * USAGE EXAMPLES
 * 
 * 1. Add to Makefile:
 * 
 * run-changroberts:
 *     cd simulations && ../src/tsnelection -u Cmdenv -c ChangRobertsRing
 * 
 * 
 * 2. Build and run:
 * 
 * ./build.sh
 * make run-changroberts
 * 
 * 
 * 3. Compare with other algorithms:
 * 
 * # Ring with 8 nodes
 * make run-ring              # O(n²) messages with flooding
 * make run-changroberts      # O(n log n) average messages
 * 
 * 
 * 4. Analyze results:
 * 
 * python3 analyze_results.py
 */

/**
 * EXTENDING FURTHER: Priority-Based Election
 * 
 * Modify to use priority values instead of IDs:
 */

/*
class PriorityElection : public ChangRoberts
{
  private:
    double priority;  // Priority value (clock quality, etc.)
    
  protected:
    virtual void initialize() override {
        ChangRoberts::initialize();
        priority = par("priority").doubleValue();
        maxIdSeen = nodeId;
    }
    
    // Override to compare priorities
    virtual bool shouldForward(int candidateId) {
        double candidatePriority = getPriorityOf(candidateId);
        return candidatePriority > priority;
    }
};
*/

/**
 * TESTING YOUR EXTENSION
 * 
 * Create a test configuration:
 */

/*
[Config TestChangRoberts]
network = ChangRobertsRing
description = "Test different orderings"

# Best case: descending order
*.numNodes = 8
*.node[0].nodeId = 7
*.node[1].nodeId = 6
*.node[2].nodeId = 5
# ... etc

# Worst case: ascending order
# (uncomment for worst case test)
#*.node[0].nodeId = 0
#*.node[1].nodeId = 1
#*.node[2].nodeId = 2
# ... etc

repeat = 10
*/

/**
 * INTEGRATION WITH EXISTING CODE
 * 
 * The new algorithm automatically inherits from TSNNode:
 * - Neighbor discovery
 * - Statistics collection
 * - Message broadcasting utilities
 * - Signal emission
 * 
 * You only need to implement:
 * - Algorithm-specific message handling
 * - State management
 * - Termination detection
 */

/**
 * BEST PRACTICES
 * 
 * 1. Use descriptive class and variable names
 * 2. Add comprehensive comments explaining algorithm
 * 3. Implement proper cleanup in destructor
 * 4. Use EV for logging (can be disabled)
 * 5. Record relevant statistics in finish()
 * 6. Test with different network sizes
 * 7. Compare with theoretical complexity
 * 8. Document in markdown files
 */

/**
 * DEBUGGING TIPS
 * 
 * 1. Run with debug output:
 *    ./tsnelection -u Cmdenv -c MyConfig --debug-on-errors=true
 * 
 * 2. Use EV_INFO for important messages:
 *    EV_INFO << "Node " << nodeId << " important event\n";
 * 
 * 3. Check gate connections:
 *    for (int i = 0; i < gateSize("port"); i++) {
 *        EV << "Gate " << i << " connected: " 
 *           << gate("port$o", i)->isConnected() << "\n";
 *    }
 * 
 * 4. Verify message reception:
 *    EV << "Received " << msg->getName() 
 *       << " from " << msg->getSenderModule()->getName() << "\n";
 * 
 * 5. Run in GUI mode for visualization:
 *    make run-gui
 */

/**
 * PERFORMANCE PROFILING
 * 
 * Compare message complexity empirically:
 */

/*
python3 << 'EOF'
import matplotlib.pyplot as plt

# Data from simulations
nodes = [4, 8, 12, 16, 20]
arbitrary = [32, 64, 144, 256, 400]    # O(n²) for ring
changroberts = [8, 24, 48, 80, 120]    # O(n log n) average

plt.figure(figsize=(10, 6))
plt.plot(nodes, arbitrary, 'b-o', label='Arbitrary (Flooding)')
plt.plot(nodes, changroberts, 'r-s', label='Chang-Roberts')
plt.xlabel('Number of Nodes')
plt.ylabel('Total Messages')
plt.title('Message Complexity Comparison')
plt.legend()
plt.grid(True)
plt.savefig('comparison.png')
plt.show()
EOF
*/

/**
 * CONTRIBUTING BACK
 * 
 * If you create a useful extension:
 * 1. Document it thoroughly
 * 2. Add test configurations
 * 3. Update README.md
 * 4. Create examples
 * 5. Share with the community
 */
