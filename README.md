# TSN Leader Election Algorithms - OMNeT++ Project

This project implements two leader election algorithms for Time-Sensitive Networking (TSN) Grand Master selection, integrated with the OMNeT++ INET Framework.

## Algorithms Implemented

### 1. Arbitrary Network Election (Section 11.2.3)
A flooding-based algorithm for general network topologies:
- **Approach**: Each node maintains `L(i)` (leader candidate ID), initially set to its own ID
- **Process**: In each round, nodes exchange L values with neighbors and update to the maximum
- **Termination**: After D rounds, where D is the network diameter
- **Message Complexity**: O(Δ·D) where Δ is the maximum node degree
- **Use Case**: Works on any network topology (ring, mesh, star, random)

### 2. Anonymous Network Election (Section 11.2.4)
A randomized algorithm for fully connected networks:
- **Approach**: Symmetry breaking through randomization without using process IDs
- **Process**: Active nodes randomly choose bits (0 or 1); those choosing 1 advance to next round
- **Termination**: When exactly one node chooses 1, it becomes the leader
- **Expected Rounds**: O(log n) for n processes
- **Message Complexity**: O(n² log n)
- **Use Case**: Fully connected (complete) networks

## Project Structure

```
complex-networks-project/
├── src/
│   ├── TSNNode.h/cc              # Base class for TSN nodes
│   ├── ArbitraryElection.h/cc    # Flooding-based algorithm
│   ├── AnonymousElection.h/cc    # Randomized algorithm
│   ├── messages.msg               # Message definitions
│   └── package.ned                # Package definition
├── simulations/
│   ├── TSNElection.ned            # Network topologies
│   └── omnetpp.ini                # Simulation configurations
├── Makefile                       # Build configuration
└── README.md                      # This file
```

## Network Topologies

The project includes several pre-configured network topologies:

1. **RingNetwork**: Circular topology, suitable for arbitrary election
2. **MeshNetwork**: 2D grid topology
3. **StarNetwork**: Central hub with spokes
4. **FullyConnectedNetwork**: Complete graph for anonymous election
5. **RandomNetwork**: Random connectivity for testing

## Building the Project

### Prerequisites
- OMNeT++ 6.0 or later
- INET Framework 4.x (optional, for advanced features)
- C++14 or later compiler

### Build Steps

1. Generate makefiles:
```bash
cd src
opp_makemake -f --deep -O out -I.
```

2. Build the project:
```bash
make
```

Or use the provided Makefile:
```bash
make all
```

## Running Simulations

### Command Line (Cmdenv)

Run specific configurations:
```bash
# Ring network topology
make run-ring

# Mesh network topology
make run-mesh

# Star network topology
make run-star

# Fully connected network (randomized algorithm)
make run-fc

# Random network topology
make run-random

# Run all configurations
make run-all
```

### GUI Mode (Qtenv)
```bash
make run-gui
```

### Direct execution
```bash
cd simulations
../src/tsnelection -u Cmdenv -c RingNetwork
../src/tsnelection -u Qtenv
```

## Configuration Options

The `omnetpp.ini` file includes multiple configuration scenarios:

- **RingNetwork**: Basic ring topology
- **MeshNetwork**: 2D mesh (3x3 grid by default)
- **StarNetwork**: Star topology with center node
- **FullyConnectedNetwork**: Complete graph for randomized algorithm
- **RandomNetwork**: Random topology with configurable connectivity
- **RingComparison**: Performance comparison across different ring sizes
- **FCComparison**: Randomized algorithm performance analysis
- **IndustrialTSN**: Realistic industrial TSN scenario
- **AutomotiveTSN**: Automotive network scenario

## Key Parameters

### Arbitrary Election Algorithm
- `diameter`: Network diameter D (number of rounds)
- `startDelay`: Initial delay before starting election
- `roundDelay`: Delay between rounds

### Anonymous Election Algorithm
- `startDelay`: Initial delay before starting election
- `roundDelay`: Delay between rounds
- `numNodes`: Must be fully connected topology

### Network Parameters
- `numNodes`: Number of nodes in the network
- `connectivity`: Connection probability for random networks
- `gridSize`: Grid dimensions for mesh networks

## Statistics and Metrics

Both algorithms collect the following statistics:

- **leaderElected**: ID of the elected Grand Master
- **messagesSent**: Total number of messages sent per node
- **roundsCompleted**: Number of rounds to complete election
- **finalState**: Final state of each node (leader/passive/active)

Results are saved in the `simulations/results/` directory as:
- Vector files (.vec): Time-series data
- Scalar files (.sca): Summary statistics

## Algorithm Visualization

When running in GUI mode (Qtenv), you can observe:
- Message exchanges between nodes
- Round progression
- Leader election announcement (nodes display "I am the Grand Master!")
- Visual highlighting of the elected leader (gold color)
- Passive nodes (gray color for anonymous algorithm)

## TSN Integration

These algorithms can be used for:
- **Grand Master Clock Selection**: Electing the primary time source in TSN networks
- **Fault Tolerance**: Re-election when Grand Master fails
- **Distributed Coordination**: Ensuring consistent timing across TSN domains
- **Network Synchronization**: IEEE 802.1AS gPTP protocol integration

## Performance Characteristics

### Arbitrary Network Election
- **Time Complexity**: O(D) rounds, where D is network diameter
- **Message Complexity**: O(Δ·D) messages, where Δ is max degree
- **Best for**: Static topologies with known diameter
- **Guaranteed**: Deterministic termination after D rounds

### Anonymous Network Election
- **Time Complexity**: O(log n) expected rounds
- **Message Complexity**: O(n² log n) expected messages
- **Best for**: Fully connected networks
- **Guaranteed**: Probabilistic termination (very high probability)

## Extending the Project

To add new topologies or algorithms:

1. Create new module in `src/`:
   ```cpp
   class MyAlgorithm : public TSNNode { ... }
   ```

2. Define NED description in `simulations/TSNElection.ned`:
   ```ned
   simple MyAlgorithm extends TSNNode { ... }
   ```

3. Add configuration in `simulations/omnetpp.ini`:
   ```ini
   [Config MyConfig]
   network = MyNetwork
   ...
   ```

## Testing

The project includes multiple test scenarios:

```bash
# Test different network sizes
cd simulations
../src/tsnelection -u Cmdenv -c RingComparison

# Test randomized algorithm convergence
../src/tsnelection -u Cmdenv -c FCComparison

# Test realistic TSN scenarios
../src/tsnelection -u Cmdenv -c IndustrialTSN
../src/tsnelection -u Cmdenv -c AutomotiveTSN
```

## Troubleshooting

### Common Issues

1. **Makefiles not found**: Run `make makefiles` first
2. **Linking errors**: Ensure OMNeT++ environment is properly sourced
3. **Network not terminating**: Check diameter parameter for arbitrary algorithm
4. **No leader elected**: Verify network connectivity (especially for fully connected requirement)

### Debug Mode

Enable debug output:
```bash
cd simulations
../src/tsnelection -u Cmdenv -c RingNetwork --debug-on-errors=true
```

## References

- Distributed Systems: An Algorithmic Approach, Section 11.2
- IEEE 802.1AS-2020: Timing and Synchronization for Time-Sensitive Applications
- OMNeT++ Simulation Manual: https://omnetpp.org/

## License

This project is licensed under the GNU Lesser General Public License v3.0 - see the LICENSE file for details.

## Authors

TSN Leader Election Implementation for OMNeT++
Complex Networks Project

## Acknowledgments

- OMNeT++ Discrete Event Simulator
- INET Framework
- IEEE 802.1 Time-Sensitive Networking Task Group