
# Luna HA-Bench

This project is part of the [Luna General Purpose HSMs](https://cpl.thalesgroup.com/encryption/hardware-security-modules/general-purpose-hsms) products suite, and more specifically of the [Luna Network HSM](https://cpl.thalesgroup.com/encryption/hardware-security-modules/network-hsms) product. 

## Introduction

Luna HA-Bench allows to

- Run basic cryptographic scenarii (typically: 5G authentication operations) on a [Luna HA-group](https://www.thalesdocs.com/gphsm/luna/7/docs/network/Content/admin_partition/ha/ha.htm),

- Measure performances in terms of transactions per second.

Luna HA-Bench can run several scenarii concurrently within the same process, each of these scenarii running its own set of tests in separate threads (1 thread per test). Typically, Luna HA-Bench can run COMP-128, Milenage and TUAK authentications concurrently in the same process, along with SUCI deconcealment operations.

Each scenario instance can be configured using flags controlling basic behaviors of the scenario. For more information on these flags, see the usage documentation provided by the HA-Bench binary.

HA-Bench can be multi-instanciated on the same machine or on several hosts sharing the same HA-group to simulate more complex configurations, and study possible side effects of resource sharing algorithms.

Each instance of a scenario (COMP-128/Milenage/TUAK authentication, SUCI deconcealment...) can use its own objects (i.e. objects uniquely created for this instance) or share objects with other instances of the same scenario running within the same process or in other processes.

Scenario objects can be session objects (non persistent) or token objects (persistent).

## Build

The HA-Bench binary can be built using the provided Makefile on a Linux machine (typically a CentOS 7.9 one), with a pre-installed Luna Client SDK.

The provided Makefile use a default configuration that enforces some optimizations at compilation time, as well as the use of typical security measures that are intended to reflect a real operational usage. This configuration can be enhanced with additional features, typically to use sanitization functions or debugging features.

Debugging can be helped using the 'verbose' and 'debug' variables in the main function of Luna HA-Bench.

## Run

Refer to the usage documentation provided by the tool (running it without any parameter).

Typical examples:

| Command  | Description | Typical Results (Mean) |
| -------- | ----------- | ---------------------- |
| ha-bench 0 co-password time-limited 20 share Milenagex01011x80 | Run Milenage authentication on slot 0, for 20 seconds, with: <ul> <li>A non default RC value (pre-loaded on the HSM as a token object)</li> <li>An OP value (pre-loaded on the HSM as a token object)</li> <li> 80 threads</li> </ul> ... and share the pre-loaded objects with other instances of the scenario | <ul> <li> With a single HA-Bench instance: <ul> <li> ~6200 TpS/appliance with 2xLNHs </li> <li> ~5700 TpS/appliance with 16xLNHs </li> </ul> </li> <li> With two HA-Bench instances: <ul> <li> ~7000 TpS/appliance with 2xLNHs </li> <li> ~6300 TpS/appliance with 16xLNHs </li> </ul> </li> <li> With six HA-Bench instances: <ul> <li> ~7200 TpS/appliance with 2xLNHs </li> <li> ~6700 TpS/appliance with 16xLNHs </li> </ul> </li> </ul> |
| ha-bench 0 co-password time-limited 20 share Milenagex00101x80 | Run Milenage authentication on slot 0, for 10 seconds, with: <ul> <li>A non default RC value (pre-loaded on the HSM as a token object)</li> <li>An eOPc value (provided with each request)</li> <li> 80 threads</li> </ul> ... and share the pre-loaded objects with other instances of the scenario            | <ul> <li> With a single HA-Bench instance: <ul> <li> ~6000 TpS/appliance with 2xLNHs </li> <li> ~5600 TpS/appliance with 16xLNHs </li> </ul> </li> <li> With two HA-Bench instances: <ul> <li> ~6800 TpS/appliance with 2xLNHs </li> <li> ~6100 TpS/appliance with 16xLNHs </li> </ul> </li> <li> With six HA-Bench instances: <ul> <li> ~6900 TpS/appliance with 2xLNHs </li> <li> ~6400 TpS/appliance with 16xLNHs </li> </ul> </li> </ul> |

## Test

HA-Bench can run:

- Time-limited tests that are executed for a limited period of time,

- Request-limited tests that have to submit a specific number of requests before to stop.

It is recommended to run time-limited tests to get more accurate and reproducible results.

Concurrent tests can be run using the following kind of command:

```console
for client in 1 2 3 4 5 6; do ./ha-bench 0 co-password time-limited 5 share milenagex01011x80 & done | grep 'Total   Requests Count' | cut -d'=' -f 2 | paste -sd+ - | bc
```

## Contributing

If you are interested in contributing to this project, please read the [Contributing guide](CONTRIBUTING.md).

## License

This software is provided under a [permissive license](LICENSE).
