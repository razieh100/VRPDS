# VRPDS: Vehicle Routing Problem with Driver Scheduling

C++17 implementation of an Iterated Local Search (ILS) heuristic that plans delivery routes and driver shifts together, under driver availability constraints.

Code for the paper: **Mousavi, R., Côté, J.-F., & Darvish, M. (2026). The vehicle routing problem with driver scheduling. *Computers & Operations Research*, 194, 107507.** [DOI](https://doi.org/10.1016/j.cor.2026.107507) 

## Problem

Classical routing models assume a driver is always available. The VRPDS drops that assumption and decides jointly:

- **Routes:** the order in which each vehicle visits customers, within time windows and capacity
- **Driver schedules:** which driver covers each route, and when their shift starts and ends, given each driver's availability
- **Outsourcing:** which deliveries go to a third-party logistics (3PL) provider

The objective is to minimize travel, shift, and outsourcing costs.

## Method

1. **Construction:** sequential insertion builds the routes, then drivers are created for them
2. **Local search:** relocate, 2-opt\*, cross-exchange, intra-route, depot and shift moves
3. **Assignment:** a driver–route assignment subproblem solved with CPLEX
4. **Perturbation:** ruin-and-recreate

## Code structure

| Component | Files |
|---|---|
| Main loop | `main.cpp`, `Solver` |
| Data model | `Data`, `Node`, `Driver`, `Solution`, `Parameters`, `LoadVrp` |
| Construction | `SeqInsertionVrp`, `CreateDriver` |
| Local search | `Move*`, `Relocate*`, `Opt2StarTw*`, `CrossExchangeTw`, `Intra` |
| Perturbation | `InsRmvMethod*`, `Perturb` |
| CPLEX subproblem | `SolveAssignmentProblem` |

## Build

Requires a C++17 compiler, CMake ≥ 3.24, and IBM ILOG CPLEX (free for academics).

```bash
mkdir build && cd build
cmake .. -DCPLEX_STUDIO_DIR=/path/to/CPLEX_Studio
cmake --build . -j
./vrpd3Feb <instance_file>
```

## Citation

```bibtex
@article{mousavi2026vrpds,
  title   = {The vehicle routing problem with driver scheduling},
  author  = {Mousavi, Razieh and C{\^o}t{\'e}, Jean-Fran{\c{c}}ois and Darvish, Maryam},
  journal = {Computers \& Operations Research},
  volume  = {194},
  pages   = {107507},
  year    = {2026},
  doi     = {10.1016/j.cor.2026.107507}
}
```

Benchmark instances: [download vrpds.zip](https://drive.google.com/file/d/1h5eqUD831vKB33vlsi-llwCBl4NWoHzp/view)

## Author

Razieh Mousavi, PhD candidate, Université Laval / CIRRELT · [LinkedIn](https://www.linkedin.com/in/razieh-mousavi-603b6580)
