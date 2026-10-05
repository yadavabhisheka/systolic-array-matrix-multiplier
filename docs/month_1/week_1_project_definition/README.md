# Week 1 — Project Definition

## Goal

- Define what is the project exactly and why making this?

## What I Need to Understand

- What is Systolic Matrix Multiplication Accelerator?
- Why we actually require a Accelerator?
- Why only Matrix Multiplication and why is this that important?
- What problem i am solving with this?
- What did the previous batch actually build, and what am I changing from their implementation?

## Problem Statement

- Making a Matrix Multiplication faster with the help of hardware accelrator?
- What limitation remains in the previous implementation that your project is trying to address?

## Motivation

## Objectives

## Scope

## Expected Outcome

## Questions / Doubts

### Computation

- What makes matrix multiplication computationally expensive?
- How does the number of MAC operations grow with matrix size?
- What type of parallelism exists in matrix multiplication?

### Systolic Architecture

- Why is a systolic array suitable for matrix multiplication?
- What data is reused?
- Why was weight-stationary selected?
- What exactly stays stationary and what moves?

### Previous Implementation

- What exactly did the previous batch implement?
- What was their complete data path?
- What role did UART play?
- What role did the FPGA fabric play?
- What limitations did they identify?

### Our Proposed System

- What exactly are we changing?
- Why do we need the Zynq PS?
- What will the C application do?
- Why do we need AXI?
- Which AXI interface is appropriate?
- Will we need DMA?
- Where will input/output matrices reside?

## Decisions Made

## References

![Previous Batch Work](previous_feb2026_implementation.pdf)


## Previous Batch Understanding 



## Project Raw Story

### My Understanding

1. What computation are we accelerating?
**Ans** : 

2. Why is this computation worth accelerating?

3. What does the systolic array do differently from a normal sequential implementation?

4. Why are we using weight-stationary dataflow?

5. What did the previous batch already accomplish?

6. How did their system communicate with the accelerator?

7. What limitation/opportunity do I see in their approach?

8. What exactly do I want to change?

9.  What role will the Zynq PS play?

10. What role will the AXI interface play?

11. What role will my C application play?

12. What should the final user/system be able to do?