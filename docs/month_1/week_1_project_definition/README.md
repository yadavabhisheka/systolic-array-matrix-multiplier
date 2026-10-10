# Week 1 — Project Definition

**Project title:** Design and Implementation of a Systolic Matrix Multiplication Accelerator  
**Repository:** https://github.com/yadavabhisheka/systolic-array-matrix-multiplier/  
**Phase:** Month 1, Week 1 — initial project definition  

---

## 1. Questions and Initial Understanding

Week 1 establishes what the project is, why it matters, and which questions require investigation. 

### Question 1 — Why matrix multiplication? Why is it important, and why accelerate it?

#### A. Mathematical understanding

A matrix is a rectangular arrangement of numbers. Matrix multiplication combines two compatible matrices to produce a third matrix.

Let

$$
A \in \mathbb{R}^{M \times K}, \qquad B \in \mathbb{R}^{K \times N}
$$
Then

$$
C = AB, \qquad C \in \mathbb{R}^{M \times N}
$$

The number of columns in $A$ must equal the number of rows in $B$. Each output element is the dot product of one row of $A$ and one column of $B$:

$$
C_{ij} = \sum_{k=0}^{K-1} A_{ik}B_{kj}
$$

For example,

$$
\begin{bmatrix}1&2\\3&4\end{bmatrix}
\begin{bmatrix}5&6\\7&8\end{bmatrix}
=
\begin{bmatrix}19&22\\43&50\end{bmatrix}
$$

The project baseline uses $(C=A\times W)$, where $A$ is the input/activation matrix and $W$ is the weight matrix. Other sources may call the second matrix $B$. Choose one notation and use it consistently.

**How does computational work grow with matrix dimensions?**

For conventional multiplication of an $(M\times K)$ matrix by a $(K\times N)$ matrix:

- Multiplications: $(MNK)$.
- Additions: $(MN(K-1))$, assuming each dot product starts with its first product and adds the remaining products.
- Total arithmetic operations under this convention: $(MN(2K-1))$.

For square $(N\times N)$ matrices, the conventional operation count grows approximately as $(2N^3)$. This helps explain why large matrix products can be demanding. Operation count alone does not determine execution time; implementation, parallelism, memory traffic, numerical precision, and processor architecture also matter.

A 4×4 product requires 64 multiplications and 48 additions, or 112 arithmetic operations by this counting convention. This is not the number of clock cycles required by an implementation.

**Why are data reuse and accumulation important?**

Matrix multiplication reuses values. Each element of $(A)$ contributes to multiple output columns, while each element of $(W)$ contributes to multiple output rows. Retaining values near the processing units can reduce repeated transfers from distant memory.

Each output also requires a sum of products. Hardware must calculate products with correct signedness and width, combine partial sums correctly, and ensure that each sum belongs to the right output. Accumulator width, initialization, overflow behavior, data alignment, and timing are part of the arithmetic contract.

#### B. Real-world applications to investigate

- **AI/ML:** Dense neural-network layers multiply activations by weight matrices. Matrix multiplication also appears in transformer projections and attention. Convolutions can sometimes be transformed into matrix multiplication, but not every implementation uses that transformation.  
    
- **Image processing and computer vision:** Feature extraction, transformations, and neural-network vision pipelines use matrix operations. A small Sobel edge detector is normally a local filter; it should not be described as necessarily requiring a large matrix-multiplication engine.  
  
- **Scientific and engineering computing:** Dense linear algebra appears in numerical simulation, transformations, and solving or analyzing systems of equations.  
  
- **Signal processing and communications:** Matrix operations appear in MIMO communications, beamforming, channel estimation, and signal transformations. Some workloads are complex-valued or structured.  
  
- **Robotics, graphics, and sensor processing:** Coordinate transformations, state estimation, and some sensor-fusion algorithms use matrix or matrix-vector operations. These may involve small matrices, where accelerator overhead matters.

Research questions:

1. Which workloads perform matrix multiplication frequently or at large scale?
2. What are their matrix dimensions, precision, latency requirements, and data-reuse patterns?
3. Which examples are directly relevant to our INT8 FPGA accelerator?
4. When might a CPU or GPU be more suitable than a dedicated FPGA accelerator?

#### C. Why might acceleration help, and when might it not?

A dedicated accelerator can help when a workload has sufficient parallelism, repeats often enough to amortize setup costs, and maps efficiently to the hardware. Specialization may improve throughput, latency, energy per operation, or predictability for a particular workload.

A CPU is not inherently inefficient: modern CPUs have caches, vector instructions, and optimized matrix libraries. For small matrices, irregular workloads, or infrequent operations, software and transfer overhead can exceed the time saved by specialized hardware.

For this project, arithmetic performance alone will not establish end-to-end benefit. A complete transaction may include software setup, input transfers, accelerator execution, output transfers, and result checking. Measure compute latency separately from end-to-end latency.

#### D. Performance terms to understand

- **Latency:** Time from a clearly defined start event to a clearly defined completion event.
- **Throughput:** Completed work per unit time, such as matrices/second or operations/second.
- **Memory bandwidth:** Rate at which data can be transferred.
- **Data reuse:** Reusing a value rather than transferring or recomputing it unnecessarily.
- **Power:** Rate of energy consumption, measured in watts.
- **Energy per task:** Energy consumed for a defined workload, measured in joules.
- **Resource utilization:** How much of the available FPGA fabric or compute capacity is used.
- **Speedup:** Baseline execution time divided by accelerated execution time, using equivalent workloads and timing boundaries.

A conceptual timing model is

$$
T_{\mathrm{total}} =
T_{\mathrm{setup}} + T_{\mathrm{input}} +
T_{\mathrm{compute}} + T_{\mathrm{output}} +
T_{\mathrm{software}}
$$

This is an accounting model, not a guarantee that all stages are sequential; some may overlap. Final metrics must state exactly what is included.

#### E. Starting evidence

**[R1] Jouppi et al., “In-Datacenter Performance Analysis of a Tensor Processing Unit” (ISCA 2017).** The authors evaluate a custom ASIC for neural-network inference using production MLP, CNN, and LSTM workloads. They report results relative to contemporary CPU and GPU baselines and discuss utilization and memory-system effects. Results are specific to their hardware, software, workloads, and experiment; they do not predict our FPGA results.

- Paper: https://arxiv.org/abs/1704.04760
- ACM publication: https://doi.org/10.1145/3140659.3080246

**[R2] Huang et al., “MALMM: A Multi-array Architecture for Large-scale Matrix Multiplication on FPGA” (IEICE Electronics Express, 2018).** The paper studies FPGA acceleration of large-scale floating-point matrix multiplication and proposes a configurable multi-array architecture, workload partitioning, and an analytical model.

- Free article: https://www.jstage.jst.go.jp/article/elex/15/10/15_20180286/_article/-char/en
- DOI: https://doi.org/10.1587/elex.15.20180286

**Initial takeaway [OUR ANALYSIS]:** Matrix multiplication is a relevant target because it appears in important workloads and offers opportunities for parallel computation and data reuse. Whether our particular accelerator is effective must be demonstrated through fair baselines and measured compute and end-to-end performance.

---

### Question 2 — What is a hardware accelerator, what types exist, and what makes one useful?

Investigate:
- Differences between CPUs, GPUs, FPGA-based accelerators, and fixed-function ASICs.
- General-purpose computing versus domain-specific hardware.
- How parallelism, specialization, reuse, memory bandwidth, and workload regularity affect performance.
- Peak throughput versus sustained application performance.
- Trade-offs in programmability, development time, flexibility, power, cost, and performance.
- What evidence demonstrates usefulness for a particular workload.

Starting sources:
1. Jouppi et al., TPU paper (2017): https://arxiv.org/abs/1704.04760
2. NVIDIA Deep Learning Accelerator (NVDLA): https://nvdla.org/
3. NVDLA hardware architecture: https://nvdla.org/hw/v1/hwarch.html
4. AMD Zynq-7000 technical reference manual: https://docs.amd.com/r/en-US/ug585-zynq-7000-SoC-TRM

Project questions:

- Which work should run in programmable logic?
- Which tasks should remain on the processor?
- What is the appropriate CPU baseline: a simple implementation, an optimized library, or both?
- Which metrics can realistically be measured on our board?

---

### Question 3 — What is a systolic array, why consider it, and what alternatives exist?

A systolic array is a spatial organization of processing elements (PEs) that perform repeated computations while data moves between elements according to a schedule. Exact data movement, PE operation, and timing depend on the architecture.

Investigate:

- What is a PE, and what arithmetic does it perform?
- How do pipelining, spatial parallelism, and local communication work?
- How does a systolic array differ from a generic array of MAC units?
- What are broadcast-based, SIMD/vector, multi-array, and other matrix-multiplication architectures?
- How do designs trade off reuse, interconnect complexity, storage, latency, throughput, and scalability?
- Which comparisons are fair for the same workload and resource budget?

Starting sources:

1. H. T. Kung, “Why Systolic Architectures?” (1982). Verify the exact bibliographic details before citing.

   - Author publication page: https://www.eecs.harvard.edu/htk/publications/
2. Asgari, Hadidi, and Kim, “MEISSA: Multiplying Matrices Efficiently in a Scalable Systolic Architecture” (ICCD 2020). The work proposes a stationary systolic architecture that separates multipliers from adders and evaluates it on a Zynq XC7Z020.
   - Author-hosted paper: https://www.cs.umd.edu/~bahar/papers/asgari-iccd20.pdf
   - DOI: https://doi.org/10.1109/ICCD50377.2020.00036
3. Huang et al., MALMM (2018): https://www.jstage.jst.go.jp/article/elex/15/10/15_20180286/_article/-char/en

Project questions:
- Is a conventional MAC systolic array the right baseline?
- Which alternatives are feasible on our FPGA and timeline?
- How should latency, throughput, resources, and verification complexity be compared?
- What experiment would justify an architecture choice?

Do not conclude that a systolic array is best merely because it appears in the project title. Research should justify the chosen architecture or document why it is used as a learning baseline.

---

### Question 4 — What is dataflow, what does weight-stationary mean, and why choose it?

Dataflow describes how operands and partial results are stored, reused, and moved. Common labels include weight-stationary, output-stationary, input-stationary, and row-stationary; exact definitions depend on the architecture and mapping.

Investigate:

- Which values stay local in each dataflow?
- Which values move between PEs, buffers, and memory?
- Where are partial sums accumulated?
- How do dataflow choices affect storage, communication, reuse, latency, and energy?
- How does dataflow differ from physical PE layout?
- How do signed values, accumulator width, and valid timing affect implementation?

Starting sources:

1. Eyeriss / row-stationary research: https://eyeriss.mit.edu/ (identify and cite the exact paper used).
2. MEISSA (2020): https://www.cs.umd.edu/~bahar/papers/asgari-iccd20.pdf
3. TPU paper (2017): https://arxiv.org/abs/1704.04760

Project questions:
- What should each PE store, and when is it loaded?
- How are activations delivered and partial sums produced?
- What mapping from matrix indices to PE coordinates is intended?
- What is the cycle-by-cycle schedule?
- What benefits are expected, and how will they be measured?

**Open decision:** PE mapping, operand directions, weight-loading sequence, valid alignment, and cycle schedule must be specified and verified before claiming a fixed compute latency.

---

### Question 5 — What did the previous batch implement, and what can our project improve or extend?

Treat the previous batch's presentation and repository as project-specific evidence. Separate reported results from details confirmed by RTL, simulation logs, implementation reports, and measurements.

Investigate:

- Matrix dimensions and parameterization.
- Numeric representation, signedness, product width, and accumulator width.
- PE organization and dataflow.
- Input/weight loading, start/completion control, output collection, and host communication.
- Verification methods and coverage.
- FPGA target, clock, resource use, power estimates, and latency measurements.
- Reported limitations, future work, and assumptions behind results.

Sources:

- Previous batch final presentation: use the project PDF and cite the exact slide/page.
- Current repository: https://github.com/yadavabhisheka/systolic-array-matrix-multiplier/
- ZedBoard documentation: https://digilent.com/reference/programmable-logic/zedboard/start

**Potential direction [PROPOSED]:** The current direction adds a PS–PL system with AXI-based control/data movement, DMA, DDR buffers, a C application, a C++ golden model, and structured RTL verification. These are proposed system-level extensions, not automatically novel contributions or guaranteed performance improvements.

Project questions:

- Which features will be reused, redesigned, or excluded?
- What evidence supports each claimed limitation?
- What does AXI/DMA integration improve compared with the previous host interface?
- What overhead does it introduce?
- What can be demonstrated within the project timeline?

---

### Question 6 — Why use an FPGA and Zynq-7000 SoC, and what roles should PS and PL perform?

The Zynq-7000 combines a processing system (PS), including ARM Cortex-A9 processors, with programmable logic (PL). This supports hardware/software co-design: software manages application and system control while custom hardware performs selected operations.

Investigate:

- PS/PL responsibilities and communication.
- Available logic, DSP, BRAM, clocks, memory, and board constraints.
- How the PS accesses DDR and PL peripherals.
- What bare-metal software must initialize and manage.
- Reset, clocking, interrupts or polling, and errors.
- Simulation and physical-board validation.

Starting sources:

1. AMD Zynq-7000 Technical Reference Manual (UG585): https://docs.amd.com/r/en-US/ug585-zynq-7000-SoC-TRM
2. Digilent ZedBoard reference: https://digilent.com/reference/programmable-logic/zedboard/start
3. AMD Vivado synthesis guide (UG901): https://docs.amd.com/r/en-US/ug901-vivado-synthesis

Project questions:

- Which tasks belong in C on the PS, and which belong in RTL on the PL?
- How are control/status registers defined?
- Which resources remain available after system integration?
- What board-level demonstration proves end-to-end operation?

---

### Question 7 — How will A and W move through DDR, DMA, AXI, and the accelerator?

The intended system-level path is:

1. The PS application prepares input matrices and expected results.
2. A and W are placed in DDR buffers.
3. The PS configures the accelerator and DMA.
4. AXI DMA MM2S reads data from memory and emits AXI4-Stream beats.
5. PL logic interprets the stream and routes values to weight/input handling.
6. The accelerator computes the result.
7. Output logic sends results over AXI4-Stream.
8. DMA S2MM writes results to a DDR output buffer.
9. The PS synchronizes cache state as required, reads the output, and compares it with the expected result.

This is the intended model, not a frozen interface specification. Buffer layout, transfer sequence, packing, TLAST behavior, and load/compute schedule remain to be defined.

Investigate:

- AXI4-Lite control versus AXI memory-mapped access versus AXI4-Stream.
- MM2S (memory-mapped to stream) and S2MM (stream to memory-mapped).
- TVALID/TREADY handshake and backpressure.
- TLAST, TKEEP, transfer lengths, alignment, and stream packing.
- How the PL distinguishes weights from activations.
- Whether A and W use separate transfers or a combined stream.
- DDR allocation, cache synchronization, completion, timeout, and error handling.

Starting sources:

1. AMD AXI DMA Product Guide (PG021): https://docs.amd.com/r/en-US/pg021_axi_dma
2. AMD AXI Reference Guide (UG1037): https://docs.amd.com/r/en-US/ug1037-vivado-axi-reference-guide
3. Zynq-7000 Technical Reference Manual (UG585): https://docs.amd.com/r/en-US/ug585-zynq-7000-SoC-TRM

Project questions:

- Are weights and activations transferred separately or in one stream?
- How is the end of each matrix identified?
- What happens when TREADY stays low for multiple cycles?
- How are DMA completion and accelerator completion distinguished?
- How does the CPU avoid reading stale output data?

---

### Question 8 — What numerical and functional contract must the accelerator satisfy?

**Current baseline [BASELINE — confirm with team]**

- Matrix dimensions: 4×4.
- Inputs and weights: signed INT8.
- Product width: INT16.
- Accumulation/output width: INT32.
- Operation: $(C=A\times W)$.

Investigate:

- Signed multiplication and sign extension.
- Accumulator initialization and clearing.
- Maximum possible dot-product magnitude.
- Overflow behavior: wraparound, saturation, or a defined wider result.
- Reset behavior, including reset during operation.
- Valid/data alignment and output ordering.
- Zero matrices, negative values, boundary values, and repeated operations.
- How the golden model matches the RTL arithmetic contract.

Project questions:

- Does INT32 represent every possible four-term signed INT8 dot product?
- What is the output ordering?
- What happens if a new operation is requested while the accelerator is busy?
- What constitutes a correct result and a completed transaction?

Starting sources:

- Accellera standards resources: https://www.accellera.org/downloads/standards
- AMD Vivado synthesis guide: https://docs.amd.com/r/en-US/ug901-vivado-synthesis

The SRS must make numerical assumptions explicit and testable.

---

### Question 9 — How will the design be verified and evaluated?

Verification layers to investigate:

- **Golden-model checking:** Compare outputs against an independent mathematical reference.
- **Directed tests:** Known cases including zero, positive, negative, boundary, and repeated-operation cases.
- **Assertions:** Check interface rules, state transitions, valid/data alignment, and protocol invariants.
- **Functional coverage:** Track whether meaningful cases and scenarios have been exercised.
- **Constrained-random/UVM:** Use transaction-based stimulus and reusable components if appropriate for the project and timeline.
- **Integration testing:** Test control registers, DMA, streams, memory buffers, and compute core together.
- **Board testing:** Confirm physical operation, correct results, completion, and error behavior.

Performance evaluation should measure, where tools and board access permit:

- Compute latency and clearly defined end-to-end latency.
- Throughput for repeated operations.
- Clock frequency and timing closure.
- LUT, FF, DSP, and BRAM use.
- Power estimates or measured power, clearly stating the method.
- CPU baseline execution time under equivalent conditions.
- Data-transfer time separately from compute time.

Starting sources:

1. AMD Vivado Logic Simulation Guide (UG900): https://docs.amd.com/r/en-US/ug900-vivado-logic-simulation
2. AMD Vivado Synthesis Guide (UG901): https://docs.amd.com/r/en-US/ug901-vivado-synthesis
3. Accellera UVM resources: https://www.accellera.org/downloads/standards/uvm

Project questions:

- What is independent of the RTL in the reference model?
- What constitutes functional completion?
- How will stalls, reset, invalid configuration, and errors be tested?
- What is a fair software baseline?
- Which results will be reproducible and recorded?

---

### Question 10 — What is the defensible research or engineering gap, and what contribution can this project make?

The gap should emerge from Week 2's comparison of papers, the previous batch, and target-platform constraints.

Potential directions to investigate (not novelty claims):

- Architecture/dataflow comparison for a defined workload.
- Measured effects of AXI/DMA data movement compared with a simpler host interface.
- Integration of a verified compute core into a PS–PL system.
- Reproducible comparison of compute latency and end-to-end latency.
- Verification of arithmetic, streaming interfaces, and system-level behavior.
- Resource/performance trade-offs on the chosen Zynq FPGA.

Week 2 questions:

1. What has published work already solved?
2. Which limitations are explicitly reported, and which can we reproduce?
3. Which limitations matter for our workload and board?
4. What exact change or comparison will this project contribute?
5. What metrics and experiments would demonstrate the contribution?
6. Is the contribution feasible in the available time?

Starting sources:

- TPU paper: https://arxiv.org/abs/1704.04760
- MALMM: https://www.jstage.jst.go.jp/article/elex/15/10/15_20180286/_article/-char/en
- MEISSA: https://www.cs.umd.edu/~bahar/papers/asgari-iccd20.pdf
- NVDLA architecture: https://nvdla.org/hw/v1/hwarch.html

---

## 2. Problem Statement — Working Draft

Matrix multiplication is a fundamental computation in applications such as machine learning, scientific computing, and signal processing. Its implementation requires repeated multiplication and accumulation, while performance can also depend on data reuse, memory movement, and the organization of computing hardware. An accelerator must therefore be evaluated not only for arithmetic capability but also for the cost of supplying inputs, controlling execution, and retrieving results.

This project investigates the design and implementation of a systolic matrix-multiplication accelerator on a Zynq-7000 FPGA platform. The current baseline is a 4×4 signed INT8 weight-stationary design with INT32 accumulation/output, integrated with a processor-side application and AXI-based control and data movement. The work will define arithmetic and interface contracts, implement and verify the compute architecture, integrate the system with DMA and DDR buffers, and evaluate correctness, latency, throughput, and FPGA resource use.

The final architecture and any claimed research or engineering contribution will be justified through a literature survey and comparison with existing approaches. Performance improvement over a CPU or previous implementation will be claimed only if equivalent workloads and measurement boundaries support the conclusion.

Questions to resolve:

- Is the problem primarily architecture, system integration, verification, or a defined combination?
- What specific limitation is supported by literature and previous-batch evidence?
- What measurable outcome demonstrates success?
- Does the final architecture remain fixed at 4×4, or will research justify a different scope?

---

## 3. Motivation — Working Draft

The project is motivated by the importance of matrix multiplication in modern computing and by the architectural opportunities created by its repeated arithmetic and data-reuse patterns. A spatial architecture can potentially execute multiple operations concurrently and organize data movement close to processing elements. Studying this on an FPGA provides an opportunity to understand the relationship between arithmetic units, storage, interconnect, scheduling, and performance.

A second motivation is hardware/software co-design. Integrating the accelerator with a Zynq processing system, AXI interfaces, DMA, and DDR buffers makes the project a complete system exercise rather than an isolated RTL simulation. The processor can prepare data and configure execution while programmable logic performs the defined computation.

A third motivation is verification and measurement. An independent golden model, RTL simulation, assertions, coverage, integration tests, and board-level tests can establish correctness. Separating compute latency from end-to-end latency helps identify whether computation or data movement dominates the system.

Published architectures such as TPU, MALMM, and MEISSA show that accelerator organization, data reuse, memory systems, and workload characteristics are active design considerations. Their results motivate investigation but do not predetermine which architecture is best for this project's constraints.

Evidence to add after reading:

- A sourced example of matrix multiplication in an important workload.
- A sourced example of measured specialized-accelerator results.
- A sourced example of FPGA matrix-multiplication architecture and trade-offs.
- A clear explanation of how those findings relate to this project.

---

## 4. Objectives — Provisional and Measurable

Revise acceptance criteria after architecture and interface decisions are finalized.

1. **Define the computation:** Specify dimensions, signedness, operand and accumulator widths, output ordering, and reset behavior.
2. **Define the architecture:** Produce a PE/array design with an explicit dataflow mapping and cycle-level schedule.
3. **Implement the compute core:** Develop synthesizable RTL for the selected architecture.
4. **Establish correctness:** Create an independent reference model and verify arithmetic across directed and boundary cases.
5. **Verify control and interfaces:** Test the defined control protocol, stream handshakes, backpressure, completion, and error cases.
6. **Integrate the system:** Connect the compute core to the selected AXI control/data path, DMA, DDR buffers, and processor-side C application.
7. **Validate on hardware:** Demonstrate correct operation on the selected ZedBoard/Zynq platform if tools and board access permit.
8. **Evaluate performance:** Report compute latency, end-to-end latency, throughput, resource utilization, and timing using documented methods.
9. **Compare fairly:** Use a defined software baseline and compare prior results only where workloads and measurement boundaries are compatible.
10. **Document the contribution:** Explain what was built, what was learned, and what limitations remain, supported by literature and experiments.

Acceptance criteria still to define: target clock, maximum latency, minimum throughput, resource limits, verification expectations, and exact board demonstration.

---

## 5. Scope

### 5.1 Current baseline [confirm with team]

- Target platform: ZedBoard with Zynq-7000 XC7Z020.
- Initial compute size: 4×4 matrix multiplication.
- Dataflow candidate: weight-stationary systolic array.
- Input/weight precision: signed INT8.
- Product width: INT16.
- Accumulator/output width: INT32.
- Software reference: C++ golden model.
- Processor-side application: bare-metal C.
- Planned integration: AXI4-Lite control, AXI DMA, AXI4-Stream, and DDR buffers.
- Verification: RTL simulation, directed tests, assertions, functional coverage, and UVM where appropriate.

These are the current baseline, not proof that every design detail is frozen.

### 5.2 Included in the intended project

- Research and comparison of relevant architectures.
- Arithmetic and interface specification.
- PE and systolic-array design.
- Control and data scheduling.
- Independent golden model.
- RTL verification and regression.
- AXI-based system integration.
- DMA/DDR movement and processor-side control.
- FPGA synthesis, implementation, and board validation.
- Performance/resource reporting and documentation.

### 5.3 Excluded from the initial baseline unless justified by research and schedule

- Scaling to 8×8 or larger arrays.
- Folded/tiled designs for arbitrary matrix sizes.
- Linux driver development.
- Scatter-gather DMA.
- Multiple independent DMA streams.
- Clock-domain crossing between unrelated clocks.
- Advanced physical-design optimization or custom ASIC implementation.
- Claims of production-level performance or superiority over commercial accelerators.

### 5.4 Scope boundaries and open decisions

- Exact PE-to-matrix index mapping.
- Activation and partial-sum movement directions.
- Weight-loading mechanism and priority relative to compute.
- Cycle schedule and first/last valid cycles.
- Input/output stream packing and TLAST/TKEEP conventions.
- Separate MM2S transfers versus a combined stream for A and W.
- AXI register map, error flags, timeout, and reset recovery.
- Performance acceptance thresholds.

Resolve these through architecture analysis and record them in the SRS and low-level design.

---

## 6. Expected Outcomes

1. A literature-backed project definition and justified architecture choice.
2. A reviewed SRS with uniquely identified, testable requirements.
3. HLD and LLD documents, including block diagrams and interface contracts.
4. A working RTL implementation of the selected compute architecture.
5. An independent golden model and repeatable verification environment.
6. Verified control/data interfaces, including defined behavior under backpressure.
7. A processor/FPGA implementation using the selected AXI, DMA, and DDR architecture.
8. A board-level demonstration if implementation and hardware access permit.
9. Recorded correctness, latency, throughput, resource, and timing results; power/energy results if a credible method is available.
10. A final discussion of limitations, comparison methodology, and future work.

These are intended outcomes, not claims of achieved results. Numerical performance targets should be set after resource and architecture analysis.

---

## 7. References and Evidence

For every source, record the claim it supports, the relevant section/page/figure, and what it does not prove.

### 7.1 Core research papers

**[R1] Jouppi et al., “In-Datacenter Performance Analysis of a Tensor Processing Unit,” ISCA 2017.**
- Paper: https://arxiv.org/abs/1704.04760
- DOI: https://doi.org/10.1145/3140659.3080246
- Use for: domain-specific acceleration, neural-network inference, workload characterization, CPU/GPU comparison, throughput/energy, utilization and memory-system considerations.
- Caution: results apply to the studied hardware, baselines, software, and workloads; do not project the reported speedup onto our FPGA.

**[R2] Huang et al., “MALMM: A Multi-array Architecture for Large-scale Matrix Multiplication on FPGA,” IEICE Electronics Express, 2018.**
- Article: https://www.jstage.jst.go.jp/article/elex/15/10/15_20180286/_article/-char/en
- DOI: https://doi.org/10.1587/elex.15.20180286
- Use for: FPGA architecture, multi-array scaling, workload partitioning, and analytical design choices.
- Caution: compare data type, array configuration, workload, and evaluation setup before comparing results.

**[R3] Asgari, Hadidi, and Kim, “MEISSA: Multiplying Matrices Efficiently in a Scalable Systolic Architecture,” IEEE ICCD 2020.**
- Author-hosted PDF: https://www.cs.umd.edu/~bahar/papers/asgari-iccd20.pdf
- DOI: https://doi.org/10.1109/ICCD50377.2020.00036
- Use for: stationary systolic architecture, separating multipliers from adders, latency/scalability, and a Zynq XC7Z020 evaluation.
- Caution: verify architecture, numerical format, workload, baselines, and measurements before drawing comparisons.

**[R4] Kung, “Why Systolic Architectures?” (1982).**
- Author publication page: https://www.eecs.harvard.edu/htk/publications/
- Use for: foundational concepts and motivation.
- Action: verify full bibliographic details from an accessible copy or academic library.

**[R5] Eyeriss / row-stationary dataflow research.**
- Project information: https://eyeriss.mit.edu/
- Use for: data reuse, dataflow trade-offs, and energy-aware accelerator organization.
- Action: identify and cite the exact paper/version; the project page alone is not a substitute for a paper citation.

### 7.2 Official technical documentation

**[D1] AMD Zynq-7000 SoC Technical Reference Manual (UG585):**  
https://docs.amd.com/r/en-US/ug585-zynq-7000-SoC-TRM  
Use for PS, DDR, interconnect, and Zynq behavior.

**[D2] AMD AXI DMA Product Guide (PG021):**  
https://docs.amd.com/r/en-US/pg021_axi_dma  
Use for MM2S/S2MM configuration, transfers, and integration.

**[D3] AMD Vivado AXI Reference Guide (UG1037):**  
https://docs.amd.com/r/en-US/ug1037-vivado-axi-reference-guide  
Use for AXI interfaces and protocol concepts.

**[D4] AMD Vivado Logic Simulation Guide (UG900):**  
https://docs.amd.com/r/en-US/ug900-vivado-logic-simulation  
Use for simulation workflow.

**[D5] AMD Vivado Synthesis Guide (UG901):**  
https://docs.amd.com/r/en-US/ug901-vivado-synthesis  
Use for RTL synthesis considerations.

**[D6] Digilent ZedBoard documentation:**  
https://digilent.com/reference/programmable-logic/zedboard/start  
Use for board features and constraints.

**[D7] Accellera UVM resources:**  
https://www.accellera.org/downloads/standards/uvm  
Use for UVM standard and verification resources.

### 7.3 Architecture and industry reference

**[A1] NVIDIA Deep Learning Accelerator (NVDLA):**
- https://nvdla.org/
- Architecture: https://nvdla.org/hw/v1/hwarch.html
- Use for modular accelerator architecture and an open hardware reference.
- Caution: not a direct performance baseline for our small FPGA design.

### 7.4 Project-specific evidence

- Current repository: https://github.com/yadavabhisheka/systolic-array-matrix-multiplier/
- Previous batch final presentation: cite the exact PDF filename and slide/page in local documentation.
- Future evidence: RTL commits, simulation logs, regression reports, synthesis/timing reports, board test logs, and benchmark scripts.

---

## 8. Decisions Made and Open Questions

### 8.1 Current baseline decisions

- [BASELINE] Initial matrix size: 4×4.
- [BASELINE] Inputs and weights: signed INT8.
- [BASELINE] Product width: INT16; accumulation/output width: INT32.
- [BASELINE] Current dataflow candidate: weight-stationary.
- [BASELINE] Target board: ZedBoard / Zynq-7000 XC7Z020.
- [BASELINE] Processor-side C application and C++ golden model.
- [PROPOSED] AXI4-Lite control plus AXI DMA/AXI4-Stream bulk data movement to/from DDR.
- [OPEN QUESTION] Exact array mapping and cycle schedule are not frozen.

Confirm the baseline with the repository and team before treating it as immutable.

### 8.2 Open questions for Week 2

- Which architecture candidates are comparable?
- Is conventional MAC systolic sufficient, or should MEISSA-style architecture be evaluated?
- What precise limitation forms the gap?
- What is the mapping of A, W, and C to PE coordinates?
- How are weights loaded and retained?
- What is the activation/partial-sum schedule?
- How are input/output values packed on AXI4-Stream?
- Should A and W use separate DMA transfers?
- What are the control/status registers and error cases?
- What performance targets are feasible?
- What will be compared with the previous batch, under equivalent conditions?

---

## 9. Week 1 Completion Checklist

- [ ] Explain matrix multiplication and its operation count.
- [ ] Explain why data reuse and accumulation matter.
- [ ] Identify real applications and distinguish large matrix multiplication from related operations.
- [ ] Read the abstract and introduction of the TPU paper.
- [ ] Read the abstract and introduction of MALMM.
- [ ] Record claims, evidence, limitations, and relevance for each source.
- [ ] Inspect the previous batch presentation and repository.
- [ ] Understand PS, PL, AXI, DMA, and DDR conceptually.
- [ ] Draft the problem statement in your own words.
- [ ] Draft motivation, objectives, scope, and expected outcomes.
- [ ] Separate baseline decisions from proposals and open questions.
- [ ] List questions requiring detailed analysis in Week 2.

---

## 10. Week 2 and Week 3 Handoff

### Week 2 — Literature survey and architecture selection

Use this document as the starting point for a structured paper comparison. Analyze each paper's problem, architecture, dataflow, numerical format, evaluation setup, results, limitations, and applicability. Compare candidate architectures and justify a contribution without claiming novelty prematurely.

### Week 3 — SRS

Convert the selected design into uniquely identified, testable requirements. Include functional behavior, numerical behavior, control/status registers, AXI protocol requirements, data formats, reset/error behavior, performance targets, resource constraints, verification requirements, and acceptance tests.

### Week 4 — HLD and LLD

Define system blocks and interfaces in the HLD. Specify PE behavior, array mapping, cycle-level schedule, buffering, controller behavior, reset, valid alignment, and error handling in the LLD. Review both against the SRS before full implementation.

---

## Final note

This file is an evolving engineering record. Keep source evidence and decisions visible, revise assumptions when research contradicts them, and never present proposed results as measured facts. The goal of Week 1 is a clear and defensible problem definition—not premature certainty about the final architecture.
