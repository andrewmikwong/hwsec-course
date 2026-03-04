import subprocess
import time
import sys
import threading
import os

# Configuration
SENDER_CPU = "7"
RECEIVER_CPU = "23"
SENDER_CMD = ["taskset", "-c", SENDER_CPU, "./sender"]
RECEIVER_CMD = ["taskset", "-c", RECEIVER_CPU, "./receiver"]

def run_e2e_test():
    print("=== E2E SYSTEM TEST ===")
    
    # 1. Start Receiver
    print("[*] Starting Receiver...")
    receiver = subprocess.Popen(
        RECEIVER_CMD, 
        stdin=subprocess.PIPE, 
        stdout=subprocess.PIPE, 
        stderr=subprocess.PIPE, 
        text=True, 
        bufsize=0
    )
    
    # 2. Wait for Calibration
    # We need to send "Enter" to start calibration
    time.sleep(1)
    print("[*] Triggering Calibration...")
    receiver.stdin.write("\n")
    receiver.stdin.flush()
    
    # Read output until "Calibration Done"
    print("[*] Waiting for Calibration (approx 2s)...")
    calibrated = False
    start_wait = time.time()
    while time.time() - start_wait < 5:
        line = receiver.stdout.readline()
        if line:
            print(f"    [Receiver] {line.strip()}")
            if "Calibration Done" in line:
                calibrated = True
                break
    
    if not calibrated:
        print("[!] FAIL: Calibration timed out.")
        receiver.kill()
        return

    # 3. Start Sender
    print("[*] Starting Sender...")
    sender = subprocess.Popen(
        SENDER_CMD, 
        stdin=subprocess.PIPE, 
        stdout=subprocess.PIPE, 
        stderr=subprocess.PIPE, 
        text=True, 
        bufsize=0
    )
    
    # 4. Send Message "42"
    target_msg = 42
    print(f"[*] Sending Message: {target_msg}")
    sender.stdin.write(f"{target_msg}\n")
    sender.stdin.flush()
    
    # 5. Verify Receipt
    print("[*] Verifying Receipt...")
    received = False
    start_wait = time.time()
    while time.time() - start_wait < 5:
        line = receiver.stdout.readline()
        if line:
            print(f"    [Receiver] {line.strip()}")
            if f"Received value: {target_msg}" in line:
                received = True
                break
    
    # 6. Cleanup
    sender.kill()
    receiver.kill()
    
    if received:
        print("\n" + "="*30)
        print("✅ E2E TEST PASSED")
        print("="*30)
    else:
        print("\n" + "="*30)
        print("❌ E2E TEST FAILED")
        print("="*30)

if __name__ == "__main__":
    run_e2e_test()
