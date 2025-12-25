#
# Makefile for TSN Leader Election Project
#

# Variables
CXX = g++
NEDFILES = simulations/TSNElection.ned src/package.ned
MSGFILES =

# Target: all
all: checkmakefiles
	cd src && $(MAKE)

# Create makefiles
makefiles:
	cd src && opp_makemake -f --deep -O out -I. -KINET_PROJ=../../inet4

# Check if makefiles exist
checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
		echo "Makefiles do not exist. Creating them..."; \
		$(MAKE) makefiles; \
	fi

# Clean
clean:
	cd src && $(MAKE) clean

cleanall:
	cd src && $(MAKE) MODE=release clean
	cd src && $(MAKE) MODE=debug clean
	rm -rf src/Makefile src/out

# Run specific configurations
run-ring:
	cd simulations && ../src/tsnelection -u Cmdenv -c RingNetwork

run-mesh:
	cd simulations && ../src/tsnelection -u Cmdenv -c MeshNetwork

run-star:
	cd simulations && ../src/tsnelection -u Cmdenv -c StarNetwork

run-fc:
	cd simulations && ../src/tsnelection -u Cmdenv -c FullyConnectedNetwork

run-random:
	cd simulations && ../src/tsnelection -u Cmdenv -c RandomNetwork

# Run all configurations
run-all:
	cd simulations && ../src/tsnelection -u Cmdenv -c RingNetwork
	cd simulations && ../src/tsnelection -u Cmdenv -c MeshNetwork
	cd simulations && ../src/tsnelection -u Cmdenv -c StarNetwork
	cd simulations && ../src/tsnelection -u Cmdenv -c FullyConnectedNetwork

# Run with GUI
run-gui:
	cd simulations && ../src/tsnelection -u Qtenv

.PHONY: all makefiles checkmakefiles clean cleanall run-ring run-mesh run-star run-fc run-random run-all run-gui
