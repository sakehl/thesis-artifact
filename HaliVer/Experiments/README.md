# Run tests
Run the following to start tests
```cmd
ctest --test-dir build
# Run only unit tests
ctest --test-dir build -L unit
# Run unit tests with only annotations
ctest --test-dir build -L unit -LE mem
# Run only experiments with annotations
ctest --test-dir build -L experiment -LE mem
# Run only experiments with only memory
ctest --test-dir build -L experiment -L mem
```

# Experiments
## Run experiments
Use 
```
python3 experiments/run_experiments.py
```
## View experiments
Use 
```
python3 -m http.server
```
To start a http server to view the results at
```
experiments/results/exp-2025-03-18.xml
```
## Make table
```
python3 experiments/xml_to_latex.py
```