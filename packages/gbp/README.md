# gbp = Gelly Binary Packer

gbp is a tool to embed dynamic-link libraries (DLLs) into a single binary. The main use case is to package DLLs without
having to ship them as files which can become a can of worms when it comes to deployment. ~~users really cannot
understand how to place DLLs in the correct directory~~

It's specifically designed to ingest DLLs, other tools like [incbin](https://github.com/graphitemaster/incbin) *would*
work but it wouldn't necessarily contain all of our helpers like we have. Those helpers being: MemoryModule support,
module name to data mapping, etc.

## Usage

```shell
gbp [comma-delimited list of DLLs] [output directory]
```

DLLs are fetched either from absolute paths or relative. Wildcards are not supported.

## Output

gbp can't output a single binary, instead, it outputs a multitude of source and header files which contain serialized
binary data.
The final output is a header file which can be included in your project to handle the deserialization of the DLLs and
loading them.

### Typical Output

> [!NOTE]
> You should only include/use the header file in your project.

```
output/
	- gbp/
		- gbp.h
		- [DLL1].h
		- [DLL1].cpp
		- [DLL2].h
		- [DLL2].cpp
		- ...
```