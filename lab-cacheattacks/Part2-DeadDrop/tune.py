import subprocess
import time
import re
import sys
import os

# --- CONFIGURATION ---
# CPUs from cpu.mk
SENDER_CPU = "7"
RECEIVER_CPU = "23"

# Commands with taskset
SENDER_CMD = ["taskset", "-c", SENDER_CPU, "./sender"]
RECEIVER_CMD = ["taskset", "-c", RECEIVER_CPU, "./receiver"]

TARGET_SET = 42
TIMEOUT = 5 

# Range for threshold tuning
# L2 Hit (12 accesses) ~= 12 * 120 = 1440 cycles
# DRAM Miss (12 accesses) ~= 12 * 300 = 3600 cycles
# We should sweep 1000 to 3000
MIN_THRESHOLD = 800
MAX_THRESHOLD = 3000
STEP = 200

def compile_code(threshold):
    """Compiles receiver.c with a specific threshold."""
    with open("receiver.c", "r") as f:
        content = f.read()
    
    # Replace threshold
    new_content = re.sub(r"if \(total_time > \d+\)", f"if (total_time > {threshold})", content)
    
    with open("receiver.c", "w") as f:
        f.write(new_content)
        
    subprocess.run(["make"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

def run_test(threshold):
    """Runs the Sender and Receiver and checks if they communicate."""
    print(f"Testing Threshold: {threshold}...", end="")
    sys.stdout.flush()
    
    receiver = subprocess.Popen(RECEIVER_CMD, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    sender = subprocess.Popen(SENDER_CMD, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    try:
        time.sleep(0.5)
        
        # Start Receiver
        receiver.stdin.write("\n")
        receiver.stdin.flush()
        
        # Start Sender
        sender.stdin.write(f"{TARGET_SET}\n")
        sender.stdin.flush()
        
        # Monitor
        start_time = time.time()
        found = False
        while time.time() - start_time < TIMEOUT:
            line = receiver.stdout.readline()
            if not line:
                break
            
            match = re.search(r"Received value: (\d+)", line)
            if match:
                val = int(match.group(1))
                if val == TARGET_SET:
                    print(f" SUCCESS! (Found {val})")
                    found = True
                    break
                else:
                    print(f" NOISE ({val})", end="")
        
        if not found:
            print(" FAILED (Timeout)")
            
    except Exception as e:
        print(f" ERROR: {e}")
        return False
    finally:
        receiver.kill()
        sender.kill()
        
    return found

def binary_search_tuning():
    print(f"--- AUTO-TUNING (CPUs {SENDER_CPU} & {RECEIVER_CPU}) ---")
    
    working_thresholds = []
    
    for t in range(MIN_THRESHOLD, MAX_THRESHOLD, STEP):
        compile_code(t)
        if run_test(t):
            working_thresholds.append(t)
            
    if not working_thresholds:
        print("\nNO WORKING THRESHOLD FOUND.")
        return
        
    print(f"\nWorking Range: {min(working_thresholds)} - {max(working_thresholds)}")
    best_threshold = sum(working_thresholds) // len(working_thresholds)
    print(f"Recommended Threshold: {best_threshold}")
    
    compile_code(best_threshold)
    print("Applied best threshold.")

if __name__ == "__main__":
    binary_search_tuning()
