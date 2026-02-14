import json
import numpy as np

with open("traces.out", "r") as f:   # <- use your file name (you have traces.out)
    data = json.loads(f.read())

traces = np.array(data["traces"])
labels = np.array(data["labels"])   


google = traces[[("google" in str(x)) for x in labels]]
nytimes = traces[[("nytimes" in str(x)) for x in labels]]

print("Google stats:")
print("Mean:", google.mean())
print("Median:", np.median(google))
print("Std:", google.std())

print("\nNYTimes stats:")
print("Mean:", nytimes.mean())
print("Median:", np.median(nytimes))
print("Std:", nytimes.std())