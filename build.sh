#!/bin/bash
#
# Build script for TSN Leader Election Project
#

set -e  # Exit on error

echo "========================================="
echo "TSN Leader Election - Build Script"
echo "========================================="
echo ""

# Check if OMNeT++ is installed
if ! command -v opp_makemake &> /dev/null; then
    echo "ERROR: OMNeT++ is not installed or not in PATH"
    echo "Please install OMNeT++ and source the setenv script"
    echo "Example: source /path/to/omnetpp/setenv"
    exit 1
fi

echo "OMNeT++ found: $(which opp_makemake)"
echo ""

# Clean previous build
echo "Step 1: Cleaning previous build..."
if [ -f src/Makefile ]; then
    cd src && make clean && cd ..
fi
rm -rf src/out
echo "Done."
echo ""

# Generate makefiles
echo "Step 2: Generating makefiles..."
cd src
opp_makemake -f --deep -O out -I.
cd ..
echo "Done."
echo ""

# Build the project
echo "Step 3: Building project..."
cd src
make MODE=release -j$(nproc)
cd ..
echo "Done."
echo ""

echo "========================================="
echo "Build completed successfully!"
echo "========================================="
echo ""
echo "To run simulations:"
echo "  make run-ring          - Ring network topology"
echo "  make run-mesh          - Mesh network topology"
echo "  make run-fc            - Fully connected network"
echo "  make run-gui           - GUI mode"
echo "  make run-all           - Run all configurations"
echo ""
