#!/usr/bin/env python3
"""
Analysis script for TSN Leader Election simulation results.
Processes OMNeT++ scalar and vector files to generate performance metrics.
"""

import os
import sys
import re
import glob
from collections import defaultdict

def parse_scalar_file(filepath):
    """Parse OMNeT++ scalar (.sca) file."""
    results = defaultdict(list)
    
    with open(filepath, 'r') as f:
        current_run = None
        current_config = None
        
        for line in f:
            line = line.strip()
            
            # Parse run declaration
            if line.startswith('run'):
                match = re.match(r'run\s+(\S+)', line)
                if match:
                    current_run = match.group(1)
            
            # Parse config
            elif line.startswith('attr configname'):
                match = re.match(r'attr configname\s+(\S+)', line)
                if match:
                    current_config = match.group(1)
            
            # Parse scalar values
            elif line.startswith('scalar'):
                match = re.match(r'scalar\s+(\S+)\s+(\S+)\s+(\S+)', line)
                if match:
                    module = match.group(1)
                    metric = match.group(2)
                    value = match.group(3)
                    
                    results[current_config].append({
                        'run': current_run,
                        'module': module,
                        'metric': metric,
                        'value': float(value) if '.' in value else int(value)
                    })
    
    return results

def analyze_results(results):
    """Analyze and summarize results."""
    
    print("=" * 70)
    print("TSN Leader Election - Simulation Results Analysis")
    print("=" * 70)
    print()
    
    for config, data in results.items():
        print(f"\nConfiguration: {config}")
        print("-" * 70)
        
        # Group by metric
        metrics = defaultdict(list)
        for entry in data:
            metrics[entry['metric']].append(entry['value'])
        
        # Calculate statistics
        for metric, values in sorted(metrics.items()):
            if len(values) > 0:
                avg = sum(values) / len(values)
                min_val = min(values)
                max_val = max(values)
                
                print(f"  {metric:30s}: avg={avg:8.2f}, min={min_val:8.2f}, max={max_val:8.2f}, n={len(values)}")
        
        # Find leader
        leaders = [e for e in data if e['metric'] == 'isLeader' and e['value'] == 1]
        if leaders:
            print(f"\n  Elected Leader(s): {[e['module'] for e in leaders]}")
        
        # Message statistics
        msg_sent = [e['value'] for e in data if e['metric'] == 'totalMessagesSent']
        if msg_sent:
            total_msgs = sum(msg_sent)
            print(f"  Total Messages Sent: {total_msgs}")
        
        print()

def main():
    """Main function."""
    
    # Find result files
    result_dir = "simulations/results"
    
    if not os.path.exists(result_dir):
        print(f"Error: Results directory '{result_dir}' not found.")
        print("Please run simulations first.")
        sys.exit(1)
    
    scalar_files = glob.glob(os.path.join(result_dir, "*.sca"))
    
    if not scalar_files:
        print(f"Error: No scalar files found in '{result_dir}'")
        print("Please run simulations first.")
        sys.exit(1)
    
    print(f"Found {len(scalar_files)} result file(s)")
    print()
    
    # Parse and analyze each file
    all_results = defaultdict(list)
    
    for sca_file in scalar_files:
        print(f"Processing: {os.path.basename(sca_file)}")
        results = parse_scalar_file(sca_file)
        
        for config, data in results.items():
            all_results[config].extend(data)
    
    # Analyze combined results
    analyze_results(all_results)
    
    print("=" * 70)
    print("Analysis complete!")
    print("=" * 70)

if __name__ == "__main__":
    main()
