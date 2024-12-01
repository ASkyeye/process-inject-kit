# Process Inject Kit

This is a port of Cobalt Strike's Process Inject Kit from C to the C++ BOF template.

## How it works

The built in fork and run process injection techniques can be change with
the `PROCESS_INJECT_SPAWN` and `PROCESS_INJECT_EXPLICIT` hooks.

This repo provides the following: 

- Source code implementing the built-in techniques.
- Aggressor script to implement the hooks.
- Visual Studio solution to compile and generate a distribution directory.

### PROCESS_INJECT_SPAWN

Allow users to define how the `spawn` process injection technique is implemented.

### PROCESS_INJECT_EXPLICIT

Allow users to define how the `explicit` process injection technique is implemented.

## References

- https://www.cobaltstrike.com/blog/process-injection-update-in-cobalt-strike-4-5/
- https://hstechdocs.helpsystems.com/manuals/cobaltstrike/current/userguide/content/topics_aggressor-scripts/as-resources_hooks.htm#PROCESS_INJECT_EXPLICIT
- https://hstechdocs.helpsystems.com/manuals/cobaltstrike/current/userguide/content/topics_aggressor-scripts/as-resources_hooks.htm#PROCESS_INJECT_SPAWN

## Usage

To use the Process Inject Kit:

- Build the VS solution in Release mode, as both x64 and x86.
- Load `process-inject.cna` into Cobalt Strike.

## Modifications

You're encouraged to make modifications to this code and use them in your engagements.

## License

This code is not subject to the end user license agreement for Cobalt Strike.
