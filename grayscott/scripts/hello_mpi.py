"""Print one line per MPI rank, plus the MPI library in use."""

from mpi4py import MPI

comm = MPI.COMM_WORLD
print(f"rank {comm.rank} of {comm.size} on {MPI.Get_processor_name()}", flush=True)
comm.Barrier()
if comm.rank == 0:
    print(MPI.Get_library_version().splitlines()[0])
