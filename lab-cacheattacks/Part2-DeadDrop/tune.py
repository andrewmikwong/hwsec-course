import subprocess
import time
import re
import sys
import os

# --- CONFIGURATION ---
SENDER_CMD = ["./sender"]
RECEIVER_CMD = ["./receiver"]
TARGET_SET = 42
TIMEOUT = 5 # Seconds per test

# Range for threshold tuning
MIN_THRESHOLD = 200
MAX_THRESHOLD = 2000

def compile_code(threshold):
    """Compiles receiver.c with a specific threshold."""
    # Read receiver.c
    with open("receiver.c", "r") as f:
        content = f.read()
    
    # Replace threshold
    # Look for: if (total_time > 800)
    new_content = re.sub(r"if \(total_time > \d+\)", f"if (total_time > {threshold})", content)
    
    with open("receiver.c", "w") as f:
        f.write(new_content)
        
    # Compile
    subprocess.run(["make"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

def run_test(threshold):
    """Runs the Sender and Receiver and checks if they communicate."""
    print(f"Testing Threshold: {threshold}...", end="")
    sys.stdout.flush()
    
    # Start Receiver
    receiver = subprocess.Popen(RECEIVER_CMD, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    # Start Sender
    sender = subprocess.Popen(SENDER_CMD, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    try:
        # Initialize
        # Receiver waits for Enter
        # Sender waits for Number
        
        # Give them a moment to start up
        time.sleep(0.5)
        
        # Tell Receiver to start listening
        receiver.stdin.write("\n")
        receiver.stdin.flush()
        
        # Tell Sender to send TARGET_SET
        sender.stdin.write(f"{TARGET_SET}\n")
        sender.stdin.flush()
        
        # Wait and read output
        start_time = time.time()
        found = False
        while time.time() - start_time < TIMEOUT:
            line = receiver.stdout.readline()
            if not line:
                break
            
            # Check for "Received value: 42"
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
    print("--- AUTO-TUNING THRESHOLD ---")
    
    # We want to find the range [Low, High] where it works.
    # But binary search finds a single point.
    # Instead, let's just sweep with a coarse step to find the working window.
    # Binary search is risky if the function isn't monotonic (noise makes it jagged).
    
    working_thresholds = []
    
    # Coarse Sweep
    for t in range(MIN_THRESHOLD, MAX_THRESHOLD, 100):
        compile_code(t)
        if run_test(t):
            working_thresholds.append(t)
            
    if not working_thresholds:
        print("\nNO WORKING THRESHOLD FOUND.")
        print("Possible issues: L2_WAYS mismatch, Wait time too short, or Hardware noise.")
        return
        
    print(f"\nWorking Range: {min(working_thresholds)} - {max(working_thresholds)}")
    best_threshold = sum(working_thresholds) // len(working_thresholds)
    print(f"Recommended Threshold: {best_threshold}")
    
    # Apply best
    compile_code(best_threshold)
    print("Applied best threshold. Ready to run manually.")

if __name__ == "__main__":
    binary_search_tuning()
