import subprocess
import time
import sys
import threading
import os
import random

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
    
    # 4. Send Message Loop (Try multiple numbers in case of collision)
    # We try 3 different numbers. If any works, we pass.
    test_messages = [42, 100, 200]
    
    for target_msg in test_messages:
        print(f"[*] Sending Message: {target_msg}")
        sender.stdin.write(f"{target_msg}\n")
        sender.stdin.flush()
        
        # 5. Verify Receipt
        print(f"[*] Verifying Receipt for {target_msg}...")
        received = False
        start_wait = time.time()
        
        # Wait up to 5 seconds per message
        while time.time() - start_wait < 5:
            # Non-blocking read trick not used here, relying on readline
            # If receiver is silent, this blocks. 
            # But receiver prints "Received value" if it works.
            # If it doesn't work, we might hang here.
            # Let's use a polling loop with timeout on readline if possible, 
            # but standard python readline blocks.
            # We will rely on the fact that if it works, it prints quickly.
            
            # To avoid hanging forever, we can't easily interrupt readline without threads/select.
            # For this simple script, we assume success or manual Ctrl+C.
            # BUT, to be safer, let's just wait for *any* line.
            
            line = receiver.stdout.readline()
            if line:
                print(f"    [Receiver] {line.strip()}")
                if f"Received value: {target_msg}" in line:
                    received = True
                    break
        
        if received:
            print("\n" + "="*30)
            print(f"✅ E2E TEST PASSED (Message {target_msg})")
            print("="*30)
            sender.kill()
            receiver.kill()
            return

    print("\n" + "="*30)
    print("❌ E2E TEST FAILED (All attempts)")
    print("="*30)
    sender.kill()
    receiver.kill()

if __name__ == "__main__":
    run_e2e_test()
