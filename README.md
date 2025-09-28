# Thesis artifact

This thesis artifact builds all software used and developed in the thesis: "Verifying Optimised Parallel Code" from "Lars Björn van den Haak". That is:

* The [VerCors](https://vercors.ewi.utwente.nl/) verifier to verify code, together with all changes discussed in Chapter 6.
* The [HaliVer](https://link.springer.com/chapter/10.1007/978-3-031-57256-2_4) tool, updated with the latest result of the thesis.
  * Including a HaliVer tutorial.
  * All experiments which were executed in Chapter 4, 5 and 6.
* The [Lean](https://github.com/leanprover/lean4) proof for rewriting quantifiers from Chapter 6.
* All code listings displayed in the thesis are based on actuall code, which we include here.
* The figshare with the card sorting results from Chapter 3 is available in its own artifact:
  https://doi.org/10.4121/12988781

Clone everything with
```bash
git clone --recurse-submodules https://github.com/sakehl/ThesisArtifact.git
```

## Submodules
If you forgot to load the submodules for the HaliVer and VerCors code, you can do the following:
```bash
git submodule update --init --recursive
```

## Running everything from Docker
The suggested method of running everything is Docker. Docker can be installed [here](https://www.docker.com/get-started/).

**Note**: To run docker without `sudo` on Linux, please follow [these steps](https://docs.docker.com/engine/install/linux-postinstall/). Otherwise add `sudo` before all `docker` commands you see here.

## Installing
Run
```bash
docker build -t haliver-experiments .
```
to set up everything.

Afterwards, I suggest mounting the HaliVerExperiments folder as a volume when running Docker.
I.e., the following opens a bash shell within the Docker container:
```bash
docker run -it \
  -v $(pwd)/HaliVerExperiments:/haliver/HaliVerExperiments \
  haliver-experiments /bin/bash
```

From within the Docker container, run:
```bash
cmake -G Ninja -S HaliVerExperiments -B HaliVerExperiments/build \
  && cmake --build HaliVerExperiments/build
```
This builds all the files from HaliVer in the HaliVerExperiments/build directory.

## HaliVer tutorial
The tutorial for HaliVer is located at `HaliVerExperiments/tutorial`.
View the `.cpp` files to see what is going on.

The generated files are located at `HaliVerExperiments/build/tutorial` and can be nice to inspect.

E.g., the first file which is generated can be verified with VerCors.
Within the docker container you can do:
```bash
vct HaliVerExperiments/build/tutorial/lesson1_f_1.c
```
From outside the docker container you can do:
```bash
docker run -it \
  -v $(pwd)/HaliVerExperiments:/haliver/HaliVerExperiments \
  haliver-experiments vct HaliVerExperiments/build/tutorial/lesson1_f_1.c
```

After the command has run, you can see something like:
```
[INFO] Verification completed successfully.
[INFO] Done: VerCors (at 13:07:47, duration: 00:00:36)
```
which indicates verification was successful.


## HaliVer Unit tests
The directory `HaliVerExperiments/tests/unit` contains all kinds of unit tests. 
From within the Docker container, these unit tests can be run with CTest as follows:

```bash
ctest --test-dir HaliVerExperiments/build -L unit
```

## HaliVer Experiments Chapter 4, 5 & Chapter 6
The experiments of Chapter 4 can be found in `HaliVerExperiments/tests/experiment`.

The experiments of Chapter 5 can be found in `HaliVerExperiments/tests/padre`.

These are the same experiments as run in Chapter 6.

### View (raw) experiment results
From outside the container, you can view the exact details of the experiment run of Chapter 6 by starting a local HTTP server. From outside the docker container, do (you need to have Python installed):
```bash
python3 -m http.server
```
Here you should be able to see the results for the regular HaliVer experiments at:
```
http://127.0.0.1:8000/HaliVerExperiments/experiments/results/exp-2025-03-18.xml
```
And the results for the padre HaliVer experiments at:
```
http://127.0.0.1:8000/HaliVerExperiments/experiments/results/padre-2025-03-18.xml
```

The results can also be made into a table with the command (need to have LaTeX installed or run from within the Docker container):
```bash
python3 HaliVerExperiments/experiments/xml_to_latex.py
```
which generates:
```
HaliVerExperiments/experiments/results/table.pdf
```

### Run experiments again
The experiments can be run again. From within the docker container, run:
```bash
python3 experiments/run_experiments.py --timestamp 28-09-2025 --repetitions 1
```
to repeat the experiments with 1 repetition. The results will be saved at:
```
HaliVerExperiments/experiments/results/padre-28-09-2025.xml
HaliVerExperiments/experiments/results/exp-28-09-2025.xml
```
These can be viewed in the local server (see previous section).
They can also be made into a table by doing:
```bash
python3 HaliVerExperiments/experiments/xml_to_latex.py --timestamp 28-09-2025
```
which generates:
```
HaliVerExperiments/experiments/results/table.pdf
```

## Code listings from the thesis
All listings from within the thesis are generated from code. They can be found in the folder `ThesisExamples`. From within the Docker container, you can run VerCors on the appropriate files.


## Lean proof from Chapter 6
The Lean proof from Chapter 6 can be found in the folder `QuantifierLean`. The actual result can be found in the file `QuantifierLean/Result.lean`. All custom definitions can be found in `QuantifierLean/Definitions.lean`.

The proof is automatically checked when the Docker image is built.

But you can also check it again. From within the Docker container, you can verify the Lean proof with:
```bash
# Navigate to the Lean project directory
cd QuantifierLean
# Check the specific file
lake build QuantifierLean.Result
```



### Install on Ubuntu 22.04
Look at the `Dockerfile` to see what needs to be installed and how everything should be configured.