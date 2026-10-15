# Final High-Level Architecture — ZedBoard Matrix-Multiplication Accelerator

## Architecture baseline
- **Board/SoC:** Digilent ZedBoard, Zynq-7000 XC7Z020-CLG484.
- **Physical compute array:** 4×4 weight-stationary systolic array (16 PEs).
- **Numerics:** signed INT8 matrix operands; INT32 accumulation and output.
- **Problem model:** `A[M×K] × B[K×N] = C[M×N]`.
- **Scaling:** larger matrices execute as tiles; edge tiles use valid-dimension masking and zero padding where the fixed-size datapath requires it.
- **Software:** ARM Cortex-A9 bare-metal C orchestrates work, programs registers/DMA, and checks results against a software golden model.
- **Data movement:** AXI DMA MM2S/S2MM for payloads, AXI4-Stream between DMA and accelerator, AXI4-Lite for control/status, PS HP path for DMA access to DDR.
- **Storage:** full matrices in DDR; local BRAM/registers/FIFOs for active A/B tiles and C/partial sums. Ping-pong A/B buffers are intended to overlap transfer and compute when the controller and protocol support it.
- **Clocking:** one PL clock domain from a Zynq PS FCLK is the initial plan; exact frequency and timing closure must be proven in Vivado.

## Block-by-block explanation

1. **ARM Cortex-A9 / bare-metal C** — allocates and fills matrices, schedules the M/N/K tile loops, packs non-contiguous row-major tiles into contiguous staging buffers for the first implementation, programs accelerator control registers and DMA, polls completion/error status, performs cache maintenance as required, and compares C against a golden model.
2. **DDR3** — stores complete A, B, and C matrices plus staging buffers. The CPU and DMA must agree on addresses, byte counts, alignment, and cache coherency.
3. **AXI4-Lite register bank** — low-bandwidth configuration and status only: start/reset, M/N/K, tile dimensions, valid edge dimensions, phase/tile metadata, busy/done/error. Matrix payloads do not pass through these registers.
4. **AXI DMA** — MM2S reads payload bytes from DDR and emits AXI4-Stream beats. S2MM receives output beats and writes them to DDR. Start with Simple/Direct Register mode; add scatter-gather/2-D transfers only if needed and supported by the configured IP.
5. **AXI4-Stream input adapter** — accepts data only on `TVALID && TREADY`, unpacks the selected stream width into INT8 values, routes the current transfer to weight loading or activation feeding according to an explicitly defined phase protocol, and handles `TLAST` according to the documented packet convention.
6. **Tile loader / skew / padding** — maps row-major tile data to PE lanes and generates the skew/delay schedule required by the chosen systolic timing. For edge tiles, it can inject INT8 zeroes and/or mask invalid rows/columns/reduction terms. Invalid output elements must never be written to C.
7. **Ping-pong local tile storage** — holds active and next A/B tiles, PE weights, FIFOs, and C/partial-sum values. Buffer depths, port widths, banking, and whether weights live in registers or BRAM are implementation choices to validate, not assume.
8. **4×4 weight-stationary PE array** — each PE retains its assigned B weight during the relevant tile computation, receives A activations on the planned schedule, multiplies signed INT8 operands, and contributes to INT32 accumulation. The exact PE interconnect, reduction direction, pipeline latency, and skew must be specified in a cycle-level microarchitecture before RTL.
9. **INT32 accumulation / output tile manager** — clears a C output tile once before the first K tile, accumulates subsequent K-tile partial sums into it, and marks only valid output rows/columns for writeback.
10. **Output packer / AXI4-Stream** — serializes completed valid C values as INT32 payload, obeys backpressure, and emits `TLAST` according to the chosen packet contract.
11. **FSM / tile controller** — coordinates configuration, weight preload, activation feed, accumulation, drain/store, next-tile scheduling, error handling, and completion signaling. The CPU can initially schedule individual transfers/tiles to keep hardware control simpler.
12. **Clock/reset** — initial design uses one PL clock domain from PS FCLK. Ensure DMA, AXI interfaces, registers, and custom logic have compatible clocks and properly sequenced resets.

## Tiling contract
Use `Mt`, `Kt`, `Nt` as software tile dimensions, initially bounded by the chosen physical mapping and local storage. For the 4×4 baseline, a natural starting point is `Kt=4`, `Nt=4`, with up to four output rows managed over time (`Mt=4`), then expand only after simulation and resource reports.

For `A[M×K] × B[K×N]`:
```text
for im = 0; im < M; im += Mt:
  for jn = 0; jn < N; jn += Nt:
    clear C_tile once
    for ik = 0; ik < K; ik += Kt:
      valid_M = min(Mt, M - im)
      valid_N = min(Nt, N - jn)
      valid_K = min(Kt, K - ik)
      pack A_tile and B_tile
      zero-pad edge lanes if required; carry valid dimensions
      load/compute; accumulate partial sums into C_tile
    write only valid C_tile elements to DDR
```
**Important:** initialize the output/partial-sum tile once per output tile, not once per K tile; otherwise earlier partial sums are lost.

## AXI connection summary
- **AXI4-Lite:** PS GP path → accelerator register bank; configuration and status.
- **AXI4 memory-mapped:** AXI DMA memory side ↔ PS HP path ↔ DDR.
- **AXI4-Stream input:** DMA MM2S → input adapter → local tile storage / PE input path.
- **AXI4-Stream output:** output packer → DMA S2MM → DDR.
- **Handshake:** a stream beat transfers only when `TVALID` and `TREADY` are both high at a rising clock edge. The source holds payload/control stable while stalled.
- **Packet contract:** define whether `TLAST` marks a tile, row, or complete output payload; align this with DMA transfer lengths.

## End-to-end execution sequence
1. Initialize PS, DDR buffers, DMA and accelerator registers.
2. Fill A and B in DDR; clean/flush source cache lines if required by the BSP and selected memory path.
3. Program M/N/K, tile configuration and edge-valid dimensions.
4. Arm S2MM for the expected output byte count.
5. MM2S sends B-weight tile payload; PL acknowledges weight loading according to the protocol.
6. MM2S sends A activation payload; adapter and loader align data to the PE schedule.
7. PE array computes and C tile manager accumulates INT32 partial sums across K tiles.
8. Output packer emits valid INT32 C values; S2MM writes them to DDR.
9. Check DMA/accelerator status; invalidate output cache lines if required.
10. Compare every C element with the software golden model and report mismatches.

## What is fixed vs what must be validated
**Baseline selected for this architecture diagram:** ZedBoard / XC7Z020, 4×4 physical array, signed INT8 operands, INT32 accumulation/output, weight-stationary reuse, tiled execution, edge padding/masking, DDR-resident matrices, AXI4-Lite control, AXI DMA + AXI4-Stream payload path, and PS-controlled bare-metal software.

**Must still be verified in RTL/Vivado:** exact PE reduction and skew schedule; bus and stream widths; `TLAST`/transfer packet definition; DMA byte counts; tile-buffer sizes and BRAM inference; signed arithmetic and overflow behavior; cache maintenance; resource utilization; timing closure; achieved throughput and power. Do not claim a target frequency, power, or performance number until measured.

## Suggested faculty presentation narration
“The ARM processor is the system manager, not the matrix compute engine. It stores complete matrices in DDR and configures the accelerator over AXI4-Lite. AXI DMA moves tile data from DDR to the PL over AXI4-Stream. The input adapter and tile loader arrange weights and activations for the 4×4 weight-stationary systolic array. Each PE performs signed INT8 multiplication with INT32 accumulation. Local buffers preserve operands and partial sums, allowing the same weight tile to be reused across output rows and K tiles to be accumulated correctly. The output packer streams valid INT32 results back through DMA to DDR, where the ARM software validates them against a golden model. Larger and rectangular matrices are handled by tiling, edge masks, and zero padding when needed.”

## Implementation bring-up order
1. Validate AXI-Lite register reads/writes.
2. Validate DMA MM2S-to-S2MM loopback with no compute.
3. Validate stream input capture and output generation using a fixed pattern.
4. Simulate and verify one PE, then a 2×2 and 4×4 array.
5. Verify weight preload, activation skew, reduction, and signed INT32 accumulation.
6. Add tile controller, K-tile accumulation, edge masking/padding, and ping-pong buffers.
7. Run end-to-end golden-model tests for square and rectangular matrices.
8. Synthesize/implement in Vivado; report LUT/FF/BRAM/DSP use and timing, then measure performance and power.