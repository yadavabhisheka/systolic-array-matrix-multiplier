# System Requirements Specification (SRS)

## ZedBoard Matrix-Multiplication Accelerator

**Target platform:** Digilent ZedBoard — Xilinx Zynq-7000 `XC7Z020-CLG484-1`  
**Document status:** Project requirements baseline; implementation details marked provisional require validation  
**Data types:** Signed INT8 inputs, signed INT32 accumulation/output  
**Architecture reference:** `ZedBoard_Full_Implementation_Guide.docx`

---

## 1. Purpose and Scope

This document defines the system requirements, performance targets, power and area (PPA) requirements, and interfaces for the matrix-multiplication accelerator implemented on the ZedBoard.

The system is intended to compute

\[
C_{M \times N} = A_{M \times K} \times B_{K \times N}
\]

using signed INT8 input operands and signed INT32 accumulation/output. It must support tiled execution for matrices larger than the physical compute array and handle rectangular matrices and partial edge tiles.

The architecture baseline uses:

- Bare-metal C on the ARM Cortex-A9 processing system (PS).
- Full input/output matrices stored in DDR3 memory.
- AXI4-Lite for accelerator control and status.
- AXI DMA in Direct Register/Simple mode for payload movement.
- AXI4-Stream between DMA and programmable logic (PL).
- A PS high-performance (HP) port for DMA access to DDR.
- Local BRAM/register/FIFO storage for active tiles and partial sums.
- A weight-stationary processing-element (PE) grid.
- Zero-padding for edge tiles when required by fixed-size hardware, with invalid output cells masked.

> **Architecture feasibility note:** The guide describes a 16×16 PE grid as a worked mapping, not a proven implementation. The XC7Z020 has 220 DSP slices, while a 16×16 grid contains 256 PEs. The physical PE count, arithmetic mapping, resource use, and timing must be validated with Vivado synthesis and implementation. Do not describe the 16×16 physical array as proven to fit until reports demonstrate it.

## 2. System Overview

### 2.1 System data path

1. Bare-metal C prepares matrices `A`, `B`, and output storage `C` in DDR.
2. Software configures accelerator registers and schedules tile/operand phases.
3. AXI DMA MM2S reads payloads from DDR through a PS HP path and emits AXI4-Stream beats.
4. A PL stream adapter accepts the stream, unpacks INT8 operands, identifies the configured phase/tile, and applies backpressure as needed.
5. Local buffers and PE-local registers hold active operands and weights.
6. The weight-stationary PE grid computes products and partial sums.
7. The output/partial-sum buffer holds results as required by the selected schedule.
8. An output packer emits completed INT32 results through AXI4-Stream.
9. AXI DMA S2MM writes output payloads to DDR.
10. Software checks completion and compares results with the C/C++ golden model.

The exact PE timing, skewing, partial-sum routing, stream framing, and buffer organization must be specified in the RTL microarchitecture before implementation.

### 2.2 Fixed requirements versus items to validate

| Category | Baseline |
|---|---|
| Board/device | ZedBoard, Zynq-7000 `XC7Z020-CLG484-1` |
| Operand format | Signed INT8 |
| Accumulation/output format | Signed INT32 |
| Matrix operation | General matrix multiplication (GEMM) |
| Large matrices | Tiled execution |
| Rectangular dimensions | Required |
| Edge handling | Padding where required and suppression of invalid output writes |
| Control | AXI4-Lite |
| Data path | AXI DMA MM2S/S2MM and AXI4-Stream |
| DDR access | DMA through a configured PS HP path |
| Software | Bare-metal C on ARM Cortex-A9 |
| Dataflow | Weight-stationary candidate mapping |
| Physical PE count | Must be selected/validated; 16×16 is not yet proven feasible |
| Clock | 100 MHz initial target; must be validated by timing reports |
| Power target | To be established from implementation and measurement |
| Numerical overflow behavior | Must be defined for supported reduction lengths |

---

## 3. Functional Requirements

Priority terms:
- **Must:** required for project acceptance.
- **Should:** desirable unless a documented reason prevents it.

| ID | Requirement | Priority | Verification |
|---|---|---|---|
| FR-01 | Compute `C = A × B` for compatible matrix dimensions. | Must | Golden-model comparison |
| FR-02 | Accept signed INT8 matrix operands. | Must | Directed and random tests |
| FR-03 | produce signed INT32 accumulated/output values. | Must | Directed and boundary tests |
| FR-04 | Support configurable dimensions `M`, `N`, and `K` within documented implementation limits. | Must | Dimension tests |
| FR-05 | Support matrices larger than the physical PE array by processing tiles. | Must | Multi-tile tests |
| FR-06 | Support rectangular matrices. | Must | Rectangular workload tests |
| FR-07 | Handle partial edge tiles using zero-padding and/or valid-element masking. | Must | Non-multiple dimension tests |
| FR-08 | Preserve partial sums across `K`-dimension chunks and initialize each output tile correctly. | Must | Multi-`K`-tile tests |
| FR-09 | Provide software-programmable control and status registers. | Must | AXI4-Lite register tests |
| FR-10 | Transfer input payloads from DDR to PL using DMA MM2S and AXI4-Stream. | Must for selected baseline | DMA integration test |
| FR-11 | Transfer output payloads from PL to DDR using AXI4-Stream and DMA S2MM. | Must for selected baseline | DMA integration test |
| FR-12 | Report idle/busy/completion/error status using a documented contract. | Must | Control/status tests |
| FR-13 | Support repeated operations with changing inputs without requiring a full system reset for every operation. | Must | Repeated-run test |
| FR-14 | Define and detect supported invalid configurations and report errors. | Should | Negative tests |
| FR-15 | Match the C/C++ golden model for all supported test cases. | Must | Automated comparison |
| FR-16 | Synthesize and implement on the target ZedBoard within agreed constraints. | Must | Vivado reports and board test |
| FR-17 | Record resource utilization, timing, latency, throughput, and power methodology/results. | Must | Final report |

## 4. Numerical and Data Requirements

### 4.1 Matrix operation

For `A` of shape `M × K`, `B` of shape `K × N`, and `C` of shape `M × N`:

\[
C[i,j] = \sum_{k=0}^{K-1} A[i,k] \times B[k,j]
\]

- `A[i,k]` and `B[k,j]` are signed 8-bit integers.
- Each product is mathematically representable in signed 16-bit range.
- The accumulation and output format is signed 32-bit integer.
- Input and output matrix layout is initially row-major.
- The implementation must define whether strides and lengths are expressed in bytes or elements; the software and RTL must use the same convention.

### 4.2 Overflow

The implementation must document the supported `K` range and accumulator overflow policy. For arbitrary signed INT8 inputs, each product has magnitude no greater than 16,384. A signed INT32 accumulator can guarantee no overflow for all possible operand combinations when `K ≤ 131071`; this is a mathematical bound, not a performance or implementation guarantee.

For the supported workload, the preferred initial requirement is exact INT32 accumulation without overflow. If larger reduction lengths are supported, specify whether the design uses wider accumulation, saturation, or wraparound. Do not leave overflow behavior implicit.

### 4.3 Packing and byte order

The stream adapter must define:
- Byte order of packed INT8 operands.
- Order in which `A` and `B` payloads are sent.
- Whether operand phases use separate DMA transfers.
- How tile identifiers and `M/N/K` valid counts are communicated.
- How output INT32 values are serialized.
- Exact relationship between payload byte count, DMA length, and `TLAST`.

For an illustrative 32-bit stream, four INT8 values can be carried per beat, subject to a documented lane/byte order. A 32-bit output stream can carry one INT32 result per beat. These are examples; stream width must be fixed in the Vivado design and recorded here before RTL/driver integration.

---

## 5. Tiling Requirements

For

\[
A_{M \times K} \times B_{K \times N} = C_{M \times N},
\]

the software schedules output tiles over `M` and `N` and reduction chunks over `K`. The software tile sizes `MT`, `NT`, and `KT` need not all be equal, and their supported values depend on the selected PE mapping and local storage.

Conceptual loop order:

```c
for (im = 0; im < M; im += MT) {
    for (jn = 0; jn < N; jn += NT) {
        clear_valid_C_tile();

        for (ik = 0; ik < K; ik += KT) {
            valid_M = min(MT, M - im);
            valid_N = min(NT, N - jn);
            valid_K = min(KT, K - ik);

            pack_A_tile();
            pack_B_tile();
            pad_edges_if_required();
            transfer_operands_and_compute();
            accumulate_partial_results();
        }

        write_valid_C_tile_to_DDR();
    }
}
```

Requirements:
1. Initialize an output/partial-sum tile once before its first `K` chunk, not before every `K` chunk.
2. Preserve accumulated partial sums across all `K` chunks belonging to the same output tile.
3. Use `valid_M`, `valid_N`, and `valid_K`, or an equivalent documented mechanism, to describe edge tiles.
4. If fixed-size payloads are required, pad invalid input positions with zero.
5. Never write padded output rows or columns beyond the true `C` dimensions.
6. Define row-major address calculations and whether strides are measured in bytes or elements.
7. For initial bring-up, software may pack non-contiguous subtiles into contiguous DDR staging buffers before DMA. A more complex address generator or transfer method is optional and should be justified by measurements.

---

## 6. Interface Requirements

### 6.1 Interface summary

| Interface/component | Role | Requirement/status |
|---|---|---|
| AXI4-Lite slave registers | Low-bandwidth configuration and status | Baseline |
| AXI DMA MM2S | DDR-to-stream input transfer | Baseline |
| AXI DMA S2MM | Stream-to-DDR output transfer | Baseline |
| AXI4-Stream input | Deliver operand payloads to PL | Baseline |
| AXI4-Stream output | Deliver results to S2MM | Baseline |
| PS HP port | DMA memory access to PS DDR | Baseline; exact port/width set in Vivado |
| PL clock/reset | Clock and reset accelerator and AXI interfaces | Must be documented |
| Internal PE links | Move operands/partial sums through the PE grid | Defined by RTL microarchitecture |

### 6.2 AXI4-Lite control interface

AXI4-Lite is for configuration and status; matrix payloads must not be moved one element at a time through control registers.

The following is a **proposed register contract**, not a frozen address map. Final offsets, widths, reset values, and semantics must be fixed before driver/RTL integration.

| Register | Suggested fields/purpose |
|---|---|
| `CONTROL` | `start`, `soft_reset` if supported, interrupt enable if supported |
| `STATUS` | `idle`, `busy`, `done`, `error`; define sticky/clear behavior |
| `M`, `N`, `K` | Full matrix dimensions |
| `MT`, `NT`, `KT` | Tile dimensions |
| `VALID_M`, `VALID_N`, `VALID_K` | Valid extents for the current edge tile |
| `MODE` | Signedness, input width, accumulator/output width, dataflow/version if configurable |
| `TILE_ID` / `PHASE` | Current tile and operand-loading phase |
| `A_BASE_ADDR`, `B_BASE_ADDR`, `C_BASE_ADDR` | Optional addresses if needed by the selected software/DMA contract |
| `ERROR_CODE` | Error details |
| `INPUT_BYTES`, `OUTPUT_BYTES` | Optional debug counters |
| `TILE_COUNT`, `CYCLE_COUNT` | Optional performance/debug counters |

For initial bring-up, software may schedule one tile at a time and program the phase/tile registers before each transfer. Avoid a complicated programmable-logic tile address generator until measurements show that software scheduling is a bottleneck.

### 6.3 AXI4-Stream protocol

The input stream adapter must:
- Accept transfers only when `TVALID && TREADY` are both high on a rising clock edge.
- Hold payload and control signals stable while `TVALID` is high and `TREADY` is low.
- Unpack INT8 values consistently with the documented byte/lane order.
- Route payloads to weight preload or activation input according to the agreed phase contract.
- Apply backpressure without losing or duplicating data.
- Define the meaning of `TLAST`, such as end of a tile or transfer, and use the same convention in software, PL, and DMA setup.

The output packer must:
- Emit only completed, valid INT32 results.
- Use the documented result ordering.
- Assert `TLAST` at the defined boundary.
- Hold data stable during backpressure.
- Emit exactly the byte count programmed for the S2MM transfer.

The exact stream width and whether `TKEEP` is used must match the configured IP and payload contract.

### 6.4 AXI DMA and DDR

Baseline data movement:
- MM2S reads operand bytes from DDR and produces AXI4-Stream input.
- S2MM receives AXI4-Stream output and writes result bytes to DDR.
- DMA memory-mapped ports connect through the configured PS HP path and required interconnect.
- DMA control registers are accessed from the PS through the configured AXI control path.
- Begin with Direct Register/Simple mode, without scatter-gather, to reduce initial complexity.

Software requirements:
1. Validate that buffers are valid DDR addresses and do not overlap code, stack, or other allocations.
2. Use the correct DMA source/destination addresses and transfer lengths.
3. Arm S2MM with sufficient receive capacity before output data is emitted.
4. Ensure MM2S transfer phases match the PL operand-loader contract.
5. If CPU data cache is enabled, flush/clean source buffer cache lines before DMA reads and invalidate destination cache lines before the CPU reads DMA output, following the BSP APIs and alignment rules.
6. Check both DMA and accelerator completion/error status.
7. Ensure the transfer byte count agrees with the actual emitted/received payload and `TLAST` convention.
8. Do not assume that a standard HP path is cache-coherent.

### 6.5 Clock and reset

- Use a single clock domain for accelerator logic where practical.
- The initial clock target is 100 MHz, subject to timing validation.
- The PL clock source and AXI clock configuration must be recorded in the Vivado block design.
- Reset polarity, assertion/deassertion behavior, reset sequencing, and AXI reset compatibility must be documented.
- If any interfaces operate in different clock domains, implement and verify the required clock-domain crossing.
- A board oscillator frequency is not evidence that the complete design meets timing at that frequency.

---

## 7. Performance Requirements

### 7.1 Initial performance target

The architecture guide identifies a 100 MHz PL clock as a planning starting point, not a guaranteed maximum frequency. Actual performance must be measured after implementation.

Peak throughput depends on the **physical** PE count, not the conceptual 16×16 example.

For `P` PEs, clock frequency `f`, and an assumption of one MAC per PE per cycle:

\[
\text{Peak MAC/s} = P \times f
\]

If a 4×4 array (16 PEs) is selected and meets 100 MHz, its theoretical peak is:

- `16 × 100 MHz = 1.6 GMAC/s`
- `3.2 GOPS` if one MAC is counted as two operations (one multiply and one add).

If a 16×16 array (256 PEs) were implemented at 100 MHz and sustained one MAC per PE per cycle, its arithmetic peak would be 25.6 GMAC/s or 51.2 GOPS under the same counting convention. This is only a mathematical illustration, **not a claim that this array fits or meets timing on XC7Z020**.

### 7.2 Performance metrics to report

| Metric | Definition |
|---|---|
| Implemented clock frequency | Frequency supported by timing analysis |
| End-to-end latency | From accepted operation start to completion, with measurement boundary stated |
| Compute latency | Time attributed to computation, separately from transfers where measurable |
| Peak MAC/s | Physical PE count × clock, under the stated MAC/cycle assumption |
| Sustained MAC/s or GOPS | Completed operations divided by measured elapsed time |
| PE utilization | Fraction of PE cycles performing useful work, if instrumented |
| Input/output transfer time | Time spent moving operands/results |
| Cycle count | Cycles for a tile and/or complete workload |
| Workload dimensions | `M`, `N`, `K`, tile sizes, and edge-tile behavior |
| Comparison basis | State whether software setup, packing, DMA, and output transfer are included |

For conventional counting of one multiplication plus one addition as two operations:

\[
\text{GOPS} = \frac{2MNK}{T \times 10^9}
\]

where `T` is elapsed time in seconds. Use a consistent convention across comparisons.

### 7.3 Performance acceptance

- The design must meet the final declared clock constraint or document the achieved frequency and the reason for revising the target.
- Report theoretical peak and measured sustained throughput separately.
- Benchmark small single-tile and larger multi-tile workloads.
- Include rectangular and edge-tile cases.
- State whether reported latency includes DMA, CPU tile packing/scheduling, and output writes.
- Do not claim a speedup against a CPU/GPU without a comparable measured baseline.

---

## 8. Power Requirements

No numerical power target is fixed yet. Establish it after the PE mapping, buffer architecture, clock, and implementation are known.

| ID | Requirement | Method |
|---|---|---|
| PWR-01 | Record estimated power for the implemented design. | Vivado power analysis with documented assumptions |
| PWR-02 | Use representative switching activity where practical. | Simulation/activity inputs if available |
| PWR-03 | Record idle-state power under defined conditions. | Tool estimate and/or hardware measurement |
| PWR-04 | Record active power during representative matrix workloads. | Tool estimate and/or hardware measurement |
| PWR-05 | State the measurement boundary. | Distinguish PL, FPGA/device, and whole-board power |
| PWR-06 | Report methodology, tool version, assumptions, and workload. | Reproducible report |
| PWR-07 | Calculate energy per operation when the measurement supports it. | Power integrated over execution time or documented approximation |

A whole-board power reading includes components beyond the accelerator and must not be presented as accelerator-only power. Tool estimates and physical measurements must be clearly distinguished.

**Power target:** TBD after baseline implementation/estimation.

---

## 9. Area and Resource Requirements

### 9.1 Device resources

The project guide lists the following approximate XC7Z020 resources:

| Resource | Device-level capacity |
|---|---:|
| LUTs | 53,200 |
| Flip-flops | 106,400 |
| DSP slices | 220 |
| BRAM36 blocks | 140 |
| Nominal aggregate BRAM | Approximately 630 KiB |
| PS DDR3 | 512 MB |

These are device totals, not an allowance solely for the accelerator. Confirm the selected part and the actual post-synthesis/post-implementation report in Vivado.

### 9.2 Initial planning budgets

The following are **proposed engineering budgets**, not figures established by the architecture guide or measured results. Review them after the PE and interface synthesis experiments.

| Resource | Initial budget | Approximate count |
|---|---:|---:|
| LUT | ≤ 35% | ≤ 18,620 |
| FF | ≤ 35% | ≤ 37,240 |
| DSP | ≤ 30% | ≤ 66 |
| BRAM36-equivalent | ≤ 50% | ≤ 70 |

The budgets are intended to preserve headroom for control, data movers, FIFOs, and future modifications. They may be revised with documented justification.

### 9.3 Area requirements

- The design must fit the selected XC7Z020 device.
- Report LUT, FF, DSP, and BRAM utilization after synthesis and implementation.
- Report timing and congestion issues alongside resource use.
- Inspect the inferred mapping of the signed INT8 multiplier and INT32 accumulator.
- Do not assume one PE equals one DSP slice; mapping is synthesis-dependent.
- Account for FIFOs, buffers, control, DMA interfaces, and output/partial-sum storage.
- Keep full matrices in DDR; reserve on-chip storage for active tiles and other structures required by the design.
- Confirm that buffering and memory banking fit available BRAM resources.

### 9.4 PE-grid feasibility

A 16×16 grid contains 256 PEs, which exceeds the device's 220 DSP slices if every PE consumes one DSP. This does not by itself prove that a 16×16 design is impossible, but it is a material feasibility risk.

Required feasibility sequence:
1. Synthesize one PE and inspect DSP/LUT/FF mapping.
2. Synthesize a small array such as 2×2 or 4×4.
3. Inspect scaling, BRAM inference, and timing.
4. Compare candidate physical arrays such as 4×4 and 8×8.
5. Attempt a 16×16 physical array only if reports justify it.
6. Freeze the physical PE count only after the resource and timing evidence is available.

---

## 10. Verification and Acceptance Plan

| Test ID | Test | Expected result |
|---|---|---|
| VT-01 | Known-value 2×2/4×4 matrix multiplication | Exact match with golden model |
| VT-02 | Signed INT8 random operands | Exact match for supported range |
| VT-03 | Extreme INT8 values | Correct signed multiplication/accumulation |
| VT-04 | Full worked 16×16 problem | Correct result if supported by selected physical architecture |
| VT-05 | Rectangular case `M=19, N=13, K=27` | Correct output and edge handling |
| VT-06 | Multiple `K` chunks | Partial sums preserved correctly |
| VT-07 | Partial edge tiles | No invalid output writes |
| VT-08 | Intermittent `TREADY` deassertion | No lost, duplicated, or corrupted stream beats |
| VT-09 | DMA DDR-to-stream-to-DDR loopback | Byte-for-byte match |
| VT-10 | AXI4-Lite register read/write | Correct register behavior |
| VT-11 | Repeated runs with changed inputs | No stale-data/cache-related failures |
| VT-12 | Invalid dimensions/configuration | Defined error behavior |
| VT-13 | Synthesis and implementation | Resource and timing reports available |
| VT-14 | Physical ZedBoard run | Hardware output matches golden model |
| VT-15 | Performance characterization | Reproducible latency/throughput results |
| VT-16 | Power characterization | Methodology and results documented |

The verification plan should progress from control-register access, to DMA loopback, to stream capture/output, to a single PE, a small array, one tile, general tiling, and finally rectangular edge cases. This isolates integration failures from arithmetic failures.

---

## 11. Implementation Constraints and Milestones

1. **Board and clock bring-up:** Confirm PS configuration, DDR, PL clock, reset, and AXI4-Lite access.
2. **DMA loopback:** Validate DDR → MM2S → stream loopback → S2MM → DDR.
3. **Protocol contract:** Freeze stream width, byte order, operand phases, `TLAST`, transfer lengths, and edge-valid counts.
4. **PE verification:** Verify a single PE, then a small array, against the golden model.
5. **Single-tile execution:** Load weights, process activations, and produce the output tile.
6. **General tiling:** Support larger `M`, `N`, and `K` with correct reduction accumulation.
7. **Rectangular edges:** Validate valid counts, zero-padding/masking, and output bounds.
8. **PPA optimization:** Evaluate buffering, stream width, interrupts, and more complex transfer schemes only when measurements justify them.
9. **Final hardware validation:** Run representative workloads on the ZedBoard and record correctness, timing, resources, and power.

---

## 12. Items to Freeze Before RTL/Software Integration

The architecture baseline is defined, but the following low-level contracts must be explicitly finalized:

- [ ] Physical PE-grid dimensions and PE arithmetic mapping.
- [ ] Exact clock and reset configuration.
- [ ] Final AXI4-Lite register offsets, widths, reset values, and clear/sticky semantics.
- [ ] AXI DMA configuration, stream widths, and memory-map widths.
- [ ] Input stream byte/lane order and operand phase sequencing.
- [ ] `TLAST` packet convention and exact MM2S/S2MM byte lengths.
- [ ] Tile dimensions `MT`, `NT`, `KT` and their legal values.
- [ ] Edge-tile valid-count/padding/masking contract.
- [ ] Partial-sum initialization and retention across `K` chunks.
- [ ] DDR buffer alignment, cache maintenance, and address/stride units.
- [ ] Supported dimension limits and accumulator overflow behavior.
- [ ] Final resource budgets and clock target after synthesis/implementation.
- [ ] Power measurement boundary and numerical target after baseline characterization.

---

## 13. Definition of Done

The project is complete when:

- The RTL implements the approved matrix-multiplication and interface contracts.
- Supported matrix sizes, including rectangular and tiled workloads, match the C/C++ golden model.
- AXI4-Lite control/status and DMA/AXI4-Stream data movement work on the ZedBoard.
- Partial sums are handled correctly across `K` chunks.
- Partial tiles do not corrupt or overwrite invalid output memory.
- The design successfully synthesizes and implements within documented resource and timing constraints.
- Measured performance is reported with workload and timing boundaries.
- Power estimates or measurements are reported with their methodology and measurement boundary.
- The repository contains reproducible build, test, and hardware bring-up instructions.

## 14. References

The following platform and IP references are identified in the implementation guide:

1. Digilent, *ZedBoard Hardware User Guide v2.2*: https://files.digilent.com/resources/programmable-logic/zedboard/ZedBoard_HW_UG_v2_2.pdf
2. AMD, *Zynq-7000 SoC Technical Reference Manual UG585 — PS–PL AXI Interfaces*: https://docs.amd.com/r/en-US/ug585-zynq-7000-SoC-TRM/PS-PL-AXI-Interfaces
3. AMD, *Zynq-7000 SoC Overview DS190*: https://www.amd.com/content/dam/xilinx/support/documents/data_sheets/ds190-Zynq-7000-Overview.pdf
4. AMD, *AXI DMA Product Guide PG021 — Core Overview*: https://docs.amd.com/r/en-US/pg021_axi_dma/Core-Overview
5. AMD, *AXI DMA PG021 — Direct Register Mode (Simple DMA)*: https://docs.amd.com/r/en-US/pg021_axi_dma/Direct-Register-Mode-Simple-DMA
6. AMD, *AXI DMA PG021 — 2-D Transfers*: https://docs.amd.com/r/en-US/pg021_axi_dma/2-D-Transfers
7. AMD, *AMBA AXI overview*: https://www.amd.com/en/products/adaptive-socs-and-fpgas/intellectual-property/axi.html

---

*Document maintenance:* Update this file when an architecture decision is frozen or when synthesis, implementation, or hardware measurements justify a change. Keep proposed targets, measured results, and verified requirements clearly distinguished.