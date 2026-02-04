import json
import numpy as np

from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import classification_report
from sklearn.model_selection import train_test_split

def eval():
    y_pred_full, y_test_full = [], []

    # Re-train 10 times in order to reduce effects of randomness
    for i in range(10):
        ### TODO: Exercise 2-5
        ### 1. Load data from traces file
        with open("traces20.out", "r") as f:
            data = json.load(f)
        
        traces = np.array(data["traces"])
        labels = np.array(data["labels"])

        ### 2. Split data into X_train, X_test, y_train, y_test with train_test_split
        X_train, X_test, y_train, y_test = train_test_split(traces, labels, test_size=0.2)

        ### 3. Train classifier with X_train and y_train
        clf = RandomForestClassifier()
        clf.fit(X_train, y_train)

        ### 4. Use classifier to make predictions on X_test. Save the result to a variable called y_pred
        y_pred = clf.predict(X_test)

        # Do not modify the next two lines
        y_test_full.extend(y_test)
        y_pred_full.extend(y_pred)

    ### TODO: Exercise 2-5 (continued)
    ### 5. Print classification report using y_test_full and y_pred_full
    print(classification_report(y_test_full, y_pred_full))

if __name__ == "__main__":
    eval()
