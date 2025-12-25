# Quick Start Guide - TSN Leader Election

This guide will help you quickly build and run the TSN Leader Election simulations.

## Prerequisites

Before starting, ensure you have:
- **OMNeT++ 6.0+** installed and configured
- **C++ compiler** (g++ or clang++)
- **Make** build tool
- **Python 3** (optional, for result analysis)

## Step 1: Environment Setup

Source the OMNeT++ environment:

```bash
# Replace with your OMNeT++ installation path
source /path/to/omnetpp-6.0/setenv

# Verify installation
opp_run --version
```

## Step 2: Build the Project

### Option A: Using the build script (recommended)

```bash
./build.sh
```

### Option B: Manual build

```bash
# Generate makefiles
cd src
opp_makemake -f --deep -O out -I.
cd ..

# Build
make all
```

## Step 3: Run Simulations

### Quick Test - Ring Network

```bash
make run-ring
```

Expected output:
```
Node 0 initialized with L=0, diameter=4
Node 1 initialized with L=1, diameter=4
...
Node 7 completed election: L=7 (I AM THE LEADER)
```

### All Topologies

```bash
# Ring topology (8 nodes)
make run-ring

# Mesh topology (3x3 grid)
make run-mesh

# Star topology (7 nodes)
make run-star

# Fully connected (randomized algorithm)
make run-fc

# Random network
make run-random
```

### GUI Mode

```bash
make run-gui
```

This opens the OMNeT++ Qtenv GUI where you can:
- Visualize network topology
- Step through execution
- Watch message exchanges
- See leader election in real-time

## Step 4: Analyze Results

Run the Python analysis script:

```bash
python3 analyze_results.py
```

Sample output:
```
Configuration: RingNetwork
----------------------------------------------------------------------
  finalLeader                   : avg=    7.00, min=    7.00, max=    7.00
  totalMessagesSent             : avg=    8.00, min=    8.00, max=    8.00
  totalRounds                   : avg=    4.00, min=    4.00, max=    4.00
  
  Elected Leader(s): ['RingNetwork.node[7]']
  Total Messages Sent: 64
```

## Understanding the Results

### Arbitrary Network Election (Ring, Mesh, Star)

- **Leader**: Node with highest ID
- **Rounds**: Exactly D (network diameter)
- **Messages**: O(Δ·D) where Δ is max degree

Example for 8-node ring:
- Diameter D = 4
- Each node sends 4 rounds × 2 neighbors = 8 messages
- Total: 8 nodes × 8 messages = 64 messages

### Anonymous Network Election (Fully Connected)

- **Leader**: Randomly selected (any node can win)
- **Rounds**: O(log n) expected
- **Messages**: O(n² log n) expected

Example for 6-node fully connected:
- Expected rounds ≈ log₂(6) ≈ 2.6 rounds
- Each round: 6 nodes × 5 neighbors = 30 messages
- Expected total: ≈ 78 messages

## Common Configurations

### Test Different Network Sizes

Edit `simulations/omnetpp.ini`:

```ini
[Config MyRing]
network = RingNetwork
*.numNodes = 16        # Increase from 8 to 16
*.diameter = 8         # Diameter = numNodes / 2
```

Run:
```bash
cd simulations
../src/tsnelection -u Cmdenv -c MyRing
```

### Test Randomized Algorithm Convergence

```bash
cd simulations
../src/tsnelection -u Cmdenv -c FCComparison
```

This runs 20 repetitions with different network sizes (4, 6, 8, 10, 12 nodes).

## Troubleshooting

### Build Errors

**Error: `opp_makemake: command not found`**
```bash
# Source OMNeT++ environment
source /path/to/omnetpp/setenv
```

**Error: `No rule to make target`**
```bash
# Clean and rebuild
cd src
make clean
cd ..
./build.sh
```

### Runtime Errors

**Error: Network diameter too small**

If arbitrary election doesn't complete, increase diameter:
```ini
*.diameter = 10  # Use larger value
```

**Error: No leader elected (anonymous)**

Verify network is fully connected:
```bash
# Check that numNodes matches actual connectivity
# For FullyConnectedNetwork, all nodes must be connected
```

### Simulation Hangs

**Issue: Simulation doesn't terminate**

Check configuration:
```ini
sim-time-limit = 100s  # Ensure time limit is set
```

## Advanced Usage

### Custom Network Topology

1. Edit `simulations/TSNElection.ned`:

```ned
network MyCustomNetwork {
    parameters:
        int numNodes = default(5);
        int diameter = default(3);
    
    submodules:
        node[numNodes]: ArbitraryElection {
            parameters:
                nodeId = index;
                diameter = parent.diameter;
        }
    
    connections:
        // Define your custom connections here
        node[0].port++ <--> {delay = 10ms;} <--> node[1].port++;
        node[1].port++ <--> {delay = 10ms;} <--> node[2].port++;
        // ...
}
```

2. Add configuration:

```ini
[Config MyCustom]
network = MyCustomNetwork
*.numNodes = 5
*.diameter = 3
```

3. Run:

```bash
cd simulations
../src/tsnelection -u Cmdenv -c MyCustom
```

### Performance Benchmarking

Run multiple configurations and compare:

```bash
# Run all comparison configs
cd simulations
../src/tsnelection -u Cmdenv -c RingComparison
../src/tsnelection -u Cmdenv -c FCComparison

# Analyze
cd ..
python3 analyze_results.py
```

### Export Results

Results are saved in `simulations/results/`:
- `*.sca`: Scalar statistics
- `*.vec`: Vector (time-series) data

Use OMNeT++ analysis tools:
```bash
# Open in OMNeT++ IDE Analysis Tool
# Or use Python pandas for custom analysis
```

## Next Steps

1. **Read the Documentation**: See [DOCUMENTATION.md](DOCUMENTATION.md) for algorithm details
2. **Explore Configurations**: Check `simulations/omnetpp.ini` for all scenarios
3. **Modify Algorithms**: Edit source files in `src/` to experiment
4. **Add New Features**: Extend base classes for custom behavior

## Getting Help

- **OMNeT++ Manual**: https://omnetpp.org/documentation
- **INET Framework**: https://inet.omnetpp.org/
- **Project Issues**: Report problems in the project repository

## Example Session

Complete workflow from start to finish:

```bash
# 1. Setup
source ~/omnetpp-6.0/setenv

# 2. Build
cd /space/neptun/myself/complex-networks-project
./build.sh

# 3. Run basic test
make run-ring

# 4. Run GUI
make run-gui
# (Select RingNetwork config, click Run)

# 5. Run all tests
make run-all

# 6. Analyze
python3 analyze_results.py

# 7. Check detailed results
ls -lh simulations/results/
```

Congratulations! You've successfully run TSN Leader Election simulations.

---

**For detailed algorithm explanations, see [DOCUMENTATION.md](DOCUMENTATION.md)**
