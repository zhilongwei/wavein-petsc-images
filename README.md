# WaveIn PETSc images

Debug and release PETSc development images for `linux/amd64`:

```text
ghcr.io/zhilongwei/wavein-petsc:3.25.4-r2-debug
ghcr.io/zhilongwei/wavein-petsc:3.25.4-r2-release
```

Both contain real, double-precision PETSc with MPICH, Fortran BLAS/LAPACK,
HDF5, ScaLAPACK, MUMPS, SuperLU, and Valgrind. PETSc is installed in
`/opt/petsc` and exposed through CMake and pkg-config.

## Use

```bash
docker pull ghcr.io/zhilongwei/wavein-petsc:3.25.4-r2-release
docker run --rm \
  ghcr.io/zhilongwei/wavein-petsc:3.25.4-r2-release \
  mpiexec -n 1 wavein-petsc-smoke
```

For an interactive shell:

```bash
docker run --rm -it -v "$PWD:/workspace" -w /workspace \
  ghcr.io/zhilongwei/wavein-petsc:3.25.4-r2-debug
```

## Publish

The weekly workflow builds missing tags; the manual workflow builds a requested
PETSc version and image revision. Versioned tags are never overwritten. Increment
`image_revision` in `containers/petsc/image-config.json` when the recipe changes.

Configure the repository secret `WAVEIN_DISPATCH_TOKEN` with a fine-grained token
limited to `zhilongwei/wavein` and **Contents: write** permission. After both image
variants are available, the workflow dispatches WaveIn's compatibility tests.
