# Literature Survey, Research Gap, and Proposed FPGA Accelerator Architecture

**Project:** Design, Verification, and FPGA Implementation of a Tiled
Matrix-Multiplication Accelerator
**Target board:** Digilent ZedBoard with Xilinx Zynq-7000
XC7Z020-CLG484-1
**Status:** Proposed baseline for implementation; verify resource use
and timing through Vivado synthesis and implementation.

> **Decision boundary:** The board is fixed. The literature review and
> board-resource analysis inform the design, but the architecture below
> is a practical project baseline---not a claim that every detail has
> already been validated by synthesis. The first implementation target
> is a **4×4 processing-element (PE) array** using **signed INT8 inputs
> and INT32 accumulation/output**, with **tiling for larger matrices**
> and **padding or valid-dimension masking for rectangular and edge
> tiles**.

## 1. Project files and working documents

Keep the following three supporting files together with this Markdown
document in the repository. The spreadsheet links below are the
comparison and board-planning files already prepared during the project.

1.  **Research-paper comparison workbook:** [Literature Survey ---
    6-paper comparison](resources/literature_survey_comparison.xlsx)

2.  **Board, interface, and tiling analysis workbook:** [ZedBoard
    Resource, Interface, and Tiling
    Analysis](resources/ZedBoard_Resource_Interface_Tiling_Analysis.xlsx)

3.  **Detailed implementation guide:** [ZedBoard Full Implementation
    Guide (PDF)](resources/ZedBoard_Full_Implementation_Guide.pdf) ·[
    diagram (PNG)](resources/ZedBoard_Full_Implementation_Dataflow.png)

These supporting artifacts should be committed alongside this document
using the paths shown in the links above.

------------------------------------------------------------------------

## 2. Problem statement

Matrix multiplication is a fundamental operation in scientific
computing, signal processing, computer vision, and machine learning.
Given matrices

$$
A \in \mathbb{Z}^{M\times K},\qquad
B \in \mathbb{Z}^{K\times N},
$$

the accelerator must produce

$$
C = AB,\qquad C \in \mathbb{Z}^{M\times N},
$$

with each output element

$$
C_{i,j}=\sum_{k=0}^{K-1} A_{i,k}B_{k,j}.
$$

A software implementation on the Zynq processing system (PS) is
straightforward and is valuable as a correctness reference, but it does
not by itself demonstrate a custom parallel datapath in programmable
logic (PL). The project is to design, verify, and implement a
matrix-multiplication accelerator in the ZedBoard PL, connect it to the
PS through AXI-based interfaces, and demonstrate correct operation on
the FPGA.

The engineering problem is broader than constructing a grid of
multipliers. A usable accelerator also needs data movement, local
storage, control and status registers, transfer-completion handling,
support for matrices larger than the physical array, correct handling of
non-multiple dimensions, and a repeatable software-to-hardware
verification flow.

### 2.1 Scope

**Included** - Signed INT8 input operands. - INT32 accumulation and
INT32 output elements. - A 4×4 PE array as the first physical
compute-array target. - Tiled execution for matrices larger than the
physical array and local buffers. - Padding with zeros or
valid-dimension masking for edge tiles and rectangular matrices. - Local
PL buffers, an AXI control path, DMA-based bulk transfers, and a
bare-metal C control application. - A C/C++ software golden model and
comparison of hardware results against it. - Simulation, synthesis,
implementation, and board-level testing.

**Not claimed at this stage** - A measured speedup over the ARM
processor. - A guaranteed clock frequency or exact resource count. - A
finalized optimal dataflow, tile size, DMA configuration, or BRAM
banking scheme. - Support for every possible matrix dimension before the
RTL interface and limits are specified. - A claim that a 16×16 physical
array fits the XC7Z020. A 16×16 array contains 256 PEs, more than the
device's 220 DSP slices if every PE requires one DSP slice.

------------------------------------------------------------------------

## 3. Literature review: what the papers contribute

The six-paper comparison workbook is the detailed evidence table. The
points below summarize the role of each paper in the design decision;
they should not be interpreted as a direct apples-to-apples performance
ranking because the papers use different devices, workloads, precision,
and evaluation methods.

### 3.1 Kung --- "Why Systolic Architectures?" (1982)

Kung provides the foundational motivation for systolic architectures:
regular processing elements exchange data locally in a predictable
schedule, enabling pipelined computation and structured data movement.
This is useful for understanding why a systolic-style design can be
attractive for matrix operations.

**Design implication:** Prefer a regular, testable PE interconnect and
an explicit schedule over a design that depends on ad hoc global data
movement.

**Limitation for this project:** This is foundational architecture work,
not a measured implementation of this accelerator on the ZedBoard. It
does not select the project's array size, INT8/INT32 arithmetic, AXI
topology, or tile dimensions.

### 3.2 Xu et al. --- "A Survey of Design and Optimization for Systolic Array-based DNN Accelerators" (2023/2024)

This survey organizes systolic-array accelerator design issues,
including dataflow choices such as output-stationary, weight-stationary,
and input-stationary; mapping and tiling; reuse; and utilization
problems. It helps identify the decisions that must be made explicitly
rather than treating "systolic array" as a complete architecture
specification.

**Design implication:** The project must define operand movement,
partial-sum movement, tile boundaries, and how the array is reused when
a matrix is larger than the array.

**Limitation:** A survey synthesizes prior work; it does not prove a
specific configuration will meet the ZedBoard's timing or resource
constraints.

### 3.3 Asgari et al. --- "Proposing a Fast and Scalable Systolic Array for Matrix Multiplication" (FCCM 2020)

This work is relevant because it studies matrix multiplication and
reports a Zynq XC7Z020/Vivado HLS context. The available local PDF
appears incomplete, so its detailed claims must be checked against a
complete paper before being used as implementation requirements. The
reported comparisons and cycle equations belong to the paper's
particular designs and assumptions.

**Design implication:** Consider the cost of the multiplier/adder
organization and evaluate cycle count separately from measured
end-to-end latency.

**Limitation:** The paper's headline relative results are not a
prediction of this project's speedup. The workload, baseline, data
movement, implementation details, and measurement setup must match
before a direct comparison is meaningful.

### 3.4 Langhammer et al. --- "High Density 8-bit Multiplier Systolic Arrays for FPGA" (FCCM 2020)

This paper investigates dense INT8 multiplier systolic arrays on Intel
Stratix 10 FPGA hardware. It is useful for understanding arithmetic
packing and the importance of FPGA-specific implementation choices.

**Design implication:** INT8 arithmetic is a reasonable precision to
investigate for an FPGA accelerator, but multiplier mapping and resource
density depend on the FPGA family and synthesis strategy.

**Limitation:** Stratix 10 resource counts, frequencies, and peak
throughput cannot be transferred to the Zynq-7020. The target device's
DSP/LUT/BRAM reports must decide feasibility.

### 3.5 Jouppi et al. --- "In-Datacenter Performance Analysis of a Tensor Processing Unit" (TPU, 2017)

The TPU paper is an example of a large-scale, application-driven
matrix/tensor accelerator. Its 256×256 MAC array, INT8 multiplication
with wider accumulation, and substantial on-chip buffering illustrate
how compute arrays must be paired with a memory system and data reuse
strategy.

**Design implication:** Analyze compute, buffer capacity, data movement,
and utilization together---not just the number of multipliers.

**Limitation:** The TPU is a purpose-built ASIC, not a small Zynq FPGA.
Its array size and performance figures are reference points for
architectural reasoning, not feasible targets for this board.

### 3.6 Chen et al. --- "Eyeriss: An Energy-Efficient Reconfigurable Accelerator for Deep Convolutional Neural Networks" (2016)

Eyeriss demonstrates that data reuse, communication structure, and
energy-aware dataflow can matter as much as raw arithmetic throughput.
It is primarily a CNN accelerator, so its choices are not automatically
the best for a general matrix-multiplication engine.

**Design implication:** Make operand reuse and movement explicit, and
avoid moving every value from external memory for every multiply if
local reuse is possible.

**Limitation:** Its ASIC-specific architecture and CNN workload do not
directly determine this project's FPGA PE grid or AXI implementation.

------------------------------------------------------------------------

## 4. Research gap and project opportunity

The literature demonstrates several important ideas, but no one cited
paper directly supplies a complete, verified implementation for this
exact combination: **ZedBoard XC7Z020, signed INT8 operands, INT32
output/accumulation, a small physical PE grid, tiled general matrix
multiplication, edge-tile handling, AXI DMA integration, bare-metal PS
control, and end-to-end FPGA validation**.

This statement is a scoped gap for this project---not a claim that no
similar implementation exists anywhere.

### 4.1 Gap table

  --------------------------------------------------------------------------------
  Area               What the literature  Gap for this      Planned response
                     establishes          project           
  ------------------ -------------------- ----------------- ----------------------
  Compute            Regular arrays and   Need a manageable Start with a 4×4 PE
  architecture       different dataflows  architecture that grid; document the
                     are established      can be verified   chosen dataflow and
                     design choices.      and fitted to the validate it in RTL.
                                          XC7Z020.          

  Precision          INT8 multiplication  Need exact        Use signed INT8 inputs
                     with wider           signedness,       and signed INT32
                     accumulation is used product width,    accumulation/output;
                     in accelerator       accumulator       define and test
                     designs.             width, overflow   arithmetic explicitly.
                                          behavior, and     
                                          output            
                                          representation in 
                                          this design.      

  Array scale        Large arrays can     A 16×16 physical  Use 4×4 as the initial
                     deliver high         array has 256 PEs physical array; scale
                     parallelism when     and may exceed    workload with tiling
                     supported by         the available DSP rather than requiring
                     sufficient resources count if one DSP  a 16×16 physical grid.
                     and bandwidth.       is used per PE.   

  Tiling             Tiling is a standard Need an explicit  Define M/N/K tiling
                     way to map larger    tile schedule for and a
                     matrices to bounded  arbitrary M, N,   software-controlled
                     compute and storage  and K dimensions, tile loop.
                     resources.           including K       
                                          accumulation.     

  Rectangular/edge   Padding and masking  Need              Zero-pad edge tiles or
  dimensions         are common mapping   deterministic     use valid-dimension
                     techniques.          behavior when     masks; never write
                                          matrix dimensions padded output
                                          are not multiples positions as valid
                                          of the tile or PE results.
                                          dimensions.       

  On-chip memory     Local storage        Need a BRAM       Begin with
                     enables reuse and    budget that       conservative buffers
                     reduces              includes input    and inspect actual
                     external-memory      buffers, weights, Vivado BRAM inference
                     traffic.             output/partial    and reports.
                                          sums, FIFOs, and  
                                          control overhead. 

  PS--PL data        AXI and DMA provide  Need an           AXI4-Lite for
  movement           standard control and end-to-end        control/status, AXI
                     bulk-transfer paths. dataflow with     DMA for bulk movement,
                                          explicit transfer AXI4-Stream for
                                          sizes, completion payload, and a PS HP
                                          handling, and     port for DDR access.
                                          cache             
                                          maintenance.      

  Verification       Papers report their  Need a            Compare RTL/FPGA
                     own evaluation       reproducible      outputs against a
                     methodology and      correctness       software golden model
                     results.             process for this  for directed, random,
                                          implementation.   rectangular, and
                                                            multi-tile cases.

  Performance claims Papers report        No project        Report measured
                     throughput/latency   measurements      latency, clock,
                     under their own      exist yet.        utilization, and
                     assumptions and                        effective operations/s
                     platforms.                             only after
                                                            implementation.
  --------------------------------------------------------------------------------

### 4.2 Main contribution of this project

The intended contribution is a **complete and reproducible small-FPGA
matrix-multiplication accelerator flow**, connecting: 1. a defined
INT8/INT32 arithmetic contract; 2. a 4×4 compute array; 3. tile
scheduling for larger matrices; 4. correct handling of rectangular and
partial tiles; 5. local storage and AXI-based data movement; 6.
bare-metal software control and a golden reference model; and 7.
simulation-to-board validation with measured implementation results.

The contribution is the integrated, validated design and its
analysis---not a claim that any individual building block is new.

------------------------------------------------------------------------

## 5. Board constraints and their architectural consequences

The target is the Digilent ZedBoard based on the Zynq-7000
XC7Z020-CLG484-1.

  --------------------------------------------------------------------------
  Board/device item       Planning fact           Consequence
  ----------------------- ----------------------- --------------------------
  Programmable logic      Approximately 53,200    Compute and buffers must
                          LUTs, 106,400           share finite PL resources.
                          flip-flops, 220 DSP     Device totals are not the
                          slices, and 140 ×       same as resources
                          36-Kbit BRAM blocks     available after all logic
                          (device-level totals)   is implemented.

  PS                      Dual-core ARM Cortex-A9 Suitable for bare-metal
                          processing system       orchestration, data setup,
                                                  DMA configuration, and
                                                  result checking.

  DDR3                    512 MB external DDR3    Store full input/output
                          connected to the PS     matrices in DDR; do not
                          memory system           treat DDR as equivalent to
                                                  low-latency PL BRAM.

  PL clock source         Board has a 100 MHz PL  It is a clock source, not
                          oscillator              a guarantee that the
                                                  accelerator can close
                                                  timing at 100 MHz.
                                                  Validate timing after
                                                  implementation.

  PS--PL connectivity     GP, HP, and ACP         Use an HP path for DMA
                          interface options are   access to PS DDR in the
                          provided by the Zynq    proposed design, after
                          PS; exact setup is      configuring and validating
                          selected in the Vivado  it.
                          block design            

  AXI DMA                 Separate IP block, not  DMA must be
                          a built-in feature of   instantiated/configured;
                          the HP port             the HP interface provides
                                                  the PS memory path.
  --------------------------------------------------------------------------

### 5.1 Why start with a 4×4 physical array?

A 4×4 grid contains

$$
N_{PE}=4\times4=16
$$

processing elements. It is a practical first implementation target
because it: - keeps the first RTL interconnect and waveforms
manageable; - makes single-tile functional tests easy to inspect; -
provides enough parallel structure to demonstrate an array rather than a
single MAC; - leaves more room than a 16×16 array for AXI logic, FIFOs,
local buffers, control, and debug; - allows larger logical matrices to
be processed through tiling without increasing the physical PE count;
and - provides a baseline that can be compared with an 8×8
implementation if synthesis and timing permit.

This is a project-engineering trade-off, not proof that 4×4 is the
optimal array. After the first working design, compare 4×4 and 8×8 using
measured LUT, FF, DSP, BRAM, timing, throughput, and end-to-end latency.
A 16×16 array contains 256 PEs, exceeding the device's 220 DSP slices if
one DSP slice is required per PE; LUT-based or multi-DSP mapping might
change the result but must not be assumed without synthesis.

### 5.2 Why not use a 16×16 physical array first?

The matrix dimensions and PE-array dimensions are different concepts. A
16×16 **matrix workload** does not require a 16×16 **physical array**.
The proposed 4×4 grid can compute a 16×16 result in multiple tiles. This
reduces the required physical parallelism at the cost of additional tile
loading, control, and compute cycles.

------------------------------------------------------------------------

## 6. Final proposed baseline architecture

### 6.1 Architecture summary

  -----------------------------------------------------------------------
  Item                                Proposed baseline
  ----------------------------------- -----------------------------------
  FPGA board                          Digilent ZedBoard / Zynq-7000
                                      XC7Z020-CLG484-1

  Host/control processor              Zynq PS ARM Cortex-A9, bare-metal C

  Compute array                       4×4 physical PE array

  Input type                          Signed two's-complement INT8

  Multiply result                     Signed 16-bit product for INT8 ×
                                      INT8

  Accumulation                        Signed INT32 partial sums

  Output                              INT32 elements

  Workload                            General matrix multiplication,
                                      `C = A × B`

  Large-matrix support                Tiled execution across M, N, and K
                                      dimensions

  Edge/rectangular support            Zero padding or valid-dimension
                                      masking; output only valid elements

  Local PL storage                    BRAM/register-based
                                      input/weight/output buffers, sized
                                      after mapping and synthesis

  Control interface                   AXI4-Lite registers

  Bulk data                           AXI DMA MM2S and S2MM channels

  Stream payload                      AXI4-Stream

  DDR access                          DMA memory-mapped side connected to
                                      a PS HP port

  Initial software                    Bare-metal C driver/control
                                      sequence and C/C++ golden model

  Verification                        RTL simulation, DMA loopback, board
                                      tests, software/hardware comparison

  Dataflow                            A regular array with a documented
                                      local data schedule; use the
                                      weight-stationary mapping as the
                                      initial candidate, then confirm its
                                      RTL schedule and correctness before
                                      treating it as final
  -----------------------------------------------------------------------

### 6.2 Important dataflow clarification

The term **weight-stationary** means that a PE keeps its assigned weight
locally while activation values and partial sums move according to the
array's schedule. The exact PE mapping and the direction of
activation/partial-sum propagation must be consistent with the
implemented RTL.

For this project, the initial conceptual mapping is: - weights from a
tile of `B` are loaded into local PE storage; - activation values from a
tile of `A` are supplied according to the PE schedule; - each output
accumulates products over the reduction dimension `K`; - the resulting
output tile is sent to the output buffer.

Before coding, draw and verify a cycle-level schedule for the selected
4×4 mapping. A weight-stationary label alone does not define the
hardware. In particular, ensure that all `K` products for every valid
output are accumulated exactly once, that partial sums have the correct
routing/ownership, and that tile changes do not corrupt stored weights
or accumulators.

### 6.3 Arithmetic contract

For signed INT8 values,

$$
-128\leq A_{i,k},B_{k,j}\leq127.
$$

Each product is mathematically in the range

$$
-16256\leq A_{i,k}B_{k,j}\leq16384.
$$

The product needs 16 signed bits. Each product is sign-extended as
required before addition into a signed 32-bit accumulator. For a
reduction length `K`, the mathematical sum must be checked against the
INT32 range:

$$
-2^{31}\leq C_{i,j}\leq2^{31}-1.
$$

If supported workloads can exceed this range, the design must define
whether overflow wraps, saturates, or is prevented by a documented
input/shape constraint. Do not leave overflow behavior implicit. A
practical first version should test boundary values and document its
arithmetic semantics.

------------------------------------------------------------------------

## 7. System block diagram

The data/control path is:

``` text
                    Zynq Processing System (PS)
              +----------------------------------+
              | ARM Cortex-A9 / bare-metal C     |
              | - Allocate/fill input matrices   |
              | - Configure AXI-Lite registers   |
              | - Start/monitor DMA and compute  |
              | - Run/compare software reference |
              +----------------+-----------------+
                               |
                         PS DDR3 memory
                     A, B, C and staging buffers
                               ^
                               | AXI memory-mapped
                         +-----+------+
                         | AXI DMA    |
                         | MM2S/S2MM  |
                         +-----+------+
                               |
                         AXI4-Stream
                               |
                 +-------------v--------------+
                 | PL input/weight loader     |
                 | stream counters / buffers  |
                 +-------------+--------------+
                               |
                 +-------------v--------------+
                 | Local tile buffers          |
                 | A / B / C or partial sums   |
                 +-------------+--------------+
                               |
                 +-------------v--------------+
                 | 4×4 PE compute array        |
                 | signed INT8 multiply        |
                 | signed INT32 accumulation   |
                 +-------------+--------------+
                               |
                 +-------------v--------------+
                 | Output buffer / packer      |
                 +-------------+--------------+
                               |
                         AXI4-Stream
                               |
                         AXI DMA S2MM
                               |
                         PS DDR3: C output

      AXI4-Lite control/status: PS -> PL control registers
      Clock/reset: configured and validated in Vivado
```

The AXI4-Lite control path is logically separate from the AXI4-Stream
data path. The AXI DMA's memory-mapped side accesses DDR through the
configured PS--PL memory interface; its stream side exchanges bulk data
with the PL accelerator.

------------------------------------------------------------------------

## 8. AXI interface plan

### 8.1 AXI4-Lite --- control and status

Use AXI4-Lite for low-bandwidth register access, not for transferring
every matrix element as a software register write. Candidate registers:

  -----------------------------------------------------------------------
  Register                            Purpose
  ----------------------------------- -----------------------------------
  `CONTROL`                           Start, soft reset, interrupt enable
                                      if used

  `STATUS`                            Busy, done, error, input-ready,
                                      output-valid

  `M`, `N`, `K`                       Logical matrix dimensions

  `MT`, `NT`, `KT`                    Tile dimensions or tile limits

  `VALID_M`, `VALID_N`, `VALID_K`     Valid sizes for edge tiles

  `MODE`                              Padding/masking and supported
                                      operation selection

  `TILE_ID` / phase                   Current tile or load/compute/output
                                      phase for debug

  Counters                            Optional cycles, transferred words,
                                      and error counters
  -----------------------------------------------------------------------

These are a proposed register map, not a frozen binary ABI. Define
register offsets, reset values, access permissions, and error behavior
before writing the driver.

### 8.2 AXI DMA and AXI4-Stream --- bulk data

-   **MM2S**: transfers bytes from PS DDR to an AXI4-Stream output, used
    to load the PL input/weight path.
-   **S2MM**: accepts an AXI4-Stream input and writes bytes into PS DDR
    for the result.
-   **AXI4-Stream**: payload transfer; each beat is accepted when
    `TVALID && TREADY` are both high.
-   **`TLAST`**: define the end-of-packet convention precisely for the
    chosen DMA mode and stream framing. Do not assume the accelerator
    and DMA agree automatically.
-   **Transfer length**: the DMA length is in bytes. Ensure the
    programmed length agrees with the number of valid stream bytes and
    the `TLAST` convention.
-   **HP port**: provides the PS DDR access path for the PL master; it
    is not itself a DMA engine.
-   **Cache maintenance**: before MM2S reads CPU-written data from DDR,
    clean/flush the corresponding cache lines using the BSP's supported
    API. After S2MM writes output into DDR, invalidate the output
    buffer's cache lines before the CPU reads the data. Follow the exact
    BSP APIs and cache-line alignment requirements.

For the first bring-up, simple/direct-register DMA mode is a reasonable
choice because it reduces software complexity compared with
scatter-gather. Confirm the selected AXI DMA IP configuration and the
matching version of AMD's PG021 documentation in Vivado.

### 8.3 Suggested transfer sequence

1.  The PS prepares `A`, `B`, and an output buffer `C` in DDR.
2.  The PS performs cache maintenance on input buffers.
3.  The PS programs dimensions/mode over AXI4-Lite.
4.  The PS configures and arms S2MM for the expected output bytes.
5.  The PS starts the relevant MM2S transfer to load weights/operands.
6.  The PL loader consumes the stream and stores the correct tile data
    in local buffers.
7.  The PL computes the tile and asserts completion only after the
    output is valid.
8.  The output packer sends valid output bytes on AXI4-Stream; S2MM
    writes them to DDR.
9.  The PS waits for both relevant DMA status and accelerator
    completion/error status.
10. The PS invalidates output cache lines, reads `C`, and compares it
    with the golden model.

The exact order of MM2S transfers depends on the final stream packet
format and whether weights and activations use one stream or separate
streams. Define that packet format before implementing the DMA driver.

------------------------------------------------------------------------

## 9. Worked example: 16×16 matrix multiplication on a 4×4 array

### 9.1 Logical workload and storage

Let

$$
A\in\mathbb{Z}^{16\times16},\quad
B\in\mathbb{Z}^{16\times16},\quad
C=AB\in\mathbb{Z}^{16\times16}.
$$

With INT8 inputs and INT32 outputs:

  Matrix        Element count   Bytes per element                   Total
  ----------- --------------- ------------------- -----------------------
  `A`                     256                   1                   256 B
  `B`                     256                   1                   256 B
  `C`                     256                   4                 1,024 B
  **Total**               ---                 ---   **1,536 B = 1.5 KiB**

This is the logical matrix size, not the complete memory footprint of
the software, DMA descriptors, FIFOs, or local buffering.

### 9.2 Tiling

With a 4×4 physical grid, choose the initial logical tile sizes as

$$
M_t=N_t=K_t=4.
$$

For a 16×16 multiplication, each output tile is 4×4. There are

$$
\frac{16}{4}=4
$$

tiles in the `M` dimension and four in `N`, so there are 16 output
tiles. Each output tile requires reduction over `K`; because `K=16` and
`K_t=4`, there are four K-chunks per output tile.

Therefore, the schedule contains

$$
N_{\text{output tiles}}=
\left\lceil\frac{M}{M_t}\right\rceil
\left\lceil\frac{N}{N_t}\right\rceil=4\times4=16
$$

output tiles and

$$
N_{K\text{-chunks}}=\left\lceil\frac{K}{K_t}\right\rceil=4
$$

reduction chunks for each output tile. This corresponds to 64
output-tile/K-chunk compute phases, before counting load, drain, and
control cycles.

### 9.3 Tile loop

A high-level software schedule is:

``` text
for m0 = 0; m0 < M; m0 += Mt:
    for n0 = 0; n0 < N; n0 += Nt:
        initialize the valid C tile to zero

        for k0 = 0; k0 < K; k0 += Kt:
            load A[m0 : m0+Mt, k0 : k0+Kt]
            load B[k0 : k0+Kt, n0 : n0+Nt]
            pad or mask edge elements
            execute one tile operation
            accumulate the partial result into the C tile

        write only the valid C tile elements to the output buffer
```

**Critical correctness rule:** initialize each output tile once before
the K loop, then accumulate every K-chunk into that same tile. Do not
reset the output partial sums between K-chunks. Resetting between
K-chunks would lose contributions and produce incorrect results.

The actual PL may accumulate partial sums internally across K-chunks or
return partial sums to a local output buffer. Choose one method and
define its ownership and handshake clearly.

### 9.4 One output element

For example,

$$
C_{0,0}=\sum_{k=0}^{15}A_{0,k}B_{k,0}.
$$

The 4×4 grid processes this reduction in four K-chunks of four terms
each:

$$
C_{0,0}=
\underbrace{\sum_{k=0}^{3}A_{0,k}B_{k,0}}_{\text{chunk 0}}+
\underbrace{\sum_{k=4}^{7}A_{0,k}B_{k,0}}_{\text{chunk 1}}+
\underbrace{\sum_{k=8}^{11}A_{0,k}B_{k,0}}_{\text{chunk 2}}+
\underbrace{\sum_{k=12}^{15}A_{0,k}B_{k,0}}_{\text{chunk 3}}.
$$

The RTL must preserve the partial sum between chunks. This equation
describes the required arithmetic, not an exact cycle-level PE schedule.

------------------------------------------------------------------------

## 10. Rectangular matrices and padding

Tiling must work when dimensions are not exact multiples of the physical
array or selected tile dimensions.

### 10.1 Worked rectangular example

Let

$$
A\in\mathbb{Z}^{19\times27},\qquad
B\in\mathbb{Z}^{27\times13},\qquad
C\in\mathbb{Z}^{19\times13}.
$$

Choose tile dimensions

$$
M_t=16,\qquad K_t=16,\qquad N_t=8.
$$

The matrix product is valid because the inner dimensions match (`K=27`).
The tile counts are:

-   `M`: `ceil(19/16) = 2` tiles;
-   `K`: `ceil(27/16) = 2` reduction chunks;
-   `N`: `ceil(13/8) = 2` tiles.

For the final M tile, only 3 rows are valid. For the final K chunk, only
11 reduction elements are valid. For the final N tile, only 5 output
columns are valid.

### 10.2 Padding/masking rules

For a padded implementation: - fill invalid A elements with zero; - fill
invalid B elements with zero; - perform the fixed-size tile operation; -
write only output rows and columns that fall within the original matrix
dimensions.

Because zero multiplied by any operand contributes zero, zero padding
preserves the valid mathematical result as long as dimensions and signed
arithmetic are handled correctly.

Alternatively, provide `VALID_M`, `VALID_N`, and `VALID_K` values and
mask invalid lanes. Masking may avoid some unnecessary work, but it
complicates control and PE enable logic. For the first version, **zero
padding plus valid output bounds** is the simpler implementation
approach. The final design can compare padding with masking if
utilization becomes a measured concern.

Do not confuse rectangular matrix multiplication with multiplying
matrices of incompatible shapes. The fundamental shape rule is that the
number of columns of `A` must equal the number of rows of `B`.

------------------------------------------------------------------------

## 11. Local buffers and BRAM planning

For a tile with dimensions `Mt × Kt` for A, `Kt × Nt` for B, and
`Mt × Nt` for C, with INT8 inputs and INT32 outputs, the raw storage
estimate is

$$
S_{\text{raw}}=M_tK_t+K_tN_t+4M_tN_t\quad\text{bytes}.
$$

If A and B each have two ping-pong buffers while C has one buffer,

$$
S_{\text{double AB}}=2M_tK_t+2K_tN_t+4M_tN_t\quad\text{bytes}.
$$

For equal square tile dimensions `T`:

  -----------------------------------------------------------------------
                Tile size   One A + one B + one C   Double-buffered A and
                                             tile          B + one C tile
  ----------------------- ----------------------- -----------------------
                    16×16                 1.5 KiB                 2.0 KiB

                    32×32                   6 KiB                   8 KiB

                    64×64                  24 KiB                  32 KiB

                  128×128                  96 KiB                 128 KiB

                  256×256                 384 KiB                 512 KiB
  -----------------------------------------------------------------------

These are raw payload estimates only. They exclude PE registers, FIFOs,
AXI infrastructure, alignment, padding overhead, control state, and BRAM
width/depth fragmentation. A nominal 140 × 36-Kbit BRAM inventory is
approximately 630 KiB when expressed using 36 Kbit = 4.5 KiB per block,
but that does **not** mean all of it is available as a single usable
memory or should be consumed by tile buffers. Actual inference and
utilization must be checked in Vivado reports.

For the initial 4×4 design, start with small buffers and a simple
transfer protocol. Increase tile dimensions only after the basic
datapath and data movement work. Buffer depth and banking must match the
number of values consumed or produced per cycle.

------------------------------------------------------------------------

## 12. Data movement: end-to-end lifecycle

### 12.1 Host preparation

The bare-metal C application: 1. selects dimensions and tile
configuration; 2. creates or loads matrices `A` and `B` in DDR; 3.
initializes the output buffer; 4. computes the expected result using a
software golden model, or uses a previously generated reference; 5.
prepares DMA buffers in a suitable memory region and performs cache
maintenance as required.

### 12.2 Input movement

The DMA transfers a defined stream packet from DDR to the PL. The PL
loader counts stream beats and stores each value into the correct
buffer/location. The packet format must specify whether the transfer
carries: - B/weight tile first, followed by A/activation tile; -
separate streams for A and B; or - a combined packet with explicit
headers or known fixed lengths.

For the first implementation, use a simple, deterministic packet format
and document it. The loader should reject or flag unexpected byte
counts, early/late `TLAST`, and unsupported dimensions rather than
silently corrupting the tile.

### 12.3 Compute

The controller checks that the required input tiles are ready,
initializes partial sums when starting a new output tile, runs the PE
schedule, and waits for the final valid results. For each output tile,
it preserves accumulated sums over all K-chunks.

### 12.4 Output movement

The output packer converts each valid INT32 output to the stream
representation defined by the interface. S2MM writes the output packet
into a contiguous DDR staging buffer. For the first implementation,
staging buffers simplify submatrix addressing and avoid assuming the DMA
can directly transfer arbitrary strided matrix subregions. More advanced
2-D DMA or scatter-gather modes can be considered later.

### 12.5 Completion and verification

The PS must distinguish: - DMA transfer complete; - accelerator
computation complete; - accelerator error; - DMA error.

Do not assume that one status bit proves every stage completed. Once the
transfer is complete, invalidate output cache lines, compare all valid
output elements against the software reference, and report the first
mismatch with matrix indices and expected/actual values.

------------------------------------------------------------------------

## 13. Verification and implementation plan

### Phase 1 --- Software model and arithmetic contract

-   Finalize row-major matrix layout, signedness, product extension,
    accumulator behavior, and output layout.
-   Make the software golden model support arbitrary `M`, `N`, and `K`.
-   Test negative values, zeros, maximum/minimum INT8 values, and long
    reductions.

### Phase 2 --- PE and array RTL

-   Verify one PE independently.
-   Verify the 4×4 array with small directed matrices.
-   Check all valid products are accumulated exactly once.
-   Define reset behavior, valid/ready behavior, and output ordering.
-   Verify stalls and backpressure if the design supports them.

### Phase 3 --- Tile controller

-   Verify one 4×4 tile.
-   Verify multiple K-chunks without losing partial sums.
-   Verify multiple M/N tiles.
-   Verify padded or masked edge tiles.
-   Verify rectangular matrices and dimension limits.

### Phase 4 --- AXI control

-   Add AXI4-Lite registers and test read/write/reset behavior.
-   Test start, busy, done, and error status.
-   Verify invalid configurations are reported predictably.

### Phase 5 --- DMA and DDR

-   Bring up AXI DMA in simple/direct-register mode.
-   First perform a DMA loopback or known-pattern transfer.
-   Verify MM2S and S2MM independently before connecting compute.
-   Verify packet lengths, `TLAST`, stalls, buffer addresses, and cache
    maintenance.

### Phase 6 --- Full integration on ZedBoard

-   Run a known 2×2 or 4×4 test.
-   Run the 16×16 workload using the 4×4 physical grid.
-   Run a rectangular workload such as 19×27 multiplied by 27×13.
-   Run random signed matrices and repeated transfers.
-   Compare hardware results with the software golden model.

### Phase 7 --- Synthesis and measurement

Record at minimum: - LUT, FF, DSP, and BRAM use; - achieved clock
frequency and timing slack; - cycles per tile and per matrix; - DMA
transfer time and end-to-end latency; - effective operations per
second; - correctness pass/fail count; - tool versions, constraints, and
board configuration.

Only claim acceleration if measured end-to-end results support it.
Compare against a clearly specified software baseline on the same
workload and include data-transfer overhead.

------------------------------------------------------------------------

## 14. Risks and mitigations

  -----------------------------------------------------------------------
  Risk                    Why it matters          Mitigation
  ----------------------- ----------------------- -----------------------
  Array does not meet     Multiply/accumulate and Start with 4×4; inspect
  timing                  routing delay may limit timing reports;
                          clock frequency.        pipeline and optimize
                                                  only after a correct
                                                  baseline.

  DSP usage exceeds       Signed widths and       Check synthesis
  expectation             inference choices       reports; do not assume
                          affect mapping.         one multiplier always
                                                  maps exactly as
                                                  expected.

  Incorrect K             Resetting partial sums  Explicitly initialize
  accumulation            between chunks loses    once per output tile
                          terms.                  and test multi-chunk
                                                  cases.

  Stream framing mismatch DMA and accelerator can Specify byte count and
                          disagree about packet   `TLAST` semantics; test
                          boundaries.             known patterns before
                                                  compute integration.

  Cache incoherency       CPU may see stale DDR   Use the BSP's correct
                          data after DMA writes,  flush/clean and
                          or DMA may read stale   invalidate APIs.
                          data.                   

  Strided tile addressing Matrix subtiles are not Use contiguous staging
                          always contiguous in    buffers initially.
                          row-major memory.       

  Edge-tile corruption    Padding may leak into   Carry valid dimensions
                          valid output or invalid and only write valid C
                          values may be stored.   rows/columns.

  Buffer overflow or      Unsupported dimensions  Validate dimensions and
  shape mismatch          can overwrite local     tile bounds in software
                          storage.                and hardware.

  Unsupported performance Peak theoretical MAC    Report measured
  claims                  rate omits stalls and   end-to-end latency and
                          transfers.              resource/timing
                                                  reports.
  -----------------------------------------------------------------------

------------------------------------------------------------------------

## 15. Why this is the proposed solution

This baseline balances architectural relevance, implementation risk, and
demonstrability on the specified board:

1.  **4×4 rather than 16×16 physical compute:** it is a conservative
    first grid for a device with 220 DSP slices and finite routing/BRAM
    resources. It reduces first-stage debugging complexity while
    preserving the ability to process larger logical matrices.
2.  **INT8 inputs with INT32 accumulation/output:** this provides a
    clear, compact input representation while retaining wider sums for
    multi-term reductions. The exact overflow contract remains an
    explicit design requirement.
3.  **Tiling rather than a huge array:** tile scheduling scales the
    logical problem beyond the physical PE count and makes local
    storage/resource trade-offs measurable.
4.  **Padding plus valid output bounds:** this provides a simple,
    verifiable way to handle rectangular matrices and final partial
    tiles. Zero padding preserves valid outputs when invalid input
    positions are zeroed and invalid output positions are not written.
5.  **PS + PL split:** bare-metal C handles orchestration and
    correctness checking; the PL performs the parallel compute.
6.  **AXI4-Lite + DMA + AXI4-Stream:** control registers remain simple,
    while bulk data uses streaming transfers instead of software writing
    every operand through control registers.
7.  **DDR for full matrices, BRAM for active tiles:** this separates
    capacity from fast local reuse and avoids trying to keep arbitrary
    full workloads entirely on-chip.
8.  **Golden-model-driven verification:** correctness is established
    before performance optimization, and every reported hardware result
    can be checked against software.
9.  **Measured scaling path:** after the 4×4 baseline works, evaluate
    8×8 or larger tiles with synthesis and timing reports rather than
    assuming a larger array is automatically better.

### Final decision status

**Proposed baseline to implement first:** 4×4 physical PE array, signed
INT8 inputs, signed INT32 accumulation/output, tiled matrix
multiplication, zero padding or valid-dimension masking for edge tiles,
DDR-resident full matrices, BRAM/register-based local tiles, bare-metal
C on the PS, AXI4-Lite control/status, AXI DMA MM2S/S2MM, AXI4-Stream
payload, and the PS HP DDR path.

**Must be finalized before RTL integration:** cycle-level PE dataflow
and partial-sum routing; exact tile and buffer dimensions; stream packet
format and `TLAST` rules; DMA configuration; register offsets;
clock/reset strategy; overflow behavior; supported dimension limits; and
any interrupt/polling policy.

This is a concrete baseline, but the design is not considered complete
until simulation, synthesis, timing analysis, and board-level tests
confirm it.

------------------------------------------------------------------------

## 16. References

1.  H. T. Kung, "Why Systolic Architectures?", *Computer*, 1982.
    [PDF](https://www.eecs.harvard.edu/~htk/publication/1982-kung-why-systolic-architecture.pdf)
    · [DOI](https://doi.org/10.1109/MC.1982.1653825)
2.  R. Xu, S. Ma, Y. Guo, and D. Li, "A Survey of Design and
    Optimization for Systolic Array-based DNN Accelerators," *ACM
    Computing Surveys*, online 2023 / issue 2024.
    [DOI](https://doi.org/10.1145/3604802) ·
    [PDF](https://www.ccrc.wustl.edu/~roger/566S.s24/3604802.pdf)
3.  B. Asgari, R. Hadidi, and H. Kim, "Proposing a Fast and Scalable
    Systolic Array for Matrix Multiplication," FCCM 2020.
    [DOI](https://doi.org/10.1109/FCCM48280.2020.00035) · [FCCM
    page](https://www.fccm.org/past/2020/forums/topic/proposing-a-fast-and-scalable-systolic-array-for-matrix-multiplication/)
4.  M. Langhammer, S. Gribok, and G. Baeckler, "High Density 8-bit
    Multiplier Systolic Arrays for FPGA," FCCM 2020. [FCCM
    page](https://www.fccm.org/past/2020/forums/topic/high-density-8-bit-multiplier-systolic-arrays-for-fpga/)
5.  N. P. Jouppi et al., "In-Datacenter Performance Analysis of a Tensor
    Processing Unit," ISCA 2017.
    [arXiv](https://arxiv.org/abs/1704.04760) ·
    [DOI](https://doi.org/10.1145/3079856.3080246)
6.  Y.-H. Chen, T. Krishna, J. Emer, and V. Sze, "Eyeriss: An
    Energy-Efficient Reconfigurable Accelerator for Deep Convolutional
    Neural Networks," ISSCC 2016. [MIT
    DSpace](http://hdl.handle.net/1721.1/101151)
7.  Digilent, *ZedBoard Hardware User Guide v2.2*.
    [PDF](https://files.digilent.com/resources/programmable-logic/zedboard/ZedBoard_HW_UG_v2_2.pdf)
8.  AMD, *Zynq-7000 SoC Technical Reference Manual, UG585*. [PS--PL AXI
    Interfaces](https://docs.amd.com/r/en-US/ug585-zynq-7000-SoC-TRM/PS-PL-AXI-Interfaces)
9.  AMD, *Zynq-7000 SoC Data Sheet: Overview, DS190*.
    [PDF](https://www.amd.com/content/dam/xilinx/support/documents/data_sheets/ds190-Zynq-7000-Overview.pdf)
10. AMD, *AXI DMA Product Guide, PG021*. [Core
    Overview](https://docs.amd.com/r/en-US/pg021_axi_dma/Core-Overview)
    · [Direct Register
    Mode](https://docs.amd.com/r/en-US/pg021_axi_dma/Direct-Register-Mode-Simple-DMA)
    · [2-D
    Transfers](https://docs.amd.com/r/en-US/pg021_axi_dma/2-D-Transfers)
11. AMD, [AXI overview and IP
    information](https://www.amd.com/en/products/adaptive-socs-and-fpgas/intellectual-property/axi.html)

------------------------------------------------------------------------

## 17. Suggested GitHub repository placement

``` text
matrix-multiplication-accelerator/
├── README.md
├── docs/
│   ├── literature_survey.md
│   ├── final_clean_literature_survey_6_papers.xlsx
│   ├── ZedBoard_Resource_Interface_Tiling_Analysis.xlsx
│   ├── ZedBoard_Full_Implementation_Guide.pdf
│   └── ZedBoard_Full_Implementation_Dataflow.png
├── golden_model/
├── rtl/
├── tb/
├── sw/
└── scripts/
```

Update the relative links in this Markdown file if you use a different
directory structure. Keep the paper-comparison workbook, board-analysis
workbook, and implementation guide as separate supporting artifacts so
that the Markdown document remains the central narrative and decision
record.