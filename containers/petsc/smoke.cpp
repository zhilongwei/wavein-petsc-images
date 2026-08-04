#include <petscdmnetwork.h>
#include <petscdmstag.h>
#include <petscksp.h>
#include <petscviewerhdf5.h>

static PetscErrorCode CheckSolver(Mat A, MatSolverType solver)
{
  Vec b, x;
  KSP ksp;
  PC pc;
  PetscReal error;

  PetscFunctionBeginUser;
  PetscCall(MatCreateVecs(A, &x, &b));
  PetscCall(VecSet(b, 2.0));
  PetscCall(KSPCreate(PETSC_COMM_WORLD, &ksp));
  PetscCall(KSPSetOperators(ksp, A, A));
  PetscCall(KSPSetType(ksp, KSPPREONLY));
  PetscCall(KSPGetPC(ksp, &pc));
  PetscCall(PCSetType(pc, PCLU));
  PetscCall(PCFactorSetMatSolverType(pc, solver));
  PetscCall(KSPSolve(ksp, b, x));

  PetscCall(VecShift(x, -1.0));
  PetscCall(VecNorm(x, NORM_INFINITY, &error));
  PetscCheck(error < 1.0e-12, PETSC_COMM_WORLD, PETSC_ERR_PLIB,
             "%s error: %g", solver, static_cast<double>(error));

  PetscCall(KSPDestroy(&ksp));
  PetscCall(VecDestroy(&x));
  PetscCall(VecDestroy(&b));
  PetscFunctionReturn(PETSC_SUCCESS);
}

int main(int argc, char **argv)
{
  Mat A;
  Vec output;
  PetscMPIInt size;

  PetscCall(PetscInitialize(&argc, &argv, nullptr, nullptr));
  PetscCallMPI(MPI_Comm_size(PETSC_COMM_WORLD, &size));
  PetscCheck(size == 1, PETSC_COMM_WORLD, PETSC_ERR_ARG_SIZ,
             "Run this test with one MPI rank");

  PetscCall(MatCreateSeqAIJ(PETSC_COMM_WORLD, 4, 4, 1, nullptr, &A));
  for (PetscInt row = 0; row < 4; ++row)
    PetscCall(MatSetValue(A, row, row, 2.0, INSERT_VALUES));
  PetscCall(MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY));
  PetscCall(MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY));

  PetscCall(CheckSolver(A, MATSOLVERSUPERLU));
  PetscCall(CheckSolver(A, MATSOLVERMUMPS));

  PetscViewer viewer;
  PetscCall(MatCreateVecs(A, &output, nullptr));
  PetscCall(VecSet(output, 2.0));
  PetscCall(PetscObjectSetName(reinterpret_cast<PetscObject>(output), "rhs"));
  PetscCall(PetscViewerHDF5Open(PETSC_COMM_WORLD,
                                "/tmp/wavein-petsc-smoke.h5",
                                FILE_MODE_WRITE, &viewer));
  PetscCall(VecView(output, viewer));
  PetscCall(PetscViewerDestroy(&viewer));

  PetscCall(VecDestroy(&output));
  PetscCall(MatDestroy(&A));
  PetscCall(PetscFinalize());
  return 0;
}
