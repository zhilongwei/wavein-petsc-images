# wavein PETSc images

Debug and release PETSc development images for `linux/amd64`.

```text
ghcr.io/zhilongwei/wavein-petsc:3.25.4-r1-debug
ghcr.io/zhilongwei/wavein-petsc:3.25.4-r1-release
```

Both images contain real, double-precision PETSc with MPICH,
f2cblaslapack, HDF5, and SuperLU. PETSc is installed in `/opt/petsc` and
applications can use it through CMake or pkg-config.

## Publish

1. Push this repository to GitHub.
2. Run **Build wavein PETSc images** from the Actions tab.
3. Make the `wavein-petsc` package public after its first build.

The weekly release check builds only missing debug or release tags. Versioned
tags are not overwritten, and build caches are kept in GHCR. Public-repository
schedules may need re-enabling after 60 days without repository activity.

## Use

```bash
docker pull ghcr.io/zhilongwei/wavein-petsc:3.25.4-r1-release
docker run --rm \
  ghcr.io/zhilongwei/wavein-petsc:3.25.4-r1-release \
  mpiexec -n 1 wavein-petsc-smoke
```

For a shell with the current directory mounted:

```bash
docker run --rm -it -v "$PWD:/workspace" -w /workspace \
  ghcr.io/zhilongwei/wavein-petsc:3.25.4-r1-debug
```

Increase `image_revision` in `containers/petsc/image-config.json` when the
image recipe changes.
