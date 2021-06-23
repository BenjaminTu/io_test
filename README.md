This is a repo for handwritten python/c bindings to test input stream functions for the aws c library:
Creating a virtual enviornment is recommended

To test:
run `python3 -m pip install . --verbose`
run `python3 test.py`

To debug:
run `python setup.py build_ext --debug install`

P.S. Remeber you cannot run lldb on system libray, you won't have permission to attach a process, make sure to change the interpreter path/program to a non-system library

launch.json example(for VsCode)
```
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Python: Current File",
            "type": "python",
            "request": "launch",
            "program": "${file}",
            "console": "integratedTerminal",
            "python": "path/to/venv/bin/python"
        },
        {
            "name": "(lldb) Launch",
            "type": "lldb",
            "request": "launch",
            "program": "path/to/venv/bin/python",
            "args": [
                "test.py"
            ],
            "cwd": "${workspaceFolder}",
        }
    ]
}
```