import json, numpy as np
import matplotlib.pyplot as plt

data = json.load(open("../traces.out"))
X = np.array(data["traces"], dtype=float)

warm = 30          # drop warm-up buckets (tune based on P)
X = X[:, warm:]

# z-score each trace row
mu  = X.mean(axis=1, keepdims=True)
sig = X.std(axis=1, keepdims=True) + 1e-9
Z = (X - mu) / sig

# clip to threshold (this is what you asked for)
thr = 2.0
Z = np.clip(Z, -thr, thr)

# rescale to [0, 1] for a clean heatmap
Z01 = (Z + thr) / (2 * thr)

plt.figure(figsize=(12,4))
plt.imshow(Z01, aspect="auto")
plt.colorbar(label="normalized (0..1)")
plt.title(f"Per-trace z-score, clipped to ±{thr}, rescaled to [0,1]")
plt.tight_layout()
plt.show()
