# ParallelCode / WorkSimultaneously

Consolidated collection of MPI and OpenMP teaching examples plus small projects (matrix multiplication, N‑body, ring, sieve). Everything now lives under `WorkSimultaneously` with supporting resources in `Resources`.

## About
Hands-on parallel programming sandbox: MPI point-to-point/collectives, OpenMP basics, and small demo projects in one place, with reference tutorial content bundled for offline study.

## Layout
- `WorkSimultaneously/Examples/MPI` – small MPI programs with a Makefile.
- `WorkSimultaneously/Examples/OpenMP` – OpenMP demos; `make` builds all targets.
- `WorkSimultaneously/MatrixMultiplication` – Fox algorithm utilities and sample data (`M2000.dat`).
- `WorkSimultaneously/NBody` – serial and parallel N-body variants.
- `WorkSimultaneously/Ring` – ring communication examples (basic and extended).
- `WorkSimultaneously/SeqSieve` – sequential and parallel sieve implementations.
- `WorkSimultaneously/Resources` – reference PDFs and the `mpitutorial` site content.

## Prerequisites
- MPI toolchain (`mpicc`, `mpirun`).
- OpenMP-capable compiler (e.g., `gcc`/`clang` with `-fopenmp`).
- Standard build tools (`make`).

## Building & Running
```bash
# MPI examples
cd WorkSimultaneously/Examples/MPI
make            # builds all listed targets
mpirun -np 4 ./hello

# OpenMP examples
cd WorkSimultaneously/Examples/OpenMP
make            # builds all OpenMP demos
./omp_hello

# N-body variants
cd WorkSimultaneously/NBody
mpicc -O2 -o nbody Nbody.c              # serial
mpicc -O2 -fopenmp -o nbody_par NbodyParallel.c  # example parallel build
```

Clean builds with `make clean` where available. Executables are ignored via `.gitignore`.

## Notes
- Duplicated sample sets were merged into the `Examples` directories; old standalone folders were removed.
- `Resources/mpitutorial` retains tutorial source markdown for reference; build output (e.g., `_site`) stays ignored.
- If you add new examples, group them under `Examples/MPI` or `Examples/OpenMP` and extend the Makefiles accordingly.
