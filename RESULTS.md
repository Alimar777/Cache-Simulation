## Results Summary

From `Cache Simulation Report.pdf`:
- 32x32 misses: 287
- 64x64 misses: 1148
- 61x67 misses: 1812

Notes:
- Results are from the reported evaluation environment in the PDF.
- If you rerun locally (especially on WSL), `valgrind` performance can vary.

## Reproduce
Recommended (native Linux):
```bash
make
./driver.py
```

WSL (slower, may time out under valgrind):
```bash
sudo apt install build-essential valgrind
make
./driver.py
```
