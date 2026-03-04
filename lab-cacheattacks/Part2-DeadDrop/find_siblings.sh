#!/bin/bash
echo "Scanning for sibling cores (Hyperthreads)..."
echo "Pairs of CPUs that share L1/L2 cache:"
echo "----------------------------------------"

for cpu in /sys/devices/system/cpu/cpu[0-9]*; do
    id=$(basename $cpu | sed 's/cpu//')
    if [ -f "$cpu/topology/thread_siblings_list" ]; then
        siblings=$(cat "$cpu/topology/thread_siblings_list")
        # Only print if it's a pair (comma separated) and we haven't seen it yet
        if [[ "$siblings" == *","* ]]; then
            # simple deduplication by only printing if id is the first number
            first=$(echo $siblings | cut -d',' -f1)
            if [ "$id" == "$first" ]; then
                echo "Core Pair: $siblings"
            fi
        fi
    fi
done
echo "----------------------------------------"
echo "Pick ONE pair (e.g., 7,23) and use those for Sender/Receiver."
