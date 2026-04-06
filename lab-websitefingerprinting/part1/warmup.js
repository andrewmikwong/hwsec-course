const runs = 10;

function measureOneLine() {
  const LINE_SIZE = 8; //64 bytes per cache line
  let result = [];

  // Fill with -1 to ensure allocation
  const M = new Array(runs * LINE_SIZE).fill(-1);

  for (let i = 0; i < runs; i++) {
    const start = performance.now();
    let val = M[i * LINE_SIZE];
    const end = performance.now();

    result.push(end - start);
  }

  return result;
}

function median(arr) {
  const sorted = [...arr].sort((a, b) => a - b);
  const mid = Math.floor(sorted.length / 2);
  return sorted.length % 2 === 0
    ? (sorted[mid - 1] + sorted[mid]) / 2
    : sorted[mid];
}

function measureNLines() {
  const LINE_SIZE = 8; 
  const N_VALUES = [1, 10, 100, 1000, 10000, 100000, 1000000, 10000000];
  let result = [];

  for (const N of N_VALUES) {
    const M = new Array(N * LINE_SIZE).fill(-1);

    const latencies = [];
    for (let r = 0; r < runs; r++) {
      const start = performance.now();
      for (let i = 0; i < N; i++) {
        let val = M[i * LINE_SIZE];
        val = val; 
      }
      const end = performance.now();
      latencies.push(end - start);
    }
    result.push(median(latencies));
  }

  return result;
}

document.getElementById(
  "exercise1-values"
).innerText = `1 Cache Line: [${measureOneLine().join(", ")}]`;

document.getElementById(
  "exercise2-values"
).innerText = `N Cache Lines: [${measureNLines().join(", ")}]`;
