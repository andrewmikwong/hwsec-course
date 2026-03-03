import subprocess
import time
import re
import sys
import select
import fcntl
import os

# --- CONFIGURATION ---
SENDER_CPU = "7"
RECEIVER_CPU = "23"

SENDER_CMD = ["taskset", "-c", SENDER_CPU, "./sender"]
RECEIVER_CMD = ["taskset", "-c", RECEIVER_CPU, "./receiver"]

TARGET_SET = 42
TIMEOUT = 3 # Fast timeout

MIN_THRESHOLD = 200
MAX_THRESHOLD = 3000
STEP = 100

def set_nonblocking(f):
    fd = f.fileno()
    fl = fcntl.fcntl(fd, fcntl.F_GETFL)
    fcntl.fcntl(fd, fcntl.F_SETFL, fl | os.O_NONBLOCK)

def compile_code(threshold):
    with open("receiver.c", "r") as f:
        content = f.read()
    new_content = re.sub(r"if \(total_time > \d+\)", f"if (total_time > {threshold})", content)
    with open("receiver.c", "w") as f:
        f.write(new_content)
    subprocess.run(["make"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

def run_test(threshold):
    print(f"Testing {threshold}...", end=" ")
    sys.stdout.flush()
    
    receiver = subprocess.Popen(RECEIVER_CMD, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, bufsize=0)
    sender = subprocess.Popen(SENDER_CMD, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, bufsize=0)
    
    # Set non-blocking output
    set_nonblocking(receiver.stdout)
    
    found = False
    try:
        # Wait for "READY"
        start_wait = time.time()
        ready = False
        while time.time() - start_wait < 2:
            try:
                line = receiver.stdout.readline()
                if "READY" in line:
                    ready = True
                    break
            except:
                time.sleep(0.1)
        
        if not ready:
            print("FAILED (Did not start)")
            return False

        # Start
        receiver.stdin.write("\n")
        receiver.stdin.flush()
        
        sender.stdin.write(f"{TARGET_SET}\n")
        sender.stdin.flush()
        
        # Monitor
        start_time = time.time()
        while time.time() - start_time < TIMEOUT:
            try:
                line = receiver.stdout.readline()
                if not line:
                    time.sleep(0.01)
                    continue
                
                match = re.search(r"Received value: (\d+)", line)
                if match:
                    val = int(match.group(1))
                    if val == TARGET_SET:
                        print(f"SUCCESS! (Found {val})")
                        found = True
                        break
                    else:
                        # print(f"NOISE({val}) ", end="")
                        pass
            except:
                time.sleep(0.01)
        
        if not found:
            print("TIMEOUT")
            
    except Exception as e:
        print(f"ERROR: {e}")
    finally:
        receiver.kill()
        sender.kill()
        
    return found

def binary_search_tuning():
    print(f"--- AUTO-TUNING (CPUs {SENDER_CPU} & {RECEIVER_CPU}) ---")
    working = []
    
    for t in range(MIN_THRESHOLD, MAX_THRESHOLD, STEP):
        compile_code(t)
        if run_test(t):
            working.append(t)
            
    if not working:
        print("\nNO WORKING THRESHOLD FOUND.")
        return
        
    print(f"\nWorking Range: {min(working)} - {max(working)}")
    best = sum(working) // len(working)
    print(f"Recommended: {best}")
    compile_code(best)

if __name__ == "__main__":
    binary_search_tuning()
