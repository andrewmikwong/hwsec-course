## 1-1

**Given the attack plan above, how many addresses need to be flushed in the first step?**



## 2-1

**Copy your code in `run_attacker` from `attacker-part1.c` to `attacker-part2.c`. Does your Flush+Reload attack from Part 1 still work? Why or why not?**



## 2-3

**In our example, the attacker tries to leak the values in the array `secret_part2`. In a real-world attack, attackers can use Spectre to leak data located in an arbitrary address in the victim's space. Explain how an attacker can achieve such leakage.**



## 2-4

**Experiment with how often you train the branch predictor. What is the minimum number of times you need to train the branch (i.e. `if offset < part2_limit`) to make the attack work?**



## 3-2

**Describe the strategy you employed to extend the speculation window of the target branch in the victim.**
I didn't want to modify the way I extended the speculation window; instead, I worked on extending the amount of time it took for the processor to determine if the branch condition was true or false to keep the speculative execution continuing as long as possible. By not flushing the bound variable in Part 3, the victim code relies on continued training of the branch predictor with in-bound inputs to create a strong prediction that the condition will be TRUE. Therefore, when I make an out of bounds memory access, the CPU initially speculatively executes the operation before realizing that the branch prediction was incorrect. In addition, the victim code adds latency to the resolution of the branch through unnecessary operations, creating even more delay in the resolution of the branch and extending the speculation window. The combination of training the branch predictor and adding latency to operation resolution creates a longer time period during which the speculative memory access can occur, and also affect the process of cache.


## 3-3

**Assume you are an attacker looking to exploit a new machine that has the same kernel module installed as the one we attacked in this part. What information would you need to know about this new machine to port your attack? Could it be possible to determine this infomration experimentally? Briefly describe in 5 sentences or less.**

I need to find out information about the new machine's cache. Specifically, I want some details on the cache hierarchy (size of caches and cache line size), timing characteristics of the cache (latency of hit and miss), and CPU behaviours surrounding speculation and branch prediction. Once I find this information, I will need to adjust parameters of the attack such as the number of addresses to access during an Address Sweep and how long a timing window to use, so that the performance of those operations matches on the new system.

To determine this information, I will need to conduct timing measurements and microbenchmarks that will allow me to look at cache behaviour and latency differences between the two systems. For instance, I can perform various patterns of memory accesses in order to derive how large the caches are in the new system and then use the timing differences between memory accesses in order to see whether the accesses were cache hits or cache misses. Once I have this data, I can repeatedly tune parameters (re-sweep), and thus, adapt the attack to function properly on the new platform in the absence of previous knowledge of the new machine's specific specifications.
