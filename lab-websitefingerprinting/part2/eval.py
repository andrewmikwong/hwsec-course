import json
import numpy as np

from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import classification_report
from sklearn.model_selection import train_test_split

# Optional: drop early buckets that often contain warm-up artifacts.
# If you don't want this, set WARMUP_BUCKETS = 0.
WARMUP_BUCKETS = 30

def eval():
    # 1) Load once
    with open("traces20.out", "r") as f:
        data = json.load(f)

    X = np.array(data["traces"], dtype=np.float32)
    y = np.array(data["labels"])

    # Optional warm-up trim
    if WARMUP_BUCKETS > 0:
        X = X[:, WARMUP_BUCKETS:]

    y_pred_full, y_test_full = [], []

    # Re-train 10 times in order to reduce effects of randomness
    for i in range(10):
        # 2) Stratified split keeps label balance
        X_train, X_test, y_train, y_test = train_test_split(
            X, y,
            test_size=0.2,
            shuffle=True,
            stratify=y,
            random_state=i
        )

        # 3) More stable RF (more trees) + reproducible
        clf = RandomForestClassifier(
            n_estimators=300,
            random_state=i,
            n_jobs=-1
        )
        clf.fit(X_train, y_train)

        y_pred = clf.predict(X_test)

        y_test_full.extend(y_test.tolist())
        y_pred_full.extend(y_pred.tolist())

    # 5) Report across all collected predictions
    print(classification_report(y_test_full, y_pred_full))

if __name__ == "__main__":
    eval()
