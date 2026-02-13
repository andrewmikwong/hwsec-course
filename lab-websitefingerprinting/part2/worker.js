let P = 20;               // ms per bucket (keep your chosen value)
let K = 0;                // computed from trace_length
let sink = 0;

// Allocate memory ONCE (avoid alloc/GC noise during recording)
const LINE_STRIDE = 16;   // 16 * 4 bytes = 64 bytes stride
const NUM_LINES = 300000; // keep reasonable to avoid memory pressure
const M = new Int32Array(NUM_LINES * LINE_STRIDE);
for (let i = 0; i < NUM_LINES; i++) {
  M[i * LINE_STRIDE] = i;
}

function doOneSweep() {
  for (let j = 0; j < NUM_LINES; j++) {
    sink = (sink + M[j * LINE_STRIDE]) | 0;
  }
}

function record(traceLengthMs) {
  K = Math.floor(traceLengthMs / P);
  const T = new Int32Array(K);

  // Warm up briefly so the first buckets aren't dominated by startup effects
  const warmEnd = performance.now() + 300;
  while (performance.now() < warmEnd) {
    doOneSweep();
  }

  const start = performance.now();

  for (let i = 0; i < K; i++) {
    const windowEnd = start + (i + 1) * P;
    let count = 0;

    while (performance.now() < windowEnd) {
      doOneSweep();
      count++;
    }
    T[i] = count;
  }

  // keep sink observable
  self.__sink = sink;

  postMessage(JSON.stringify(Array.from(T)));
}

// DO NOT MODIFY BELOW THIS LINE -- PROVIDED BY COURSE STAFF
self.onmessage = (e) => {
  if (e.data.type === "start") {
    const tl =
      typeof e.data.trace_length === "number" ? e.data.trace_length : 5000;
    setTimeout(() => record(tl), 0);
  }
};
