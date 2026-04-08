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

In order to implement the attack described in Part 3, I increased the effective prediction time length by heavily training the branch predictor through repeated access to in-bound data before performing the out-of-bounds access. Since we no longer flush our bound variable in the attack as we did in Part 2, I created an eviction buffer based on a strided access pattern to flush out as much cache information from private caches as possible, therefore making the victim's ability to resolve control flow slower and less predictable. After performing the training, I flushed the shared memory side-channel pages, made a single out-of-bounds kernel call and then reloaded all of my candidate pages to determine which of them had been brought back into cache through speculation. By running this process over many trials, I was able to build up scores for each possible byte value and ultimately retrieve the secret. My approach was therefore a combination of branch predictor training, cache eviction, and repeated scoring, creating a strong enough speculative signal.

## 3-3

**Assume you are an attacker looking to exploit a new machine that has the same kernel module installed as the one we attacked in this part. What information would you need to know about this new machine to port your attack? Could it be possible to determine this infomration experimentally? Briefly describe in 5 sentences or less.**

To port the attack to a new machine, I would need to know the cache hierarchy. Specifically, the sizes, line size, and likely eviction behavior, cache hit/miss timing thresholds, and how the CPU’s branch predictor and speculative execution behave. I would also need to retune attack parameters such as the cache-hit threshold, number of training calls, number of trials, and eviction-buffer size/stride. This can be mostly determined experimentally by running timing tests and microbenchmarks on the new system. By tuning these parameters, I would be able to adapt the attack even without detailed hardware documentation.