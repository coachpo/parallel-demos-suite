# Parallel Demos Suite

Hands-on sandbox for learning and teaching parallel programming. It bundles small, self-contained MPI and OpenMP examples plus a few demo projects (N-body, ring, matrix multiplication, sieve) and offline tutorial resources.

## Highlights
- MPI point-to-point and collective examples with ready Makefile.
- OpenMP basics: threads, reductions, worksharing, matrix multiply.
- Mini-projects: N-body variants, ring communication, sieve implementations.
- Offline reference: `Resources/mpitutorial` and OpenMP PDFs.

## Repository Structure
- `WorkSimultaneously/Examples/MPI` – MPI samples (`make` builds all targets).
- `WorkSimultaneously/Examples/OpenMP` – OpenMP demos (`make` builds all).
- `WorkSimultaneously/NBody` – serial & parallel N-body versions.
- `WorkSimultaneously/Ring` – ring communication examples.
- `WorkSimultaneously/MatrixMultiplication` – Fox algorithm utilities and sample matrix file (`M2000.dat`).
- `WorkSimultaneously/SeqSieve` – sequential and parallel sieve variants.
- `WorkSimultaneously/Resources` – OpenMP PDFs and the mpitutorial site source.

## Prerequisites
- MPI toolchain: `mpicc`, `mpirun` (e.g., MPICH or OpenMPI).
- OpenMP-capable C compiler (`gcc`/`clang` with `-fopenmp`).
- Standard build tools: `make`.

## Quick Start
```bash
git clone git@github.com:coachpo/parallel-demos-suite.git
cd parallel-demos-suite
```

### Build MPI examples
```bash
cd WorkSimultaneously/Examples/MPI
make          # builds all executables
mpirun -np 4 ./hello
```

### Build OpenMP demos
```bash
cd WorkSimultaneously/Examples/OpenMP
make          # builds all demos
./omp_hello
```

### Other projects
- N-body: `mpicc -O2 -fopenmp -o nbody_par WorkSimultaneously/NBody/NbodyParallel.c`
- Ring: `mpicc -O2 -o ring WorkSimultaneously/Ring/Ring.c`
- Sieve: build any of the `SeqSieve` sources with your compiler of choice.

## Resources
- `WorkSimultaneously/Resources/mpitutorial` contains the full markdown source of mpitutorial.com for offline reading. Build output (`_site`) is ignored by `.gitignore`.
- OpenMP reference PDFs live in `WorkSimultaneously/Resources/OpenMP`.

## Contributing
Add new examples under `Examples/MPI` or `Examples/OpenMP` and extend the Makefiles. Keep samples minimal and well-commented so they remain teaching-friendly.

## License
See `WorkSimultaneously/LICENSE` for licensing details.
