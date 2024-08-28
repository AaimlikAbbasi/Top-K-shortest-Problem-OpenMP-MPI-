# Top-K-shortest-Problem-OpenMP-MPI-
This report presents the implementation and evaluation of a parallel solution for the Top  K Shortest Path Problem using a combination of Message Passing Interface (MPI) for  distributed computing and OpenMP for shared memory parallelization within MPI  processes.

Cluster Setup and MPI Execution Instructions 
This README provides instructions for setting up a cluster environment using Docker and executing MPI 
programs within the cluster. 
1. Cluster Setup 
Follow these steps to set up the cluster environment: - Clone the Cluster-Setup repository from GitHub: 
git clone https://github.com/MARafey/Cluster-Setup.git - Navigate to the `PDC-Cluster-Setup` directory: 
cd PDC-Cluster-Setup - Navigate to the `cluster` directory: 
cd cluster - Execute the `cluster.sh` script with the `up` command to bring up the cluster. Specify the number of 
desired slaves using the `size` parameter (from 1 to 100): 
./cluster.sh up size=4 
2. Cluster Execution 
After setting up the cluster, follow these steps to execute MPI programs within the cluster: - Once the cluster is up, execute the `cluster.sh` script with the `exec` command followed by the 
`mpirun` command to run MPI programs: 
./cluster.sh exec mpirun -n <num_processes> <mpi_program> 
Replace `<num_processes>` with the desired number of MPI processes and `<mpi_program>` with the 
name of the MPI program you want to run. 
For example, to execute the `mpi_hello_world` program: 
./cluster.sh exec mpirun -n 4 mpi_hello_world 
3. Exiting the Cluster 
To exit the cluster environment, follow these steps: - Execute the `exit` command to exit from the kernel level: 
exit 
4. Additional Notes - Ensure Docker is installed on your system before setting up the cluster. - Adjust the size of the cluster according to your requirements by changing the `size` parameter during 
setup. - Ensure that MPI programs are compiled and available within the cluster environment before execution. - Monitor the execution of MPI programs for errors and performance optimization
