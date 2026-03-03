Question 1.2

Commands for replicating the results:
```
ssh YourOnyen@152.2.135.251
lscpu --caches
```

| Cache | Cache Line Size | Total Size | Number of Ways (Associativity) | Number of Sets | Raw Latency |
|------:|----------------:|-----------:|-------------------------------:|---------------:|------------:|
| L1    | 64 Bytes        | 32KB       | 8                              | 64             |             |
| L2    | 64 Bytes        | 1MB        | 16                             | 1024           |             |
| L3    | 64 Bytes        | 11MB       | 11                             | 16384          |             |

             :  L1   L2   L3   Mem
Minimum      :    74    82   126   272
Bottom decile:    74    82   128   276
Median       :    76    82   130   294
Top decile   :    78    84   134   298
Maximum      :    78    90   630  1070