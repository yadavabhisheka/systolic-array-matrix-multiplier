# FPGA-Based Matrix Multiplication Accelerator

## Preliminary Project Definition --- Before Literature Survey

**Project platform:** ZedBoard based on the AMD/Xilinx Zynq-7000
Zynq-7020 SoC\
**Project stage:** Problem definition and literature survey\
**Architecture status:** Open --- to be selected after literature review
and feasibility analysis

> **Important design decision:** At this stage, the project does not
> commit to any array size, a particular dataflow, a numeric precision, or a
> specific processing-element organization. The only fixed
> implementation constraint is the target FPGA board. These choices will
> be justified from the literature survey, resource constraints,
> verification effort, and measurable project goals.

------------------------------------------------------------------------

## 1. Project Overview

The project aims to **design, verify, and implement a matrix-multiplication
accelerator on the ZedBoard Zynq-7020 FPGA platform**. The accelerator's
precise operation, architecture, dataflow, numerical format, scale, and
interfaces will be selected after a structured literature survey.

Matrix multiplication is a useful starting point for investigating
hardware acceleration because it is a core operation in machine
learning, image processing, signal processing, and scientific computing.
However, the project should not assume in advance that a particular
matrix-multiplication architecture is necessarily the best choice. The
literature survey must compare established approaches and determine a
design that is technically meaningful and feasible on the available
FPGA.

The intended workflow is:

1.  Define the computational problem and use cases.
2.  Review academic architectures and deployed accelerator products.
3.  Compare architectures, dataflows, numerical formats, memory systems,
    and interface choices.
4.  Select a feasible design and specify measurable requirements.
5.  Develop an independent software golden model.
6.  Design and verify the RTL.
7.  Integrate the accelerator with the Zynq processing system as
    appropriate.
8.  Implement the design on the FPGA and report measured results.

## 2. Fundamental Problem Statement

Many modern workloads require large numbers of arithmetic operations
over structured data. Matrix multiplication is one important example:

$$
C = A \times B
$$

For compatible matrices, each output element is computed as

$$
C_{i,j} = \sum_{k=0}^{K-1} A_{i,k} B_{k,j}.
$$

A direct implementation performs repeated multiplication and
accumulation operations. As matrix sizes and workload volumes increase,
the design must manage not only arithmetic throughput but also the
movement, storage, reuse, and scheduling of operands and partial
results.

A general-purpose processor can execute these operations in software,
while GPUs, AI accelerators, and custom hardware can exploit parallel
arithmetic. Dedicated hardware is not automatically faster or more
efficient in every situation: performance depends on workload
dimensions, data types, memory bandwidth, data reuse, control overhead,
and the cost of transferring data to and from the accelerator.

**The fundamental problem for this project is to determine and implement
a suitable FPGA-based architecture for accelerating a well-defined
matrix-computation workload, while balancing functional correctness,
computational performance, memory/data movement, FPGA resource use, and
integration complexity.**

The problem has two connected parts:

-   **Computational architecture:** How should arithmetic units,
    storage, interconnect, scheduling, and data reuse be organized for
    the selected workload?
-   **System integration and verification:** How can the accelerator be
    configured, supplied with input data, checked against a trusted
    reference, and evaluated after FPGA implementation?

The architecture is deliberately not fixed yet. The literature survey
and resource feasibility analysis will determine the final scope.

## 3. Where Matrix Computation Is Used

### 3.1 Artificial intelligence and machine learning

Matrix multiplication and general matrix-matrix multiplication (GEMM)
are fundamental to fully connected neural-network layers, attention and
transformer workloads, and many training and inference operations.
Convolution operations can also be transformed or mapped to matrix-like
computations.

For example, NVIDIA's documentation describes GEMM as a fundamental
building block of fully connected, recurrent, and convolutional
neural-network operations. The exact mapping and efficiency depend on
the layer and implementation.\
Source: [NVIDIA --- Matrix Multiplication Background User's
Guide](https://docs.nvidia.com/deeplearning/performance/dl-performance-matrix-multiplication/index.html)

### 3.2 Image processing and computer vision

Image-processing pipelines use matrix and vector operations for
transformations, filtering, feature extraction, and computer-vision
algorithms. Some tasks are better expressed as convolutions or stencil
operations rather than general matrix multiplication, so the target
application must be selected carefully rather than assuming every
image-processing task maps directly to GEMM.

### 3.3 Digital signal processing and communications

Linear transforms, filtering, beamforming, and parts of
wireless/baseband processing involve repeated multiply-accumulate
operations and matrix/vector calculations. Latency, predictable
scheduling, numeric precision, and sustained data rates may be important
design requirements.

### 3.4 Scientific and engineering computation

Linear algebra is used in numerical simulation, solving systems of
equations, transformations, and data analysis. Large scientific
workloads often use optimized CPU or GPU libraries; a small FPGA
accelerator is more appropriately evaluated as a bounded design
experiment unless a specific application and performance target are
defined.

### 3.5 Embedded and edge systems

An FPGA-based accelerator may be useful when a workload needs
configurable hardware, deterministic interfaces, or integration with
other programmable logic. The Zynq-7000 family combines an Arm-based
processing system with programmable logic, enabling software and custom
hardware to work together. This makes the ZedBoard a suitable platform
for an accelerator-design project, although its resource limits differ
substantially from those of datacenter accelerators.\
Sources: - [AMD --- Zynq-7000 SoC
overview](https://www.amd.com/en/products/adaptive-socs-and-fpgas/soc/zynq-7000.html) -
[AMD --- Zynq-7000 Technical Reference
Manual](https://docs.amd.com/r/en-US/ug585-zynq-7000-SoC-TRM/Introduction)

------------------------------------------------------------------------

## 4. Existing Accelerator Examples and Their Architectural Approaches

These examples show that matrix computation is accelerated in several
different ways. They are **reference points for the literature survey,
not designs that must be copied**. Product-level architecture
descriptions are often incomplete, and some implementation details are
proprietary. The descriptions below therefore distinguish publicly
documented architectural facts from assumptions that would require
further research.

### 4.1 Google Tensor Processing Unit (TPU)

**Use:** Machine-learning training and inference in Google's TPU
ecosystem.

**Documented architectural approach:** Google TPU generations use
TensorCores containing matrix-multiplication units (MXUs), together with
vector and scalar units. TPU systems also use high-bandwidth memory and
inter-chip interconnects to support large workloads.

**What to learn from it:** - Dedicated matrix engines are integrated
with memory and other compute units. - A high-throughput matrix unit is
only one part of a complete accelerator system. - Data movement and
scaling across multiple chips matter in addition to the arithmetic
datapath.

**Important distinction:** A TPU is a commercial accelerator system, not
a small FPGA design. Its compute capacity and memory system cannot be
reproduced directly on a Zynq-7020. The useful comparison is
architectural: compute organization, data reuse, memory hierarchy, and
system integration.

Sources: - [Google Cloud --- TPU v4 architecture and
configurations](https://docs.cloud.google.com/tpu/docs/v4?hl=en)

### 4.2 NVIDIA GPUs with Tensor Cores

**Use:** Deep-learning training and inference, as well as supported
matrix operations in scientific and high-performance computing.

**Documented architectural approach:** NVIDIA GPUs combine
general-purpose GPU execution resources with specialized Tensor Cores
that accelerate matrix multiply-accumulate operations. Software
libraries map workloads to matrix tiles and select suitable
implementations based on dimensions, data types, hardware generation,
and other constraints.

**What to learn from it:** - Tiling and data reuse are central to
efficient matrix multiplication. - Specialized matrix units coexist with
general-purpose execution resources. - Numeric formats and accumulation
precision influence both performance and correctness. - Matrix
dimensions, memory traffic, and library/runtime mapping affect real
performance.

**Important distinction:** Tensor Cores should not simply be labelled a
classic systolic array without a generation-specific source establishing
that claim. For this project, treat Tensor Cores as specialized matrix
engines and study their documented programming and execution model
separately from explicitly systolic architectures.

Sources: - [NVIDIA --- Matrix Multiplication Background User's
Guide](https://docs.nvidia.com/deeplearning/performance/dl-performance-matrix-multiplication/index.html) -
[NVIDIA --- GPU Performance
Background](https://docs.nvidia.com/deeplearning/performance/dl-performance-gpu-background/index.html)

### 4.3 Intel Gaudi AI accelerators

**Use:** AI training and inference, including neural-network operations
that can be lowered to matrix multiplication.

**Documented architectural approach:** Intel Gaudi uses a heterogeneous
compute architecture with a Matrix Multiplication Engine (MME) and
programmable Tensor Processor Core (TPC) resources. The MME handles
matrix-multiplication-oriented operations such as fully connected
layers, convolutions, and batched GEMM, while the TPCs handle other
programmable operations. The system also includes memory and networking
subsystems.

**What to learn from it:** - A practical accelerator is often
heterogeneous rather than one arithmetic array alone. - Different
compute engines can be assigned to different classes of operation. -
Memory bandwidth and communication are part of the architecture, not
afterthoughts.

**Important distinction:** The publicly documented MME is a matrix
engine; do not assume its internal implementation is a conventional
systolic array unless a primary source confirms that detail.

Sources: - [Intel Gaudi architecture
documentation](https://docs.habana.ai/en/latest/Gaudi_Overview/Gaudi_Architecture.html) -
[Intel --- Gaudi 3 AI Accelerator white
paper](https://cdrdv2-public.intel.com/817486/gaudi-3-ai-accelerator-white-paper.pdf)

### 4.4 AMD/Xilinx Deep Learning Processor Unit (DPU)

**Use:** Convolutional-neural-network acceleration on supported
AMD/Xilinx FPGA and adaptive-SoC platforms.

**Documented architectural approach:** AMD describes the DPU as a
programmable engine dedicated to CNN workloads, supported by Vivado and
available for supported devices including Zynq-7000 and Zynq UltraScale+
families. The exact supported configuration and software flow depend on
the DPU version and target device.

**What to learn from it:** - A programmable-logic accelerator can be
integrated into a Zynq system. - Accelerator IP, software tools,
interface configuration, and device limitations must be considered
together. - A workload-specific accelerator may be more practical than a
completely general-purpose matrix engine.

**Important distinction:** The DPU is a broader CNN processor, not
simply a standalone GEMM core. It is relevant as a platform and
integration reference, but it may be too complex or restrictive to serve
as the direct design template for this project. Check the product guide
for the exact target device and tool version.

Sources: - [AMD --- DPU for Convolutional Neural
Network](https://www.amd.com/en/products/adaptive-socs-and-fpgas/intellectual-property/dpu.html) -
[AMD --- Zynq-7000 DPU configuration
guide](https://docs.amd.com/r/3.3-English/pg338-dpu/Customizing-and-Generating-the-Core-in-Zynq-7000-Devices)

### 4.5 Intel Advanced Matrix Extensions (AMX)

**Use:** Matrix-intensive AI operations executed on supported Intel Xeon
processor cores.

**Documented architectural approach:** AMX adds tile registers and a
Tile Matrix Multiplication engine (TMUL) to supported CPU cores. The
documented use cases include AI training and inference with supported
numeric formats such as BF16 and INT8.

**What to learn from it:** - Matrix acceleration can be integrated
inside a CPU rather than implemented as a separate accelerator card. -
Tile storage and the movement of data into and out of the matrix engine
are important parts of the programming model. - A specialized engine can
accelerate matrix work while the general-purpose CPU handles the
surrounding software.

Source: - [Intel --- What Is Intel Advanced Matrix Extensions
(AMX)?](https://www.intel.com/content/www/us/en/products/docs/accelerator-engines/what-is-intel-amx.html)

### 4.6 Summary comparison

  ----------------------------------------------------------------------------------
  Example           Main workload        Publicly documented     Main lesson for
                                         compute approach        this project
  ----------------- -------------------- ----------------------- -------------------
  Google TPU        Machine learning     TensorCores with        Study
                                         matrix-multiplication   compute--memory
                                         units, vector/scalar    balance and
                                         units, and system-level matrix-engine
                                         memory/interconnect     organization

  NVIDIA GPU Tensor AI and supported HPC Specialized matrix      Study tiling,
  Cores             matrix operations    multiply-accumulate     precision, data
                                         units within a GPU;     reuse, and software
                                         tiled software mapping  mapping

  AMD/Xilinx DPU    CNN acceleration     Programmable            Study FPGA
                                         workload-specific       integration, IP
                                         accelerator IP for      constraints, and
                                         supported FPGA/SoC      software/hardware
                                         platforms               partitioning  

  Intel AMX         CPU-based AI matrix  Tile registers plus a   Study tiled
                    operations           Tile Matrix             operands and a
                                         Multiplication engine   matrix engine
                                                                 integrated with a
                                                                 CPU    
                                                                        
Intel Gaudi       AI                   Matrix Multiplication   Study heterogeneous
                    training/inference   Engine plus             compute and system
                                         programmable Tensor     integration
                                         Processor Cores         

  
  ----------------------------------------------------------------------------------

These products establish that matrix acceleration is a real and widely
used engineering problem. They do **not** establish that all these
designs use the same dataflow or the same internal architecture.
Architecture-specific claims should be supported by primary
documentation or research papers in the literature survey.

------------------------------------------------------------------------

## 5. Candidate Architectures to Investigate

The final architecture will be selected after reviewing research papers
and evaluating feasibility on the target board. The following are
candidates for investigation, not project decisions.

### 5.1 Systolic array

Processing elements communicate with neighboring elements in a regular
pattern. Data advances through the array in a scheduled manner,
potentially enabling reuse and pipelined parallel computation.

**Questions to investigate:** - Which dataflow is used:
weight-stationary, output-stationary, input-stationary, or another
scheme? - What are the operand movement and storage requirements? - How
are matrices mapped to the array, especially when dimensions do not
match the array size? - What are the pipeline latency, utilization, and
resource costs?

### 5.2 Parallel or broadcast MAC array

Operands may be distributed or broadcast to multiple multiply-accumulate
units, with partial results accumulated in local or centralized
structures.

**Questions to investigate:** - How much fanout and routing does
broadcasting require? - Where are partial sums stored and combined? -
Does the design fit the available DSP, LUT, register, and memory
resources? - Is the architecture easier to verify or better suited to
the intended workload?

### 5.3 Tiled or time-multiplexed architecture

A smaller number of arithmetic units may be reused over multiple cycles
to compute a larger matrix operation. This can reduce hardware
requirements but may increase execution time and control complexity.

**Questions to investigate:** - How are tiles loaded, computed, and
written back? - What is the balance between parallelism and resource
utilization? - How much buffering is needed? - What are the scheduling
and control costs?

### 5.4 Hybrid or application-specific architecture

The final design may combine parallel arithmetic, local storage,
pipelining, and tiled scheduling. The best solution does not have to
reproduce one commercial accelerator architecture.

The literature survey should consider whether the project is best framed
as a matrix-multiplication engine, a small GEMM accelerator, or a more
specific application accelerator. A clearly defined workload and
measurable evaluation criteria are more important than choosing an
architecture name early.

------------------------------------------------------------------------

## 6. Motivation

The motivation is to explore how a computationally important operation
can be implemented as dedicated hardware and evaluated systematically on
an FPGA.

The project is motivated by:

1.  **Computational demand:** Matrix operations are common in AI, signal
    processing, computer vision, and scientific computing.
2.  **Hardware parallelism:** Custom datapaths can execute arithmetic
    operations concurrently.
3.  **Data movement:** Operand storage, reuse, buffering, and memory
    bandwidth can limit performance as much as arithmetic throughput.
4.  **Design trade-offs:** Different architectures trade hardware
    resources, latency, throughput, flexibility, and verification
    complexity.
5.  **FPGA suitability:** The Zynq-7020 provides programmable logic
    alongside an Arm-based processing system, enabling hardware/software
    integration experiments.
6.  **Engineering methodology:** The project provides experience in
    literature-driven architecture selection, software reference
    modeling, RTL design, verification, FPGA implementation, and
    measured evaluation.

The aim is not to claim that the project will outperform commercial
accelerators. The aim is to build a well-specified, verified, measurable
design and explain the trade-offs demonstrated by its implementation.

## 7. Project Objectives

The objectives are intentionally architecture-neutral at this stage.

1.  **Conduct a literature survey** of matrix-computation accelerators,
    including systolic arrays, broadcast or parallel MAC arrays, tiled
    designs, and relevant FPGA implementations.
2.  **Study deployed reference systems** to understand their compute
    engines, memory hierarchy, data movement, numeric formats, and
    hardware/software interfaces.
3.  **Define a target workload** with clear input/output behavior and
    use cases suitable for the available board.
4.  **Compare candidate architectures** using throughput, latency,
    resource use, data reuse, memory requirements, scalability,
    implementation effort, and verification complexity.
5.  **Select and justify the architecture** only after literature review
    and FPGA feasibility analysis.
6.  **Develop an independent C++ golden model** for the chosen
    mathematical operation and numerical semantics.
7.  **Design the RTL and control/data interfaces** appropriate to the
    selected architecture.
8.  **Verify functionality** using simulation and comparison against the
    golden model, including boundary conditions and numerical corner
    cases.
9.  **Implement the design on the ZedBoard Zynq-7020** using the
    selected FPGA tool flow.
10. **Measure and report results**, including available resource
    utilization and timing reports, and latency/throughput measurements
    where supported.
11. **Document limitations and future work** based on evidence from the
    implementation rather than assumed performance gains.

## 8. Project Scope

### 8.1 Fixed constraints

-   Target board: ZedBoard with Zynq-7020 SoC.
-   The project must include design, verification, and FPGA
    implementation.
-   A software golden model will be used to establish expected results.
-   The final architecture will be selected after literature survey and
    feasibility analysis.

### 8.2 Decisions intentionally left open

The following have **not** been fixed:

-   Exact computation/application beyond the current matrix-computation
    direction.
-   Array size or number of processing elements.
-   Systolic, broadcast, tiled, or hybrid architecture.
-   Dataflow, including weight-stationary or output-stationary.
-   Operand precision and signedness.
-   Accumulator width, output format, and overflow behavior.
-   Buffer organization and memory hierarchy.
-   Whether to use DSP blocks, LUT-based arithmetic, or a combination.
-   Interface details and whether DMA is required.
-   Clock target and performance goals.
-   Whether scaling to larger matrix sizes is in scope.

These choices should be settled by the literature survey, the target
workload, resource estimates, and the time available for implementation
and verification.

### 8.3 Expected work products

The project is expected to produce:

-   A literature survey and architecture comparison.
-   A documented problem definition and requirements specification.
-   A justified high-level and low-level architecture.
-   A C++ golden model.
-   RTL implementation and simulation testbench.
-   Verification results and a record of corner cases tested.
-   FPGA synthesis and implementation reports.
-   Performance/resource evaluation and final documentation.

### 8.4 Out of scope until justified

No commitment is made at this stage to: - Build a commercial-scale AI
accelerator. - Reproduce the internal architecture of a proprietary
commercial product. - Implement every candidate architecture. -
Guarantee a speedup over CPU software before measurements. - Include
advanced features such as fault tolerance, sparsity, quantization-aware
processing, or multi-chip scaling.

Such features can be considered later only if the literature survey and
project goals justify them.

------------------------------------------------------------------------

## 9. Literature Survey: Questions That Must Be Answered

The literature survey should not merely collect papers that use the
phrase "systolic array." It should compare architectures against the
same questions.

### Architecture and computation

-   What operation and workload does each design target?
-   Is it a systolic array, broadcast array, tiled MAC engine, vector
    engine, or hybrid?
-   How are processing elements connected and scheduled?
-   What dataflow is used, and why?
-   Is the design spatially parallel, time-multiplexed, or a
    combination?

### Numeric representation

-   Which data types are supported?
-   What are the multiplier and accumulator widths?
-   How are rounding, saturation, truncation, and overflow handled?
-   Is accuracy evaluated against a software reference?

### Memory and communication

-   Where are input operands, weights, partial sums, and outputs stored?
-   How are data reused?
-   What is the memory bandwidth requirement?
-   Are interfaces on-chip streaming interfaces, AXI memory-mapped
    interfaces, DMA, or another method?
-   Is data-transfer overhead included in reported latency?

### FPGA implementation

-   Which FPGA device was used?
-   How many LUTs, registers, DSPs, and block RAM resources were
    consumed?
-   What clock frequency and latency were reported?
-   Are measurements post-synthesis, post-implementation, or measured on
    hardware?
-   Can the design plausibly fit on the Zynq-7020?

### Evaluation quality

-   What is the baseline used for comparison?
-   Are the workloads and numeric formats comparable?
-   Is throughput reported as operations per second, operations per
    cycle, or another measure?
-   Does the paper include interface and end-to-end overhead?
-   Which claims are supported by measured results, and which are
    estimates?

### Selection criteria

The final design should be selected by a documented comparison, not by
the popularity of an architecture. A smaller, fully verified
implementation with clear measurements may be a stronger capstone than a
larger architecture that cannot be integrated or evaluated reliably.

------------------------------------------------------------------------

## 10. Preliminary Success Criteria

The final numerical targets should be set after the architecture and
workload are selected. At this stage, success means:

-   The selected problem and workload are clearly defined.
-   The architecture is supported by a literature-based rationale.
-   The design fits the available FPGA resources.
-   The C++ reference model and RTL agree for the defined test suite.
-   Reset, control, and data-transfer behavior are verified.
-   The design can be implemented on the target board.
-   Resource use and timing are documented from implementation reports.
-   Performance claims are supported by measurements with clearly stated
    conditions.
-   Limitations and possible improvements are reported honestly.

Do not set an arbitrary throughput, latency, or utilization target
before estimating what the selected design can achieve on the target
FPGA.

## 11. Preliminary Conclusion

This project will investigate the design and implementation of a
matrix-computation accelerator on the ZedBoard Zynq-7020 FPGA platform.
Matrix computation is used widely in AI, signal processing, vision, and
scientific computing, and deployed systems demonstrate multiple
approaches to accelerating it. These include specialized matrix engines,
GPU matrix units, CPU-integrated matrix engines, and programmable
FPGA-based accelerators.

The project will first study these approaches and relevant academic
designs, then choose a workload and architecture appropriate to the
board and project constraints. It will subsequently develop a software
reference model, implement and verify the hardware, integrate it with
the system as appropriate, and evaluate its resource use and
performance.

**The architecture is an outcome of the literature survey, not an
assumption made in the problem statement.**

------------------------------------------------------------------------

## References and Starting Points

1.  NVIDIA, *Matrix Multiplication Background User's Guide*:
    https://docs.nvidia.com/deeplearning/performance/dl-performance-matrix-multiplication/index.html
2.  NVIDIA, *GPU Performance Background*:
    https://docs.nvidia.com/deeplearning/performance/dl-performance-gpu-background/index.html
3.  Google Cloud, *TPU v4*:
    https://docs.cloud.google.com/tpu/docs/v4?hl=en
4.  Intel, *Gaudi Architecture*:
    https://docs.habana.ai/en/latest/Gaudi_Overview/Gaudi_Architecture.html
5.  Intel, *Gaudi 3 AI Accelerator White Paper*:
    https://cdrdv2-public.intel.com/817486/gaudi-3-ai-accelerator-white-paper.pdf
6.  AMD, *DPU for Convolutional Neural Network*:
    https://www.amd.com/en/products/adaptive-socs-and-fpgas/intellectual-property/dpu.html
7.  AMD, *Zynq-7000 DPU Configuration Guide*:
    https://docs.amd.com/r/3.3-English/pg338-dpu/Customizing-and-Generating-the-Core-in-Zynq-7000-Devices
8.  AMD, *Zynq-7000 SoCs*:
    https://www.amd.com/en/products/adaptive-socs-and-fpgas/soc/zynq-7000.html
9.  Intel, *What Is Intel Advanced Matrix Extensions (AMX)?*:
    https://www.intel.com/content/www/us/en/products/docs/accelerator-engines/what-is-intel-amx.html

*These are starting references for architecture context. The formal
literature survey should add peer-reviewed papers, FPGA implementation
papers with measured resource/timing results, and primary technical
documentation for each shortlisted architecture.*