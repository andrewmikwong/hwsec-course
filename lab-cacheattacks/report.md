Question 1.2

Commands for replicating the results:
```
ssh YourOnyen@152.2.135.251
lscpu --caches
```

| Cache | Cache Line Size | Total Size | Number of Ways (Associativity) | Number of Sets | Raw Latency |
|------:|----------------:|-----------:|-------------------------------:|---------------:|------------:|
| L1    | 64 Bytes        | 512K       | 8                              | 64             |             |
| L2    | 64 Bytes        | 16M        | 16                             | 1024           |             |
| L3    | 64 Bytes        | 22M        | 11                             | 16384          |             |

