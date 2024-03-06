# fledgepower-filter-TASE2topivot

A filter plugin which can be used to convert TASE.2 data objects to FledgePower pivot model objects

This filter plugin can run in the TASE.2 north plugin, the TASE.2 south plugin, and the control dispatcher (to filter operations from north to south).

## Build the debug version with test cases

```bash
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Coverage ..
$ make
```

## Build the release version

```bash
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make
```

