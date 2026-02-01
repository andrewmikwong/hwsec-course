// Number of sweep counts
// TODO (Exercise 2-1): Choose an appropriate value!
let P = 20;

// Number of elements in your trace
let K = 5 * 1000 / P; 

// Array of length K with your trace's values
let T;

// Value of performance.now() when you started recording your trace
let start;

function record() {
  // Create empty array for saving trace values
  T = new Array(K);

  // Fill array with -1 so we can be sure memory is allocated
  T.fill(-1, 0, T.length);

  // Save start timestamp
  start = performance.now();

  const LINE_SIZE = 16;
  const NUM_LINES = 1000000;  
  const M = new Array(NUM_LINES * LINE_SIZE).fill(-1);

  for (let i = 0; i < K; i++) {
    const windowEnd = start + (i + 1) * P;
    let count = 0;
    while (performance.now() < windowEnd) {
      for (let j = 0; j < NUM_LINES; j++) {
        let val = M[j * LINE_SIZE];
        val = val;
      }
      count++;
    }
    T[i] = count;
  }

  // Once done recording, send result to main thread
  postMessage(JSON.stringify(T));
}

// DO NOT MODIFY BELOW THIS LINE -- PROVIDED BY COURSE STAFF
self.onmessage = (e) => {
  if (e.data.type === "start") {
    setTimeout(record, 0);
  }
};
